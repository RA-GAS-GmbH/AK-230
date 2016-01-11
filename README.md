# AK-230 

## Versionen
### ohne Akku

* [AK-230-HU-PIC16F873A](https://github.com/zzeroo/AK-230/tree/AK-230-HU-PIC16F873A) (alte MCU)
* [AK-230-HU-PIC16F883](https://github.com/zzeroo/AK-230/tree/AK-230-HU-PIC16F883) (neue MCU)

```
Anschlusskasten mit 12V, 24V oder 230Volt Spannungsversorgung.
Sobald das System mit Spannung versorgt wird ertönt die
Hupe 120 Sekunden lang im 1 Sekunden Takt
und die
LED leuchtet im 1 Sekunden Takt
```

### mit Akku

* [AK-230-AKKU-HU-PIC16F873A](https://github.com/zzeroo/AK-230/tree/AK-230-AKKU-HU-PIC16F873A) (alte MCU)
* [AK-230-AKKU-HU-PIC16F883](https://github.com/zzeroo/AK-230/tree/AK-230-AKKU-HU-PIC16F883) (neue MCU)

```
Anschlusskasten mit 12V Akku
Alarm sobald Stromversorung ausgefallen ist.
Hupe 120 Sekunden
LED bis Akku leer
```

# Anmerkungen für die Entwickler

In der Datei AK-230.c wird im oberen Teil über die 
`#define HARDWARE_MIT_AKKU` gesteuert wie sich die Software verhalten wird.
`#define HARDWARE_MIT_AKKU 1` bedeutet das die Software davon ausgeht, dass
ein Akku verbaut wurde. Die Hupe und die LEDs werden dann nur bei Spannungsausfall
aktiviert.
Dagegen steht `#define HARDWARE_MIT_AKKU 0` dafür, dass im System kein
Akku present ist. Hupe und LEDs werden sobald die Spannung anliegt aktiviert.



