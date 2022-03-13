#ifndef HTTP_WEB_H
#define HTTP_WEB_H
#include "iostream"
#include "sys/socket.h"
#include "sys/types.h"
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "strings.h"
#include <assert.h>
#include "pthread.h"
#include "fcntl.h"
#include "signal.h"

#include "restrict.h"

using namespace std;
using namespace RESTRICT;

class http_conn
{
	private:
		int m_sockfd;
		struct sockaddr_in m_address;
	public:
		static int users_count;
		static int m_epollfd;
		static const int READ_BUFFER_SIZE=2048;
		static const int WRITE_BUFFER_SIZE=1024;
		int m_read_idx;
		int once_read;
		char read_array[2048];
		char write_array[1024];
		//状态机
		//HTTP请求结果状态机
		enum HTTP_RESULT{
		    NO_REQUEST,//没有客户数据在缓冲区中，或者数据不完整
		    GET_REQUEST,//得到完整数据
		    BAD_REQUEST,//请求数据有语法错误
		    FORBIDDEN_REQUEST,//客户对资源没有足够的访问权限
		    INTERNAL_ERROR,//服务器内部错误
		    CLIENT_CLOSED //客户端已经关闭连接
		};
		//主状态机
		enum PARSE_STATE{
		    PARSE_REQUEASTLINE,//解析请求行
		    PARSE_HEADER,//解析消息头
		    PARSE_CONTENT//解析消息体
		};
		//HTTP请求方法
		enum METHED{
		    GET,
		    POST,
		    HEAD,
		    PUT,
		    DELETE,
		    TRACE,
		    OPTIONS,
		    CONNECT,
		    PATCH
		};
		//行的读取状态
		enum LINE_STATE{
		    LINE_OK,//READ SUCCESS
		    LINE_BAD,//行的语法有错
		    LINE_LACK//行数据还不完整
		};
	public:
		http_conn(){};
		~http_conn(){};
		void init(int fd,const struct sockaddr_in& cliaddr);
		void init();
		bool read();
		bool write();
		void close_conn(bool read_close=true);
		void process();
		HTTP_RESULT process_read();
		bool process_write(HTTP_RESULT ret);
};

#endif
