#SHELL=/usr/bin/ksh
LINK.c=cc
main: Problem2.o mt19937ar.o
	gcc -o main Problem2.o mt19937ar.o -lpthread
Problem2.o:Problem2.c
	gcc -c -g Problem2.c -lpthread
mt19937ar.o:mt19937ar.c
	gcc -c mt19937ar.c -lthread
clean:
	rm main Problem2.o mt19937ar.o