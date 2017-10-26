#SHELL=/usr/bin/ksh
LINK.c=cc
main: prod_con.o mt19937ar.o
	gcc -o main prod_con.o mt19937ar.o -lpthread
prod_con.o:prod_con.c
	gcc -c prod_con.c -lpthread
mt19937ar.o:mt19937ar.c
	gcc -c mt19937ar.c -lthread
clean:
	rm main prod_con.o mt19937ar.o