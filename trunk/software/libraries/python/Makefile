all:
	swig -python -outdir ./ ../C/openkubus.i
	gcc -fPIC -c ../C/openkubus.c
	gcc -fPIC -c ../C/openkubus_wrap.c -I /usr/include/python2.6
	gcc -fPIC -shared openkubus.o openkubus_wrap.o ../C/rijndael.so ../C/base64.so -o _openkubus.so

install:
	cp _openkubus.so /usr/local/lib/python2.5/site-packages/
	cp openkubus.py /usr/local/lib/python2.5/site-packages/

