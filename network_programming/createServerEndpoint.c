    #include "./My_net_lib.h"


int print_list(struct addrinfo* l)
{
	int count=0;
	struct addrinfo hints, *tmp=NULL;;

	printf("==========================================================\n");	
 	for(tmp=l; tmp; tmp=tmp->ai_next)
       	{	count++;
       		
		printf("protocol type ipV");
		if( tmp->ai_family == PF_INET)
			printf("4\n");
		if( tmp->ai_family == PF_INET6)
			printf("6\n");	
           	printf("type %d\nprotocol %d\n",tmp->ai_socktype,tmp->ai_protocol);
		printf("----------------------------------------------------------\n");
	}
		
	return count;
}

struct addrinfo* swap_nodes(struct addrinfo* l)
{
	struct addrinfo hints, *tmp=NULL;;
	

		tmp=l;
		l=tmp->ai_next;
		l->ai_next=tmp;
		tmp->ai_next=NULL;
		
		return l;
	
}

int createServerEndpoint(const char *host, const char *service,EndpointInfo *info)
{

	int sd,result,x,tr=1,i,list_population;
        struct addrinfo hints, *tmp=NULL;;

        if(service==NULL)
        {
    	    fprintf(stderr,"service can't be NULL\n");
            return -1;
        }

	memset(&hints, 0, sizeof(hints));
	if(host==NULL)
           hints.ai_flags= AI_PASSIVE;
        if(info->type==UDPEndpoint)
	   hints.ai_socktype=SOCK_DGRAM;
	else if(info->type==TCPEndpoint)
	   hints.ai_socktype=SOCK_STREAM;

        result=getaddrinfo(host, service,&hints,&info->addr);

        if (result != 0)
        {
              fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
              return -1;
        }
        if (info->addr->ai_addrlen > sizeof(struct sockaddr_storage))/*extra check*/
        {
        	   fprintf(stderr, "sockaddr too large\n");
		   freeaddrinfo(info->addr);
		   info->addr = NULL;
           return -1;
        }
/*
	printf("list before...\n");
	print_list(info->addr);
	info->addr=swap_nodes(info->addr);
	printf("list after\n");
	print_list(info->addr);
*/		
	printf("-------------------SERVER ENDPOINTS ----------------------\n");
	info->sdlen = 0;
	list_population=print_list(info->addr);	
	info->sd=malloc(list_population);

	for(tmp=info->addr; tmp; tmp=tmp->ai_next)
	{
		sd=socket(tmp->ai_family,tmp->ai_socktype,tmp->ai_protocol);
		printf("socket id %d \n",sd);
		if(sd==-1)
		{
			fprintf(stderr, "socket failed : %s\n",strerror(errno));
			freeaddrinfo(tmp);
			//tmp = NULL;
			continue;
		}
//		if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int))==-1)
//		{
//			fprintf(stderr, "setsockopt %s",strerror(errno));
//			continue;
//		}
//		x=bind(sd,tmp->ai_addr, tmp->ai_addrlen);
//		if(x<0)
//		{
//			/*bind didn't make it*/
//			printf("tried to bind %d with no success \n",sd);
//			fprintf(stderr, "bind failed :%s\n",strerror(errno));
//			close(sd);
//			//freeaddrinfo(tmp);
//		        continue;
//      	}
//        	//bind success/
//		//printf("%d binded succesfully\n",sd);
//                else
//                {
                    info->sd[info->sdlen]=sd;
                    info->sdlen++;
//                }
	}
	printf("binded ::\n");
	for(i=0;i<info->sdlen;i++)
		printf("info->sd[%d] =%d\n",i,info->sd[i]);

	if(info->addr->ai_protocol==IPPROTO_TCP)
	{
		i=0;
		for(i=0;i<info->sdlen;i++)
		{
			x=listen(info->sd[i],info->backlog);
       			if(x<0)
       			{
        	      		/*listen didn't make it*/
				fprintf(stderr, "socket with id %d listen failed :%s\n",info->sd[i],strerror(errno));
				close(info->sd[i]);
				//freeaddrinfo(tmp);
				info->sd[i]=-1;
				//info->sdlen--;
        	       		continue;
       			}
			/*listen success*/
			printf("%d listens\n",info->sd[i]);
			
		}
	}



	printf("server has %d endpoints ready\n",info->sdlen);
//	for(i=0; i<info->sdlen; i++)
//		printf("sd id %d connected\n",info->sd[i]);
	

        return result;
}



/*
	x=listen(4,5);
	if(x<0)
       	{
        	/*listen didn't make it
		fprintf(stderr, "listen failed :%s\n",strerror(errno));
		setsockopt(4,SOL_SOCKET,SO_LINGER,&tr,sizeof(tr));
		close(4);
        }
	else
	{
		/*listen success
		printf("4 listens\n");
	}
	x=listen(3,5);
	if(x<0)
       	{
        	/*listen didn't make it
		fprintf(stderr, "listen failed :%s\n",strerror(errno));
		setsockopt(3,SOL_SOCKET,SO_LINGER,&tr,sizeof(tr));
		close(3);
        }
	else
	{
		/*listen success
		printf("3 listens\n");
	}


*/

