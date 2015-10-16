#!/bin/bash
gcc -o selfie selfie.c
./selfie -m 32 out -t > ./debug/temp.txt
./debug/converter ./debug/temp.txt ./debug/log.txt
rm ./debug/temp.txt	
gedit ./debug/log.txt &