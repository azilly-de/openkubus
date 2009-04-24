all:
	gcc -Wall -lgcrypt openkubus.c -c
	gcc -Wall base64.c -c
	gcc test.c  *o -lgcrypt -o test
clean:
	rm -f *.o test
