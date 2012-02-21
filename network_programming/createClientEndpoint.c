#include "My_net_lib.h"

print_list(struct addrinfo* l)
{
	struct addrinfo hints, *tmp=NULL;;

	printf("==========================================================\n");	
 	for(tmp=l; tmp; tmp=tmp->ai_next)
       	{	
       		
		
		if( tmp->ai_family == AF_INET)
			printf("family ipV4\n");
		if( tmp->ai_family == AF_INET6)
			printf("family ipV6\n");	
		if( tmp->ai_family == AF_UNSPEC)
			printf("family ip not known\n");
		//printf("ip family= %d\n",tmp->ai_family);
		if(tmp->ai_socktype== SOCK_STREAM)
		       	printf("protocol type SOCK_STREAM\n");
		if(tmp->ai_protocol== IPPROTO_TCP) 
			printf("protocol type =TCP\n");
		if(tmp->ai_protocol== IPPROTO_UDP) 
			printf("protocol type =UDP\n");
		printf("----------------------------------------------------------\n");
	}
		
}


int createClientEndpoint(const char *host, const char *service,EndpointInfo *info)
{
	int sd, result, x=-1;
	struct addrinfo hints, *res =NULL, *tmp=NULL;

	if(host==NULL || service==NULL)
	{
		fprintf(stderr, "host and client can't be NULL\n");
		return -1;
	}

	memset(&hints, 0, sizeof(struct addrinfo));
       
	if(info->type==UDPEndpoint)
		hints.ai_socktype=SOCK_DGRAM;
	else if(info->type==TCPEndpoint)
		hints.ai_socktype=SOCK_STREAM;
	
	result=getaddrinfo(host, service,&hints,&res);
	if (result != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
	        return -1;
       	}
        if (res->ai_addrlen > sizeof(struct sockaddr_storage))/*perritos elegxos*/
        {
        	fprintf(stderr, "sockaddr too large\n");
           	freeaddrinfo(res);
		res = NULL;
           	return -1;
       	}

	info->sd=malloc(10);
	info->sdlen = 0;
	print_list(res);
       	for(tmp=res; tmp; tmp=tmp->ai_next)
       	{	
        	
           	//info->sd[0]
		sd=socket(tmp->ai_family,tmp->ai_socktype,tmp->ai_protocol);
           	printf("socket id %d \n",sd);
           	if(sd==-1)
           	{
               		fprintf(stderr, "socket failed :%d %s\n",errno,strerror(errno));
			continue;
           	}
           	if(info->type==TCPEndpoint)
           	{
                	x=connect(sd,tmp->ai_addr,tmp->ai_addrlen);
                 	if(x<0)
                	{
                    		/*connect didn't make it*/
				close(sd);
				fprintf(stderr, "connect failed :%s\n",strerror(errno));
                    		/*will try to connect to another socket*/
				continue;
			}
			/*connect success*/
			//printf("socket id:%d connected succsfully(connect returned %d) \n",sd,x);
			
		}
		/*connect success*/
		info->sd[info->sdlen] = sd;
		info->sdlen ++;                    
          	break;
       	}

       	if(info->sdlen == 0)
       	{
		freeaddrinfo(res);
	        fprintf(stderr, "Client :: couldn'create Endpoint.\n");
        	return -1;
       	}
       	info->sd  = (int*)malloc(sizeof(int));
		info->sd[0] = sd;
       	info->addr=res;

       return 0;
}
