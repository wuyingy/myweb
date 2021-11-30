#include "iostream"
#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/sem.h"
#include "sys/wait.h"
#include "unistd.h"
using namespace std;

union semun
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
int main(int argc,char** argv)
{
	//create sem use IPC_PRIVATE
	int sem_id=semget(IPC_PRIVATE,1,0666);
	union semun sem_un;
	sem_un.val=1;
	semctl(sem_id,0,SETVAL,sem_un);

	pid_t pid=fork();

	if(pid<0)
	{
		perror("fork error.");
		return -1;
	}else if(pid==0)
	{
		pv(sem_id,-1);
		cout<<"Child get sem."<<endl;
		sleep(5);
		pv(sem_id,1);
		exit(0);
	}else if(pid>0)
	{
		pv(sem_id,-1);
		cout<<"Father thread get."<<endl;
		sleep(5);
		pv(sem_id,1);
	}
	waitpid(pid,NULL,0);
	semctl(sem_id,0,IPC_RMID,sem_un);

	return 0;
}
