#include "sys/socket.h"
#include "sys/types.h"
#include "unistd.h"
#include "netinet/in.h"
#include "assert.h"
#include "cstring"
#include "stdlib.h"
#include "processpool_cgi.h"
using namespace std;

#define SA struct sockaddr
#define BUFFER    1024


class cgi
{
	private:
		int cgi_sockfd,cgi_epollfd;
		//processpool<cgi>* cgi_propool=NULL;
	public:
		cgi(){}
		void cgi_init(int cgi_sfd,int cgi_epfd);
		void cgi_work();
		~cgi()
		{
			//close(listenfd);
			//delete cgi_propool;
			//cgi_propool=NULL;
		}
};
void cgi::cgi_init(int cgi_sfd,int cgi_epfd)
{
	cgi_sockfd=cgi_sfd;
	cgi_epollfd=cgi_epfd;
}
void cgi_create(int port)
{
	int listenfd;
	sockaddr_in servaddr;
	int reuse=1;
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	int retb=bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
	if(retb!=0)
	{
		perror("bind error");
		exit(0);
	}

	listen(listenfd,5);

	processpool<cgi>* cgi_propool=processpool<cgi>::create(listenfd);
	if(cgi_propool)
	{
		cgi_propool->run();
	        delete cgi_propool;
	}
}
void cgi::cgi_work()
{
	char r_buf[BUFFER];

	while(1)
	{
		int ret=recv(cgi_sockfd,r_buf,BUFFER-1,0);
		if(ret<0)
		{
			if(errno!=EAGAIN)
				removefd(cgi_epollfd,cgi_sockfd);
			break;
		}else if(ret==0)
		{
			removefd(cgi_epollfd,cgi_sockfd);
			break;
		}else
		{
			if(access(r_buf,F_OK)==-1)
			{
				cout<<"No such file exist:"<<endl;
				fputs(r_buf,stdout);
				bzero(r_buf,ret);
				break;
			}

			pid_t pid=fork();
			if(pid==0)
			{
				close(STDOUT_FILENO);
				dup(cgi_sockfd);
				execl(r_buf,NULL,NULL);
				exit(0);
			}
		}
	}
}
int main(int argc,char** argv)
{
	if(argc<2)
	{
		cout<<"Argument is not eonugh,try again."<<endl;
		return -1;
	}
	//cgi cgipro(atoi(argv[1]));
	//cgi::cgi_create();
//	processpool<cgi>* cgi_propool=processpool<cgi>::create(listenfd);
//	if(cgi_propool)
//	{
//		cgi_propool->run();
//	}
        cgi_create(atoi(argv[1]));

	return 0;
}
