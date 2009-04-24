module:
	gcc -Wall -fPIC  -c pam_stick.c
	gcc -shared -Xlinker -x -o pam_stick.so pam_stick.o ../libraries/C/*.o -lgcrypt
clean:
	rm -r *.o *.so	
install:
	cp pam_stick.so /lib/security/
	
