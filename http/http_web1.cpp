#include "iostream"
#include "cstring"
#include "string.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include <unistd.h>
#include "stdarg.h"
#include "sys/uio.h"
#include "http_web.h"

using namespace std;

int http_conn:: users_count=0;
int http_conn:: m_epollfd=-1;
static const char* root_dir="/home/wujinyou";

const char* ok_200_title="OK";
const char* error_400_title="Bad Request";
const char* error_400_form="Your request has bad syntax or is inherently impossible to satisfy.\n";
const char* error_403_title="Forbidden";
const char* error_403_form="You do not have permission to get this file from this server.\n";
const char* error_404_title="Not Found";
const char* error_404_form="The requested file was not found on this server.\n";
const char* error_500_title="Internal Error";
const char* error_500_form="There was an unusual problem serving the requestted file.\n";
const char* file_blank="<html><body></body></html>";

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
	m_write_idx=0;
	line_parse_state=PARSE_REQUESTLINE;
	start_line_idx=0;
	http_content_len=0;
	m_host=NULL;
	m_longer=false;
	m_methed=GET;
	m_file_add=NULL;

	memset(read_array,'\0',READ_BUFFER_SIZE);
	memset(write_array,'\0',WRITE_BUFFER_SIZE);
	memset(file_buffer,'\0',FILENAME_LEN);
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
				break;//??????????????????????????????????????????????????????????????????
			return false;
		}
		else if(once_read==0)
			return false;
		printf("Get 1 http request:\n%s\n",&read_array[m_read_idx]);
		m_read_idx+=once_read;
	}
	return true;
}
char* http_conn::once_start_line()
{
	return read_array+start_line_idx;
}
http_conn::LINE_STATE http_conn::read_line()
{
	char temp;
	for(;now_array_idx<m_read_idx;now_array_idx++)
	{
		temp=read_array[now_array_idx];
		if(temp=='\r')
		{
			if((now_array_idx+1)==m_read_idx)
				return LINE_LACK;
			if(read_array[now_array_idx+1]=='\n')
			{
				read_array[now_array_idx++]='\0';
				read_array[now_array_idx++]='\0';
				return LINE_OK;
			}
			else return LINE_BAD;
		}
		else if(temp='\n')
		{
			if(read_array[now_array_idx-1]=='\r')
			{
				read_array[now_array_idx-1]='\0';
				read_array[now_array_idx++]='\0';
				return LINE_OK;
			}
			else
				return LINE_BAD;
		}
	}
	return LINE_LACK;
}
http_conn::HTTP_RESULT http_conn::parse_requestline(char* text)
{
	HTTP_RESULT l_http_ret=NO_REQUEST;
	m_url=strpbrk(text," \t");
	if(m_url==NULL)
		return BAD_REQUEST;
	*m_url++='\0';
	if(strcasecmp(text,"GET")==0)
		m_methed=GET;
	else return BAD_REQUEST;

	//move to the first letter of URL
	m_url+=strspn(m_url," \t");
	m_version=strpbrk(m_url," \t");
	*m_version++='\0';

	if(strncasecmp(m_url,"http://",7)==0)
	{
		m_url+=7;
		m_url=strchr(m_url,'/');
	}
	if(!m_url||m_url[0]!='/')
		return BAD_REQUEST;

	//move to the first letter of version
	m_version+=strspn(m_version," \t");
	if(strcasecmp(m_version,"http/1.1")!=0)
		return BAD_REQUEST;

	line_parse_state=PARSE_HEADER;

	return l_http_ret;
}
http_conn::HTTP_RESULT http_conn::parse_header(char* text)
{
	HTTP_RESULT h_http_ret=NO_REQUEST;

	//if the read line is "\r\n"
	if(text[0]=='\0')
	{
		if(http_content_len==0)
			return GET_REQUEST;
		line_parse_state=PARSE_CONTENT;
		return NO_REQUEST;
	}
	//parse the requesting header column
	if(strncasecmp(text,"host:",5)==0)
	{
		text+=5;
		text+=strspn(text," \t");
		m_host=text;
	}else if(strncasecmp(text,"Connection:",11)==0)
	{
		text+=11;
		text+=strspn(text," \t");
		if(strcasecmp(text,"keep-alive")==0)
			m_longer=true;
	}else if(strncasecmp(text,"Content-Length:",15)==0)
	{
		text+=15;
		text+=strspn(text," \t");
		http_content_len=atol(text);
	}else 
		printf("Other header column:\n%s\n",text);

	return h_http_ret;
}
http_conn::HTTP_RESULT http_conn::parse_content(char* text)
{
	HTTP_RESULT c_http_ret=NO_REQUEST;
	//just to judge whether content read completely
	if(m_read_idx>=(now_array_idx+http_content_len))
	{
		text[http_content_len]='\0';
		return GET_REQUEST;
	}

	return c_http_ret;
}

