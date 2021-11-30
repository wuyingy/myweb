#include "iostream"
#include "sys/wait.h"
#include "assert.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "cstring"
#include "sys/socket.h"
#include "sys/types.h"
#include "sys/epoll.h"
#include "netinet/in.h"
#include "signal.h"
#include "arpa/inet.h"
#include "sys/stat.h"
#include "sys/ipc.h"
#include "sys/sem.h"
using namespace std;

#define MAX_EVENT_N 1024
#define SA struct sockaddr
#define MAX_BUFFER  1024

int semid;
union semval
{
	int val;
};
void pv(int semid,int op)
{
	struct sembuf sbuf;
	sbuf.sem_num=0;
	sbuf.sem_op=op;
	sbuf.sem_flg=SEM_UNDO;
	semop(semid,&sbuf,1);
}
void semFunc()
{
	semid=semget(IPC_PRIVATE,1,0666);
	assert(semid!=-1);
	union semval sem_val;
	sem_val.val=1;
	int ret=semctl(semid,0,SETVAL,sem_val);
	assert(ret!=-1);
}
int sig_pipefd[2];
class processchild
{
	public:
		processchild():pid(-1){}
		pid_t pid;
		int pipefd[2];
};
template<typename T>//define class to be module because need to use other variable or function int T
class processpool
{
	private:
		int max_sonpro_n;
		int m_listenfd;
		int m_son_id;
		int max_users;
		bool m_exit;
		int tast_child_n;
		T* users=NULL;
		processchild* son_pro_arr=NULL;
		static processpool<T>* m_processpool;
		processpool(int lis,int max_son=8,int max_user=50);
	public:
		static processpool<T>* create(int lis,int max_son=8,int max_user=50)
		{
			if(!m_processpool)
			{
				m_processpool=new processpool<T>(lis,max_son,max_user);
			}
			return m_processpool;
		}
		~processpool()
		{
			delete users;
			users=NULL;
			delete [] m_processpool;
			m_processpool=NULL;
		}

		void run();
		void run_child();
		void run_parent();
};
template<typename T>
processpool<T>* processpool<T>::m_processpool=NULL;

template<typename T>
processpool<T>::processpool(int lis,int max_son,int max_user):
m_listenfd(lis),max_sonpro_n(max_son),tast_child_n(0),m_son_id(-1),m_exit(true)
{
	processchild* son_pro_arr=new processchild[max_sonpro_n];
	assert((max_sonpro_n>0)&&son_pro_arr);

	int i;
	semFunc();
	for(i=0;i<max_sonpro_n;i++)
	{
		//create pipe
		int ret=socketpair(PF_LOCAL,SOCK_STREAM,0,son_pro_arr[i].pipefd);
		if(ret<0)
		{
			perror("Socketpair create pipe error");
			exit(0);
		}	
	        son_pro_arr[i].pid=fork();
	        if(son_pro_arr[i].pid>0)
		{
			close(son_pro_arr[i].pipefd[0]);
			continue;
		}else if(son_pro_arr[i].pid==0)
		{
			m_son_id=i;
			close(son_pro_arr[i].pipefd[1]);
			break;
		}
	}
}	
void setnonblock(int fd)
{
	int old_state=fcntl(fd,F_GETFL);
again:   int ret=fcntl(fd,F_SETFL,old_state|O_NONBLOCK);
	if(ret<0)
	{
		cout<<"Set nonblock faid,do you want try again:y/n"<<endl;
		string str;
		cin>>str;
		if(str=="Y"||str=="y")
		       goto again;
	}
}
void removefd(int epollfd,int fd)
{
	epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
	cout<<"remove "<<fd<<" from epoll event table."<<endl;
	close(fd);
}

