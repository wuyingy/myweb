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

void setnonblock(int fd)
{
	int old_state=fcntl(fd,F_GETFL);
	int state=fcntl(fd,F_SETFL,old_state|O_NONBLOCK);
	if(state==-1)
	{
		perror("Setnonblock failure.");
		exit(0);
	}
}
void addfd(int epollfd,int fd)
{
	//create event array
	epoll_event event;
	event.data.fd=fd;
	event.events|=EPOLLIN;

	string 	ET;
	cout<<"Please select this fd mode:e(ET)/d(default)"<<endl;
	cin>>ET;
	if(ET.compare("e")==0)
	{
		event.events|=EPOLLET;
		cout<<"Set EPOLLET success."<<endl;
	}
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);

	string setb;
	cout<<"Do you want to set this fd nonblock:y/n"<<endl;
	cin>>setb;
	if(setb.compare("y")==0||setb.compare("Y")==0)
		setnonblock(fd);
}
void func_ET(struct epoll_event event,int epollfd,int fd,int listenfd)
{
	cout<<"func_ET"<<endl;
	int ret;
	char r_buf[1024];
	char* sendbuf="Server--ET get your message.\n";
	if(fd==listenfd)
	{
		int connfd;
		sockaddr_in cliaddr;
		socklen_t clilen=sizeof(cliaddr);
		connfd=accept(listenfd,(SA*)&cliaddr,&clilen);
		assert(connfd!=-1);
		addfd(epollfd,connfd);
		return;
	}
	else if(event.events&EPOLLIN)
	{
		while(1)
		{
			ret=recv(fd,r_buf,1023,0);
			if(ret>0)
			{
				fputs(r_buf,stdout);
				bzero(r_buf,ret);
				ret=send(fd,sendbuf,strlen(sendbuf),0);
			}else if(ret<0)
			{
				perror("read ut:");
				continue;
			}
			else if(ret==0)
			{
				cout<<"client closed."<<endl;
				close(fd);
				break;
			}
		}
	}
}
void func_LT(struct epoll_event event,int epollfd,int fd,int listenfd)
{
	//cout<<"func_LT"<<endl;
	int ret;
	char r_buf[1024];
	char* sendbuf="Server--LT get your message.\n";
	if(fd==listenfd)
	{
		int connfd;
		sockaddr_in cliaddr;
		socklen_t clilen=sizeof(cliaddr);
		connfd=accept(listenfd,(SA*)&cliaddr,&clilen);
		assert(connfd!=-1);
		addfd(epollfd,connfd);
		return;
	}
	else if(event.events&EPOLLIN)
	{
		while(1)
		{
			ret=recv(fd,r_buf,1023,0);
			if(ret>0)
			{
				fputs(r_buf,stdout);
				bzero(r_buf,ret);
				ret=send(fd,sendbuf,strlen(sendbuf),0);
			}else if(ret<0)
			{
				perror("read out:");
				continue;
			}
			else if(ret==0)
			{
				cout<<"client closed."<<endl;
				close(fd);
				break;
			}
		}
	}
}
int main(int argc,char** argv)
{
	if(argc<2)
	{
		cout<<"Argument is not enough."<<endl;
		return 0;
	}

	int listenfd;
	struct sockaddr_in servaddr;
	socklen_t serlen=sizeof(servaddr);
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	assert(listenfd!=-1);

	bzero(&servaddr,serlen);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[1]));
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	int retb=bind(listenfd,(SA*)&servaddr,serlen);
	if(retb<0)
	{
		perror("Bind error:");
		return 0;
	}

	int retl=listen(listenfd,5);
	assert(retl!=-1);
	//for epoll
	struct epoll_event arrEvent[MAX_EVENT_N];
	int epollfd=epoll_create(5);
	assert(epollfd!=-1);
	addfd(epollfd,listenfd);

	while(1)
	{
		//cout<<"come to wait"<<endl;
		int readyn=epoll_wait(epollfd,arrEvent,MAX_EVENT_N,-1);
		//cout<<"ready over"<<endl;
		if(readyn<0)
		{
			perror("Wait error.");
			break;
		}
		int i;
		for(i=0;i<readyn;i++)
		{
			//cout<<"enter for"<<endl;
			if(arrEvent[i].events&EPOLLET)
				func_ET(arrEvent[i],epollfd,arrEvent[i].data.fd,listenfd);
			else
				func_LT(arrEvent[i],epollfd,arrEvent[i].data.fd,listenfd);
		}
	}

	return 0;
}
