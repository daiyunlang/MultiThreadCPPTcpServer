#include <QCoreApplication>
#include "tcpserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TcpServer ser;
    ser.Initial();

    return a.exec();
}
