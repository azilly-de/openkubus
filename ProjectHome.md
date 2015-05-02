# OpenKubus #

OpenKubus ist ein einfaches Framework für sicherere Authentifizierung mittels automatisch generierter One-Time-Pads.

## Übersicht ##

<table>
<tr>
<td width='500'>
<ul><li><a href='Design.md'>Konzept</a>
</li><li><a href='Hardware.md'>Hardware</a>
</li><li><a href='Software.md'>Software</a>
</li><li><a href='Ideas.md'>Ideen</a>
</li><li><a href='Howtos.md'>Howtos</a>
<ul><li><a href='PAM.md'>PAM</a>
</li><li><a href='SetupStick.md'>Stick in Betrieb nehmen</a>
</li></ul></li><li><a href='Examples.md'>Einsatzbeispiele</a>
</li><li><a href='FAQ.md'>FAQ</a>
</li><li><a href='Credits.md'>Credits</a>
</td></li></ul>

<td align='right'>
<img src='http://openkubus.googlecode.com/svn/trunk/images/hw_v1.jpg' />
</td>

</tr>
</table>

## Beschreibung ##

Normalerweise wird für Authentifizierung ein einfaches Passwort benutzt. Wählen die Benutzer ihre Passwörter selbst aus, sind sie oft zu kurz und leicht zu erraten und damit anfällig. Werden die Passwörter automatisch generiert, sind sie schwer zu merken und die Nutzer neigen dazu, die Passwörter auf Zetteln oder Textdateien zu speichern. Zudem besitzen Passwörter eine lange Gültigkeit, weil sie selten bis nie geändert werden.

Möchte man sich von nicht vertrauenswürdigen Rechner bei einem Dienst anmelden, so sind Passwörter ungeeignet, da möglicherweise ein Passwortsniffer auf dem PC läuft. Eine Lösung sind One-Time-Pads, also Passwörter, die sofort nach dem Einloggen ihre Gültigkeit verlieren. Allerdings muss der Benutzer dann immer eine Liste mit den Passwörtern herumtragen und sie vor fremden Blicken schützen.

Der OpenKubus-Stick (Hardwarelayout frei verfügbar) ist ein kleiner und günstiger USB-Stick, der One-Time-Pads generiert mit dem sich der Nutzer an Systemen authentifizieren kann. OpenKubus kann einfach in bestehende Programme integriert werden. Bibliotheken für C, Perl und PHP sind vorhanden, ein Server sowie ein PAM-Modul.

<img src='http://openkubus.googlecode.com/svn/trunk/images/openkubus_pammodul.png' width='400'>
<img src='http://openkubus.googlecode.com/svn/trunk/images/openkubus_webanwendung.png' width='400'>