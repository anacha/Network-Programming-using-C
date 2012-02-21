#include "My_net_lib.h"




/*typedef struct
   {
      EndpointType  type;    		// Type of the endpoint.
      int           backlog;            // For TCP server sockets.
      int*           sd;                // Table of socket descriptors.
      int          sdlen;               // # elements in sd.
      struct addrinfo* addr;            // Info for the above descriptors.
   } EndpointInfo;

 *
 * struct addrinfo {
             int ai_flags;            input flags
             int ai_family;           protocol family for socket(ipv4 or ipv6)
             int ai_socktype;         socket type		(sockdgram)
             int ai_protocol;         protocol for socket	(tcp/udp/raw)
             socklen_t ai_addrlen;    length of socket-address
             struct sockaddr *ai_addr;  socket-address for socket
             char *ai_canonname;      canonical name for service location
             struct addrinfo *ai_next;  pointer to next in list
     };

 */
int FindMaxFd(int *a,int size)
{
	int i,max=0;
	for(i=0;i<size;i++)
	{
		if(a[i]>max)
			max=a[i];
	}
	return max;

}

int getNextClientFromEndpoint(const EndpointInfo *serverInfo,EndpointInfo *clientInfo)
{
	int x=0,i,j,num_of_ready_sds,*ready_sds=NULL;
	char c;
	


	memset(clientInfo, 0, sizeof(EndpointInfo));
	clientInfo->addr=malloc(sizeof(struct addrinfo));
	memset(clientInfo->addr, 0, sizeof(struct addrinfo));
	clientInfo->addr->ai_addr =(struct sockaddr*)malloc(sizeof(struct sockaddr_storage));
	clientInfo->type=serverInfo->type;

	fd_set read_fdset,write_fdset,error_fdset,result_set;
	/*file descriptor set preparing -> only read_fdset non empty*/
   		FD_ZERO(&read_fdset);
		FD_ZERO(&write_fdset);
		FD_ZERO(&error_fdset);
		for(i=0;i<serverInfo->sdlen;i++)
		{
			//printf("will insert to read_fds %d\n",serverInfo->sd[i]);
			FD_SET(serverInfo->sd[i], &read_fdset);
		}
	while (1)
	{
		/*select : block until something changes in the read_fdset of sockets*/

		//printf("will call select\n");
		num_of_ready_sds=select( FindMaxFd(serverInfo->sd,serverInfo->sdlen)+1, &read_fdset , &write_fdset , &error_fdset , NULL);
		if(num_of_ready_sds == -1)
		{
			fprintf(stderr, "select failed :%s\n",strerror(errno));
		}
		printf("members ready: %d \n",num_of_ready_sds);
		ready_sds=malloc(num_of_ready_sds * sizeof(int));
		for(j=0;j<num_of_ready_sds;j++)
		{
			for(i=0;i<FindMaxFd(serverInfo->sd,serverInfo->sdlen)+1;i++)
			{
				if(FD_ISSET(i,&read_fdset)!=0)
					ready_sds[j]=i;
			}
		}		
		/*lets see which socket  is ready to accept*/
		clientInfo->addr->ai_addrlen = sizeof(struct sockaddr_storage);
		for(i=0; i<num_of_ready_sds ; i++)
		{
			printf("sd=%d ready to accept\n",ready_sds[i]);
		
    			if(serverInfo->type == TCPEndpoint)
			{
         			//printf("will call accept\n");
				x=accept(ready_sds[0]  , clientInfo->addr->ai_addr, &clientInfo->addr->ai_addrlen);
			}
			else if (serverInfo->type == UDPEndpoint)
			{
				printf("will call recvfrom\n");
				x=recvfrom(serverInfo->sd[0],&c,1,MSG_PEEK,clientInfo->addr->ai_addr, &clientInfo->addr->ai_addrlen);
				printf("finished with recvfrom\n");
			}
			if (x < 0 && errno == EINTR)
				continue;
		}
		break;
         }
         if(x==-1)
         {
		printf("accept failure:%s\n",strerror(errno));
		free(clientInfo->addr->ai_addr);
		free(clientInfo->addr);
		clientInfo->addr = NULL;
             return -1;
         }

	printf(" accept success \n");
	clientInfo->sd	  = malloc(sizeof(int));
	clientInfo->sd[0] = (serverInfo->type == TCPEndpoint ? x : serverInfo->sd[0]);
	clientInfo->sdlen = 1;
	return 0;
}
