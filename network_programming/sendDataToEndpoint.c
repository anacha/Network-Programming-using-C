#include "./My_net_lib.h"



int sendDataToEndpoint(const EndpointInfo *info, const void *data,int  datalen)

{
	int x;

	while (1)
	{
		if(info->type == TCPEndpoint)
        		x=write(info->sd[0],data,datalen);
		else if (info->type == UDPEndpoint)
		{
			printf("will try to send to %d \n",info->sd[0]);
			x=sendto(info->sd[0],data,datalen,MSG_DONTROUTE,info->addr->ai_addr,info->addr->ai_addrlen);
			//
		}
		if (x < 0 && errno == EINTR)
			continue;
		break;
    	}
	return x;	
}
