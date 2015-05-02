## OpenKubus und PAM ##

![http://openkubus.googlecode.com/svn/trunk/images/pam1.png](http://openkubus.googlecode.com/svn/trunk/images/pam1.png)

OpenKubus lässt sich dank des PAM-Moduls leicht zum Authentifizieren von Linux-Rechnern verwenden. Dazu muss das PAM-Modul kompiliert und die zentrale Passwortdatei `/etc/openkubus-passwd` erstellt werden. Informationen und Hilfe dazu finden sich auf der [PAM](PAM.md)-Seite.

OpenKubus ist v.a. für SSH interessant. Dadurch ist es möglich, sich auch von unsicheren Rechnern aus per SSH auf seine Rechner zu verbinden.


## OpenKubus in Webanwendungen ##

![http://openkubus.googlecode.com/svn/trunk/images/web1.png](http://openkubus.googlecode.com/svn/trunk/images/web1.png)

OpenKubus kann auch benutzt werden, um Web-Applikationen und Web-Seiten zu schützen. [login\_sample.php](http://code.google.com/p/openkubus/source/browse/trunk/software/libraries/PHP/login_sample.php) zeigt eine einfache Möglichkeit, bestehende Seiten zu schützen.

Natürlich kann OpenKubus auch in bestehende Login-Verfahren integriert werden. Im Prinzip muss dazu nur der HTML-Quelltext um ein Eingabefeld erweitert werden und im Server-Code der Pad für den Benutzer mittels OpenKubus überprüft werden.