void addfd(int fd,int epollfd)
{
	pv(semid,-1);
	cout<<"this process if "<<getpid()<<endl;
	epoll_event event;
	event.data.fd=fd;
	event.events=EPOLLIN;
	string et;
	cout<<"Do you want to set the fd to ET mode:y/n"<<endl;
	cin>>et;
	if(et=="Y"||et=="y")
		event.events|=EPOLLET;
	string shot;
	cout<<"Do you want to set the fd to EPOLLONESHOT:y/n"<<endl;
	cin>>shot;
	if(shot=="Y"||shot=="y")
		event.events|=EPOLLONESHOT;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
	string block;
	cout<<"Do you want to set this fd to nonblock:y/n"<<endl;
	cin>>block;
	if(block=="y"||block=="Y")
		setnonblock(fd);
	pv(semid,1);
}
void sig_handler(int sig)
{
	int save_errno=errno;
	int msg=sig;
	send(sig_pipefd[1],(char*)&msg,1,0);
	errno=save_errno;
}
void addsig(int sig,void(handler)(int))//the second argument is type of defining a function
{
	struct sigaction sa;
	memset(&sa,'\0',sizeof(sa));
	sa.sa_handler=handler;//1
	sa.sa_flags|=SA_RESTART;//2,重启被信号处理中断的程序
	sigfillset(&sa.sa_mask);//3,将所有信号加入信号集
	int ret=sigaction(sig,&sa,NULL);
	if(ret==-1)
	{
		perror("adddsig error");
		exit(0);
	}
}

void sig_pipefunc(int epollfd)
{
	//addsig(SIGCHLD,sig_handler);
	//addsig(SIGTERM,sig_handler);
	//addsig(SIGINT,sig_handler);
	//addsig(SIGKILL,sig_handler);
	//addsig(SIGPIPE,SIG_IGN);
	
again:  int ret=socketpair(AF_LOCAL,SOCK_STREAM,0,sig_pipefd);
	if(ret<0)
	{
		string two;
		cout<<"Sig pipe create faild,do you want to try agian:y/n"<<endl;
		cin>>two;
		if(two=="y"||two=="Y")
			goto again;
		else 
		{
			cout<<"sig pipe create fail,you will cannot recieve data correctly."<<endl;
			cout<<"Sure exit:y/n"<<endl;
			cin>>two;
			if(two=="n"||two=="N")
				goto again;
			else
			        exit(0);
		}
	}

	//setnonblock(sig_pipefd[1]);
	//统一事件源，I/O复用需要检测信号管道输出端
	addfd(epollfd,sig_pipefd[0]);//add to epoll table

	addsig(SIGCHLD,sig_handler);
	addsig(SIGTERM,sig_handler);
	addsig(SIGINT,sig_handler);
	//addsig(SIGKILL,sig_handler);
	addsig(SIGPIPE,SIG_IGN);
}
template<typename T>
void processpool<T>::run()
{
	if(m_son_id)
	{
		run_child();
	}
	run_parent();
}

template<typename T>
void processpool<T>::run_child()
{
	//child process work:
	//1:signal processing
	//2:accept data and deal with
	//3:accept listenfd

	users=new T[max_users];
	assert(users);

	epoll_event eventArr[MAX_EVENT_N];
	int epollfd=epoll_create(5);
	assert(epollfd);
	sig_pipefunc(epollfd);
	addfd(epollfd,son_pro_arr[m_son_id].pipefd[0]);

	for(;;)
	{
		int readyfd=epoll_wait(epollfd,eventArr,MAX_EVENT_N,-1);
		assert(readyfd);
		int i;
		for(i=0;i<readyfd;i++)
		{
			int sockfd=eventArr[i].data.fd;
			if((sockfd==sig_pipefd[0])&&(eventArr[i].events&EPOLLIN))//deal data
			{
				char sigArr[MAX_BUFFER];
				while(1)
				{
					int ret=recv(sockfd,sigArr,MAX_BUFFER-1,0);
					assert(ret!=-1);
					int i=0;
					for(i;i<ret;i++)
					{
						switch(sigArr[i])
						{
							case SIGCHLD: 
							{
								int state;
								pid_t pid;
								while(pid=waitpid(-1,&state,WNOHANG)>0)
								{
									//Getting in is successful,continue
								        cout<<"pid="<<pid<<" process terminate."<<endl;
									continue;
								}
							}
							case SIGTERM:
							{
								cout<<"parent process need to terminate this process,do you want to kill it:y/n"<<endl;
								string k;
								cin>>k;
								if(k=="y"||k=="Y")
									exit(0);
							}
							//case SIGINT:  cout<<"ctrl+c"<<endl;
						        case SIGKILL: cout<<"SIGKILL"<<endl; 
						}
					}
				}
			}else if((sockfd==son_pro_arr[m_son_id].pipefd[0])&&(eventArr[i].events&EPOLLIN))//common
			{
				char r_buf[MAX_BUFFER];

				while(1)
				{
					int ret=recv(sockfd,r_buf,MAX_BUFFER-1,0);
					if(ret<0)
					{
						if(errno==EAGAIN)
							continue;
						break;
					}else if(ret==0)
					{
						cout<<"Client close."<<endl;
						removefd(epollfd,sockfd);
					}else 
					{
						//judge whether to accept
						string str_accept="accept";
						string str(r_buf);
						if(str==str_accept)
						{
							sockaddr_in cliaddr;
							socklen_t clilen=sizeof(cliaddr);
							int connfd=accept(m_listenfd,(SA*)&cliaddr,&clilen);
							assert(connfd);
							//accept successfully,then add to epoll table
							addfd(epollfd,connfd);
							users[sockfd].cgi_init(sockfd,epollfd);
							cout<<"accept successfully."<<endl;
						        bzero(r_buf,ret);
						}
					}
				}
			}else if(eventArr[i].events&EPOLLIN)//common
			{
				char r_buf[MAX_BUFFER];

				while(1)
				{
					int ret=recv(sockfd,r_buf,MAX_BUFFER-1,0);
					if(ret<0)
					{
						if(errno==EAGAIN)
							continue;
						break;
					}else if(ret==0)
					{
						cout<<"Client close."<<endl;
						users[sockfd]=users[max_users];
						removefd(epollfd,sockfd);
					}else 
					{
						//fputs(r_buf,stdout);
						users[sockfd].cgi_work();
						bzero(r_buf,ret);
					}
				}
			}
		}
	}
	close(epollfd);
	close(sig_pipefd[0]);
	close(son_pro_arr[m_son_id].pipefd[0]);
}

