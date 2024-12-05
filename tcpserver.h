#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QFuture>
#include <WinSock2.h>
class TcpServer : public QObject
{
public:
    TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    bool Initial();

    void AcceptCons();

    void Communitation(SOCKET cfd);

signals:

private:
    //监听的套接字
    SOCKET fd;
    //通信套接字
//    int cfd;
    QFuture<void> future;
    bool stop;
};

#endif // TCPSERVER_H
