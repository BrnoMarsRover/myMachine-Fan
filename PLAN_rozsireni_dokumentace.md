# Plán rozšíření dokumentace

Tento dokument popisuje, co v dokumentaci chybí a co je třeba doplnit.
Každá sekce obsahuje popis, co přesně napsat, kam to uložit a na co nezapomenout.

Cílová skupina: středoškoláci, kteří budou projekt realizovat.

---

## 1. Podrobný návod na zapojení (wiring guide)

**Priorita: VYSOKÁ**

**Co chybí:** Schéma zapojení existuje (`hardware/pinout.md`), ale obsahuje pouze obrázek bez textového popisu. Středoškolák, který s tím nemá zkušenosti, potřebuje vědět, co kam zapojit a proč.

**Co udělat:**
- Rozšířit stávající `docs/hardware/pinout.md` nebo vytvořit nový soubor `docs/hardware/zapojeni.md`
- Popsat:
  - **Tabulku pinů ESP32** – co je kam připojeno:

    | GPIO | Komponenta | Funkce |
    |------|-----------|--------|
    | 23 | WS2812 LED pásek | Datový signál (NeoPixel) |
    | 32 | BTS7960B #1 | LPWM (motor levý/zpět) |
    | 33 | BTS7960B #1 | RPWM (motor pravý/vpřed) |
    | ??? | BTS7960B #2 | LPWM (druhý motor – doplnit) |
    | ??? | BTS7960B #2 | RPWM (druhý motor – doplnit) |
    | ??? | HC-SR05 #1 | TRIG + ECHO |
    | ??? | HC-SR05 #2 | TRIG + ECHO |

    (Piny pro druhý motor a senzory je třeba zvolit a zdokumentovat)

  - **Postup zapojení na prototypovací PCB** (pořadí pájení, co zapojit jako první)
  - **Napájení:**
    - Baterie 24V → motor drivery BTS7960B (přímo)
    - Baterie 24V → step-down LM2596 → 3.7V → LED pásek (DC-DC měnič v hlavici)
    - Baterie 24V → step-down LM2596 → ESP32
  - **Zapojení motorů:** motor → BTS7960B → ESP32
  - **Zapojení LED pásku:** datový pin GPIO 23, napájení přes DC-DC měnič v hlavici, signálový kabel veden trubkou
  - **Zapojení ultrazvukových senzorů:** TRIG/ECHO piny
  - **Upozornění na časté chyby:** přehozená polarita, špatný pin, zkrat, aktivace obou směrů motoru současně
- Pokud nový soubor, přidat do `mkdocs.yml` pod sekci `Hardware`

---

## 2. Dokumentace firmware + návod na doprogramování zatáčení

**Priorita: VYSOKÁ**

**Co chybí:** `sw.md` popisuje jen instalaci Arduino IDE a základy ovládání motorů. Hotový firmware (`mymachine.ino`, 224 řádků) má LED animace, Xbox ovladač, 4 režimy – nic z toho není zdokumentováno. Navíc v kódu **chybí zatáčení** (jen jeden motor) – to je na středoškolácích, aby doprogramovali.

**Co udělat:**
- Rozšířit `docs/software/sw.md` nebo vytvořit `docs/software/firmware.md`
- Popsat:
  - **Celková struktura programu:**
    - `setup()` – inicializace LED, motorů, Bluetooth
    - `loop()` – kontrola připojení, spuštění vybraného LED režimu, ovládání motorů
    - `onValueChanged()` – callback volaný při změně stavu ovladače
  - **Párování Xbox ovladače:** jak ovladač zapnout, jak poznat připojení (LED přestanou červeně pulzovat)
  - **LED režimy:**
    - Tlačítko A → jednobarevné svícení (barva podle úhlu levého joysticku)
    - Tlačítko B → pohyblivý bod s ocáskem
    - Tlačítko X → duhová animace
    - Tlačítko Y → dýchání (triggery přidávají červenou/modrou)
  - **Ovládání:**
    - Pravý joystick X → rychlost animace (0.1× až 3.0×)
    - Pravý joystick Y → jas LED (10–255)
    - Levý joystick Y → pohyb vpřed/vzad (dead zone ±0.05)
  - **Chování při odpojení:** červené pulzování (indikace čekání na spárování)
  - **Jak nahrát kód na ESP32:** vybrat desku ESP32 Dev Module, COM port, kliknout Upload

### Úkol pro středoškoláky: doprogramování zatáčení

