#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include "signal.h"
#include "threadpool.h"
#include "http_web.h"
using namespace std;
using namespace THREADPOOL;

#define MAX_FD 65535
#define SA struct sockaddr
#define MAX_EVENT_NUMBER 10000

void addsig(int sig,void(handler)(int))
{
	struct sigaction sa;
	bzero(&sa,sizeof(sa));

	sa.sa_handler=handler;
	sa.sa_flags|=SA_RESTART;//restarting don`t to worry process blocked 
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig,&sa,NULL)!=-1);
}
extern bool  addfd(int epollfd,int fd,bool judge);
void show_error(int fd,const char* info)
{
	printf("%s\n",info);
	send(fd,info,strlen(info),0);
	close(fd);
}
int main(int argc,char** argv)
{
	if(argc<2)
	{
		perror("The arguments are less,try again");
		return 0;
	}

	//create threadpool
	threadpool<http_conn>* pool=NULL;

	try
	{
		pool=new threadpool<http_conn>(8,10000);
	}
	catch(...)
	{
		cout<<"Create threadpool error."<<endl;
		return -1;
	}

	//create connection
	addsig(SIGPIPE,SIG_IGN);

	int listenfd;
	struct sockaddr_in servaddr;
	listenfd=socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[1]));
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	int retb=bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
	assert(retb==0);

	int retl=listen(listenfd,5);
	assert(retl==0);

	//create client objects
	http_conn* users=new http_conn[MAX_FD];
	assert(users);
	int user_count=0;

	//create epoll events
	epoll_event events[MAX_EVENT_NUMBER];
	int epollfd=epoll_create(5);
	assert(epollfd!=-1);
	//add listenfd to epoll event table
	bool reta=addfd(epollfd,listenfd,false);
	assert(reta);
	http_conn::m_epollfd=epollfd;

	while(1)
	{
		int number=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
		if((number<0)&&(errno!=EINTR))
		{
			printf("Epoll_wait failure\n");
			break;
		}
		//cout<<number<<endl;
		for(int i=0;i<number;i++)
		{
			int sockfd=events[i].data.fd;//fetch socket from readying events to user array
			if(sockfd==listenfd)
			{
				int connfd;
				struct sockaddr_in cliaddr;
				socklen_t clilen=sizeof(cliaddr);
				connfd=accept(listenfd,(SA*)&cliaddr,&clilen);
				if(connfd<0)
				{
					printf("Accept error.\n");
					continue;
				}
				//judge wether the connects is full:
				if(http_conn::users_count>=MAX_FD)
				{
					show_error(connfd,"Internal is buzy.");
					continue;
				}
				users[connfd].init(connfd,cliaddr);//the socket just used for the index of users
				//cout<<"Connect success."<<endl;
			}
			else if(events[i].events&EPOLLIN)
			{
				//cout<<"read----"<<endl;
				if(users[sockfd].read())
				{
					pool->append(users+sockfd);//如果数据读取成功，启动线程
				}
				else
					users[sockfd].close_conn();
			}
			else if(events[i].events&EPOLLOUT)
			{
				if(!users[sockfd].write())//false to close sockfd
					users[sockfd].close_conn();
			}
			else if(events[i].events&(EPOLLRDHUP|EPOLLHUP|EPOLLERR))
			{
				printf("Events have error.\n");
				users[sockfd].close_conn();
			}
			else
			{
				printf("Another event happened.\n");
			}
		}
	}

	close(epollfd);
	close(listenfd);
	delete []users;
	delete pool;

	return 0;
}
