CFLAGS=-O3 -Wall

PROGRAMS=selfie


default: compile run 

compile: 
	-rm -r $ test.mips 
	-touch out
	./selfie -c < test.c
	-mv out test.mips
	-touch out
	./selfie -c < selfie.c
	-mv out selfie.mips
	-touch out
	./selfie -c < selfieProducerConsumer.c
	-mv out producerConsumer.mips

run: 
	./selfie -m 32 test.mips
