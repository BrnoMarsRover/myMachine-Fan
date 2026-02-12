# Instalace
Desku lze programovat v prostředí [Arduino IDE](https://www.arduino.cc/en/software/), [Arduino in 100 seconds](https://www.youtube.com/watch?v=1ENiVwk8idM&pp=ygUMYXJkdWlubyBpZGUg)


Pro správnou komunikaci s deskou je potřeba nainstalovat ovladač z: https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads

![driver](driver.png)

Dále v Arduino IDE nainstalovat desky ESP32.
V záložce Tools -> Boards: -> Board Manager nainstalovat balíček esp32

![esp32](esp32.png)

Pro nahrání je potřeba vybrat správný COM port a desku ESP32 Dev Module

Pro otestování, že se kód správně nahrává, lze použít tento kód, který rozbliká LEDku na modulu:

```cpp
/*
 * https://circuits4you.com
 * ESP32 LED Blink Example
 * Board ESP23 DEVKIT V1
 * 
 * ON Board LED GPIO 2
 */

#define LED 2

void setup() {
  // Set pin mode
  pinMode(LED,OUTPUT);
}

void loop() {
  delay(500);
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
}
```
## Seznam doporučených knihoven
Knihovny lze instalovat pomocí *Library manager* (vlevo)

- BLE-Gamepad-Client
- Adafruit Neopixel

### Ovládání motorů
Motory lze řídit pomocí příkazu  ```analogWrite(pin, hodnota)```.

Kdy hodnota je číslo od 0 (netočí) po 255 (maximální rychlost). Směr se určuje podle pinu (R_PWM jeden směr, L_PWM druhý), na kterém danou hodnotu nastavím, viz schéma.

**Pozor**: Nenastavujte nikdy otáčení do obou směrů zároveň, např. 
```cpp
analogWrite(R_PWM, 100);
analogWrite(L_PWM, 50);
```
Jinak by mohlo dojít k poškození driveru! 


