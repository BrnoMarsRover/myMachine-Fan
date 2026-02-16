# Popis firmware

Tato stránka popisuje strukturu a funkce programu `mymachine.ino`, který běží na ESP32 a ovládá celé zařízení.

Zdrojový kód je k dispozici v [repozitáři na GitHubu](https://github.com/BrnoMarsRover/myMachine-Fan/blob/main/mymachine.ino).

## Přehled funkcí

Program zajišťuje:

- Připojení k Xbox ovladači přes Bluetooth
- Ovládání pohybu (motory)
- Ovládání LED pásku (4 animační režimy)
- Indikaci stavu připojení

## Struktura programu

```
mymachine.ino
├── Konstanty a konfigurace (#define)
├── Globální proměnné (stav ovladače, režim LED, jas, rychlost)
├── onValueChanged()     ← callback – volá se při každé změně stavu ovladače
├── Pomocné funkce pro LED efekty:
│   ├── colorFromLeftStick()  – barva podle úhlu joysticku
│   ├── modeSolidColor()      – režim 0: jednobarevné svícení
│   ├── modeMovingDot()       – režim 1: pohyblivý bod
│   ├── modeRainbow()         – režim 2: duha
│   └── modeBreathing()       – režim 3: dýchání
├── moveMotor()          ← ovládání motorů
├── setup()              ← inicializace při zapnutí
└── loop()               ← hlavní smyčka (opakuje se ~60× za sekundu)
```

## Inicializace (`setup`)

Při zapnutí ESP32 se provede:

1. Nastavení pinů motorů jako výstupní (`pinMode`)
2. Zapnutí sériové komunikace pro ladění (`Serial.begin(115200)`)
3. Inicializace LED pásku (40 LED, pin 23, vypnutý)
4. Spuštění Bluetooth a registrace callbacku pro ovladač

## Hlavní smyčka (`loop`)

Smyčka se opakuje každých 15 ms (~60 FPS):

1. **Kontrola připojení ovladače:**
   - Pokud **není připojený** → zobrazí červené pulzování (pomalé dýchání) jako signál, že čeká na spárování
   - Pokud **je připojený** → spustí vybraný LED režim
2. **Zobrazení LED efektu** (`strip.show()`)
3. **Ovládání motorů** (`moveMotor()`)

## Párování Xbox ovladače

1. Zapněte ESP32 (LED začne červeně pulzovat – čeká na ovladač)
2. Zapněte Xbox ovladač podržením tlačítka Xbox (logo)
3. Ovladač se automaticky připojí přes Bluetooth
4. Po připojení LED přestanou červeně pulzovat a přepnou se na aktivní režim

Knihovna `BLE-Gamepad-Client` zajišťuje komunikaci. Při každé změně stavu ovladače (pohyb joysticku, stisk tlačítka) se zavolá funkce `onValueChanged()`.

<!-- TODO: Doplnit, zda je potřeba první párování provést speciálně (např. podržet párovací tlačítko na ovladači) -->

## Ovládání pohybu

### Aktuální stav (jeden motor)

V současném kódu funguje pouze **jeden motor** na pinech:

- `RPWM` (GPIO 33) – pohyb vpřed
- `LPWM` (GPIO 32) – pohyb vzad

Ovládání levým joystickem (osa Y):

- Joystick nahoru → `analogWrite(RPWM, rychlost)` → jede vpřed
- Joystick dolů → `analogWrite(LPWM, rychlost)` → jede vzad
- Joystick uprostřed (dead zone ±0.05) → oba piny na 0 → stojí

**Bezpečnostní pravidlo:** Nikdy nenastavovat oba piny (RPWM i LPWM) jednoho motoru na nenulovou hodnotu současně – hrozí zničení motor driveru!

---

## Úkol: doprogramování zatáčení (diferenciální řízení)

**Toto je hlavní programátorský úkol pro středoškoláky.**

### Co je potřeba:

Aktuálně je ovládán jen jeden motor. Pro zatáčení potřebujete ovládat **dva motory nezávisle** – to se nazývá diferenciální řízení. Princip: pokud levý motor jede rychleji než pravý, zařízení zatáčí doprava (a naopak).

### Postup implementace:

#### 1. Přidejte piny pro druhý motor

Na začátek souboru přidejte definice pinů pro druhý BTS7960B:

```cpp
// Motor #1 (stávající)
#define RPWM1 33
#define LPWM1 32

// Motor #2 (nový – zvolte volné GPIO piny!)
// TODO: Zvolte vhodné GPIO piny a doplňte
#define RPWM2 ???
#define LPWM2 ???
```

V `setup()` přidejte:
```cpp
pinMode(RPWM2, OUTPUT);
pinMode(LPWM2, OUTPUT);
```

#### 2. Upravte funkci `moveMotor()`

Aktuální funkce používá jen `stick` (levý joystick Y). Potřebujete přidat i pravý joystick X pro zatáčení.

Nejdříve přidejte novou globální proměnnou:
```cpp
float stickTurn = 0;  // pravý joystick X – zatáčení
```

V callbacku `onValueChanged()` přidejte:
```cpp
stickTurn = s.rightStickX;  // -1 = doleva, +1 = doprava
```

**Pozor:** Pravý joystick X se momentálně používá pro rychlost LED animace. Budete muset rozhodnout, zda chcete zatáčení přesunout na jiný vstup, nebo ponechat a LED ovládání přemapovat. Doporučení: použít levý joystick pro oba (X = zatáčení, Y = rychlost) a pravý joystick nechat pro LED.

#### 3. Nová verze `moveMotor()`

Princip diferenciálního řízení:

```cpp
void moveMotor() {
  // stick = levý joystick Y (-1..1), vpřed/vzad
  // stickTurn = levý joystick X (-1..1), zatáčení

  // Dead zone
  float speed = (abs(stick) < 0.05) ? 0 : stick;
  float turn  = (abs(stickTurn) < 0.05) ? 0 : stickTurn;

  // Výpočet rychlostí pro každý motor
  float leftMotor  = speed + turn;   // levý motor
  float rightMotor = speed - turn;   // pravý motor

  // Ořezání na rozsah -1..1
  leftMotor  = constrain(leftMotor, -1.0, 1.0);
  rightMotor = constrain(rightMotor, -1.0, 1.0);

  // Převod na PWM hodnoty (0–255) a nastavení směru
  // Motor #1 (levý)
  if (leftMotor > 0) {
    analogWrite(RPWM1, leftMotor * 255);
    analogWrite(LPWM1, 0);
  } else {
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, -leftMotor * 255);
  }

  // Motor #2 (pravý)
  if (rightMotor > 0) {
    analogWrite(RPWM2, rightMotor * 255);
    analogWrite(LPWM2, 0);
  } else {
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, -rightMotor * 255);
  }
}
```

#### 4. Testování

1. Nahrajte kód na ESP32
2. Zvedněte zařízení (kola ve vzduchu) a testujte:
   - Joystick dopředu → obě kola se točí vpřed
   - Joystick dozadu → obě kola se točí vzad
   - Joystick doleva → levé kolo pomaleji, pravé rychleji
   - Joystick doprava → pravé kolo pomaleji, levé rychleji
3. Pokud se kola točí opačně, prohoďte vodiče na motoru nebo prohoďte RPWM/LPWM piny v kódu
4. Teprve potom testujte na zemi

---

## LED režimy

Přepínání režimů tlačítky na Xbox ovladači:

### Tlačítko A – jednobarevné svícení (`modeSolidColor`)

Celý LED pásek svítí jednou barvou. Barva se mění podle **úhlu levého joysticku** – otáčením joysticku dokola měníte odstín (HSV barevné kolo).

### Tlačítko B – pohyblivý bod (`modeMovingDot`)

Po pásku se pohybuje světelný bod s malým ocáskem. Automaticky se posouvá rychlostí 5 LED/sekundu (základní rychlost). Levý joystick X posune pozici bodu, barva se řídí stejně jako v režimu A.

### Tlačítko X – duha (`modeRainbow`)

Duhový gradient se plynule posouvá po celém pásku. Rychlost posunu závisí na nastavení rychlosti animace.

### Tlačítko Y – dýchání (`modeBreathing`)

LED pásek plynule zesiluje a zeslabuje jas (efekt dýchání). Barva se řídí levým joystickem. Navíc:

- **Levý trigger (LT)** přidává červenou složku
- **Pravý trigger (RT)** přidává modrou složku

### Společné ovládání pro všechny režimy

| Ovládací prvek | Funkce | Rozsah |
|---------------|--------|--------|
| Pravý joystick X | Rychlost animace | 0.1× až 3.0× |
| Pravý joystick Y | Jas LED | 10–255 (nahoru = jasnější) |

## Nahrání kódu na ESP32

1. Otevřete `mymachine.ino` v Arduino IDE
2. Nainstalujte potřebné knihovny (viz [ESP32 program](sw.md))
3. V menu **Tools → Board** vyberte **ESP32 Dev Module**
4. V menu **Tools → Port** vyberte COM port s ESP32
5. Klikněte na **Upload** (šipka doprava)
6. Pokud nahrávání selže, zkuste podržet tlačítko **BOOT** na ESP32 během nahrávání
7. Po nahrání otevřete **Serial Monitor** (115200 baud) pro kontrolu výpisů

## Ladění

Program vypisuje na sériový monitor pozice joysticků:
```
lx: 0.00, ly: 0.50, rx: -0.30, ry: 0.00
```

- `lx`, `ly` = levý joystick X, Y
- `rx`, `ry` = pravý joystick X, Y

Hodnoty jsou v rozsahu -1.0 až 1.0. Toto je užitečné pro ověření, že ovladač komunikuje správně, a pro ladění dead zone motorů.
