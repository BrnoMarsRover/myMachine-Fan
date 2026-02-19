# Návod na zapojení

Tato stránka popisuje, jak správně propojit všechny elektronické komponenty. Před zapojením si
prostudujte [schéma zapojení](pinout.md).

## Přehled pinů ESP32

| GPIO | Komponenta       | Funkce                       | Poznámka                            |
|------|------------------|------------------------------|-------------------------------------|
| 23   | WS2812 LED pásek | Datový signál (NeoPixel)     | Signálový žlutý drát vedený trubkou |
| 32   | BTS7960B #1      | LPWM (motor #1 – směr vzad)  | Nikdy nenastavovat současně s RPWM! |
| 33   | BTS7960B #1      | RPWM (motor #1 – směr vpřed) | Nikdy nenastavovat současně s LPWM! |
| 25   | BTS7960B #2      | LPWM (motor #2 – směr vzad)  | Nikdy nenastavovat současně s RPWM! |
| 26   | BTS7960B #2      | RPWM (motor #2 – směr vpřed) | Nikdy nenastavovat současně s LPWM! |
| 19   | HC-SR05 #1       | TRIG                         | Levý senzor                         |
| 18   | HC-SR05 #1       | ECHO                         | Levý senzor                         |
| 5    | HC-SR05 #2       | TRIG                         | Pravý senzor                        |
| 17   | HC-SR05 #2       | ECHO                         | Pravý senzor                        | 


## Napájení

Zařízení používá **24V baterii** z původního větráku. Z ní se napájí všechny komponenty:

```
Baterie 24V
├── Motor driver BTS7960B #1 (přímo 24V)
├── Motor driver BTS7960B #2 (přímo 24V)
├── Step-down LM2596 #1 → 5V → ESP32 (přes VIN (5V) pin)
└── Step-down LM2596 #2 → 3.7V → LED pásek WS2812 (měnič v hlavici)
```

### Postup nastavení step-down měničů

Měniče LM2596 mají na sobě trimr. **Před připojením komponent** je nutné:

1. Připojit vstup měniče na 24V
2. Multimetrem měřit výstupní napětí
3. Šroubovákem otáčet trimrem, dokud nedosáhnete požadovaného napětí:
    - Měnič pro ESP32: **5V**
    - Měnič pro LED pásek: **3.7V**
4. Teprve potom připojit komponenty na výstup

**Pozor:** Nikdy nepřipojujte ESP32 nebo LED pásek na měnič, dokud nemáte ověřené správné výstupní napětí! 24V by je
okamžitě zničilo.

## Zapojení motor driverů (BTS7960B)

Každý motor driver má tyto důležité piny:

| Pin BTS7960B | Kam zapojit        | Popis                                                 |
|--------------|--------------------|-------------------------------------------------------|
| B+ (VCC)     | Baterie 24V+       | Napájení motoru                                       |
| B- (GND)     | Baterie 24V-       | Zem                                                   |
| M+           | Motor vodič 1      | Výstup na motor                                       |
| M-           | Motor vodič 2      | Výstup na motor                                       |
| RPWM         | ESP32 GPIO         | Otáčení jedním směrem                                 |
| LPWM         | ESP32 GPIO         | Otáčení druhým směrem                                 |
| R_EN         | 5V nebo ESP32 GPIO | Enable pravý (je propojeno na 5V pro trvalé povolení) |
| L_EN         | 5V nebo ESP32 GPIO | Enable levý (je propojeno na 5V pro trvalé povolení)  |
| GND          | ESP32 GND          | Společná zem!                                         |

**Důležité:** GND ESP32 a GND motor driveru musí být propojeny (společná zem), jinak signály nebudou fungovat.

### Schéma zapojení jednoho motoru

```
ESP32 GPIO (RPWM) ──────► BTS7960B RPWM
ESP32 GPIO (LPWM) ──────► BTS7960B LPWM
GND 5V měniče ───────────────► BTS7960B GND
5V ──────────────────────► BTS7960B R_EN + L_EN

Baterie 24V+ ────────────► BTS7960B B+
Baterie 24V- ────────────► BTS7960B B-

BTS7960B M+ ─────────────► Motor vodič 1
BTS7960B M- ─────────────► Motor vodič 2
```

