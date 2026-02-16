# Sestavení zařízení

Tento návod popisuje krok za krokem, jak sestavit celé zařízení od jednotlivých dílů až po funkční pojízdný větrák.

**Než začnete**, ujistěte se, že máte:

- Všechny komponenty ze [seznamu](komponenty.md)
- Rozložený větrák podle [návodu na rozborku](rozborka.md)
- Vytištěné [3D díly](3d_tisk.md)
- Přečtený [návod na zapojení](../hardware/zapojeni.md)

## Krok 1: Příprava 3D tištěného podvozku

<!-- TODO: Přidat fotku vytištěného podvozku -->

1. Vytiskněte všechny díly podvozku podle [návodu na 3D tisk](3d_tisk.md)
2. Odstraňte podpěry a začistěte hrany (pilníkem nebo nožem)
3. Vložte **opěrné kuličky** (5 ks) do připravených otvorů na spodní straně podvozku – kuličky zajišťují stabilitu a umožňují volný pohyb
4. Vložte **ložiska do kol** (2× ZKL 626-2Z) do připravených otvorů pro osy

## Krok 2: Montáž motorů a kol

<!-- TODO: Přidat fotku namontovaných motorů -->

1. Vložte oba motory **JGY-370** do připravených úchytů v podvozku
2. Připevněte motory šroubky ze spodní strany
3. Nasaďte kola na hřídele motorů
4. Zkontrolujte, že se kola volně otáčejí a nebrzdí o podvozek

## Krok 3: Příprava elektroniky na prototypovacím PCB

V podvozku jsou dva univerzální plošné spoje. Na ně zapájíte konektory pro propojení elektroniky.

<!-- TODO: Přidat fotku hotového PCB s popisky -->

1. Napájejte konektory JST-XH na PCB (pro motory, senzory, napájení)
2. Zapájejte napájecí rozvod:
   - 24V přívod z baterie
   - Výstup na motor drivery (24V)
   - Výstup na step-down měnič (24V vstup → 5V výstup pro ESP32)
3. Zapájejte signálové vodiče z ESP32 pinů ke konektorům
4. Podrobný postup viz [návod na zapojení](../hardware/zapojeni.md)

## Krok 4: Montáž motor driverů

<!-- TODO: Přidat fotku motor driverů v podvozku -->

1. Umístěte oba moduly **BTS7960B** do podvozku
2. Připevněte šroubky
3. Propojte motor drivery s PCB pomocí připravených konektorů:
   - Napájení (24V)
   - Signálové vodiče (RPWM, LPWM, EN)
   - GND
4. Připojte vodiče motorů k výstupům M+/M- na motor driverech

## Krok 5: Osazení ESP32

<!-- TODO: Přidat fotku ESP32 v podvozku -->

1. ESP32 umístěte na PCB (ideálně do dutinkové patice, aby šla snadno vyměnit)
2. Propojte s napájením (5V z měniče na VIN pin, GND)
3. **Před zapnutím** zkontrolujte multimetrem napětí na step-down měniči (musí být 5V!)
4. Zapojte signálové vodiče ke konektorům na PCB

## Krok 6: Montáž ultrazvukových senzorů

1. Senzory **HC-SR05** vtlačte do připravených otvorů v podvozku (nejsou šroubované, drží třením)
2. Připojte konektory senzorů na PCB (VCC, GND, TRIG, ECHO)

<!-- TODO: Přidat fotku senzorů v podvozku, doplnit kam přesně směřují -->

## Krok 7: Nasazení sloupku a protažení kabelů

Toto je klíčový krok – sloupek spojuje podvozek s hlavicí větráku a vedou jím kabely.

1. Protáhněte sloupkem:
   - **Napájecí kabel** (24V pro motor ventilátoru a DC-DC měnič v hlavici)
   - **Žlutý signálový drát** pro LED pásek (datový pin GPIO 23)
2. Připojte konektory na obou koncích kabelu (spodní + horní)
3. Nasaďte sloupek na podvozek a zajistěte aretačními šroubky
4. Postup rozebírání/skládání konektorů ve sloupku viz [rozborka](rozborka.md)

![LED konektor v hlavici](../images/hw/LED_konektor_hlavice.jpg)
![LED konektor v podstavci](../images/hw/LED_konektor_baze.jpg)

## Krok 8: Montáž hlavice – DC-DC měnič a LED pásek

1. Do hlavice umístěte **step-down měnič LM2596** nastavený na **3.7V**
   - Vstup: 24V z kabelu vedeného sloupkem
   - Výstup: 3.7V pro LED pásek

![Pozice DC-DC měniče](../images/hw/dcdc_misto.jpg)

2. LED pásek **WS2812** připevněte utahovacími pásky podél obvodu klece větráku
   - **Pozor na směr!** Šipky na pásku ukazují směr toku dat
   - Datový vstup (DIN) připojte na žlutý signálový drát z trubky
   - Napájení (VCC + GND) připojte na výstup DC-DC měniče

![LED pásek na kleci](../images/hw/LED_pasek.jpg)

3. Nasaďte hlavici zpět na sloupek a zajistěte šroubky

## Krok 9: Vložení baterie a test

1. Vložte baterii zpět do přihrádky (pozor na pogo piny!)
2. Zapněte zařízení
3. LED pásek by měl začít červeně pulzovat (čeká na spárování ovladače)
4. Spárujte Xbox ovladač – LED by se měly rozsvítit barevně
5. Otestujte:
   - Motory reagují na joystick
   - LED režimy se přepínají tlačítky A/B/X/Y
   - Pokud něco nefunguje, viz [Řešení problémů](../troubleshooting.md)

## Krok 10: Nádobka na bonbóny

<!-- TODO: Doplnit popis umístění nádobky – je součástí 3D tištěných dílů -->
<!-- TODO: Přidat fotku nádobky na bonbóny -->

1. Připevněte 3D tištěnou nádobku na bonbóny na určené místo na zařízení
2. Zkontrolujte, že nádobka nebrání otáčení kol ani ventilátoru

## Krok 11: Nasazení mexického kostýmu

1. Umístěte **sombrero** na vršek hlavice (uchycení přes ložisko)
2. Navlékněte **poncho** na tělo větráku
3. Zkontrolujte, že dekorace:
   - Neblokuje kola
   - Neblokuje ventilaci
   - Nezakrývá ultrazvukové senzory
   - Nespadává při jízdě

Podrobnosti viz [Kostým a dekorace](kostym.md).

## Hotovo!

Pokud jste úspěšně prošli všemi kroky, máte sestaveného mexického chodícího větráka. Kompletní návod k ovládání najdete v sekci [Návod k obsluze](../manual.md).
