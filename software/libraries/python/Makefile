all:
	swig -python -outdir ./ ../C/openkubus.i
	gcc -fPIC -c ../C/openkubus.c -I /usr/include/python2.5
	gcc -fPIC -shared openkubus.o /lib/libgcrypt.so ../C/base64.so -o _openkubus.so

install:
	cp _openkubus.so /usr/local/lib/python2.5/site-packages/
	cp openkubus.py /usr/local/lib/python2.5/site-packages/

