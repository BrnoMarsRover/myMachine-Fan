# Specifikace firmware

Tato stránka popisuje, co musí výsledný program na ESP32 dělat a jak to implementovat.

Chování zařízení z pohledu uživatele je popsáno v [Návodu k obsluze](../manual.md) – ten je závazný. Tato stránka vysvětluje, **jak** toho dosáhnout v kódu.

## Výchozí bod

V repozitáři je soubor [`mymachine.ino`](https://github.com/BrnoMarsRover/myMachine-Fan/blob/main/mymachine.ino) s rozpracovaným kódem, který obsahuje:

- Připojení Xbox ovladače přes Bluetooth (funguje)
- 4 LED animační režimy (fungují)
- Ovládání **jednoho** motoru levým joystickem Y (funguje)

**Co je potřeba doplnit / změnit:**

1. Přidat ovládání druhého motoru (zatáčení)
2. Přidat plynulý rozjezd a brzdění (rampa)
3. Přidat omezení maximální rychlosti
4. Přidat čtení ultrazvukových senzorů a ochranu proti nárazu
5. Zjednodušit LED režimy – pouze přepínání tlačítky A/B/X/Y, bez konfigurace z joysticku
6. Nastavit výchozí LED režim na duhu

## Konfigurace pinů

Viz [schéma zapojení](../hardware/zapojeni.md). Definice pinů na začátku programu:

```cpp
// LED pásek
#define NEOPIXEL_PIN  23
#define NUM_PIXELS    40

// Motor #1 (levý)
#define RPWM1 33
#define LPWM1 32

// Motor #2 (pravý)
#define RPWM2 26
#define LPWM2 25

// Ultrazvukový senzor #1 (levý)
#define TRIG1 19
#define ECHO1 18

// Ultrazvukový senzor #2 (pravý)
#define TRIG2 5
#define ECHO2 17
```

V `setup()` nastavit všechny motorové piny jako výstup:
```cpp
pinMode(RPWM1, OUTPUT);
pinMode(LPWM1, OUTPUT);
pinMode(RPWM2, OUTPUT);
pinMode(LPWM2, OUTPUT);
```

---

## Úkol 1: Diferenciální řízení (zatáčení)

**Cíl:** Levý joystick ovládá jízdu – Y = vpřed/vzad, X = zatáčení.

### Princip

Zařízení má dva motory (levý a pravý). Zatáčení funguje tak, že jeden motor jede rychleji než druhý:

- Joystick vpřed → oba motory jedou vpřed stejně rychle
- Joystick vpřed + doprava → levý motor rychleji, pravý pomaleji → zatáčí doprava
- Joystick jen doprava (bez vpřed) → levý motor vpřed, pravý vzad → otáčení na místě

### Implementace

```cpp
// Globální proměnné pro jízdu
float driveSpeed = 0;  // levý joystick Y (-1..1)
float driveTurn  = 0;  // levý joystick X (-1..1)

// V callbacku onValueChanged():
driveSpeed = s.leftStickY;
driveTurn  = s.leftStickX;
```

Funkce pro ovládání motorů:

```cpp
#define MAX_SPEED 0.6f    // Omezení na 60 % výkonu
#define DEAD_ZONE 0.05f   // Ignorovat malé výchylky joysticku

void moveMotors() {
  // Dead zone
  float speed = (abs(driveSpeed) < DEAD_ZONE) ? 0 : driveSpeed;
  float turn  = (abs(driveTurn)  < DEAD_ZONE) ? 0 : driveTurn;

  // Diferenciální řízení
  float leftMotor  = speed + turn;
  float rightMotor = speed - turn;

  // Omezení maximální rychlosti
  leftMotor  = constrain(leftMotor,  -MAX_SPEED, MAX_SPEED);
  rightMotor = constrain(rightMotor, -MAX_SPEED, MAX_SPEED);

  // Aplikovat rampu (viz Úkol 2)
  // ...

  // Nastavení PWM pro levý motor
  driveOneMotor(RPWM1, LPWM1, leftMotor);
  // Nastavení PWM pro pravý motor
  driveOneMotor(RPWM2, LPWM2, rightMotor);
}

void driveOneMotor(int pinFwd, int pinBwd, float value) {
  int pwm = abs(value) * 255;
  if (value > 0) {
    analogWrite(pinFwd, pwm);
    analogWrite(pinBwd, 0);
  } else if (value < 0) {
    analogWrite(pinFwd, 0);
    analogWrite(pinBwd, pwm);
  } else {
    analogWrite(pinFwd, 0);
    analogWrite(pinBwd, 0);
  }
}
```

**Bezpečnostní pravidlo:** Funkce `driveOneMotor` zajišťuje, že nikdy nejsou oba piny jednoho motoru aktivní současně.

### Testování

1. Nahrajte kód, zvedněte zařízení (kola ve vzduchu)
2. Joystick vpřed → obě kola vpřed
3. Joystick vzad → obě kola vzad
4. Joystick doprava → levé kolo vpřed, pravé vzad (otáčení)
5. Pokud je směr otáčení opačný → prohoďte vodiče na jednom z motorů, nebo prohoďte RPWM/LPWM piny v kódu
6. Teprve potom testujte na zemi

---

## Úkol 2: Plynulý rozjezd a brzdění (rampa)

**Cíl:** Zařízení nezačne okamžitě jet plnou rychlostí a nezastaví náhle. Tím se zabrání trhání, vysypání bonbónů a překvapení dětí.

### Princip

Místo okamžitého nastavení rychlosti na cílovou hodnotu se aktuální rychlost postupně přibližuje k cílové:

```
cílová rychlost:    [0] ──────────── [0.6] ──────────── [0]
aktuální rychlost:  [0] ──/────────/ [0.6] \──────────\ [0]
                         rozjezd              brzdění
```

### Implementace

```cpp
#define RAMP_RATE 2.0f  // Jak rychle se mění rychlost (jednotek za sekundu)

float currentLeft  = 0;  // Aktuální rychlost levého motoru
float currentRight = 0;  // Aktuální rychlost pravého motoru

// Pomocná funkce – posuň hodnotu směrem k cíli
float rampTowards(float current, float target, float maxStep) {
  if (current < target) {
    return min(current + maxStep, target);
  } else {
    return max(current - maxStep, target);
  }
}

void moveMotors() {
  float speed = (abs(driveSpeed) < DEAD_ZONE) ? 0 : driveSpeed;
  float turn  = (abs(driveTurn)  < DEAD_ZONE) ? 0 : driveTurn;

  float targetLeft  = constrain(speed + turn, -MAX_SPEED, MAX_SPEED);
  float targetRight = constrain(speed - turn, -MAX_SPEED, MAX_SPEED);

  // Plynulá rampa
  float dt = 0.015f;  // 15 ms smyčka
  float maxStep = RAMP_RATE * dt;
  currentLeft  = rampTowards(currentLeft,  targetLeft,  maxStep);
  currentRight = rampTowards(currentRight, targetRight, maxStep);

  driveOneMotor(RPWM1, LPWM1, currentLeft);
  driveOneMotor(RPWM2, LPWM2, currentRight);
}
```

Hodnotu `RAMP_RATE` dolaďte při testování – vyšší = rychlejší reakce, nižší = plynulejší rozjezd.

---

## Úkol 3: Ultrazvukové senzory a ochrana proti nárazu

**Cíl:** Zařízení nesmí narazit do překážky ani do osoby. Při přiblížení k překážce postupně zpomaluje a ve 15 cm úplně zastaví.

### Princip

Senzory měří vzdálenost. Podle naměřené vzdálenosti se omezí maximální povolená rychlost vpřed:

| Vzdálenost | Rychlostní limit vpřed |
|------------|----------------------|
| > 40 cm | 100 % (bez omezení) |
| 40–15 cm | Lineárně klesá na 0 % |
| < 15 cm | 0 % (zastavit, lze pouze couvat) |

Couvání a zatáčení nejsou omezeny – aby se zařízení mohlo od překážky vzdálit.

### Implementace

```cpp
#include <Ultrasonic.h>

Ultrasonic sonarLeft(TRIG1, ECHO1);
Ultrasonic sonarRight(TRIG2, ECHO2);

#define DIST_STOP 15    // cm – úplné zastavení
#define DIST_SLOW 40    // cm – začátek zpomalování

// Vrátí koeficient 0.0–1.0 pro omezení rychlosti vpřed
float getObstacleLimit() {
  int distL = sonarLeft.read();
  int distR = sonarRight.read();
  int dist = min(distL, distR);  // Bereme bližší překážku

  if (dist >= DIST_SLOW) return 1.0f;         // Volno
  if (dist <= DIST_STOP) return 0.0f;         // Stop
  // Lineární přechod mezi DIST_STOP a DIST_SLOW
  return (float)(dist - DIST_STOP) / (float)(DIST_SLOW - DIST_STOP);
}
```

Ve funkci `moveMotors()` aplikovat limit **pouze na kladné (dopředné) rychlosti**:

```cpp
float limit = getObstacleLimit();

// Omezit dopředný pohyb, couvání nechat bez omezení
if (targetLeft > 0)  targetLeft  *= limit;
if (targetRight > 0) targetRight *= limit;
```

### Testování

1. V Serial Monitoru vypisujte naměřené vzdálenosti
2. Přiložte ruku před senzor – zařízení by mělo zpomalit a zastavit
3. Zkuste jet vpřed na zeď – musí plynule zastavit
4. Ve stavu zastavení zkuste couvat – musí fungovat

---

## Úkol 4: Zjednodušení LED režimů

**Cíl:** LED režimy se přepínají pouze tlačítky A/B/X/Y. Žádná další konfigurace z joysticku, D-padu ani triggerů – režimy běží autonomně s pevnými parametry.

### Co je potřeba změnit v dodaném kódu

Dodaný kód používá levý joystick pro ovládání barvy LED a pravý joystick pro rychlost animace a jas. To je potřeba **odstranit**, protože:

- Levý joystick nově řídí jízdu (viz Úkol 1)
- Ovládání má být co nejjednodušší – děti na ZŠ jen mačkají tlačítka

### Implementace

Odstraňte z callbacku `onValueChanged()` vše, co se týká LED konfigurace z joysticků. Ponechte pouze přepínání režimů tlačítky:

```cpp
// V callbacku onValueChanged():

// Jízda (levý joystick)
driveSpeed = s.leftStickY;
driveTurn  = s.leftStickX;

// LED režimy – pouze přepínání tlačítky
if (s.buttonA) currentMode = 0;   // jednobarevné
if (s.buttonB) currentMode = 1;   // pohyblivý bod
if (s.buttonX) currentMode = 2;   // duha
if (s.buttonY) currentMode = 3;   // dýchání

// To je vše – žádné další ovládání LED z joysticků/triggerů
```

LED efekty budou pracovat s **pevnými parametry**:

```cpp
#define LED_BRIGHTNESS 128       // Pevný jas (0–255)
#define LED_ANIM_SPEED  1.0f     // Pevná rychlost animace

// V setup():
strip.setBrightness(LED_BRIGHTNESS);
```

### Úprava jednotlivých režimů

Režimy z dodaného kódu je potřeba upravit tak, aby nepotřebovaly vstup z joysticku:

**Režim A – jednobarevné svícení:**
Místo barvy z joysticku použijte pevnou barvu (např. teplá bílá nebo mexická oranžová):
```cpp
void modeSolidColor() {
  // Teplá mexická oranžová
  uint32_t c = strip.Color(255, 140, 0);
  for (int i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, c);
  }
}
```

**Režim B – pohyblivý bod:**
Bod obíhá automaticky pevnou rychlostí, barva se plynule mění sama:
```cpp
void modeMovingDot(uint32_t now) {
  static float pos = 0.0f;
  static uint16_t hue = 0;

  float dt = (now - lastUpdateMs) / 1000.0f;
  pos += dt * 5.0f * LED_ANIM_SPEED;
  if (pos >= NUM_PIXELS) pos -= NUM_PIXELS;
  hue += 100;  // Pomalá automatická změna barvy

  uint32_t c = strip.gamma32(strip.ColorHSV(hue, 255, 255));

  strip.clear();
  int idx = (int)pos;
  strip.setPixelColor(idx, c);
  // Ocásek
  strip.setPixelColor((idx - 1 + NUM_PIXELS) % NUM_PIXELS, strip.Color(10, 10, 10));
  strip.setPixelColor((idx + 1) % NUM_PIXELS, strip.Color(10, 10, 10));
}
```

**Režim X – duha:**
Funguje téměř beze změny, stačí nahradit `animSpeedFactor` konstantou `LED_ANIM_SPEED`.

**Režim Y – dýchání:**
Odstranit ovládání triggerů, použít pevnou barvu (např. stejná mexická oranžová):
```cpp
void modeBreathing(uint32_t now) {
  static uint32_t startMs = 0;
  if (startMs == 0) startMs = now;
  float t = (now - startMs) / 1000.0f;
  float phase = (sin(2.0f * PI * 0.5f * t) + 1.0f) * 0.5f;

  uint8_t r = (uint8_t)(255 * phase);
  uint8_t g = (uint8_t)(140 * phase);
  uint8_t b = (uint8_t)(0   * phase);

  for (int i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
}
```

---

## Úkol 5: Výchozí LED režim a odpojení ovladače

### Výchozí režim po připojení

Po spárování ovladače se LED přepnou do režimu **duha** (`modeRainbow`):

```cpp
// V setup():
currentMode = 2;  // duha jako výchozí
```

### Chování při odpojení ovladače

Již implementováno v dodaném kódu – červené pulzování. Navíc je potřeba zajistit **okamžité zastavení motorů**:

```cpp
if (!controller.isConnected()) {
  // Zastavit motory!
  analogWrite(RPWM1, 0); analogWrite(LPWM1, 0);
  analogWrite(RPWM2, 0); analogWrite(LPWM2, 0);
  currentLeft = 0;
  currentRight = 0;

  // Červené pulzování (existující kód)
  // ...
}
```

---

## Struktura výsledného programu

```
mymachine.ino (výsledný)
│
├── #define a konstanty
│   ├── Piny (motory, LED, senzory)
│   ├── MAX_SPEED, DEAD_ZONE, RAMP_RATE
│   └── DIST_STOP, DIST_SLOW
│
├── Globální proměnné
│   ├── Stav ovladače (driveSpeed, driveTurn)
│   ├── Stav motorů (currentLeft, currentRight)
│   └── Stav LED (currentMode)
│
├── onValueChanged(state)      ← callback ovladače
│   ├── Uloží joysticky do proměnných pro jízdu
│   └── Zpracuje tlačítka (A/B/X/Y → LED režim)
│
├── getObstacleLimit()         ← čtení ultrazvuků → koeficient 0–1
├── rampTowards(cur, target)   ← plynulá rampa
├── driveOneMotor(fwd, bwd, v) ← bezpečné nastavení jednoho motoru
├── moveMotors()               ← diferenciální řízení + rampa + limit senzorů
│
├── LED efekty (ponechat z dodaného kódu)
│   ├── modeSolidColor()
│   ├── modeMovingDot()
│   ├── modeRainbow()
│   └── modeBreathing()
│
├── setup()
│   ├── Inicializace pinů, Serial, LED, Bluetooth
│   └── Výchozí režim = duha
│
└── loop()
    ├── Kontrola připojení (odpojeno → stop + červené pulzování)
    ├── LED efekt podle currentMode
    ├── strip.show()
    ├── moveMotors()
    └── delay(15)
```

---

## Nahrání kódu na ESP32

1. Otevřete `mymachine.ino` v Arduino IDE
2. Nainstalujte potřebné knihovny (viz [ESP32 program](sw.md))
3. V menu **Tools → Board** vyberte **ESP32 Dev Module**
4. V menu **Tools → Port** vyberte COM port s ESP32
5. Klikněte na **Upload** (šipka doprava)
7. Po nahrání otevřete **Serial Monitor** (115200 baud) pro kontrolu výpisů

## Ladění

Program by měl vypisovat na sériový monitor:

```
lx: 0.00, ly: 0.50, rx: -0.30, ry: 0.00 | dist: 45/120 cm | L: 0.30 R: 0.28
```

Doporučené výpisy pro ladění:
- Pozice joysticků (již existuje)
- Vzdálenosti ze senzorů
- Aktuální rychlosti motorů (po rampě a limitu)

## Pořadí implementace

Doporučený postup – od nejdůležitějšího po nejméně kritické:

| Pořadí | Úkol | Důvod |
|--------|------|-------|
| 1. | Diferenciální řízení (Úkol 1) | Bez toho zařízení nezatáčí |
| 2. | Omezení rychlosti + rampa (Úkol 2) | Bezpečnost |
| 3. | Ultrazvukové senzory (Úkol 3) | Ochrana proti nárazu |
| 4. | Zjednodušení LED režimů (Úkol 4) | Odstranění joystick konfigurace, pevné parametry |
| 5. | Výchozí režim + odpojení (Úkol 5) | Dokončení |

Každý úkol testujte samostatně, než přejdete na další.
