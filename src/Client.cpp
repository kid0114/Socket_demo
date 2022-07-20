//
// Created by kid on 5/21/22.
//

#include "Client.h"
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>
#include <thread>
#include <sys/epoll.h>
#include "functional"

#include <arpa/inet.h>

#include <cstring>
#include <string>
#include <sys/ioctl.h>
#include<poll.h>
#include <iostream>
#include <netinet/tcp.h>

#define SA sockaddr
#define LISTENN 40
#define  PORT  6001
#define  MAXLINES 2048
using namespace std;


//参数解释
//fd:网络连接描述符
//start:首次心跳侦测包发送之间的空闲时间
//interval:两次心跳侦测包之间的间隔时间
//count:探测次数，即将几次探测失败判定为TCP断开

int set_tcp_keepAlive(int fd, int start, int interval, int count)
{
    int keepAlive = 1;
    if (fd < 0 || start < 0 || interval < 0 || count < 0) return -1;   //入口参数检查 ，编程的好习惯。
    //启用心跳机制，如果您想关闭，将keepAlive置零即可
    if(setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,(void*)&keepAlive,sizeof(keepAlive)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    //启用心跳机制开始到首次心跳侦测包发送之间的空闲时间
    if(setsockopt(fd,SOL_TCP,TCP_KEEPIDLE,(void *)&start,sizeof(start)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    //两次心跳侦测包之间的间隔时间
    if(setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,(void *)&interval,sizeof(interval)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    //探测次数，即将几次探测失败判定为TCP断开
    if(setsockopt(fd,SOL_TCP,TCP_KEEPCNT,(void *)&count,sizeof(count)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    return 0;
}
char cmdlinec[10][100]={
        "exit",
        "read",
        "write"
};
//0 1 2
int Client::checkcmd() {
    memset(cmdx,0,sizeof(cmdx));

    scanf("%s",cmdx);
    //printf("%s ",cmdx);
    for(int i=0;i<ncmd;i++){
        if(strcmp(cmdx,cmdlinec[i])==0){
            return i;
        }
    }
    return -1;
}
void Client::readserver(){
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

void Client::sendserver() {
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
//threads read write
int Client::CommuServer2() {
    thread tr(std::bind(&Client::readserver,this));
    thread ts(std::bind(&Client::sendserver,this));
    tr.join();
    ts.detach();
    return 0;
}
int Client::CommuServer1(){
    memset(clibuf,0,sizeof(clibuf));
    memset(servbuf,0,sizeof(servbuf));
    while(~scanf("%s",clibuf)){
        send(clientfd,clibuf,strlen(clibuf),0);
        if(strcmp(clibuf,"exit")==0){
            printf("client %s\n",servbuf);
            break;
        }
        recv(clientfd,servbuf,sizeof(servbuf),0);
        if(strcmp(servbuf,"exit")==0){
            printf("server %s\n",servbuf);
            break;
        }
        printf("%s\n",servbuf);
        memset(clibuf,0,sizeof(clibuf));
        memset(servbuf,0,sizeof(servbuf));
    }
    return 0;
}

#include <thread>




//在h文件中为静态方法
void * Client::heartbeat()
{
    //
    int result=1;
    while (1){
        sleep(3000);   //定时3秒  此处的单位为毫秒
       // char[] tempdata = "";   //空包
        int result= write(serverfd,"",sizeof(""));
        if (result<0) //result>0是发送成功的返回码  小于零就是发送失败
        {
            printf("Socket Closed\n");
            break;
        }
        sleep(5000);    //定时3秒  此处的单位为毫秒
    }
    return &result;
}



Client::Client(char *ip,int portx) {
    port=portx;
    clientfd= socket(AF_INET,SOCK_STREAM,0);
    if(clientfd==-1){
        printf("Client socket create error\n");
        return ;
    }else{
        printf("Client socket create already\n");
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(ip);
    servaddr.sin_port= htons(port);
    //inet_pton(AF_INET, reinterpret_cast<const char *>(&cliaddr.sin_addr), &servaddr.sin_addr);
    connectx=connect(serverfd, (SA *)&servaddr, sizeof(servaddr));
    if(connectx==-1){
        printf("connection error\n");
        return ;
    }else {
        printf("connection OK\n");
    }
    memset(servbuf, 0, sizeof(servbuf));
    read(serverfd, servbuf, sizeof(servbuf));
    printf("Connection--> %s\n",servbuf);
    while(1){

        memset(servbuf, 0, sizeof(servbuf));
        memset(clibuf, 0, sizeof(clibuf));
        read(serverfd, servbuf, sizeof(servbuf));
        cout<<"write to server: ";
        cin>>clibuf;
        //read(0,clibuf,sizeof(clibuf));
        int x=write(serverfd,clibuf,sizeof(clibuf));
        if(x==-1||(x==0&&strlen(clibuf)!=0)){
            cout<<"Connection end\n";
            break;
        }else{
            cout<<"Send "<<strlen(clibuf)<<" chars to Server\n";
        }
    }
   // CommuServer2();
    cout<<"Connection fd="<<clientfd<<" end\n";
    close(clientfd);
    //exit(0);

}