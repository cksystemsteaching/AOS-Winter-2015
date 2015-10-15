CFLAGS=-O3 -Wall

PROGRAMS=selfie


default: compile run 

compile: 
	-rm -r $ list.mips 
	-touch out
	./selfie < list.c
	-mv out list.mips

run: 
	./selfie -m 32 list.mips
