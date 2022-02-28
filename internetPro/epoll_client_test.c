//
// Created by lenovo on 2021/11/12.
//
#include "epoll_service_test.h"
#define MAX 1024
typedef struct message{
    int type;//消息类型
    char name[30];//自己的名字
    char dst_name[30];//想要聊天的对象
    char text[MAX];//消息内容
}MSG;
MSG msg1;//用于消息的发送
MSG msg;//全局变量，用于子线程接收消息
int i;//定义全局变量，进行功能选择
void message_handler(int cfd){
    int ret;
    while (1){
        bzero(&msg,sizeof(msg));
        ret=recv(cfd,&msg,sizeof(msg),0);
        if (ret==-1){
            perror("[recv]");
            break;
        }
        if (ret==0){
            printf("Service has disconnect!\n");
            break;
        }
        if (msg.type==1){
            printf("Service:%s\n",msg.text);

        }
        if (msg.type==2 || msg.type==3){//如果是群发消息，或者是私聊消息就进行接收
            printf("%s : %s\n",msg.name,msg.text);
        }
        if (msg.type==4){
            printf("%s",msg.text);
        }
    }
}

int connectfd_init(int port){
    int cfd;
    struct sockaddr_in service;
    bzero(&service,sizeof(service));
    service.sin_family=AF_INET;
    service.sin_port=htons(port);
    inet_pton(AF_INET,"192.168.200.134",&service.sin_addr.s_addr);

    if ((cfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("[socket]");
        return -1;
    }

    if (connect(cfd,(struct sockaddr *)&service,sizeof(service))==-1){
        perror("[connect]");
        return -1;
    }

    return cfd;
}

int main(int argc ,char *argv[]){
    printf("--------------------------------\n");
    printf("Welcome to jacky's chat room!\n");
    printf("\n");
    printf("Please register first!\n");
    printf("\n");
    printf("\n");

    int cfd;//连接套接字
    pthread_t tid;
    int ret;//用于检查返回值,i用于进行功能的选择
    cfd=connectfd_init(8888);
    if (cfd==-1){
        return -1;
    }

    //创建线程用于接收消息
    pthread_create(&tid,NULL,(void *)message_handler,cfd);
    pthread_detach(tid);


    //进行注册
    bzero(&msg1,sizeof(msg1));
    printf("What's your name?\n");
    fgets(msg1.name,sizeof(msg1.name),stdin);
    msg1.name[strlen(msg1.name)-1]=0;
    msg1.type=1;
    if (send(cfd,&msg1,sizeof(msg1),0)==-1){
        perror("[send]");
        return -1;
    }

    while (1){//对功能进行处理，子线程进行接收消息。

        //进行功能的选择，私聊还是群聊
        printf("\n");
        printf("1.私聊\n");
        printf("2.群聊\n");
        printf("3.打印在线信息\n");
        printf("4.退出系统\n");
        printf("\n");
        printf("Please enter your choice:");
        scanf("%d",&i);

        switch (i) {
            case 1:
                getchar();//用于吸收scanf的\n
                bzero(msg1.dst_name,sizeof(msg1.dst_name));//清空对象的名字

                printf("Who you want to chat?\n");
                fgets(msg1.dst_name,sizeof(msg1.dst_name),stdin);
                msg1.dst_name[strlen(msg1.dst_name)-1]=0;

                msg1.type=2;//更改消息类型,注册用户的名字就不用进行改了，一直保留

                while (1){//进行聊天
                    bzero(msg1.text,sizeof(msg1.text));//清空消息内容
                    printf("text:");
                    fgets(msg1.text,sizeof(msg1.text),stdin);
                    msg1.text[strlen(msg1.text)-1]=0;

                    //进行消息的退出判断
                    if (!strncmp(msg1.text,"quit",4)){
                        break;
                    }

                    //进行消息的发送
                    if (send(cfd,&msg1,sizeof(msg1),0)==-1){
                        perror("[send]");
                        printf("Disconnect! Please reconnect!\n");
                        break;
                    }

                }
                break;
            case 2://进行群聊
                getchar();
                bzero(msg1.dst_name,sizeof(msg1.dst_name));//清空
                msg1.type=3;//指定消息类型为群聊

                while (1){
                    bzero(msg1.text,sizeof(msg1.text));
                    printf("text:");
                    fgets(msg1.text,sizeof(msg1.text),stdin);
                    msg1.text[strlen(msg1.text)-1]=0;

                    //进行退出判断
                    if (!strncmp(msg1.text,"quit",4)){
                        break;
                    }
                    if (send(cfd,&msg1,sizeof(msg1),0)==-1){
                        perror("[send_2]");
                        printf("Disconnect! Please reconnect!\n");
                        break;
                    }
                }
                break;
            case 3:
                getchar();//打印在线信息
                msg1.type=4;
                if (send(cfd,&msg1,sizeof(msg1),0)==-1){
                    perror("[send_3]");
                    break;
                }
                break;
            case 4:
                getchar();//退出系统
                printf("Goodbye!\n");
                exit(0);
                break;
            default:
                getchar();
                printf("Sorry, there is an error! please select again.\n");
                break;
        }
    }
    return 0;
}
