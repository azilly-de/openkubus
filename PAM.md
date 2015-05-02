# Einrichten des PAM-Moduls #

PAM (Pluggable Authentication Modules) ist eine Programmierschnittstelle für
Authentifizierungsdienste. Mittels des PAM-Moduls für den OpenKubus-Stick ist es
möglich, die Benutzerauthentifizierung um die Eingabe des Sticks zu erweitern.
Um sich am System anmelden zu können (z.B. via SSH), ist es dann notwendig, den
richtigen Stick zu besitzen.

## Installation ##
Zum Kompilieren sind die Entwicklungsdateien von gcrypt und PAM notwendig,
sowie gcc, make und die C Entwicklungsdateien. (Debian Pakete: make
libgcrypt11-dev, make, gcc, libc6-dev)

  1. in das Unterverzeichnis `pam` wechseln und mittels `make` den Quellcode kompilieren
  1. PAM-Modul `pam_stick.so` mittels `make install` in das Verzeichnis `/lib/security/pam` kopieren
  1. Dateirecht von `/lib/security/pam\_stick.so` überprüfen (solte `-rw-r--r--` sein)
  1. PAM-Konfigurationsdateien anpassen (`/etc/pam.d/`)
  1. Passwort-Datei `/etc/openkubus-passwd` erstellen und Rechte auf `-rw-------` setzen

## `/etc/openkubus-passwd` ##
In der Datei /etc/openkubus-passwd stehen die notwendigen Daten, um Benutzer mittels
des OpenKubus-Sticks authentifizieren zu können. Diese Datei darf nur vom Benutzer root
gelesen und geschrieben werden dürfen!

Format: Benutzer AES-Schlüssel Datenblock Nummer
  * Benutzer: Benutzer auf dem System
  * AES-Schlüssel: AES-Schlüssel, der auf dem Stick gespeichert ist (32 Zeichen)
  * Datenblock: Datenblock, der auf dem Stick gespeichert ist (14 Zeichen)
  * Nummer: Counter, der automatisch hochgezählt wird

Der AES-Schlüssel und der Datenblock müssen geheim gehalten werden, um die
Sicherheits der Authentifizierung zu gewährleisten.


## Konfiguration von PAM ##

Um PAM dazu zu bewegen, das neue Modul für die Authentifizierung zu nutzen, muss man die PAM Konfigurationsdateien bearbeitet werden. Unter Ubuntu und Debian findet man diese unter `/etc/pam.d/`.

Beispiel für `sudo` (`/etc/pam.d/sudo`):
```
#%PAM-1.0

@include common-auth
@include common-account

auth    required pam_openkubus.so

session required pam_permit.so
session required pam_limits.so
```


## SSH ##

Um das OpenKubus PAM-Modul für die Authentifizierung bei SSH zu benutzen, muss neben der PAM-Konfigurationsdatei für SSH (`/etc/pam.d/ssh`) auch die Konfigurationsdatei für den SSH-Daemon (`/etc/ssh/sshd_config`) angepasst werden: ` UsePAM yes `