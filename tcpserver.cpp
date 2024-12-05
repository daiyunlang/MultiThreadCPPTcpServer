#include "tcpserver.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>//主要提供对POSIX操作系统接口的访问，使用io.h进行替代
#include <io.h>
//#include <arpa/inet.h>//使用winsock2.h
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>//inet_ntop
#include <QtConcurrent>
unsigned short PROT = 9999;

TcpServer::TcpServer(QObject *parent)
    : QObject{parent}
    ,stop(false)
{

}

TcpServer::~TcpServer()
{
    stop = true;
    if(future.isRunning()){
        future.waitForFinished();
    }
    //6.关闭文件描述符
    closesocket(fd);
    //_close(fd);//直接使用_close会触发中断
    //_close(cfd);

    WSACleanup();
}

bool TcpServer::Initial()
{
    //windows需要初始化socket
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    //1.创建监听的套接字
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }
    //2.绑定本地额IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PROT);//大端
    saddr.sin_addr.S_un.S_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }

    //3.设置监听
    ret = listen(fd, 128);
    if (ret == -1) {
        perror("listen");
        return -1;
    }

//    future = QtConcurrent::run<TcpServer*, void(TcpServer::*)()>(this,&TcpServer::AcceptCons);
    future = QtConcurrent::run([&](){
        this->AcceptCons();
    });
}

void TcpServer::AcceptCons()
{
    while(!stop){
        //4.阻塞并等待客户端连接
        struct sockaddr_in caddr;
        int caddrLen = sizeof(caddr);
        SOCKET cfd = accept(fd, (struct sockaddr*)&caddr, &caddrLen);
        if (cfd == -1) {
            perror("accept");
            return;
        }

        //连接成功，打印客户端的ip和端口信息
        char ip[32];
        printf("客户端ip：%s, 端口：%d \n",
               //大端整形到点分十进制
               inet_ntop(AF_INET, &caddr.sin_addr.S_un.S_addr, ip, sizeof(ip)), ntohs(caddr.sin_port));
        QtConcurrent::run([&](){
            this->Communitation(cfd);
        });
    }


}

void TcpServer::Communitation(SOCKET cfd)
{
    //5.通信
    while (!stop) {
        //接收数据
        char buff[1024];
        int len = recv(cfd, buff, sizeof(buff), 0);
        if (len > 0) {
            printf("client say : %s \n", buff);
            send(cfd, buff, len, 0);
        }
        else if (len == 0) {
            printf("客户端已经断开连接");
            break;
        }
        else {
            if (errno == EINTR) {
                // 信号中断，可以选择忽略或重新开始recv
                break;
            }
            else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 非阻塞模式下没有数据，可以稍后再尝试接收
                break;
            }
            else if (errno == ECONNRESET || errno == EPIPE) {
                // 连接被对方重置或对端已关闭
                break;
            }
            else {
                // 其他错误，根据具体错误处理
                perror("recv failed");
                break;
            }
        }
        Sleep(1000);
    }
    closesocket(cfd);
}
