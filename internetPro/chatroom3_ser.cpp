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
#define BUFFER 1024
#define NFDS   100

struct client_data
{
	int fd=-1;
	//char w_buf[BUFFER]={'\0'};
	char* w_buf=NULL;
};

int maxusern=0;
int usern=0;
client_data* userArr=new client_data[NFDS+1];

void setnonblock(int fd)
{
again:
	int old_state=fcntl(fd,F_GETFL);
	int state=fcntl(fd,F_SETFL,old_state|O_NONBLOCK);
	if(state==-1)
		goto again;
}
void connFunc(int sockfd,pollfd *parr)
{
	sockaddr_in cliaddr;
	socklen_t clilen=sizeof(cliaddr);
	const char* p="Sorry,users are too many,cannot connect more,try a moment.";

	int connfd=accept(sockfd,(SA*)&cliaddr,&clilen);
	assert(connfd!=-1);

	if((usern+1)==NFDS)//keep not to init the last one
	{
		int rets=send(connfd,p,strlen(p),0);
		exit(0);
	}

	maxusern=(++usern);//maxusern keep the last one user
	userArr[connfd].fd=connfd;

	//set connfd to parr
	int a=1;
	for(;a<NFDS;a++)
	{
		if(parr[a].fd==-1)
		{
			parr[usern].fd=connfd;
			parr[usern].events=POLLIN|POLLERR|POLLRDHUP;
			parr[usern].revents=0;
		        break;
		}
	}
	cout<<"connect sucess."<<endl;
}

int main(int argc,char** argv)
{
	if(argc<2)
	{
		cout<<"Arguments is not enough for 2"<<endl;
		return -1;
	}
	int listenfd,connfd;
	sockaddr_in servaddr;
	socklen_t serlen=sizeof(servaddr);

	listenfd=socket(AF_INET,SOCK_STREAM,0);
	int reuse=1;
	int setb=setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
	assert(setb!=-1);

	bzero(&servaddr,serlen);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[1]));
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	int retb=bind(listenfd,(SA*)&servaddr,serlen);
	assert(retb!=-1);

	listen(listenfd,5);

	//poll
	pollfd parr[NFDS+1];
	//init parr
	int i=0;
	for(;i<NFDS+1;i++)
	{
		parr[i].fd=-1;
		parr[i].events=0;
		parr[i].revents=0;
	}

	parr[0].fd=listenfd;
	parr[0].events=POLLIN;
	parr[0].revents=0;


	//while--poll
	int readyn;
	char r_buf[BUFFER];
	while(1)
	{
		cout<<"poll"<<endl;
		readyn=poll(parr,maxusern+1,-1);
		assert(readyn!=-1);
		//cout<<"readyn="<<readyn<<endl;
		for(i=0;i<maxusern+1;i++)
		{
			int sockfd=parr[i].fd;
			if(sockfd==-1)
				continue;
			//cout<<sockfd<<endl;
			if((sockfd==listenfd)&&(parr[i].revents&POLLIN))
			{
				connFunc(listenfd,parr);
			}
			//sleep(5);
			if(i==0)
				continue;
			else if(parr[i].revents&POLLIN)
			{
				cout<<"read data"<<endl;
				//sleep(5);
				bzero(r_buf,BUFFER);
				int retr=recv(sockfd,r_buf,BUFFER-1,0);
				//fputs(r_buf,stdout);
				//sleep(5);
				if(retr>0)
				{
					int j=1;
					for(;j<maxusern+1;j++)
					{
						if((parr[j].fd==sockfd)||(parr[j].fd==-1))
							continue;
						userArr[parr[j].fd].w_buf=r_buf;
						parr[j].events&=~POLLIN;//|POLLOUT;
						parr[j].events|=POLLOUT;
						printf("%d copy data to %d\n",sockfd,parr[j].fd);
					}
					//bzero(r_buf,retr);
				}else if(retr==0)
				{
					cout<<"retr==0,Client closed."<<endl;
					usern--;
					userArr[sockfd]=userArr[NFDS];
					parr[i]=parr[NFDS];
					close(sockfd);
					continue;
				}else
				{
					cout<<"Read error."<<endl;
					usern--;
					userArr[sockfd]=userArr[NFDS];
					parr[i]=parr[NFDS];
					close(sockfd);
					continue;
				}
			}else if(parr[i].revents&POLLOUT)
			{
				//cout<<"come to send data."<<endl;
				//sleep(5);
				//int connfd=parr[i].fd;
				cout<<userArr[sockfd].w_buf;//<<connfd<<endl;
				//cout<<sockfd<<endl;
				if(!userArr[sockfd].w_buf)
					continue;
				int rets=send(sockfd,userArr[sockfd].w_buf,strlen(userArr[sockfd].w_buf),0);
				cout<<rets<<endl;
				parr[i].events &= ~POLLOUT;//|POLLIN;
				parr[i].events |= POLLIN;
				//bzero(userArr[sockfd].w_buf,rets);
				userArr[sockfd].w_buf=NULL;
			}else if(parr[i].revents&POLLERR)
			{
				//when get the error from fd,
				char errors[100];
				socklen_t len=sizeof(errors);
				if(getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&errors,&len))
				{
					cout<<"error:"<<errors<<endl;
					continue;
				}
				cout<<"Get sockfd error failed."<<endl;
			}else if(parr[i].revents&POLLRDHUP)
			{
				cout<<"get POLLRDHUP,Client closed."<<endl;
				usern--;
				userArr[sockfd]=userArr[NFDS];
				parr[i]=parr[NFDS];
				close(sockfd);
				continue;
			}
		}
	}

	close(listenfd);
	delete []userArr;
	return 0;
}
