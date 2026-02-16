# Řešení problémů

Zde najdete řešení nejčastějších problémů při sestavování a provozu zařízení.

## ESP32 se nepřipojí k počítači

**Příznaky:** Po připojení USB kabelu se v Arduino IDE neobjeví COM port.

**Řešení:**

1. Nainstalujte ovladač CP2102: [stáhnout zde](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=)
2. Zkuste jiný USB kabel – některé kabely jsou pouze nabíjecí (nemají datové vodiče)
3. Zkuste jiný USB port na počítači
4. Na Windows: zkontrolujte ve Správci zařízení, zda se zařízení zobrazuje (i s chybou)
5. Restartujte počítač po instalaci ovladače

## Kód se nenahraje na ESP32

**Příznaky:** Arduino IDE hlásí chybu při nahrávání (upload failed, timeout).

**Řešení:**

1. Zkontrolujte, že máte vybranou správnou desku: **Tools → Board → ESP32 Dev Module**
2. Zkontrolujte, že máte vybraný správný COM port: **Tools → Port**
3. Zkuste podržet tlačítko **BOOT** na ESP32 během nahrávání (podržte ho těsně před tím, než nahrávání začne hledat desku)
4. Zkontrolujte, že není otevřený Serial Monitor (může blokovat port)
5. Zkuste snížit Upload Speed v nastavení na 115200

## Xbox ovladač se nespáruje

**Příznaky:** LED na větráku stále červeně pulzují, ovladač bliká a nepřipojí se.

**Řešení:**

1. Zkontrolujte, že ovladač má nabité baterie (AA)
2. Vypněte a znovu zapněte ESP32
3. Vypněte a znovu zapněte ovladač (podržet Xbox tlačítko)
4. Ujistěte se, že ovladač není spárovaný s jiným zařízením (Xbox, PC) v dosahu – vypněte ostatní Bluetooth zařízení
5. Zkuste ovladač resetovat: podržte malé tlačítko na zadní straně ovladače (u USB-C portu) a pak znovu zapněte
<!-- TODO: Ověřit přesný postup prvního párování s BLE-Gamepad-Client knihovnou -->

## Motory se netočí

**Příznaky:** Ovladač je spárovaný (LED svítí barevně), ale motory nereagují na joystick.

**Řešení:**

1. **Zkontrolujte napájení** – motor drivery potřebují 24V přímo z baterie
2. **Zkontrolujte zapojení pinů** – RPWM na GPIO 33, LPWM na GPIO 32 (viz [zapojení](hardware/zapojeni.md))
3. **Zkontrolujte společnou zem** – GND ESP32 musí být propojený s GND motor driveru
4. **Zkontrolujte EN piny** – R_EN a L_EN na motor driveru musí být připojeny na 5V (nebo HIGH)
5. **Otevřete Serial Monitor** (115200 baud) a pohybujte joystickem – měli byste vidět měnící se hodnoty `ly`
6. Pokud vidíte hodnoty, ale motor se netočí → problém je v zapojení hardware
7. Pokud nevidíte hodnoty → ovladač nekomunikuje správně

## Motory se točí špatným směrem

**Příznaky:** Při pohybu joystickem vpřed jede větrák vzad (nebo doleva místo doprava).

**Řešení:**

- **Prohoďte vodiče na motoru** (přehoďte M+ a M- na motor driveru), nebo
- **Prohoďte RPWM a LPWM** piny v kódu (upravte `#define`)

## Větrák nejede rovně

**Příznaky:** Při pohybu vpřed zatáčí na jednu stranu.

**Řešení:**

1. Zkontrolujte, že obě kola mají stejný odpor (volně se otáčejí)
2. Zkontrolujte, že opěrné kuličky se volně otáčejí
3. V kódu přidejte korekční faktor – jednomu motoru snižte rychlost o pár procent:
```cpp
// Příklad korekce – pomalý motor pojede o 10 % rychleji
float korekce = 1.10;
analogWrite(RPWM_pomalejsi, rychlost * korekce * 255);
```

## LED pásek nesvítí

**Příznaky:** Ovladač je spárovaný, ale LED pásek zůstává zhasnutý.

**Řešení:**

1. **Zkontrolujte napájení** – měřte multimetrem napětí na výstupu DC-DC měniče v hlavici (má být 3.7V)
2. **Zkontrolujte datový vodič** – žlutý drát musí být propojený od GPIO 23 na ESP32 přes trubku až k DIN vstupu LED pásku
3. **Zkontrolujte směr pásku** – na pásku jsou malé šipky, data tečou ve směru šipek. DIN (data in) musí být na začátku
4. **Zkontrolujte GND** – GND LED pásku musí být propojený se společnou zemí
5. Zkuste jednoduchý testovací kód:
```cpp
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(40, 23, NEO_GRB + NEO_KHZ800);
void setup() {
  strip.begin();
  strip.setBrightness(50);
  for (int i = 0; i < 40; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0)); // zelená
  }
  strip.show();
}
void loop() {}
```
Pokud s tímto kódem pásek svítí, problém je v hlavním firmware. Pokud nesvítí, problém je v zapojení.

## LED pásek bliká nebo zobrazuje náhodné barvy

**Příznaky:** LED pásek svítí, ale barvy jsou náhodné nebo bliká.

**Řešení:**

1. **Špatný signál** – datový vodič je příliš dlouhý nebo nekvalitní spoj. Zkontrolujte pájení.
2. **Rušení** – zkuste přidat 470Ω rezistor do datového vodiče (mezi GPIO 23 a DIN)
3. **Nedostatečné napájení** – pokud svítí hodně LED najednou na plný jas, proud může být nedostatečný. Zkuste snížit jas v kódu.
4. **Špatný počet LED** – v kódu je nastaveno `NUM_PIXELS 40`, zkontrolujte, že váš pásek má skutečně 40 LED

## Ultrazvukové senzory nefungují

**Příznaky:** Zařízení nereaguje na překážky.

**Řešení:**

1. Zkontrolujte zapojení pinů TRIG a ECHO
2. Zkontrolujte napájení senzorů (VCC = 5V)
3. Zkontrolujte, že senzory nejsou zakryté (poncho, bonbónová nádobka)
4. Otestujte senzor jednoduchým kódem:
```cpp
#include <Ultrasonic.h>
// TODO: Doplnit skutečné piny TRIG a ECHO
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);
void setup() {
  Serial.begin(115200);
}
void loop() {
  int distance = ultrasonic.read();
  Serial.print("Vzdálenost: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(500);
}
```

## Baterie se rychle vybíjí

**Řešení:**

1. Neponechávejte zařízení zapnuté, když ho nepoužíváte
2. Snižte jas LED pásku (méně proudu)
3. Při stání nebudíte motory (dead zone v kódu to zajišťuje)
4. Zkontrolujte, že nikde není zkrat (měřte proud multimetrem)

## Stále nevyřešeno?

Pokud problém přetrvává:

1. Otevřete **Serial Monitor** v Arduino IDE (115200 baud) a sledujte výpisy
2. Zkontrolujte všechny spoje multimetrem (napětí, kontinuita)
3. Zkuste problém izolovat – odpojte vše a testujte komponenty jednotlivě
4. Obraťte se na autory projektu nebo založte issue v [repozitáři na GitHubu](https://github.com/BrnoMarsRover/myMachine-Fan/issues)
