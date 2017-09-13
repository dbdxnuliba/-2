#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include "socketthread.h"
#include "dataupdate.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0,const int port = 0);
    void startServer();
    int port_receive;

protected:
    void incomingConnection(qintptr socketDescriptor);

public slots:
    void sendPath(QString path);
    void setRunProgramSpeed(int speedPercentage);
    void stopProgramActive();
    void pauseProgramActive();
    void resumeProgramActive();
    void sendRunStopSignal();
    void sendRunErrorSignal();
    void sendErrorMsg2Client(QString msg);
    void connected(bool port2000, bool port2001);

signals:
    void sendPathAgain(QString);
    void setRunProgramSpeed2UI(int);
    void stopScript();
    void pauseScript();
    void resumeScript();
    void threadSendRunStopSignal();
    void threadSendRunErrorSignal();
    void sendErrorMsg2Client2(QString msg);
    void tcpConnected(bool, bool);
};

#endif // TCPSERVER_H