http_conn::HTTP_RESULT http_conn::do_request()
{
	char* str=strcpy(file_buffer,root_dir);
	assert(str!=0);
	int len=strlen(root_dir);
	str=strncpy(file_buffer+len,m_url,FILENAME_LEN-len-1);
	assert(str!=NULL);

	if(stat(file_buffer,&m_file_stat)<0)
		return NO_RESOURCE;
	if(!m_file_stat.st_mode&S_IROTH)
		return FORBIDDEN_REQUEST;
	if(S_ISDIR(m_file_stat.st_mode))
		return BAD_REQUEST;

	int fd=open(file_buffer,O_RDONLY);
	m_file_add=(char*)mmap(0,m_file_stat.st_size,PROT_READ,MAP_PRIVATE,fd,0);//6
	assert(m_file_add!=NULL);
	close(fd);

	return DO_REQUEST_SUCCESS;
}
void http_conn::unmap()
{
	if(m_file_add)
	{
		munmap(m_file_add,m_file_stat.st_size);
		m_file_add=0;
	}
}
http_conn::HTTP_RESULT http_conn::process_read()
{
	LINE_STATE line_state;
	HTTP_RESULT http_ret=NO_REQUEST;
	//line_state=read_line();
	//read_line success
	char* text=NULL;
	while((line_parse_state==PARSE_CONTENT&&line_state==LINE_OK)||(line_state=read_line())==LINE_OK)
	{
		//?????????????????????????????????SC??????????????????????????????????????????????????????????????????
		text=once_start_line();
		start_line_idx=now_array_idx;

		switch(line_parse_state)
		{
                    case PARSE_REQUESTLINE:
		    {
			    http_ret=parse_requestline(text);
			    if(http_ret==BAD_REQUEST)
				    return http_ret;
			    break;
		    }
		    case PARSE_HEADER:
		    {
			    http_ret=parse_header(text);
			    if(http_ret==BAD_REQUEST)
				    return http_ret;
			    else  if(http_ret=GET_REQUEST)
				    return do_request();
			    break;
		    }
		    case PARSE_CONTENT:
		    {
			    http_ret=parse_content(text);
			    if(http_ret==BAD_REQUEST)
				    return http_ret;
			    if(http_ret==GET_REQUEST)
				    return do_request();
			    //??????content?????????????????????
			    if(http_ret==NO_REQUEST)
			    {
				    line_state=LINE_LACK;
			    }	    
		            break;
		    }
		    default: return INTERNAL_ERROR;
		}
	}

	//read_line failure
	return http_ret;
}
//write part
bool http_conn::http_conn::write()
{
	int temp=0;
	int bytes_have_send=0;
	int bytes_to_send=m_write_idx;
	if(bytes_to_send==0)
	{
		modfd(m_epollfd,m_sockfd,EPOLLIN);
		init();
		return true;
	}

	while(1)
	{
		temp=writev(m_sockfd,m_iov,iov_count);
		if(temp<0)
		{
			if(errno==EAGAIN)
			{
				modfd(m_epollfd,m_sockfd,EPOLLOUT);
				return true;
			}
			unmap();
			return false;
		}

		bytes_to_send-=temp;
		bytes_have_send+=temp;
		if(bytes_to_send<=bytes_have_send)
		{
			unmap();
			if(m_longer)
			{
				init();
				modfd(m_epollfd,m_sockfd,EPOLLOUT);
				return true;
			}
			else
			{
				modfd(m_epollfd,m_sockfd,EPOLLOUT);
				return false;
			}
		}
	}
}
bool http_conn::add_content_len(int len)
{
	return add_respone_line("Content_Length: %d\r\n",len);
}
bool http_conn::add_longer()
{
	return add_respone_line("Connection: %s\r\n",(m_longer==true)? "Keep-alive":"close");
}
bool http_conn::add_blankLine()
{
	return add_respone_line("%s","\r\n");
}
bool http_conn::add_respone_line(const char* format,...)
{
	if(m_write_idx>=WRITE_BUFFER_SIZE)
		return false;

	va_list ap;
	va_start(ap,format);
	int len=vsnprintf(write_array+m_write_idx,WRITE_BUFFER_SIZE-m_write_idx-1,format,ap);
	if(len>(WRITE_BUFFER_SIZE-m_write_idx-1))
	{
		return false;
	}

	m_write_idx+=len;
	va_end(ap);

	return true;
}
bool http_conn::add_status_line(int status,const char* c_phrase)
{
	return add_respone_line("%s %d %s\r\n","HTTP/1.1",status,c_phrase);
}
bool http_conn::add_header(int content)
{
	bool ret;
	ret=add_content_len(content);
	if(ret==false)
		return ret;
	ret=add_longer();
	if(ret==false)
		return ret;
	ret=add_blankLine();
	if(ret==false)
		return ret;
	return true;
}
bool http_conn::add_content(const char* content)
{
	bool ret=add_respone_line("%s",content);
	return ret;
}
bool http_conn::process_write(HTTP_RESULT ret)
{
	bool add_ret;
	switch(ret)
	{
                // BAD_REQUEST,//???????????????????????????
		case BAD_REQUEST:
		{
			add_ret=add_status_line(400,error_400_title);
			if(!add_ret) return add_ret;
			add_ret=add_header(strlen(error_400_form));
			if(!add_ret) return add_ret;
			add_ret=add_content(error_400_form);
			if(!add_ret) return add_ret;
			break;
		}
	        // FORBIDDEN_REQUEST,//??????????????????????????????????????????
		case FORBIDDEN_REQUEST:
		{
			add_ret=add_status_line(403,error_403_title);
			if(!add_ret) return add_ret;
			add_ret=add_header(strlen(error_403_form));
			if(!add_ret) return add_ret;
			add_ret=add_content(error_403_form);
			if(!add_ret) return add_ret;
			break;
		}
	        // INTERNAL_ERROR,//?????????????????????
		case INTERNAL_ERROR:
		{
			add_ret=add_status_line(500,error_500_title);
			if(!add_ret) return add_ret;
			add_ret=add_header(strlen(error_500_form));
			if(!add_ret) return add_ret;
			add_ret=add_content(error_500_form);
			if(!add_ret) return add_ret;
			break;
		}
	        // NO_RESOURCE,//????????????????????????
		case NO_RESOURCE:
		{
			add_ret=add_status_line(404,error_404_title);
			if(!add_ret) return add_ret;
			add_ret=add_header(strlen(error_404_form));
			if(!add_ret) return add_ret;
			add_ret=add_content(error_404_form);
			if(!add_ret) return add_ret;
			break;
		}
	        // DO_REQUEST_SUCCESS //HTTP???????????????????????????????????????
		case DO_REQUEST_SUCCESS:
		{
			add_ret=add_status_line(200,ok_200_title);
			if(m_file_stat.st_size>0)
			{
				add_ret=add_header(m_file_stat.st_size);
				m_iov[0].iov_base=write_array;
				m_iov[0].iov_len=m_write_idx;
				m_iov[1].iov_base=m_file_add;
				m_iov[1].iov_len=m_file_stat.st_size;
				iov_count=2;
				return add_ret;
			}
			else
			{
				add_ret=add_header(strlen(file_blank));
			        if(!add_ret) return add_ret;
				add_ret=add_content(file_blank);
			        if(!add_ret) return add_ret;
			}
		}
		default: return false;
	}

	m_iov[0].iov_base=write_array;
	m_iov[0].iov_len=m_write_idx;
	iov_count=1;
	return add_ret;
}

void http_conn::process()
{
	//1:?????????????????????
	HTTP_RESULT read_ret=process_read();
	if(read_ret==NO_REQUEST)
	{
		modfd(m_epollfd,m_sockfd,EPOLLIN);
		return;
	}
	//2:?????????????????????????????????
	bool write_ret=process_write(read_ret);
	if(!write_ret)
	{
		close_conn();
		//return;
	}
	//??????write??????
	modfd(m_epollfd,m_sockfd,EPOLLOUT);
}
