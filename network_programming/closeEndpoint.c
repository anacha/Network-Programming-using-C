#include "./My_net_lib.h"




int closeEndpoint(EndpointInfo *info)
{
    int x;

    
	if (info->sd)
	{
		x=close(info->sd[0]);
		free(info->sd);
	}
	if (info->addr)
    		freeaddrinfo(info->addr);
	info->sd = NULL;
	info->addr = NULL;
	info->sdlen = 0;
   
    return x;
}
