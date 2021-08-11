#
# Bethelhem Nigat NIbret
# 20170884
# Computer Network - Project 2
#
all: proxy 

proxy : proxy.o
	gcc -o proxy proxy.o

proxy.o : proxy.c
	gcc -c -o proxy.o proxy.c

clean :
	rm proxy *.o