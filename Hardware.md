# Hardware #

## Secstick v1 ##

Bei der Schaltung handelt es sich um eine abgespeckte Variante des [usbprog](http://usbn2mc.berlios.de/index.php?page_id=135).

| **Mikroprozessor** | Taktrate   | **Flash** | **RAM** | **EEPROM**  | **USB**    | **USB-Stack** |
|:-------------------|:-----------|:----------|:--------|:------------|:-----------|:--------------|
| Atmega32         | 16 MHz     | 32 kB   | 1 kB  | 512 Bytes | USBN9604 | usbn2mc     |

**[USBprog als openKubus Token](UsbprogToken.md)**


## Secstick v2 ##

Die Schaltung besteht im wesentlichen aus einem Atmega16u4 mit integrierten USB-Controller. Ein externer Quarz ist nicht notwendig, was die Schaltung kleiner und günstiger macht.

| **Mikroprozessor** | Taktrate   | **Flash** | **RAM**    | **EEPROM**  | **USB**    | **USB-Stack** |
|:-------------------|:-----------|:----------|:-----------|:------------|:-----------|:--------------|
| Atmega16u4       | 8 MHz      | 16 kB   | 1,25 kB  | 512 Bytes | intern   | LUFA        |