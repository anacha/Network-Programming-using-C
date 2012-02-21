#makefile for network programming autumn 09
CC ?= gcc
CFLAGS = -Wall -W -g2
LDFLAGS = -L. -lmynetlib
LIBBILOUOBJS = createClientEndpoint.o createServerEndpoint.o getNextClientFromEndpoint.o sendDataToEndpoint.o recvDataFromEndpoint.o closeEndpoint.o
 
#Makefile for lmynet  ->basic API for network functions
default: libmynetlib.a server client
  
server: libmynetlib.a server.c
	$(CC) $(CFLAGS) server.c $(LDFLAGS) -o server
 
client: libmynetlib.a client.c
	$(CC) $(CFLAGS) client.c $(LDFLAGS) -o server
 		
libmynetlib.a: $(LIBBILOUOBJS)
	rm -f libmynetlib.a
	ar rc libmynetlib.a $(.ALLSRC)
	ranlib libmynetlib.a
 
.c.o: My_net_lib.h
	$(CC) $(CFLAGS) -c $(.IMPSRC)

clean:
	rm -f *.o libmynetlib.a client server
