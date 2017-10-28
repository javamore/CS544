#SHELL=/usr/bin/ksh
LINK.c=cc
main: philosopher.o mt19937ar.o
	gcc -o main philosopher.o mt19937ar.o -lpthread
philosopher.o:philosopher.c
	gcc -c -g philosopher.c -lpthread
mt19937ar.o:mt19937ar.c
	gcc -c mt19937ar.c -lthread
clean:
	rm main philosopher.o mt19937ar.o