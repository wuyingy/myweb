#include "stdio.h"

int main(int argc,char** argv)
{
	//int sockfd=atoi(argv[0]);
	char s_buf[]="CONGRARULATION CGI SUCCESSFULLY";
	//send(sockfd,s_buf,sizeof(s_buf),0);
	puts(s_buf);
	return 0;
}
