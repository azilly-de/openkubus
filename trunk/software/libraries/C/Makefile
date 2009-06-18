all:
	gcc -Wall openkubus.c -c
	gcc -Wall base64.c -c
	gcc -Wall rijndael.c -c
	gcc test.c  *o -o test
	gcc -Wall test2.c rijndael.c openkubus.c base64.c -o test_create
clean:
	rm -f *.o test
