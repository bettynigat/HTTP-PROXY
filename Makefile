
/* Makefile */

all: proxy 

proxy : proxy.o
	gcc -o proxy proxy.o

proxy.o : proxy.c
	gcc -c -o proxy.o proxy.c

clean :
	rm proxy *.o
