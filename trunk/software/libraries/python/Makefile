all:
	swig -python -outdir ./ ../C/openkubus.i
	gcc -fPIC -c ../C/openkubus.c -I /usr/include/python2.5
	gcc -fPIC -shared openkubus.o openkubus.o /lib/libgcrypt.so -o _openkubus.so
