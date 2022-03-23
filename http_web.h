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
#define FILENAME_LEN 200

using namespace std;
using namespace RESTRICT;

class http_conn
{
	private:
		int m_sockfd;
		struct sockaddr_in m_address;
		int http_content_len;
		static const int WRITE_BUFFE_SIZE=1024;
		char* m_host;
		//whether need to keep connection
		bool m_longer;
		struct iovec m_iov[2];
		int iov_count;

	public:
		static int users_count;
		static int m_epollfd;
		static const int READ_BUFFER_SIZE=2048;
		static const int WRITE_BUFFER_SIZE=1024;
		int m_read_idx;
		int m_write_idx;
		int once_read;
		char read_array[2048];
		char write_array[1024];
		int line_parse_state;
		int start_line_idx;
		int now_array_idx;
		char* m_url=NULL;
		char* m_version=NULL;
		char file_buffer[FILENAME_LEN];
		struct stat m_file_stat;
		char* m_file_add;

		//状态机
		//HTTP请求结果状态机
		enum HTTP_RESULT{
		    NO_REQUEST,//没有客户数据在缓冲区中，或者数据不完整
		    GET_REQUEST,//得到完整数据
		    BAD_REQUEST,//请求数据有语法错误
		    FORBIDDEN_REQUEST,//客户对资源没有足够的访问权限
		    INTERNAL_ERROR,//服务器内部错误
		    CLIENT_CLOSED, //客户端已经关闭连接
	            NO_RESOURCE,//文件不在本服务器
		    DO_REQUEST_SUCCESS //HTTP解析成功，接下来进行写操作
		};
		//主状态机
		enum LINE_PARSE_STATE{
		    PARSE_REQUESTLINE,//解析请求行
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

		METHED m_methed;
	public:
		http_conn(){};
		~http_conn(){};
		void init(int fd,const struct sockaddr_in& cliaddr);
		void init();
		bool read();
		bool write();
		void close_conn(bool read_close=true);
		void process();
		char* once_start_line();
		LINE_STATE read_line();
		HTTP_RESULT parse_requestline(char* text);
		HTTP_RESULT parse_header(char* text);
		HTTP_RESULT parse_content(char* text);
		void unmap();
		HTTP_RESULT do_request();
		HTTP_RESULT process_read();

		bool add_respone_line(const char* format,...);
		bool add_status_line(int status,const char* c_phrase);
		bool add_header(int content_len);
		bool add_content_len(int content);
		bool add_longer();
		bool add_blankLine();
		bool add_content(const char* content);
		bool process_write(HTTP_RESULT ret);
};

#endif
