SHELL=/usr/bin/ksh
LINK.c=cc
pc: pc.o mt19937ar.o
	gcc -o pc pc.o mt19937ar.o -lpthread
pc.o:pc.c
	gcc -c pc.c -lpthread
mt19937ar.o:mt19937ar.c
	gcc -c mt19937ar.c -lthread
clean:
	rm pc.o mt19937ar.o