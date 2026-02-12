#include <Arduino.h>
#include <BLEGamepadClient.h>
#include <Adafruit_NeoPixel.h>

// ================= NeoPixel config =================
#define NEOPIXEL_PIN  23        // change to your actual data pin
#define NUM_PIXELS    40

//Motor control pins
#define RPWM 33
#define LPWM 32

Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// ================= Gamepad config ==================
XboxController controller;

// Global state for controller
XboxControlsState currentState;

// Animation state
uint8_t  currentMode      = 0;      // 0: solid, 1: moving dot, 2: rainbow
float    animSpeedFactor  = 1.0f;   // scaled by right stick X
uint8_t  globalBrightness = 128;    // scaled by right stick Y (0–255)
uint32_t lastUpdateMs     = 0;

// Joystick values
float stick = 0;

// Utility: constrain float to range
float clampFloat(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

// =============== Controller callback ===============
void onValueChanged(XboxControlsState &s) {
  currentState = s;

  // Example debug output
  Serial.printf("lx: %.2f, ly: %.2f, rx: %.2f, ry: %.2f\n",
                s.leftStickX, s.leftStickY, s.rightStickX, s.rightStickY);

  // Mode switching via face buttons
  if (s.buttonA) currentMode = 0;   // solid color
  if (s.buttonB) currentMode = 1;   // moving dot
  if (s.buttonX) currentMode = 2;   // rainbow
  if (s.buttonY) currentMode = 3;   // breathing

  // Right stick X: speed (-1..1) -> 0.1 .. 3.0
  animSpeedFactor = 1.0f + s.rightStickX * 2.0f;   // -1..1 -> -1..3
  animSpeedFactor = clampFloat(animSpeedFactor, 0.1f, 3.0f);

  // Right stick Y: brightness (-1..1) -> 10 .. 255
  float ry = -s.rightStickY;  // invert so up = brighter
  float b  = (ry + 1.0f) * 0.5f;      // -1..1 -> 0..1
  globalBrightness = (uint8_t)(10 + b * 245.0f);   // 10..255
  strip.setBrightness(globalBrightness);
  stick = s.leftStickY;
}

// =============== Effect helpers ====================

// Map left stick to color (hue)
uint32_t colorFromLeftStick() {
  float lx = currentState.leftStickX; // -1..1
  float ly = currentState.leftStickY; // -1..1

  // Convert to angle 0..360
  float angle = atan2f(ly, lx);       // -pi..pi
  float norm  = (angle + PI) / (2.0f * PI); // 0..1
  uint16_t hue = (uint16_t)(norm * 65535.0f); // 0..65535

  // Use HSV-like mapping via ColorHSV
  return strip.gamma32(strip.ColorHSV(hue, 255, 255));
}

// Mode 0: solid color based on left stick
void modeSolidColor() {
  uint32_t c = colorFromLeftStick();
  for (int i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, c);
  }
}

// Mode 1: moving dot, position from left stick X, color from left stick
void modeMovingDot(uint32_t now) {
  static float pos = 0.0f;

  // Move automatically over time, scaled by animSpeedFactor
  float dt = (now - lastUpdateMs) / 1000.0f; // seconds
  pos += dt * 5.0f * animSpeedFactor;        // 5 LEDs per second base speed
  if (pos >= NUM_PIXELS) pos -= NUM_PIXELS;

  // Offset by left stick X to shift position
  float lx = currentState.leftStickX; // -1..1
  float offset = (lx * (NUM_PIXELS / 2));
  float finalPos = pos + offset;
  while (finalPos < 0) finalPos += NUM_PIXELS;
  while (finalPos >= NUM_PIXELS) finalPos -= NUM_PIXELS;

  uint32_t c = colorFromLeftStick();

  strip.clear();
  int idx = (int)finalPos;
  strip.setPixelColor(idx, c);

  // add simple tail
  int idxPrev = (idx - 1 + NUM_PIXELS) % NUM_PIXELS;
  int idxNext = (idx + 1) % NUM_PIXELS;
  strip.setPixelColor(idxPrev, strip.Color(10, 10, 10));
  strip.setPixelColor(idxNext, strip.Color(10, 10, 10));
}

// Mode 2: rainbow, speed via right stick X
void modeRainbow(uint32_t now) {
  static uint16_t baseHue = 0;
  float dt = (now - lastUpdateMs) / 1000.0f;
  baseHue += (uint16_t)(200 * dt * animSpeedFactor); // hue step

  for (int i = 0; i < NUM_PIXELS; i++) {
    uint16_t hue = baseHue + i * (65535 / NUM_PIXELS);
    uint32_t c = strip.gamma32(strip.ColorHSV(hue, 255, 255));
    strip.setPixelColor(i, c);
  }
}

// Mode 3: breathing color (left stick hue, LT/RT mix)
void modeBreathing(uint32_t now) {
  static uint32_t startMs = 0;
  if (startMs == 0) startMs = now;
  float t = (now - startMs) / 1000.0f;

  // 0..1..0 breathing
  float phase = (sin(2.0f * PI * 0.5f * t) + 1.0f) * 0.5f;

  // Base color from left stick
  uint32_t base = colorFromLeftStick();

  // Simple brightness scale on top of globalBrightness
  uint8_t b = (uint8_t)(phase * 255);

  // Mix triggers into color: LT adds red, RT adds blue
  float lt = currentState.leftTrigger;   // 0..1
  float rt = currentState.rightTrigger;  // 0..1

  uint8_t r = ((base >> 16) & 0xFF);
  uint8_t g = ((base >> 8)  & 0xFF);
  uint8_t bl = (base & 0xFF);

  r = clampFloat(r + lt * 100, 0, 255);
  bl = clampFloat(bl + rt * 100, 0, 255);

  uint32_t c = strip.Color((uint8_t)(r * phase),
                           (uint8_t)(g * phase),
                           (uint8_t)(bl * phase));

  for (int i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, c);
  }
}

void moveMotor(){
  if (abs(stick) < 0.05) {
    analogWrite(RPWM,0);
    analogWrite(LPWM,0);
  }
  else {
    if (stick > 0) {
      analogWrite(RPWM,stick*255);
      analogWrite(LPWM,0);
    }
    else {
      analogWrite(RPWM,0);
      analogWrite(LPWM,stick*255*-1);
    }
  }
}

// ================== Arduino setup/loop ==================
void setup() {
  pinMode(RPWM,OUTPUT);
  Serial.begin(115200);

  strip.begin();
  strip.show();              // initialize all pixels to 'off'
  strip.setBrightness(globalBrightness);

  controller.begin();
  controller.onValueChanged(onValueChanged);
}

void loop() {
  uint32_t now = millis();

  // If not connected, show a slow breathing red
  if (!controller.isConnected()) {
    static uint32_t startMs = 0;
    if (startMs == 0) startMs = now;
    float t = (now - startMs) / 1000.0f;
    float phase = (sin(2.0f * PI * 0.3f * t) + 1.0f) * 0.5f;
    uint8_t b = (uint8_t)(phase * 255);
    strip.setBrightness(80);
    for (int i = 0; i < NUM_PIXELS; i++) {
      strip.setPixelColor(i, strip.Color(b, 0, 0));
    }
  } else {
    // Connected: run selected mode
    switch (currentMode) {
      case 0: modeSolidColor();          break;
      case 1: modeMovingDot(now);       break;
      case 2: modeRainbow(now);         break;
      case 3: modeBreathing(now);       break;
      default: modeSolidColor();        break;
    }
  }

  strip.show();
  moveMotor();
  lastUpdateMs = now;
  delay(15);   // ~60 FPS
}
