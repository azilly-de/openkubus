all:
	gcc -Wall -lgcrypt openkubus.c -c
	gcc -Wall base64.c -c
	gcc test.c  *o -lgcrypt -o test
	gcc -Wall -lgcrypt test2.c openkubus.c base64.c -o test_create
clean:
	rm -f *.o test