Celý postup opakujte pro druhý motor s druhým BTS7960B modulem.

## Zapojení LED pásku (WS2812)

LED pásek má 3 vodiče:

| Vodič      | Kam zapojit                                  |
|------------|----------------------------------------------|
| VCC        | Výstup step-down měniče 3.7V (v hlavici)     |
| GND        | GND step-down měniče                         |
| DIN (data) | GPIO 23 na ESP32 (žlutý drát vedený trubkou) |

### Postup:

1. LED pásek připevněte utahovacími pásky podél obvodu klece větráku
2. DC-DC měnič (LM2596 nastavený na 3.7V) umístěte do hlavice
3. Napájení (VCC + GND) pásku připojte přímo na výstup měniče v hlavici
4. Datový (signálový) vodič propojte se žlutým drátem, který je vedený trubkou konstrukce dolů k podvozku
5. Ve spodní části žlutý drát propojte s GPIO 23 na ESP32

**Pozor na směr pásku!** Na pásku jsou malé šipky – data tečou ve směru šipek. Připojte DIN na začátek pásku (kde šipky
začínají).

## Zapojení ultrazvukových senzorů (HC-SR05)

Každý senzor má 4 funkční piny:

| Pin senzoru | Kam zapojit                      |
|-------------|----------------------------------|
| VCC         | 5V z ESP32 nebo step-down měniče |
| GND         | GND (společná zem)               |
| TRIG        | ESP32 GPIO (výstupní pin)        |
| ECHO        | ESP32 GPIO (vstupní pin)         |

Senzory jsou vtlačeny do připravených děr v podvozku (nejsou šroubované).

## Pájení na prototypovací PCB

V podvozku jsou dva univerzální plošné spoje (prototypovací PCB) pro propojení komponent.

### Doporučený postup pájení:

1. **Nejdříve** napájet pinové lišty / konektory JST-XH na PCB
2. Propojit napájecí větve (24V, 5V, GND) silnějšími vodiči
3. Zapojit signálové vodiče z ESP32 ke konektorům
4. Před prvním zapnutím zkontrolovat multimetrem:
    - Že mezi VCC a GND není zkrat
    - Že napájecí větve mají správné napětí
5. ESP32 připojit jako poslední (po ověření napětí)

## Společná zem

Všechny komponenty musí sdílet společnou zem (GND):

- ESP32 GND
- BTS7960B #1 GND
- BTS7960B #2 GND
- Step-down měnič #1 GND (vstup i výstup)
- Step-down měnič #2 GND (vstup i výstup)
- Ultrazvukové senzory GND
- LED pásek GND

Bez společné země nebudou signály mezi ESP32 a ostatními komponenty fungovat správně.

## Časté chyby při zapojování

| Chyba                                  | Důsledek                                                      | Jak se vyhnout                                                                                   |
|----------------------------------------|---------------------------------------------------------------|--------------------------------------------------------------------------------------------------|
| Přehozená polarita na step-down měniči | Zničení měniče                                                | Zkontrolovat +/- před zapojením                                                                  |
| Zapojení 24V přímo na ESP32            | Zničení ESP32                                                 | Vždy použít step-down měnič                                                                      |
| Chybějící společná zem                 | Motory/LED nereagují                                          | Propojit všechny GND (stačí skrze zem měniče)                                                    |
| Oba směry motoru aktivní současně      | Zničení motor driveru                                         | V kódu vždy jeden pin na 0                                                                       |
| Napájení LED pásku                     | Pásek nesvítí, nebo se při programování chová nepředvídatelně | LED pásek musí být napájen napětím maximálně 4V, pak již nereaguje správně na řidící signál 3.3V |
| Nepájený studený spoj                  | Náhodné výpadky                                               | Zkontrolovat kvalitu pájení                                                                      |
