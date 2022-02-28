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
	public:
		http_conn(){};
		~http_conn(){};
		void process();
};

#endif
