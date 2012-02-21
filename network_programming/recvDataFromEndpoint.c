#include "./My_net_lib.h"



int recvDataFromEndpoint(const EndpointInfo *info, void *data, int datalen)
{
	int x;
	while(1)
	{
		if(info->type == TCPEndpoint)
			x=read(info->sd[0],data,datalen);
		else if (info->type == UDPEndpoint)
			x=recvfrom(info->sd[0],data,datalen,0,NULL, NULL);
		if (x < 0 && errno == EINTR)
			continue;
		break;
	}
	return x;
}
