#makefile for network programming autumn 09

#Makefile for lmynet  ->basic API for network functions

CC=gcc

all:
	$(CC) -c *Endpoint.c
	ar rc libmynet.a *.o
	rm -rf *.o
	ranlib libmynet.a
	$(CC) client.c -L. -lmynet  -o client -lsocket
	$(CC) server.c -L. -lmynet  -o server -lsocket


