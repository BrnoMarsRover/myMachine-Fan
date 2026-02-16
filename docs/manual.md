# Návod k obsluze

Návod k ovládání hotového zařízení – mexického chodícího větráku.

## Zapnutí

<!-- TODO: Doplnit, kde přesně je vypínač / jak se zapíná baterie (původní tlačítko větráku?) -->

1. Zapněte zařízení
2. LED pásek začne **červeně pulzovat** – to znamená, že zařízení čeká na připojení ovladače

## Spárování ovladače

1. Zapněte **Xbox ovladač** podržením tlačítka Xbox (logo uprostřed)
2. Ovladač se automaticky připojí přes Bluetooth k ESP32
3. Po úspěšném připojení LED pásek přestane červeně pulzovat a přepne se do aktivního režimu (barevné svícení)

<!-- TODO: Ověřit, zda je při prvním párování potřeba speciální postup (párovací režim ovladače) -->

## Ovládání jízdy

| Ovládací prvek | Funkce |
|---------------|--------|
| Levý joystick ↑ | Jízda vpřed |
| Levý joystick ↓ | Jízda vzad |
| Pravý joystick ← | Zatáčení doleva |
| Pravý joystick → | Zatáčení doprava |

Rychlost jízdy závisí na tom, jak daleko joystick vychýlíte – mírné vychýlení = pomalá jízda, plné vychýlení = plná rychlost.

**Poznámka:** Zařízení má ultrazvukové senzory, které zabrání nárazu do překážky.
<!-- TODO: Doplnit, jak přesně senzory fungují – zastaví motor? Zpomalí? Zvukový signál? (záleží na implementaci) -->

## LED efekty

LED pásek kolem hlavice větráku podporuje 4 režimy, přepínané tlačítky:

| Tlačítko | Režim | Popis |
|----------|-------|-------|
| **A** | Jednobarevné | Celý pásek svítí jednou barvou – barvu měníte otáčením levého joysticku |
| **B** | Pohyblivý bod | Po pásku se pohybuje světelný bod |
| **X** | Duha | Duhový gradient se plynule posouvá |
| **Y** | Dýchání | Jas plynule stoupá a klesá |

### Nastavení LED

| Ovládací prvek | Funkce |
|---------------|--------|
| Pravý joystick ←→ | Rychlost animace (doleva = pomalá, doprava = rychlá) |
| Pravý joystick ↑↓ | Jas LED (nahoru = jasnější, dolů = tmavší) |
| Levý trigger (LT) | V režimu dýchání přidává červenou |
| Pravý trigger (RT) | V režimu dýchání přidává modrou |

## Rozvoz bonbónů

<!-- TODO: Doplnit popis nádobky – kde je, jak se otevírá, jaké bonbóny se do ní vejdou -->

1. Otevřete nádobku na bonbóny
2. Vložte balené bonbóny
3. Zavřete nádobku
4. Ovládejte větrák k cíli a nechte si bonbóny vyzvednout

## Vypnutí

<!-- TODO: Doplnit postup vypnutí (vypínač? Odpojení baterie?) -->

1. Vypněte zařízení
2. Vypněte Xbox ovladač podržením tlačítka Xbox

## Bezpečnost

- Zařízení neprovozujte v blízkosti vody
- Pokud se motory zaseknou nebo zařízení vydává neobvyklé zvuky, okamžitě ho vypněte
- Neblokujte kola ručně za chodu – motory jsou silné
- Bonbóny vkládejte pouze balené (hygiena)
- Při nabíjení baterie používejte původní nabíječku
