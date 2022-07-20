//
// Created by kid on 5/21/22.
//

#include "Server.h"
#include <iostream>

//#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <thread>
#include <sys/epoll.h>
#include "functional"
#define  PORT  6001
#define  MAXLINES 2048
#include <arpa/inet.h>
#define SA sockaddr
#define LISTENN 40
#include <cstring>
#include <string>
#include <sys/ioctl.h>
#include<poll.h>
#include "streambuf"
using namespace std;

char cmdlines[10][100]={
        "exit",
        "read",
        "write"
};
//0 1 2
int Server::checkcmd() {
    char cmdx[1024];
    memset(cmdx,0,sizeof(cmdx));
    printf("\nCOMMAD INPUT: ");
    scanf("%s",cmdx);
    //printf("%s ",cmdx);
    for(int i=0;i<3;i++){
        if(strcmp(cmdx,cmdlines[i])==0){
            return i;
        }
    }
    return -1;
}

void Server::readclient(){
    while(1){
        memset(clibuf,0,sizeof(clibuf));
        recv(clientfd,clibuf,sizeof(clibuf),0);
        if(strcmp(clibuf,"exit")==0){
            printf("client %s\n",clibuf);
            return;
        }else{
            printf("%s\n",clibuf);
        }
    }
}

void Server::sendclient() {
    while(1){
        memset(servbuf,0,sizeof(servbuf));
        scanf("%s",servbuf);
        send(clientfd,servbuf,sizeof(servbuf),0);
        if(strcmp(servbuf,"exit")==0){
            printf("server %s\n",servbuf);
            return;
        }
    }

}

int Server::CommuClient2() {
    thread tr(bind(&Server::readclient,this));
    thread ts(bind(&Server::sendclient,this));
    tr.join();
    ts.detach();
    return 0;
}

int Server::CommuClient1() {
    while(1){
        int x=checkcmd();
        if(x==0){
            printf("EXIT\n");
            break;
        }else if(x==1){
            if(read(clientfd,clibuf,sizeof(clibuf))==0){
                printf("READ NOTHING\n");
            } else{
                printf("READ: Client write Server %s\n",clibuf);
            }

        }else if(x==2){
            printf("WRITE: ");
            scanf("%s",servbuf);
            write(clientfd,servbuf, sizeof(servbuf));
        }else if(x==-1){
            printf("COMMAND NOT FOUND\n");
        }

        //write(socket_client,"WRITE NOTHING", sizeof("WRITE NOTHING"));
    }
    return 0;
}



int Server::multip_connect() {
    pid_t pid;
    while(1){

        //调用fork()函数时  如果是父进程调用则返回进程号，如果是子进程调用则返回0
        //这里父进程用来创建连接  子进程负责处理请求  所以当pid=0才执行
        memset(&cliaddr,0,sizeof(cliaddr));
        //memset(servbuf,0,sizeof(servbuf));
        memset(clibuf,0,sizeof(clibuf));
        len=sizeof (cliaddr);
        clientfd= accept (serverfd, (SA *)&cliaddr, reinterpret_cast<socklen_t *>(&len));
        if(clientfd==-1){
            printf("Socket client accept error\n");
        }else{
            printf("Socket client accept OK\n");
        }
        if((pid=fork())==0){
            close(serverfd);
            printf("connection from %s, port %d\n",
                   inet_ntop(AF_INET,&cliaddr.sin_addr,servbuf,sizeof(servbuf)),
                   ntohs(cliaddr.sin_port));
            ticks= time(NULL);
            snprintf(servbuf,sizeof(servbuf),"%.24s\r\n", ctime(&ticks));
            write(clientfd,servbuf, sizeof(servbuf));
            CommuClient2();
            close(clientfd);
            exit(0);
        }

        //write(connfd,servbuf, sizeof(servbuf));
    }
    close(serverfd);
    return 0;
}
//thread
void *check_link(void *t){
    char buf[1024]="";

}

int Server::multit_connect() {
    pthread_t tid;
    while(1){
        //pthread_create(&tid, NULL, do_work, (void*)&ts[i]);
    }
    return 0;
}

