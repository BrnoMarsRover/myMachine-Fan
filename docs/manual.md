# Návod k obsluze

Tento dokument popisuje, jak má hotové zařízení fungovat z pohledu uživatele – tj. dětí na základní škole a jejich učitelů.

Slouží zároveň jako **specifikace chování** pro středoškoláky, kteří budou psát výsledný program.

---

## Zapnutí a připojení ovladače

<!-- TODO: Doplnit, kde přesně je vypínač / jak se zapíná baterie -->

1. Zapněte zařízení
2. LED pásek začne **červeně pulzovat** – zařízení čeká na ovladač
3. Zapněte **Xbox ovladač** podržením tlačítka Xbox (logo uprostřed)
4. Ovladač se automaticky spáruje přes Bluetooth
5. Po připojení LED pásek přejde do **výchozího režimu** (duhová animace) – zařízení je připraveno k jízdě

## Ovládání jízdy

Ovládání je navrženo tak, aby bylo intuitivní i pro děti – stačí **jeden joystick**:

| Ovládací prvek | Funkce |
|---------------|--------|
| Levý joystick ↑ | Jízda vpřed |
| Levý joystick ↓ | Jízda vzad |
| Levý joystick ← | Zatáčení doleva |
| Levý joystick → | Zatáčení doprava |
| Levý joystick šikmo | Kombinace – např. vpřed a doleva = oblouk doleva |

- **Rychlost** závisí na vychýlení joysticku – mírné vychýlení = pomalá jízda, plné vychýlení = maximální (omezená) rychlost
- **Rozjezd je plynulý** – zařízení nezačne okamžitě jet plnou rychlostí, ale postupně zrychluje, aby nedošlo k trhnutí nebo vysypání bonbónů
- **Zastavení je také plynulé** – při puštění joysticku zařízení plynule zabrzdí

### Ochrana proti nárazu

Zařízení má ultrazvukové senzory, které chrání před nárazem do překážek nebo osob:

| Vzdálenost od překážky | Chování |
|----------------------|---------|
| Více než 40 cm | Normální jízda |
| 40–15 cm | Postupné zpomalování (čím blíž, tím pomalejší) |
| Méně než 15 cm | Úplné zastavení – vpřed nelze jet, lze pouze couvat nebo zatáčet pryč |

Senzory fungují pouze ve směru jízdy – brání nárazu vpřed, nebrání couvání.

### Omezení rychlosti

Maximální rychlost je programově omezena na přibližně **60 % výkonu motorů**. Zařízení rozváží bonbóny mezi dětmi, ne závodí – nízká rychlost zajišťuje bezpečnost a zabraňuje vysypání nákladu.

## LED efekty

LED pásek kolem hlavice slouží jako dekorace a přitahuje pozornost dětí. Přepínání režimů je jednoduché – **jedním tlačítkem**:

| Tlačítko | Režim | Popis |
|----------|-------|-------|
| **A** | Jednobarevné svícení | Celý pásek svítí jednou barvou |
| **B** | Pohyblivý bod | Po pásku obíhá světelný bod s ocáskem |
| **X** | Duha | Plynule se posouvající duhový gradient (výchozí režim) |
| **Y** | Dýchání | Jas plynule stoupá a klesá (efekt dýchání) |

## Rozvoz bonbónů

1. Vložte bonbóny
2. Řiďte větrák k cíli – děti si bonbóny vezmou samy

## Bezpečnostní chování

Zařízení má několik automatických ochran:

| Situace | Reakce zařízení |
|---------|----------------|
| Ovladač se odpojí (vybitá baterie, ztráta signálu) | Motory se okamžitě zastaví, LED začnou červeně pulzovat |
| Překážka blíže než 15 cm | Pohyb vpřed zablokován, lze pouze couvat |

## Vypnutí

1. Pusťte joystick – zařízení zastaví
2. Vypněte Xbox ovladač podržením tlačítka Xbox
3. Vypněte zařízení

## Tipy pro provoz

- Zařízení nejlépe funguje na **rovném hladkém povrchu** (lino, dlažba, hladký beton)
- Na koberci bude mít problémy s jízdou (větší odpor)
- Pokud jede zařízení mírně šikmo, je to normální – motory mohou mít lehce odlišné otáčky
- Pokud LED pásek nesvítí nebo motory nereagují, zkuste zařízení vypnout a znovu zapnout
- Při delším provozu může být spodek zařízení teplý – to je normální
- Skrze indikátory na původním zařízení hlídejte stav baterie a včas zařízení nabijte. Při nabíjení nechte zařízení vypnuté.
