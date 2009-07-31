#!/bin/bash

export passwort=`pwgen 46 1`
export datei=`echo $datei_`

sudo ./stick-write -p $passwort -O $1

if [ ! -f $datei ]; 
then
echo "---" >> $datei
fi


echo "$1:" >> $datei
echo "  key: $passwort" >> $datei
echo "  number: 0" >> $datei
echo "  offset: 0" >> $datei
