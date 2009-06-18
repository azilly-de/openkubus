all:
	gcc -Wall openkubus.c -c
	gcc -Wall base64.c -c
	ld -shared base64.o -o base64.so
	gcc -Wall rijndael.c -c
	ld -shared rijndael.o -o rijndael.so
	gcc test.c  *o -o test
	gcc -Wall test2.c rijndael.c openkubus.c base64.c -o test_create
clean:
	rm -f *.o test
