#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>

#include "threadpool.h"
#include "http_web.h"
using namespace std;
using namespace THREADPOOL;

int main(int argc,char** argv)
{
	if(argc<3)
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

	return 0;
}
