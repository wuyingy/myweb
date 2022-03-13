#include "iostream"
#include "http_web.h"

using namespace std;

int http_conn:: users_count=0;
int http_conn:: m_epollfd=-1;

int setNonblocking(int fd)
{
	int old_status=fcntl(fd,F_GETFL);
	if(old_status==-1)
		return -1;
	int new_status=old_status|O_NONBLOCK;
	int status=fcntl(fd,F_SETFL,new_status);
	if(status==-1)
		return -1;
	return 0;
}

bool addfd(int epollfd,int fd,bool one_shot)
{
	epoll_event event;
	event.data.fd=fd;
	event.events=EPOLLIN|EPOLLET|EPOLLRDHUP;//EPOLLRDHUG--read opposition closed

	if(one_shot)
	{
		event.events|=EPOLLONESHOT;
	}

	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event)!=0)
		return false;
        int retset=setNonblocking(fd);
	if(retset==-1)
		return false;
	return true;
}
void modfd(int epollfd,int fd,int ev)
{
	epoll_event event;
	event.data.fd=fd;
	event.events=ev|EPOLLONESHOT|EPOLLRDHUP;
	epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);
}
void removefd(int mfd,int fd)
{
	epoll_ctl(mfd,EPOLL_CTL_DEL,fd,0);
	close(fd);
}
void http_conn::close_conn(bool real_close)
{
	if(real_close&&(m_sockfd!=-1))
	{
		removefd(m_epollfd,m_sockfd);
		m_sockfd=-1;
		users_count--;
	}
}
void http_conn::init(int fd,const struct sockaddr_in& cliaddr)
{
	users_count++;
        m_sockfd=fd;
	m_address=cliaddr;
	//time_wait set
	int reuse=1;
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
	addfd(m_epollfd,fd,true);
	init();
}
void http_conn::init()
{
	m_read_idx=0;
}

//read part
bool http_conn:: read()
{
	if(m_read_idx>=READ_BUFFER_SIZE)
		return false;
	int once_read=0;
	//oneshot---while
	while(1)
	{
		once_read=recv(m_sockfd,read_array+m_read_idx,READ_BUFFER_SIZE-m_read_idx,0);
		if(once_read<0)
		{
			if(errno==EAGAIN||errno==EWOULDBLOCK)
				break;//后期处理时没有读取到数据，进行再次注册再读取
			return false;
		}
		else if(once_read==0)
			return false;
		m_read_idx+=once_read;
	}
	return true;
}
http_conn::HTTP_RESULT http_conn::process_read()
{
}
//write part
bool http_conn::http_conn::write()
{

}
bool http_conn::process_write(HTTP_RESULT ret)
{
}

void http_conn::process()
{
	//1:读取和处理数据
	HTTP_RESULT read_ret=process_read();
	if(read_ret==NO_REQUEST)
	{
		modfd(m_epollfd,m_sockfd,EPOLLIN);
		return;
	}
	//2:对处理的结果判定和处理
	bool write_ret=process_write(read_ret);
	if(!write_ret)
	{
		close_conn();
		//return;
	}
	//注册write事件
	modfd(m_epollfd,m_sockfd,EPOLLOUT);
}
