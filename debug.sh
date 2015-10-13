#!/bin/bash
clang -w -m32 -D'main(a, b)=main(int argc, char **argv)' -o selfie selfie.c
./selfie -c < selfie.c
./selfie -m 32 out -t > ./debug/temp.txt
./debug/converter ./debug/temp.txt ./debug/log.txt
rm ./debug/temp.txt	
open -t ./debug/log.txt &