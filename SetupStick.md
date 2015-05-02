# Stick in Betrieb nehmen #

Um einen Stick benutzen zu können, muss er zunächst programmiert werden und der Schlüssel und der Datenblock in das EEPROM des Sticks geschrieben werden.

## Firmware kompilieren und flashen ##

Die Firmware ist der Teil der Software, der auf dem Stick direkt abläuft und für die USB-Kommunikation und das Erzeugen des Pads bei einem Tastendruck zuständig ist.

### `secstick_v1` ###

Firmware für Schaltungen mit Atmega16/Atmega32 und USBN9604.

In das Verzeichnis `firmware/secstick_v1/crypto-lib/` wechseln und mit `make` die Kryptographie Bibliothek kompilieren. Danach im Vereichnis `firmware/secstick_v1/` mit `make` die Firmware kompilieren.

Die Firmware (`main.bin`) kann z.B. mittels avrdude und usbprog auf den Stick geflashed werden. Die Fuse-Bits müssen so gesetzt sein, dass der AVR Mikroprozessor einen externen Quarz als Takt nutzt. Bei einem usbprog können die Fuses mit `make fuse` gesetzt und der Stick mit `make download` geflashed werden.


### `secstick_v2` ###

Firmware für Schaltungen mit Atmega16u4/Atmega32u4.


In das Verzeichnis `firmware/secstick_v2/firmware/crypto-lib/` wechseln und mit `make` die Kryptographie Bibliothek kompilieren. Danach im Vereichnis `firmware/secstick_v2/firmware` mit `make` die Firmware kompilieren.

Danach die Firmware auf den Stick flashen. Ein Takt von 8 MHz ist aussreichend, es wird also kein externer Quarz benötigt.



## EEPROM beschreiben ##

Der Stick ist jetzt programmiert, es muss aber noch das Passwort in den Stick geschrieben werden. Es ist möglich per USB-Vendor-Request in das EEPROM des Sticks zu schreiben, wenn diese Option nicht bereits deaktiviert wurde (siehe nächster Absatz).

Für `secstick_v1` gibt es ein einfaches Programm in `firmware/secstick_v1/create-stick`, das per `make` kompiliert werden kann:

`Usage: ./stick-write key data [lock]`

  * `key`: AES-256-Schlüssel. 32-Bytes lang
  * `data`: Teil des Datenblocks. 14-Bytes lang
  * `lock`: Wenn dieser Parameter übergeben wird, ist es nach nicht mehr möglich per USB ins EEPROM zu schreiben.

key und data sollten zufällig generiert sein und auch Sonderzeichen und Ziffern enthalten. Wird ein Parameter für `lock` übergeben, kann in Zukunft nicht mehr per USB in das EEPROM geschrieben werden. Sie sollten den Stick für den Produktiveinsatz immer `lock`en!


## Optional: Lock-Bits ##

Es kann weiter sinnvoll sein, die Lock-Bits für den Mikroprozessor zu setzen, um zu verhindern, dass Flash und EEPROM ausgelesen werden können. Zudem kann SPI-Programmierung deaktiviert werden.