all:
	gcc -Wall openkubus.c -c
	gcc -Wall base64.c -c
	ld -shared base64.o -o base64.so
	gcc -Wall rijndael.c -c
	ld -shared rijndael.o -o rijndael.so
clean:
	rm -f *.o *.so
