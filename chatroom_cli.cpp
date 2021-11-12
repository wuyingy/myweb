#include "iostream"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "unistd.h"
#include "cstring"
#include "string.h"
#include "arpa/inet.h"
#include "stdlib.h"
#include "fcntl.h"
#include "poll.h"
#include "assert.h"
using namespace std;

#define SA struct sockaddr
#define ARR 2

int main(int argc,char** argv)
{
	if(argc<3)
	{
		cout<<"Argument is not enough"<<endl;
		return -1;
	}
	int sockfd;
	sockaddr_in servaddr;
	socklen_t serlen=sizeof(servaddr);

	sockfd=socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,serlen);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

	int retc=connect(sockfd,(SA*)&servaddr,serlen);
	assert(retc!=-1);
	//poll
	pollfd parray[ARR];
	//init
	parray[0].fd=0;
	parray[0].events=POLLIN;
	parray[0].revents=0;

	parray[1].fd=sockfd;
	parray[1].events=POLLIN|POLLRDHUP;
	parray[1].revents=0;

	//create pipe
	int pipefd[2];
	int retp=pipe(pipefd);
	assert(retp!=-1);

	//while--poll
	int readyn;
	char r_buf[1024];
	while(1)
	{
		readyn=poll(parray,ARR,-1);
		if(readyn==-1)
		{
			perror("Poll error");
			continue;
		}
		if(parray[0].revents&POLLIN)
		{
			int rets=splice(0,NULL,pipefd[1],NULL,1023,SPLICE_F_MORE|SPLICE_F_MOVE);
			rets=splice(pipefd[0],NULL,sockfd,NULL,rets,SPLICE_F_MORE|SPLICE_F_MOVE);
		}
		else if(parray[1].revents&POLLIN)
		{
		        int retr=recv(sockfd,r_buf,1024,0);
		        if(retr>0)
			{
				fputs(r_buf,stdout);
				bzero(r_buf,retr);
			}	
		}else if(parray[1].revents&POLLRDHUP)
		{
			cout<<"Client closed."<<endl;
			close(sockfd);
			close(pipefd[1]);
			close(pipefd[0]);
		}
	}

	return 0;
}
