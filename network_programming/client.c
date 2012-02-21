#include <errno.h>
#include "./My_net_lib.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>


#define BUFSIZE 512
#define RETR_CHOICE 1
#define STORE_CHOICE 2


void create_args(char *source,char *res1,char *res2)
{
	char *tmp=NULL;
	tmp=malloc(strlen(source));

	tmp=strstr(source,":");
	tmp[0]='\0';
	tmp=&tmp[1];
	strcpy(res2,tmp);
	strcpy(res1,source);
}



int Choose_Action(char* choice)
{
	int x=0;
	int i;
	char a[BUFSIZE];

	strcpy(a,choice);
	if( (a[0]=='R' || a[0]=='r') &&
	    (a[1]=='E' || a[1]=='e') &&
	    (a[2]=='T' || a[2]=='t') &&
	    (a[3]=='R' || a[3]=='r')
	  )
		x=RETR_CHOICE;
	if(x==RETR_CHOICE && strlen(a) >4)
	{ 
	    if( (a[4]=='I' || a[4]=='i') &&
		(a[5]=='V' || a[5]=='v') &&
     	        (a[6]=='E' || a[6]=='e') && strlen(a)==7
	        )
		{
	   		x=RETR_CHOICE;
			printf("1\n");
		}
	  	else 
		{
			printf("2\n");
			x=0;
		}
	}
	if( (a[0]=='S' || a[0]=='s') &&
	    (a[1]=='T' || a[1]=='t') &&
	    (a[2]=='O' || a[2]=='o') &&
	    (a[3]=='R' || a[3]=='r') 
	    )    
		x=STORE_CHOICE;  
	if(x==STORE_CHOICE && strlen(a)>4)
	{
		if(a[4]=='E' || a[4]== 'e' && (strlen(a)==5))
			x=STORE_CHOICE;
		else
			x=0;
	}
	printf("ACTION %d\n",x);
	return x;	
}




int main(int argc, char **argv)
{

	EndpointInfo server;

	char *s_data=NULL;
	char *f_data=NULL;
	char* msg=NULL;
	s_data=malloc(BUFSIZE);
	char *node=NULL;
	char *port=NULL;
	char *s=NULL;
	FILE *fd1;
	
	int x,k,i,res,j,sum=0;;

	if (argc > 5)
	{
        	fprintf(stderr, "Usage: %s<host:port/service> function(R/S) filename <tcp|udp>?\n", argv[0]);
        	return 1;
      	}

	if(argc == 5 && strcmp(argv[4],"udp")==0)
		server.type=UDPEndpoint;
      	else
		server.type=TCPEndpoint;
	
	f_data=malloc(BUFSIZE);
	msg=malloc(BUFSIZE);
	node=malloc(strlen(argv[1]));
	port=malloc(strlen(argv[1]));
	s   =malloc(strlen(argv[1]));
	strcpy(s,argv[1]);
	
	create_args(s,node,port);

	for(i=0;i<5;i++)
		printf("argv[%d]=%s\n",i,argv[i]);
	printf("node= %s\n",node);
	printf("port= %s\n",port);
	printf("s=%s\n",s);
	
      	if (createClientEndpoint(node , port ,&server))
      	{
		printf("%s \n",strerror(errno));
        	return 1;
      	}
	
	


	strcpy(s_data,argv[2]);
	printf("sdata =%s\n",s_data);
	res=Choose_Action(s_data);			
	
	strcat(s_data," ");
	strcat(s_data,argv[3]);
	strcat(s_data,"\0");

	printf("aithsh:%s\n",s_data);
	x=sendDataToEndpoint(&server,s_data ,strlen(s_data)+1);
	if (x<0)
      	{
		//printf("edw egine mia malakitsa\n");
		printf("%s \n",strerror(errno));
        	return 1;
      	}
        s_data=malloc(BUFSIZE);
	//recvDataFromEndpoint(&server,s_data,BUFSIZE);
	//printf("server replied %s",s_data);
	x=BUFSIZE;


	switch(res)
	{
		case RETR_CHOICE :
		{
                        j=0;
			fd1=fopen(argv[3],"w");
			x=recvDataFromEndpoint(&server,s_data,strlen("200 command okay")+strlen(argv[2])+strlen(argv[3])+3);
			printf("received %d bytes %s-------\n",x,s_data);
			if(x== strlen("200 command okay")+strlen(argv[2])+strlen(argv[3])+3)
			{
				do
				{
					printf("waiting\n");
                                        //if(server.type==TCPEndpoint)
                                        x=recvDataFromEndpoint(&server,f_data,BUFSIZE);
                                        sum=sum+x;
                                        printf("received inside do %d bytes %s\n",x,f_data);
					fwrite(f_data,1,x,fd1);
					//printf("x=%d\n",x);
				}while(x == BUFSIZE);
				x=fclose(fd1);
                                if(x==EOF)
                                {
                                        fprintf(stderr,errno);
                                        break;
                                }
                                
			}
			break;
		}
		case STORE_CHOICE :
		{
                    j=0;
			printf("file requested to store %s \n", argv[3]);
			fd1=fopen(argv[3],"r");
			if(fd1!=NULL)				
			{
				do
				{       
                                        j++;
                                        memset(s_data,' ',BUFSIZE);
                                        i=fread(s_data,1,BUFSIZE,fd1);
                                        //printf("%s",s_data);
                                        
                                        if(ferror(fd1))
                                            fprintf(stderr,"error oqured while reading file %s",argv[3]);
                                        if(feof(fd1))
                                            printf("reached end of file\n");
                                        printf(" j=%d read... i=%d ",j,i);
  					if(i>0)	
					{
                                                x=sendDataToEndpoint(&server,s_data ,BUFSIZE);
                                                printf(" sending %d\n",x);
                                                sum=sum+x;
                                                //usleep(500);
                                        }
					if(i<0)
						fprintf(stderr,"read failed %s",strerror(errno));
                                        
                                }while(i>0);
				
                                printf("finished sent %d bytes\n",sum);
				x=fclose(fd1);
                                if(x==EOF)
                                {
                                        fprintf(stderr,errno);
                                        break;
                                }
                               
				break;
			}
			else
			{
				fprintf(stderr,"550 file not found %s",argv[3]);
				sprintf(msg,"550 file not found %s",argv[3]);
				sendDataToEndpoint(&server, msg, (strlen(msg)));
			}
			break;
		}
		default:
			recvDataFromEndpoint(&server,f_data,BUFSIZE);
                        fprintf(stderr,f_data);

		break;
	}


      	if (closeEndpoint(&server))
      	{
		
		printf("%s \n",strerror(errno));
        	return 1;
      	}
      	return 0;
}