int Server::try_connect() {
    len=sizeof (cliaddr);
    clientfd= accept (serverfd, (SA *)&cliaddr, reinterpret_cast<socklen_t *>(&len));
    if(clientfd==-1){
        printf("Socket client accept error\n");
    }else{
        printf("Socket client accept OK\n");
    }
    connect_print();
    //write(connfd,servbuf, sizeof(servbuf));
    //CommuClient1();
    while(1){
        read(clientfd,clibuf,sizeof(clibuf));
        cout<<"serving client on fd "<<clientfd<<" -->read get: "<<clibuf<<"\n";
    }
    return 0;
}

//select
int Server::select_connect(){

    int maxfd=FD_SETSIZE;
    fd_set readfd,testfd;
    FD_ZERO(&readfd);
    FD_SET(serverfd, &readfd);//将服务器端socket加入到集合中

   while(1) {
       memset(clibuf, 0, sizeof(clibuf));
       memset(servbuf, 0, sizeof(servbuf));

       testfd = readfd;

       printf("server waiting...\n");
       int sx = select(maxfd + 1, &testfd, NULL, NULL, NULL);
       if (sx < 0) {
           printf("select error\n");
           break;
       } else if (sx == 0) {
           printf("select timeout\n");
           break;
       } else {
           printf("select success\n");
       }
       int fd;//fd[1]=-1
       int nread;
       for (fd = 0; fd < FD_SETSIZE; fd++) {
           memset(servbuf, 0, sizeof(servbuf));
           memset(clibuf, 0, sizeof(clibuf));
           if (FD_ISSET(fd, &testfd)) {
               if (fd == serverfd) {
                   len = sizeof(cliaddr);
                   clientfd = accept(serverfd, (SA *) &cliaddr, reinterpret_cast<socklen_t *>(&len));
                   FD_SET(clientfd, &readfd);//将客户端socket加入到集合中
                   connect_print();
               }else{
                   ioctl(fd, FIONREAD, &nread);//取得数据量交给nread
                   //printf("%d\n",nread);
                   if (nread == 0) {
                       close(fd);
                       FD_CLR(fd, &readfd);
                       printf("removing client on fd %d\n", fd);
                   } else {
                       read(fd, clibuf, sizeof(clibuf));
                       printf("serving client on fd %d -->", fd);
                       cout << "read get";
                       printf("%s\n", clibuf);

                   }

               }
           }
       }
   }
   return 0;
}
//poll
# define MAXPOLL 2048
int Server::poll_connect(){
    pollfd fds[MAXPOLL+1];
    fds[0].fd=serverfd;
    fds[0].events = POLLRDNORM;    //设置listenfd监听普通读事件
    for(int i=1;i<MAXPOLL;i++)fds[i].fd=-1;
    int maxi=0;
    int timeout=-1;

    int fdx;

    while(1){
        int ret= poll(fds,maxi+1,timeout);

        if(ret==-1){
            printf("poll error\n");
            return ret;
        }else{
            printf("poll waiting...\n");
        }
        memset(servbuf, 0, sizeof(servbuf));
        memset(clibuf, 0, sizeof(clibuf));
        if(fds[0].revents&POLLRDNORM){
            len=sizeof(cliaddr);
            clientfd= accept(serverfd, (SA *) &cliaddr, reinterpret_cast<socklen_t *>(&len));
            connect_print();
            int t;
            for(t=1;t<=MAXPOLL;t++){
                if(t<MAXPOLL&&fds[t].fd<0){
                    fds[t].fd=clientfd;
                    break;
                }
            }
            if(t==MAXPOLL){
                printf("POLL Client FULL\n");
                continue;
            }
            fds[t].events = POLLRDNORM;
            if(t>maxi)maxi=t;
            if(ret==0){
                printf("poll nothing\n");
                continue;
            }
        }
        for(int i=1;i<=maxi;i++){
            if((fdx=fds[i].fd)<0){
              continue;
            }
            if(fds[i].revents&(POLLRDNORM|POLLERR)){
                int n=read(fdx,clibuf,sizeof(clibuf));
                if(n<0){
                    if(errno=ECONNRESET){
                        printf("fds[%d] aborted connection\n", i);
                        close(fdx);
                        fds[i].fd=-1;
                    }else{
                        printf("read error\n");
                    }
                }else if(n==0){
                    printf("fds[%d]=%d removed from connection\n",i,fdx);
                    close(fdx);
                    fds[i].fd=-1;
                    //break;
                }else{
                    printf("from client fd=%d read: %s\n",fdx,clibuf);
                    //cout<<"send client: ";
                    //scanf("%s",servbuf);
                    //write(fdx,servbuf,sizeof(servbuf));
                }
            }
        }
    }
    return 0;
}

