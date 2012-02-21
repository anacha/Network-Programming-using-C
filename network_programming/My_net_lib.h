   #ifndef __MY_NET_LIB_H__
   #define __MY_NET_LIB_H__

   #include <stdio.h>
   #include <sys/types.h>
   #include <sys/time.h>
   #include <sys/socket.h>
   #include <netdb.h>
   #include <stdlib.h>
   #include <errno.h>
   #include <string.h>
   #include <unistd.h>


   typedef enum
   {                          
      TCPEndpoint,	// TCP endpoint.
      UDPEndpoint,      // UDP endpoint
   } EndpointType;

   typedef struct
   {
      EndpointType  type;    		// Type of the endpoint.
      int           backlog;            // For TCP server sockets.
      int*           sd;                 // Table of socket descriptors.
      int          sdlen;              // # elements in sd.
      struct addrinfo* addr;         // Info for the above descriptors.
   } EndpointInfo;

   int createServerEndpoint(const char *host, const char *service,EndpointInfo *info);
   int createClientEndpoint(const char *host, const char *service,EndpointInfo *info);
   int closeEndpoint(EndpointInfo *info);
   int getNextClientFromEndpoint(const EndpointInfo *serverInfo,EndpointInfo *clientInfo);
   int sendDataToEndpoint(const EndpointInfo *info, const void *data,int  datalen);
   int recvDataFromEndpoint(const EndpointInfo *info, void *data, int datalen);

   #endif
