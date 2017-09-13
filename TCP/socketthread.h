#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(int ID, QObject *parent = 0);
    void run();
    void translateMsg(QByteArray buffer);
    void expoCenter(QByteArray buffer);
    void sendStatusFeedback(); //send status feedback

private:
    QTcpSocket *socket;
    int socketDescriptor;


public slots:
    void readyRead();
    void disconnected();
    void sendMessage(char msg);
    void sendRunStopSignal(); //send a program finished message to client
    void sendRunErrorSignal(); //an error comes when running program
    void sendErrorMsg2Client(QString msg);

signals:
    void error(QTcpSocket::SocketError socketerror);
    void runProgram(QString path);
    void setRunProgramSpeed(int speedPercentage);
    void stopProgram();
    void pauseProgram();
    void resumeProgram();
    void connected(bool port2000, bool port2001);
};

#endif // SOCKETTHREAD_H
