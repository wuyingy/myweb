#include "iostream"
#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"
#include "netinet/in.h"
#include "cstring"
#include "strings.h"
#include "stdio.h"
#include "stdlib.h"
#include "arpa/inet.h"
#include "assert.h"
#include "sys/epoll.h"
#include "fcntl.h"
using namespace std;

#define SA struct  sockaddr
#define MAX_EVENT_N 1024

void str_cli(int sockfd)
{
	int ret;
	char w_buf[1024],r_buf[1024];

	while(fgets(w_buf,1024,stdin))
	{
		//fgets(stdin,w_buf,1024);
		ret=send(sockfd,w_buf,strlen(w_buf),0);
		if(ret<0)
		{
			perror("Send error:");
			break;
		}
		bzero(w_buf,ret);
		ret=recv(sockfd,r_buf,1024,0);
		if(ret>0)
		{
			fputs(r_buf,stdout);
			bzero(r_buf,ret);
		}else if(ret<0)
		{
			perror("read error:");
			break;
		}else
		{
			cout<<"Server closed"<<endl;
			break;
		}
	}
}
int main(int argc,char** argv)
{
	if(argc<3)
	{
		cout<<"Argument is not enough."<<endl;
		return 0;
	}
	int sockfd;
	sockaddr_in servaddr,cliaddr;
	socklen_t clilen=sizeof(cliaddr),serlen=sizeof(servaddr);

	sockfd=socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,serlen);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

	int retc=connect(sockfd,(SA*)&servaddr,serlen);
	if(retc<0)
	{
		perror("Connect error.");
		return 0;
	}
	str_cli(sockfd);

	return 0;
}
