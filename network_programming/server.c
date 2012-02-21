#include <errno.h>
#include "./My_net_lib.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>

extern int errno;

#define BUFSIZE 512
#define RETR_CHOICE 1
#define STORE_CHOICE 2




int Choose_Action(char* choice)
{
	int x=0;
	
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
        EndpointInfo server, client;
	char *buf=NULL;	
	char *tmp=NULL;
	ssize_t nread;
	int i,j,choice,x,sum=0;;
	char *file=NULL;
	FILE *fd1;

	char *msg=NULL;
	char *f_data=NULL;
	char *s_data=NULL;


		
	if(argc == 3 && strcmp(argv[2],"udp")==0)
		server.type=UDPEndpoint;
	if(argc == 3 && strcmp(argv[2],"tcp")==0)
	{
	    server.type=TCPEndpoint;
	    server.backlog = 5;
	}
        //printf("creating server endpoint ");
        if (createServerEndpoint("localhost", argv[1], &server))
        {
	    printf("%s \n",strerror(errno));
            return 1;
        }
        printf("completed successfully\n");
	buf=malloc(BUFSIZE);
	file=malloc(BUFSIZE);
	msg=malloc(100);
	f_data=malloc(100);
	s_data=malloc(100);
	tmp=malloc(BUFSIZE);
	while (1)
        {
            
        	if (getNextClientFromEndpoint(&server, &client))
		{
            		memset(buf, 0, sizeof(buf));
	    		printf("memset finished\n");
	    	}

	    	if ((nread = recvDataFromEndpoint(&client, buf, BUFSIZE)) < 0)
            	{
	       		printf("%s \n",strerror(errno));
               		return 1;
	    	}
                
           	printf("CLIENT SAYS: %s\n",buf);
		
		file=strstr(buf," ");
		file[0]='\0';
		file=&file[1];
		choice=Choose_Action(buf);
		
		switch(choice)
		{
			case RETR_CHOICE:
			{
                                sum=0;
                                j=0;
				fd1=fopen(file,"r");
				if(fd1!=NULL)				
				{
					fprintf(stderr,"200 command okay %s %s\n",buf,file);
					sprintf(msg,"200 command okay %s %s\n",buf,file);
					sendDataToEndpoint(&client, msg, (strlen(msg)));
					do
					{

                                                memset(tmp,' ', BUFSIZE);
						i=fread(tmp,1,BUFSIZE,fd1);
                                                sum=sum+i;
                                                printf("sending... i=%d\n",i);
  						if(i>0)	
						{
                                                        sendDataToEndpoint(&client,tmp,strlen(tmp));
    							printf("\nwrote %s\n",tmp);
						}
						if(i<0)
							fprintf(stderr,"read failed %s",strerror(errno));
					}while(i == BUFSIZE);
					printf("finished\n");
					fclose(fd1);
					break;
				}
				else
				{
					fprintf(stderr,"550 file not found %s",file);
					sprintf(msg,"550 file not found %s",file);
					sendDataToEndpoint(&client, msg, (strlen(msg)));
				}
				break;
			}
			case STORE_CHOICE:
			{
                                j=0;
                                sum=0;
				fd1=fopen(file,"w");
                                if(fd1==NULL)
                                {
                                    fprintf(stderr,"file=%s could not be opened\n",file,errno);
                                    break;
                                }
                                else
                                {
                                        fprintf(stderr,"200 command okay %s %s\n",buf,file);
                                	sprintf(msg,"200 command okay %s %s\n",buf,file);
                                	
                                	do
                                	{
                                                j++;
                                                memset(f_data,' ',BUFSIZE);
                                                x=recvDataFromEndpoint(&client,f_data,BUFSIZE);
                                                //printf("f_data=%s",f_data);
                                                sum=sum+x;
                                                fwrite(f_data,1,x,fd1);
                                                printf("\nj=%d  x=%d",j,x);
                                                
                                        }while(x>0);
                                        x=fclose(fd1);
                                        if(x==EOF)
                                        {
                                           fprintf(stderr,errno);
                                           break;
                                        }
                                        printf("\nfinished received %d bytes\n",sum);
                                }
				break;
			}
			default :
			fprintf(stderr,"502 Command Not Implemented\n");
			sprintf(msg,"502 Command Not Implemented\n");
			sendDataToEndpoint(&client, msg, (strlen(msg)));

			break;
		}
	}
        return 0;
}
