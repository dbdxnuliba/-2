#include "tcpserver.h"


TcpServer::TcpServer(QObject *parent,const int port):
    QTcpServer(parent),
    port_receive(port)
{}


void TcpServer::startServer()
{
    if(!this->listen(QHostAddress::Any, port_receive))
        qDebug() << "could not Start server";
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    //qDebug() << socketDescriptor << "connecting...";
    SocketThread *thread = new SocketThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(runProgram(QString)), this, SLOT(sendPath(QString)));
    connect(thread, SIGNAL(setRunProgramSpeed(int)), this, SLOT(setRunProgramSpeed(int)));

    connect(thread, SIGNAL(stopProgram()), this, SLOT(stopProgramActive()));
    connect(thread, SIGNAL(pauseProgram()), this, SLOT(pauseProgramActive()));
    connect(thread, SIGNAL(resumeProgram()), this, SLOT(resumeProgramActive()));
    connect(thread, SIGNAL(connected(bool, bool)), this, SLOT(connected(bool, bool)));


    connect(this, SIGNAL(threadSendRunStopSignal()), thread, SLOT(sendRunStopSignal()));
    connect(this, SIGNAL(threadSendRunErrorSignal()), thread, SLOT(sendRunErrorSignal()));
    connect(this, SIGNAL(sendErrorMsg2Client2(QString)), thread, SLOT(sendErrorMsg2Client(QString)));
    thread->start();
}

void TcpServer::sendPath(QString path)
{
    emit sendPathAgain(path);
}

void TcpServer::setRunProgramSpeed(int speedPercentage)
{
    emit setRunProgramSpeed2UI(speedPercentage);
}

void TcpServer::stopProgramActive()
{
    emit stopScript();
}

void TcpServer::pauseProgramActive()
{
    emit pauseScript();
}

void TcpServer::resumeProgramActive()
{
    emit resumeScript();
}

void TcpServer::sendRunStopSignal()
{
    emit threadSendRunStopSignal();
}

void TcpServer::sendRunErrorSignal()
{
    emit threadSendRunErrorSignal();
}

void TcpServer::sendErrorMsg2Client(QString msg)
{
    emit sendErrorMsg2Client2(msg);
}

void TcpServer::connected(bool port2000, bool port2001)
{
    emit tcpConnected(port2000, port2001);
}


