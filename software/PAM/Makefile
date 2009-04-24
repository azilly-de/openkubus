module:
	gcc -Wall -fPIC  -c pam_openkubus.c
	gcc -shared -Xlinker -x -o pam_openkubus.so pam_openkubus.o ../libraries/C/*.o -lgcrypt
clean:
	rm -rf *.o *.so	
install:
	cp pam_openkubus.so /lib/security/
	chown root:root /lib/security/pam_openkubus.so
	chmod 644 /lib/security/pam_openkubus.so
	