//epoll
#define MAXEPOLL 2048
int Server::epoll_connect(){
    int fds[MAXEPOLL+1];
    epoll_event tep,ep[MAXEPOLL];
    for(int i=0;i<MAXEPOLL;i++)fds[i]=-1;
    int maxi=-1;
    int efd= epoll_create(MAXEPOLL);
    if(efd==-1){
        cout<<"epoll create error.\n";
        return efd;
    }
    tep.events=EPOLLIN;
    tep.data.fd=serverfd;
    //为服务器进程注册epoll事件
    int ret= epoll_ctl(efd,EPOLL_CTL_ADD,serverfd,&tep);
    if(ret==-1){
        cout<<"EPOLL_CTL error\n";
        return ret;
    }
    while(1){
        int red= epoll_wait(efd,ep,MAXEPOLL,-1);
        if(red==-1){
            cout<<"epoll_wait error\n";
            return red;
        }
        for(int i=0;i<red;i++){
            if(!(ep[i].events&EPOLLIN)){
                continue;
            }
            if(ep[i].data.fd==serverfd){
                len=sizeof(cliaddr);
                clientfd=accept(serverfd, (SA *) &cliaddr, reinterpret_cast<socklen_t *>(&len));
                connect_print();
                int t;
                for(t=0;t<=MAXEPOLL;t++){
                    if(t<MAXEPOLL&&fds[t]<0){
                        fds[t]=clientfd;
                        break;
                    }
                }
                if(t==MAXEPOLL){
                    cout<<"EPOLL Client FULL\n";
                    break;
                }
                if(t>maxi){
                    maxi=t;
                }
                tep.events=EPOLLIN;
                tep.data.fd=clientfd;
                //为新建立连接的进程注册事件
                ret= epoll_ctl(efd,EPOLL_CTL_ADD,clientfd,&tep);
                if(ret==-1){
                    cout<<"epoll_ctl client error\n";
                }
            }else{
                int fdx=ep[i].data.fd;
                memset(clibuf,0, sizeof(clibuf));
                memset(servbuf,0, sizeof(servbuf));
                int n=read(fdx, clibuf, sizeof(clibuf));
                if(n==0){
                    int j;
                    for(j=0;j<=maxi;j++){
                        if(fds[j] == fdx){
                            fds[j]=-1;
                            break;
                        }
                    }
                    ret= epoll_ctl(efd, EPOLL_CTL_DEL, fdx, NULL);
                    if(ret==-1){
                        cout<<"epoll_ctl client error\n";
                    }
                    close(fdx);
                    printf("fds[%d]=%d closed connection\n", j, fdx);
                }else{
                    printf("from client fd=%d read: %s\n",fdx,clibuf);
                }

            }

        }

    }
    //close(serverfd);
    //close(efd);
    return 0;



}
void Server::connect_print() {
    printf("adding client on fd %d\n", clientfd);
    printf("connection from %s, port %d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr, servbuf, sizeof(servbuf)),
           ntohs(cliaddr.sin_port));
    ticks = time(NULL);
    snprintf(servbuf, sizeof(servbuf), "%.24s\r\n", ctime(&ticks));
    write(clientfd, servbuf, sizeof(servbuf));
}
Server::Server(char *ip,int x) {
    port=x;
    serverfd= socket(AF_INET,SOCK_STREAM,0);
    if(serverfd==-1){
        printf("Sever socket create error\n");
        return;
    }else{
        printf("Sever socket create already\n");
    }
    //bzero(&servaddr,sizeof (servaddr));//init 0
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;//ipv4
    servaddr.sin_addr.s_addr=inet_addr(ip);
    servaddr.sin_port= htons(port);
    len=sizeof(servaddr);
    bindx=bind(serverfd,(SA *) &servaddr, sizeof(servaddr));
    if(bindx==-1){
        printf("Server bind Socket error\n");
    }else{
        printf("Server IP->PORT bind Socket OK\n");
    }

    listenx=listen(serverfd,LISTENN);//n listen 阻塞40
    // numN connection requests will be queued before further requests are refused.
    if(listenx==-1){
        printf("listen error\n");
    }else{
        printf("listen prepared\n");
    }
}
