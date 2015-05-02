# Programmierung von AT90USBKey oder OpenKUBUS mit der Software FLIP #

Im AT90USBKey oder OpenKUBUS stecken die gleichen Prozesorkerne. Die modernen Mikrocontroller mit USB-Schnittstelle von Atmel kann man einfach mit der Software FLIP programmieren.

Diese Seite zeigt die wesentlichen Unterschiede. Der AT90USBKey hat einen etwas größeren Prozessor als der OpenKUBUS mit welchem sich wesentlich größere Anwendungen wie z.B. Messgeräte bauen lassen. Für kleine Dongelanwendungen reicht der ATMega16u4 von OpenKUBUS in 99% aller Fälle aus.


## Installation FLIP ##
  * Download von ATMEL Homepage
http://www.atmel.com/dyn/products/tools_card.asp?tool_id=3886
  * Installation Starten
  * AT90USB Key oder Open KUBUS anstecken
  * Windows sollte das Gerät erkennen und den Treiber installieren (Notfalls von Hand angeben: C:\Programme\FLIP\usb)
  * FLIP starten
  * Hex Datei Laden
  * RUN klicken
  * Start Application

Ab jetzt ist die Software aktiv.


## AT90 USB Key ##
Von Atmel Evaluierungsboard.
![http://www.eproo.de/jpg.jpeg](http://www.eproo.de/jpg.jpeg)

## Open KUBUS ##
Von embedded projects Evaluierungsboard
![http://www.eproo.de/jpg2.jpeg](http://www.eproo.de/jpg2.jpeg)

## Hinweise ##

Es gibt die Bibliothek LUFA mit welcher viele USB-Dongles vor allem aus der Open-Source Szene entwickelt werden. Die meisten Programme lassen sich mit minimalen Aufwand portieren. Oft findet man auch bei Google viele Projekte für die einzelnen Prozessoren fertig im Internet.