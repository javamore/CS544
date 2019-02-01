#SHELL=/usr/bin/ksh
LINK.c=cc
main: search-insert-delete.o mt19937ar.o problem1.o
	gcc -o main search-insert-delete.o mt19937ar.o -lpthread
	gcc -o main2 problem1.o mt19937ar.o -lpthread

problem1.o:problem1.c
	gcc -c -g problem1.c -lpthread	
	
search-insert-delete.o:search-insert-delete.c
	gcc -c -g search-insert-delete.c -lpthread

	
mt19937ar.o:mt19937ar.c
	gcc -c mt19937ar.c -lthread
clean:
	rm main main2 search-insert-delete.o mt19937ar.o problem1.o mt19937ar.o