template<typename T>
void processpool<T>::run_parent()
{
	epoll_event eventArr[MAX_EVENT_N];
	int epollfd=epoll_create(5);
	assert(epollfd);
	addfd(epollfd,m_listenfd);
	sig_pipefunc(epollfd);
	//addfd(epollfd,son_pro_arr[1]);//because parent process just use this pipefd to send,
	//so no neeod to add to event table of epoll

	while(m_exit)
	{
		int readyn=epoll_wait(epollfd,eventArr,MAX_EVENT_N,-1);
		assert(readyn>0);
		int i=0;
		for(i;i<readyn;i++)
		{
			int sockfd=eventArr[i].data.fd;
			if((sockfd==m_listenfd)&&(eventArr[i].events&EPOLLIN))
			{
				//judge whether there was still used child process left
				if((tast_child_n<=max_sonpro_n)&&(son_pro_arr[tast_child_n].pid!=-1))
				{
					send(son_pro_arr[tast_child_n].pipefd[1],"accept",7,0);
					printf("Using one child process to accept.");
					tast_child_n++;
				}else if(tast_child_n>max_sonpro_n)
					tast_child_n=0;
				else 
					tast_child_n++;
			}else if((sockfd==sig_pipefd[0])&&(eventArr[i].events&EPOLLIN))
			{
				char sign[MAX_BUFFER];

				while(1)
				{
					int ret=recv(sockfd,sign,MAX_BUFFER-1,0);
					assert(ret>0);
					int j=0;
					for(;j<ret;j++)
					{
					        switch(sign[j])
						{
							case SIGCHLD:
						        {
								pid_t pid;
								int state;
								while((pid=waitpid(-1,&state,WNOHANG))>0)
								{
									cout<<"pid="<<pid<<" terminate."<<endl;
									//restore this process data
									int a=0;
									for(;a<max_sonpro_n;a++)
									{
										if(pid==son_pro_arr[a].pid)
										{
											son_pro_arr[a].pid=-1;
											close(son_pro_arr[a].pipefd[1]);
											//another pipefd has close befor
										}
									}
									continue;
								}

								int ii;
								m_exit=false;
								for(ii=0;ii<max_sonpro_n;ii++)
								{
									if(son_pro_arr[ii].pid!=-1)
									{
										m_exit=true;
										cout<<"All process over."<<endl;
									}
								}
								
							}
							case SIGINT:
							{
								cout<<"User press 'ctrl+c' to terminate"<<endl;
								int c=0;
								for(c;c<max_sonpro_n;c++)
								{
									pid_t pid=son_pro_arr[c].pid;
									kill(pid,SIGTERM);
								}
							}
							default: continue;
						}
					}
				}
			}
		}
	}
	close(epollfd);
}
