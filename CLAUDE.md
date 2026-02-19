# CLAUDE.md – myMachine-Fan

## Projekt
Dokumentace k projektu **My Machine – Chodící Větrák**: pojízdný větrák ve stylu mexičana, který rozváží bonbóny. Vzniká ve spolupráci ZŠ Milotice (nápad) a SŠ (realizace) v rámci programu MyMachine.

## Technologie
- **Dokumentace**: MkDocs s tématem ReadTheDocs, deploy přes GitHub Actions na GitHub Pages
- **Jazyk dokumentace**: čeština
- **Firmware**: Arduino IDE, ESP32 (CP2102), kód v `mymachine.ino`
- **Nasazení**: `mkdocs gh-deploy --force` (automaticky při push na `main`)

## Struktura repozitáře
```
docs/
  index.md              – úvodní stránka
  big_picture.md        – koncept a vize projektu
  construction/
    komponenty.md       – nákupní seznam komponent
    rozborka.md         – návod na rozborku větráku (14 kroků s fotkami)
  hardware/
    hw.md               – příprava HW komponent (podvozek, LED, DC-DC)
    pinout.md           – schéma zapojení
  software/
    sw.md               – instalace Arduino IDE, knihovny, ovládání motorů
  images/               – obrázky (hw/, sw/, rozborka/)
mkdocs.yml              – konfigurace navigace a tématu
mymachine.ino           – firmware pro ESP32
mymachine.drawio        – schéma v draw.io
```

## Konvence
- Dokumentace se píše **česky**, stručně a srozumitelně pro středoškoláky
- Obrázky ukládat do `docs/images/<sekce>/`, v markdownu odkazovat relativně (`../images/...`)
- Navigace se definuje v `mkdocs.yml` v sekci `nav:`
- Commit messages v češtině
- Formát tabulek: Markdown pipe tables (viz `komponenty.md`)

## HW přehled
- Bateriový větrák Stadler Form Finn (24V baterie)
- 2x motor JGY-370 150RPM 24V + 2x BTS7960B H-můstek
- ESP32 DevKit (řízení + Bluetooth)
- 2x Step-down LM2596 (24V → 3.7V pro LED)
- WS2812 LED pásek (40 LED, pin 23)
- 2x HC-SR05 ultrazvukové senzory
- Xbox Wireless Controller (Bluetooth ovládání)

## SW přehled
- Knihovny: BLE-Gamepad-Client, Adafruit NeoPixel, Ultrasonic
- 4 LED režimy (A/B/X/Y tlačítka): solid color, moving dot, rainbow, breathing
- Pravý joystick: rychlost animace (X) + jas (Y)
- Levý joystick: pohyb vpřed/vzad (motor control)
- Odpojený stav: červené pulzování

## Důležité
- Při úpravách `mkdocs.yml` zachovat stávající strukturu `nav:`
- Před přidáním nové stránky vytvořit soubor v `docs/` a přidat do `nav:` v `mkdocs.yml`
- Obrázky pojmenovávat popisně, malými písmeny, bez diakritiky
