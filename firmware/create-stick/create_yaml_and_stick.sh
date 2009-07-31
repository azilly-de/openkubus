#!/bin/bash

export passwort=`pwgen 46 1`

sudo ./stick-write -p $passwort -O $1

if [ ! -f geheim.yaml ]; 
then
echo "---" >> geheim.yaml
fi


echo "$1:" >> geheim.yaml
echo "  key: $passwort" >> geheim.yaml
echo "  number: 0" >> geheim.yaml
echo "  offset: 0" >> geheim.yaml
