#include "iostream"
#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"
#include "netinet/in.h"
#include "stdlib.h"
#include "arpa/inet.h"
#include "cstring"
#include "strings.h"
#include "fcntl.h"
#include "pthread.h"
#include "assert.h"
#include "sys/epoll.h"

using namespace std;

#define SA struct sockaddr
#define MAX_EVENT_N  1024
struct fd_epollfd
{
	int epollfd;
	int fd;
};

void setnonblock(int fd)
{
	int old_state=fcntl(fd,F_GETFL);
	int state=fcntl(fd,F_SETFL,old_state|O_NONBLOCK);
	if(state==-1)
	{
		perror("set nonblock error:");
	}
}
void addfd(int epollfd,int fd)
{
	epoll_event event;
	event.data.fd=fd;
	event.events|=EPOLLIN;
	string shot;
	cout<<"Do your want to set EPOLLONESHOT:y/n"<<endl;
	cin>>shot;
	if(shot=="y"||shot=="Y")
		event.events|=EPOLLONESHOT;
	cout<<"Do your want to set ET mode:y/n"<<endl;
	string et;
	cin>>et;
	if(et=="y"||et=="Y")
		event.events|=EPOLLET;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);

	cout<<"Do you need to set nonblock:y/n"<<endl;
	string block;
	cin>>block;
	if(block=="y"||block=="Y")
		setnonblock(fd);
}
void reset_shot(int epollfd,int fd)
{
	epoll_event event;
	event.data.fd=fd;
	event.events=EPOLLIN|EPOLLONESHOT;
	cout<<"Do your want to set ET mode:y/n"<<endl;
	string et;
	cin>>et;
	if(et=="y"||et=="Y")
		event.events|=EPOLLET;
	epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
//套接字依然是那个套接字，nonblock已经设置过了。	
}
void* getData(void* arg)
{
	int sockfd=((fd_epollfd*)arg)->fd;
	int ret;
	char r_buf[1024];

	while(1)
	{
		cout<<"read"<<endl;
		ret=read(sockfd,r_buf,1024);
		cout<<ret<<endl;
		if(ret>0)
		{
			fputs(r_buf,stdout);
			bzero(r_buf,ret);
		}else if(ret==0)
		{
			cout<<"client closed."<<endl;
			close(sockfd);
			break;
		}else if(ret<0)
		{
			cout<<"retset shot."<<endl;
			reset_shot(((fd_epollfd*)arg)->epollfd,sockfd);
			break;
		}
	}
}

int main(int argc,char** argv)
{
	if(argc<2)
	{
		cout<<"Argument is not enough."<<endl;
		return -1;
	}
	int listenfd,connfd;
	sockaddr_in servaddr,cliaddr;
	socklen_t clilen=sizeof(cliaddr),serlen=sizeof(servaddr);

	listenfd=socket(AF_INET,SOCK_STREAM,0);
	int reuse=1;
        setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

	bzero(&servaddr,serlen);
	servaddr.sin_family=PF_INET;
	servaddr.sin_port=htons(atoi(argv[1]));
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	int retb=bind(listenfd,(SA*)&servaddr,serlen);
	if(retb==-1)
	{
		perror("Bind error:");
		return -1;
	}

	listen(listenfd,5);

	//epoll
        epoll_event arrEvent[MAX_EVENT_N];
	int epollfd=epoll_create(3);
	assert(epollfd>0);

	addfd(epollfd,listenfd);

	//wait to do
	int readyn;
	int sockfd;
	while(1)
	{
		//cout<<"wait"<<endl;
		readyn=epoll_wait(epollfd,arrEvent,MAX_EVENT_N,-1);
		//cout<<"readyn="<<readyn<<endl;
		if(readyn==-1)
		{
			perror("epoll_wait error:");
			return -1;
		}
		int i=0;
		for(;i<readyn;i++)
		{
			sockfd=arrEvent[i].data.fd;
		//	cout<<sockfd<<endl;
			if(sockfd==listenfd)
			{
				connfd=accept(listenfd,(SA*)&cliaddr,&clilen);
				assert(connfd>0);
				addfd(epollfd,connfd);
			}else if(arrEvent[i].events&EPOLLIN)
			{
				fd_epollfd fds;
				fds.epollfd=epollfd;
				fds.fd=sockfd;
				pthread_t thread;
				//getData(sockfd);
				pthread_create(&thread,NULL,getData,(void*)&fds);
			}else
			{
				cout<<arrEvent[i].events<<endl;
				cout<<"unkown event occurred."<<endl;
				return -1;
			}
		}
	}

	return 0;
}