Aktuální kód ovládá jen jeden motor (piny `RPWM` 33, `LPWM` 32). Pro zatáčení je potřeba:

1. **Zvolit GPIO piny** pro druhý motor driver (BTS7960B #2) a doplnit je jako `#define`
2. **Implementovat diferenciální řízení** ve funkci `moveMotor()`:
   - Levý joystick Y (`leftStickY`) = rychlost vpřed/vzad
   - Pravý joystick X (`rightStickX`) = zatáčení doprava/doleva
   - Princip: při zatáčení doprava zpomalit pravý motor a zrychlit levý (a naopak)
   - Vzorec:
     ```
     rychlost_levy  = rychlost + zataceni
     rychlost_pravy = rychlost - zataceni
     ```
   - Ořezat hodnoty na rozsah 0–255, správně nastavit směr (RPWM/LPWM)
3. **Otestovat** na volném prostranství (motor bez zátěže se chová jinak než s kolem)

Důležité: nikdy nenastavovat oba piny (RPWM i LPWM) jednoho motoru na nenulovou hodnotu současně!

- Pokud nový soubor, přidat do `mkdocs.yml` pod `Software`

---

## 3. Kompletní návod na sestavení (assembly guide)

**Priorita: VYSOKÁ**

**Co chybí:** Existuje návod na rozborku a popis připravených komponent, ale chybí návod, jak celé zařízení sestavit dohromady.

**Co udělat:**
- Vytvořit soubor `docs/construction/sestaveni.md`
- Popsat postup sestavení v logickém pořadí:
  1. Vytisknout 3D díly (odkaz na sekci 3D tisk)
  2. Příprava podvozku – vložení ložisek, opěrných kuliček
  3. Montáž motorů a kol do podvozku
  4. Zapojení motor driverů (BTS7960B) na prototypovací PCB
  5. Připojení ESP32 na prototypovací PCB
  6. Zapojení napájení (baterie → step-down měniče → komponenty)
  7. Propojení ultrazvukových senzorů (vtlačit do připravených děr)
  8. Nasazení sloupku a protažení kabelů (napájení + signálový žlutý drát pro LED)
  9. Montáž hlavice – vložení DC-DC měniče, připojení LED pásku (utahovací pásky)
  10. Vložení nádobky na bonbóny
  11. Nasazení dekorace (sombrero, poncho)
- **Ke každému kroku přidat fotografii** – fotit průběžně během sestavování
- Odkázat na schéma zapojení u elektrických kroků
- Přidat do `mkdocs.yml`:
  ```yaml
  - Sestavení zařízení: construction/sestaveni.md
  ```

---

## 4. Dokumentace 3D tištěných dílů

**Priorita: STŘEDNÍ**

**Co chybí:** Podvozek a další díly (včetně nádobky na bonbóny) jsou 3D tištěné, ale v dokumentaci nejsou žádné informace.

**Co udělat:**
- Vytvořit soubor `docs/construction/3d_tisk.md`
- STL/STEP soubory budou přidány do repozitáře (složka `stl/`)
- Popsat:
  - **Seznam všech 3D tištěných dílů** s fotkami:
    - Podvozek (spodní + horní díl)
    - Držáky/adaptéry pro senzory a motory
    - Nádobka na bonbóny
    - Případné další díly
  - **Odkaz na STL soubory** ve složce `stl/`
  - **Doporučené nastavení tisku:**
    - Materiál: PLA bílý (Prusament PLA Pristine White)
    - Výška vrstvy: doporučit (např. 0.2 mm)
    - Výplň: doporučit (např. 20 %)
    - Podpěry: kde jsou potřeba
    - Orientace na tiskové ploše
  - **Fotky vytištěných dílů** – před a po začištění
- Přidat do `mkdocs.yml`:
  ```yaml
  - 3D tištěné díly: construction/3d_tisk.md
  ```

---

## 5. Uživatelský manuál (jak to ovládat)

**Priorita: STŘEDNÍ**

**Co chybí:** Žádný návod pro koncového uživatele – jak větrák zapnout, jak spárovat ovladač, co dělají tlačítka.

**Co udělat:**
- Vytvořit soubor `docs/manual.md`
- Popsat:
  - **Zapnutí zařízení** (kde je vypínač, jak zapnout baterii)
  - **Spárování Xbox ovladače** (zapnout ovladač → automatické připojení přes Bluetooth)
  - **Indikace stavu LED:**
    - Červené pulzování = ovladač není připojený
    - Barevné LED = připojeno a funkční
  - **Ovládání:**
    - Levý joystick ↑↓ = jízda vpřed/vzad
    - Pravý joystick ←→ = zatáčení
    - A/B/X/Y = přepínání LED režimů (solid / bod / duha / dýchání)
    - Pravý joystick ↑↓ = jas, ←→ = rychlost animace
  - **Kam vložit bonbóny** (otevření nádobky)
  - **Jak zařízení bezpečně vypnout**
  - Ideálně přidat jednoduché schéma ovladače s popisky
- Přidat do `mkdocs.yml`:
  ```yaml
  - Návod k obsluze: manual.md
  ```

---

## 6. Mexický kostým (sombrero + poncho)

**Priorita: NÍZKÁ – kreativní úkol pro středoškoláky**

**Co udělat:**
- Vytvořit soubor `docs/construction/kostym.md`
- Popsat:
  - **Sombrero:** přibližné rozměry (~110 mm průměr, ~120 mm výška), uchycení přes ložisko na vršku hlavice, materiál a postup výroby je na kreativitě středoškoláků
  - **Poncho:** uchycení na tělo větráku, nesmí blokovat ventilaci ani kola, materiál dle uvážení
  - **Ložisko na klobouk:** umožňuje rotaci klobouku nezávisle na hlavici větráku
  - Odkaz na rozměry v `komponenty.md`
- Přidat do `mkdocs.yml`:
  ```yaml
  - Kostým a dekorace: construction/kostym.md
  ```

---

## 7. Řešení problémů (troubleshooting)

**Priorita: NÍZKÁ – doplňovat průběžně**

**Co udělat:**
- Vytvořit soubor `docs/troubleshooting.md`
- Popsat běžné problémy a řešení:
  - **ESP32 se nepřipojí k PC:** nainstalovat CP2102 driver, zkontrolovat COM port, zkusit jiný USB kabel
  - **Ovladač se nespáruje:** zkontrolovat baterie v ovladači, resetovat Bluetooth, restartovat ESP32
  - **Motory se netočí:** zkontrolovat zapojení pinů, napájení 24V, `analogWrite` hodnoty
  - **LED pásek nesvítí:** zkontrolovat datový pin (GPIO 23), napájení 3.7V z DC-DC měniče, směr pásku (šipky)
  - **Ultrazvukové senzory nefungují:** zkontrolovat TRIG/ECHO piny, napájení
  - **Větrák nejede rovně:** kalibrace motorů v kódu (různý odpor, různé otáčky)
  - **Kód se nenahraje:** vybrat správnou desku (ESP32 Dev Module), podržet BOOT tlačítko při nahrávání
- Přidat do `mkdocs.yml`:
  ```yaml
  - Řešení problémů: troubleshooting.md
  ```

---

## Výsledná navigace v `mkdocs.yml`

```yaml
nav:
  - Home: index.md
  - Dětský Vynález: big_picture.md
  - Komponenty:
      - Nákupní seznam: construction/komponenty.md
      - Rozborka větráku: construction/rozborka.md
      - 3D tištěné díly: construction/3d_tisk.md
      - Kostým a dekorace: construction/kostym.md
      - Sestavení zařízení: construction/sestaveni.md
  - Hardware:
      - Schéma: hardware/pinout.md
      - Návod na zapojení: hardware/zapojeni.md
      - Připravené komponenty: hardware/hw.md
  - Software:
      - ESP32 program: software/sw.md
      - Popis firmware: software/firmware.md
  - Návod k obsluze: manual.md
  - Řešení problémů: troubleshooting.md
```

---

## Shrnutí priorit

| # | Úkol | Soubor | Priorita |
|---|------|--------|----------|
| 1 | Návod na zapojení | `hardware/zapojeni.md` | Vysoká |
| 2 | Popis firmware + zatáčení | `software/firmware.md` | Vysoká |
| 3 | Sestavení zařízení | `construction/sestaveni.md` | Vysoká |
| 4 | 3D tištěné díly | `construction/3d_tisk.md` | Střední |
| 5 | Uživatelský manuál | `manual.md` | Střední |
| 6 | Kostým a dekorace | `construction/kostym.md` | Nízká |
| 7 | Řešení problémů | `troubleshooting.md` | Nízká |

**Doporučený postup:** Začít úkoly 1–3 (nelze bez nich sestavit a zprovoznit zařízení), pak 4–5 (pro reprodukovatelnost a použitelnost), nakonec 6–7 (doplňkové).
