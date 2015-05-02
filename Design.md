# Konzept #

## Funktionsweise ##

  * Der Stick wird von Windows, Linux und MacOS X als USB-Tastatur erkannt.
  * Sobald der Stick betriebsbereit ist, blinkt die rote LED.
  * Bei einem Tastendruck wird ein etwa 25 Zeichen langer Pad ausgegeben.
  * Ein Block mit 16 Bytes wird mittels AES-256 verschlüsselt und mit einer leicht modifizierten Base64-Kodierung ausgegeben.
  * Enthaltene Zeichen: A-z, 0-9 sowie Plus (+), Minus (-), Schrägstrich (/) sowie Gleichheitsszeichen (=) und Leerzeichen ( )


## Kryptographie ##

Zur Generierung des One-Time-Pads wird im Moment AES-256 als symmetrisches Verschlüsselugnsverfahren benutzt. Der AES-Schlüssel (32-Bytes) und ein Teil des Datenblocks (14-Bytes) werden beim Beschreiben des Sticks zufällig generiert und müssen auch dem Server bekannt sein.

Drückt der Benutzer auf den Taster, verschlüsselt der Stick eine Laufnummer und den Datenblock mit dem AES-Schlüssel und gibt das Ergebnis base64-kodiert aus. Dieser String kann dann z.B. durch ein Formular an einen Webserver geschickt werden, der die Gültigkeit des Pads überprüfen kann. Ist das Pad gültig, so akzeptiert der Server fortan nur noch Pads mit einer Laufnummer, die größer als der zuletzt gesendeten sind.


## Vorteile ##

  * Pads sind einmalig - Tastaturlogger sind kein Sicherheitsproblem.
  * einfache Benutzung
  * Portabilität: USB-HID wird von quasi allen Betriebssystemen unterstützt.
  * bestehende Anwendungen können leicht um OpenKubus-Unterstützung erweitert werden.
  * Passwort und Datenblock kann nur sehr schwer aus dem Stick ausgelesen werden.
  * OpenKubus kann auch zusammen mit herkömlichen Passwörtern kombiniert werden.


## Probleme ##

  * Die Ausgabe des OpenKubus-Stick ist abhängig vom Tastaturlayout.
  * Symmetrische Verschlüsselung: Passwort und Datenblock müssen auf dem Server gespeichert und geheim gehalten werden.
  * Pads, die nicht an den Server geschickt wurden, bleiben vorerst gültig.

Einige dieser Probleme werden in zukünftigen Versionen behoben werden. Siehe [Ideen](Ideen.md).