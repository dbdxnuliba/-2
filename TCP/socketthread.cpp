#include "socketthread.h"
#include "dataupdate.h"
#include "../UI/warningwindow.h"
#include <QFile>
#include "../Log/logfile.h"

extern bool programIsRunning;
extern AdsMessage_Read   adsReadStruct;
extern AdsMessage_Write  adsWriteStruct;
extern Status_Struct     _status_struct;
extern bool functionFinish;
extern bool systemError;
extern int sendStatusFeedbackCycle;
extern bool connected2000;
extern bool connected2001;

QString msg = ""; //received message converted to qstring
QByteArray buffer;//received message in byte array
QByteArray feedback;//heart beat message feedback

int seriID_temp = -1;

bool isConnecting = false;
bool StatusFeedback = false;

SocketThread::SocketThread(int ID, QObject *parent) : QThread(parent)
{
    this->socketDescriptor = ID;
}

void SocketThread::run()
{
    //thread starts here
    socket = new QTcpSocket();

    if(!socket->setSocketDescriptor(this->socketDescriptor)){
        emit error(socket->error());
        qDebug() << socketDescriptor << "error";
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);

    isConnecting = true;

    if(socket->localPort() == PORT_RECEIVE){
        qDebug() << "Remote PC connected, port" << socket->localPort();
        connected2000 = true;
        LogFile::logFileCreate("Remote PC connected, port ", QString::number(socket->localPort())); //save log
        emit connected(connected2000, connected2001);
    }
    else if(socket->localPort() == PORT_STATUS){
        qDebug() << "Receive status feedback connected, port" << socket->localPort();
        connected2001 = true;
        LogFile::logFileCreate("Receive status feedback connected, port ", QString::number(socket->localPort())); //save log
        StatusFeedback = true;
        emit connected(connected2000, connected2001);
        while(StatusFeedback) sendStatusFeedback();
    }

    exec();
}

void SocketThread::translateMsg(QByteArray buffer)
{
    QString msg = (QString)buffer;

    QStringList keywordPieces = msg.split('(');
    QString keyWord = keywordPieces.value(0);
    QStringList parameterPieces = keywordPieces.value(1).split( ")" ).value(0).split( "," );

    //feedback current status
    if(keyWord == "status"){
        for(int i = 0; i < 8; i++) _status_struct.externalIORead[i] = adsReadStruct.externalIORead[i];
        for(int i = 0; i < 7; i++) _status_struct.axisCurrentAngle[i] = adsReadStruct.axisCurrentAngle[i];
        for(int i = 0; i < 6; i++) _status_struct.currentFlangePosition[i] = adsReadStruct.currentFlangePosition[i];
        for(int i = 0; i < 6; i++) _status_struct.currentFlangePositionTool[i] = adsReadStruct.currentFlangePositionTool[i];

        //tcp send message
        socket->write((char*)&_status_struct, (qint64)sizeof(_status_struct));
        //tcp receive message
        socket->waitForBytesWritten(500);

        //qDebug()<<"***********"<<_status_struct.robotCurrentState;
    }

    //run a target program
    if(keyWord == "run"){
        QString programPath = "C:/SiasunRobot/program/" + keywordPieces.value(1).split( ")" ).value(0) + ".spf";

        emit runProgram(programPath);

        //send feedback
        socket->write("prog Start");
        socket->waitForBytesWritten(500);

        //wait to Start program
        msleep(200);
        //while(programIsRunning) sleep(1);
    }

    if(keyWord == "stop"){
        if(programIsRunning){
            qDebug() << "***************stop";
        }
    }

    if(keyWord == "MoveJ"){
        //7 joints move
        if(parameterPieces.length() == 7){
            //check if parameters are all numbers
            for(int i = 0; i < 6; i++){
                //trimmed() will remove whitespace
                if(parameterPieces.value(i).toDouble() == 0 && parameterPieces.value(i).trimmed() != "0"){

                    //if(parameterPieces.value(i).trimmed().split('.').value(0) == "0") return true; //for 0.0000
                    //if(parameterPieces.value(i).trimmed().split('.').value(0) == "-0") return true; //for -0.0000
                    //qDebug() << "socket command error: parameters are not all numbers";
                    socket->write("error: parameters are not all numbers");
                    _status_struct.robotCurrentState = 2; //error
                    return;
                }
            }

            //check axis 1 3 5 7 range
            for(int i = 0; i < 7; i+=2){
                if(parameterPieces.value(i).toDouble() < -360 || parameterPieces.value(i).toDouble() > 360){
                    //qDebug() << "socket command error: axis 1 3 5 7 range wrong";
                    socket->write("error: axis 1 3 5 7 range wrong");
                    _status_struct.robotCurrentState = 2; //error
                    return;
                }
            }

            //check axis 2 4 6 range
            for(int i = 1; i < 7; i+=2){
                if(parameterPieces.value(i).toDouble() < -90 || parameterPieces.value(i).toDouble() > 90){
                    //qDebug() << "socket command error: check axis 2 4 6 range wrong";
                    socket->write("error: check axis 2 4 6 range wrong");
                    _status_struct.robotCurrentState = 2; //error
                    return;
                }
            }

            //execute command, joint move
            adsWriteStruct.progStop=false;
            adsWriteStruct.enteredAxisValue[0] = parameterPieces.value(0).toDouble();
            adsWriteStruct.enteredAxisValue[1] = parameterPieces.value(1).toDouble();
            adsWriteStruct.enteredAxisValue[2] = parameterPieces.value(2).toDouble();
            adsWriteStruct.enteredAxisValue[3] = parameterPieces.value(3).toDouble();
            adsWriteStruct.enteredAxisValue[4] = parameterPieces.value(4).toDouble();
            adsWriteStruct.enteredAxisValue[5] = parameterPieces.value(5).toDouble();
            adsWriteStruct.enteredAxisValue[6] = parameterPieces.value(6).toDouble();
            adsWriteStruct.progRunSpeed = 70;//default moveto speed
            adsWriteStruct.jogMode = 4;

            socket->write("MoveJ Start");
            socket->waitForBytesWritten(500);
            QThread::msleep(100);//wait for sync

            while(!adsReadStruct.jogEnterValueMoveFinished) msleep(100);

            socket->write("MoveJ finish");
            adsWriteStruct.jogMode = 0; //reset
        }


        //move to the target end position (parameters are x y z rx ry rz)
        if(parameterPieces.length() == 6){
            //check if parameters are all numbers
            for(int i = 0; i < 5; i++){
                //trimmed() will remove whitespace
                if(parameterPieces.value(i).toDouble() == 0 && parameterPieces.value(i).trimmed() != "0"){
                    //if(parameterPieces.value(i).trimmed().split('.').value(0) == "0") return true; //for 0.0000
                    //if(parameterPieces.value(i).trimmed().split('.').value(0) == "-0") return true; //for -0.0000
                    //qDebug() << "socket command error: parameters are not all numbers";
                    socket->write("error: parameters are not all numbers");
                    _status_struct.robotCurrentState = 2; //error
                    return;
                }
            }

            //check for value range
            if(parameterPieces.value(0).toDouble() < -999 || parameterPieces.value(0).toDouble() > 999){
                socket->write("error: parameters 1 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(1).toDouble() < -999 || parameterPieces.value(1).toDouble() > 999){
                socket->write("error: parameters 2 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(2).toDouble() < -211 || parameterPieces.value(2).toDouble() > 1372){
                socket->write("error: parameters 3 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(3).toDouble() < -360 || parameterPieces.value(3).toDouble() > 360){
                socket->write("error: parameters 4 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(4).toDouble() < -360 || parameterPieces.value(4).toDouble() > 360){
                socket->write("error: parameters 5 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(5).toDouble() < -360 || parameterPieces.value(5).toDouble() > 360){
                socket->write("error: parameters 6 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }

            //enter value position move
            adsWriteStruct.progStop=false;
            adsWriteStruct.enteredXyzValue[0] = parameterPieces.value(0).toDouble();
            adsWriteStruct.enteredXyzValue[1] = parameterPieces.value(1).toDouble();
            adsWriteStruct.enteredXyzValue[2] = parameterPieces.value(2).toDouble();
            adsWriteStruct.enteredXyzValue[3] = parameterPieces.value(3).toDouble();
            adsWriteStruct.enteredXyzValue[4] = parameterPieces.value(4).toDouble();
            adsWriteStruct.enteredXyzValue[5] = parameterPieces.value(5).toDouble();
            adsWriteStruct.progRunSpeed = 70;//default moveto speed
            adsWriteStruct.jogMode = 5;

            socket->write("MoveJ Start");
            socket->waitForBytesWritten(500);
            QThread::msleep(100);//wait for sync

            while(!adsReadStruct.jogEnterValueMoveFinished) msleep(100);

            socket->write("MoveJ finish");
            adsWriteStruct.jogMode = 0; //reset

        }


    }

    if(keyWord == "MoveL"){
        if(parameterPieces.length() == 6){
            //check if parameters are all numbers
            for(int i = 0; i < 5; i++){
                //trimmed() will remove whitespace
                if(parameterPieces.value(i).toDouble() == 0 && parameterPieces.value(i).trimmed() != "0"){
                    //qDebug() << "socket command error: parameters are not all numbers";
                    socket->write("error: parameters are not all numbers");
                    _status_struct.robotCurrentState = 2; //error
                    return;
                }
            }

            //check for value range
            if(parameterPieces.value(0).toDouble() < -999 || parameterPieces.value(0).toDouble() > 999){
                socket->write("error: parameters 1 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(1).toDouble() < -999 || parameterPieces.value(1).toDouble() > 999){
                socket->write("error: parameters 2 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(2).toDouble() < -211 || parameterPieces.value(2).toDouble() > 1372){
                socket->write("error: parameters 3 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(3).toDouble() < -360 || parameterPieces.value(3).toDouble() > 360){
                socket->write("error: parameters 4 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(4).toDouble() < -360 || parameterPieces.value(4).toDouble() > 360){
                socket->write("error: parameters 5 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }
            if(parameterPieces.value(5).toDouble() < -360 || parameterPieces.value(5).toDouble() > 360){
                socket->write("error: parameters 6 value range incorrect");
                _status_struct.robotCurrentState = 2; //error
                return;
            }

            adsWriteStruct.progRun = true;
            adsWriteStruct.dataFinish = 0;

            //send end position value
            for(int i = 0; i < 6; i++) adsWriteStruct.progMoveLAxis[i] = parameterPieces.value(i).toDouble();
            adsWriteStruct.progRunSpeed = 100; //fix the speed to 100mm/s
            adsWriteStruct.blockType = 4; //moveL
            socket->write("MoveL Start");
            socket->waitForBytesWritten(500);

            while(!adsReadStruct.oneCommandFinish) msleep(100);

            socket->write("MoveL finish");
            adsWriteStruct.blockType = 4; //program finished
            adsWriteStruct.progRun = false;
        }


    }

}

//this function just works for siasun expo center
void SocketThread::expoCenter(QByteArray buffer)
{
    int seriID  = (int)buffer.at(0);
    int taskID  = (int)buffer.at(2);
    int placeID = (int)buffer.at(4);
    qDebug()<<"*" << (int)buffer.at(0)<< (int)buffer.at(1)<< (int)buffer.at(2)<< (int)buffer.at(3)\
             << (int)buffer.at(4)<< (int)buffer.at(5)<< (int)buffer.at(6)<< (int)buffer.at(7);\
             //<< (int)buffer.at(8)<< (int)buffer.at(9)<< (int)buffer.at(10)<< (int)buffer.at(11);
    qDebug() << seriID << taskID << placeID;

    //check if received a new ID
    if(seriID != seriID_temp){
        seriID_temp = seriID;

        QString programPath = "C:/SiasunRobot/program/" + QString::number(taskID) + ".spf";
        emit runProgram(programPath);

        //wait to Start program
        msleep(200);
        while(programIsRunning) msleep(500);

        feedback.clear();
        //qDebug()<<feedback.length();


        feedback.insert(0,buffer.at(0));
        feedback.insert(1,buffer.at(1));
        feedback.insert(2,buffer.at(2));
        feedback.insert(3,buffer.at(3));
        feedback.insert(4,char(3));
        feedback.insert(5,char(0));
        //qDebug()<<feedback.length();

        //send feedback
        socket->write(feedback);
        socket->waitForBytesWritten(500);
    }
}

void SocketThread::sendStatusFeedback()
{
    //for port 2001, send status feedback
    for(int i = 0; i < 8; i++) _status_struct.externalIORead[i] = adsReadStruct.externalIORead[i];
    for(int i = 0; i < 7; i++) _status_struct.axisCurrentAngle[i] = adsReadStruct.axisCurrentAngle[i];
    for(int i = 0; i < 6; i++) _status_struct.currentFlangePosition[i] = adsReadStruct.currentFlangePosition[i];
    for(int i = 0; i < 6; i++) _status_struct.currentFlangePositionTool[i] = adsReadStruct.currentFlangePositionTool[i];

    //tcp send message
    socket->write((char*)&_status_struct, (qint64)sizeof(_status_struct));
    //tcp receive message
    socket->waitForBytesWritten(500);
    msleep(sendStatusFeedbackCycle);
}


void SocketThread::readyRead()
{
    if(socket->localPort() == PORT_RECEIVE){
        if(systemError){
            socket->write("System error");
            LogFile::logFileCreate("TCP reply", "System error"); //save log
            socket->waitForBytesWritten(500);
            return;
        }

        if(adsReadStruct.stateMachine != 3){
            socket->write("System not ready");
            LogFile::logFileCreate("TCP reply", "System not ready"); //save log
            socket->waitForBytesWritten(500);
            return;
        }

        buffer = socket->readAll();

        LogFile::logFileCreate("Get TCP msg:", QString(buffer)); //save log

        if(QString(buffer).split('(').value(0) == "run"){
            if(!programIsRunning){
                QString progName = QString(buffer).split('(').value(1).split(')').value(0);
                if(progName.contains(',')){
                    emit setRunProgramSpeed(progName.split(',').value(1).toInt());
                    emit runProgram("C:/SiasunRobot/program/" + progName.split(',').value(0));

                }else{
                    //no speed setting
                    emit setRunProgramSpeed(100);
                    emit runProgram("C:/SiasunRobot/program/" + progName);

                }

                //send feedback
                socket->write("Script start");
                LogFile::logFileCreate("TCP reply", "Script start"); //save log
                socket->waitForBytesWritten(500);
            }else{
                socket->write("System busy");
                LogFile::logFileCreate("TCP reply", "System busy"); //save log
                socket->waitForBytesWritten(500);
            }
            return;
        }

        if(QString(buffer) == "stop"){
            if(programIsRunning){
                emit stopProgram();
                //send feedback
                socket->write("Script stop");
                LogFile::logFileCreate("TCP reply", "Script stopping"); //save log
                socket->waitForBytesWritten(500);
            }else{
                //send feedback
                socket->write("Already stopped");
                LogFile::logFileCreate("TCP reply", "Already stopped"); //save log
                socket->waitForBytesWritten(500);
            }
            return;
        }

        if(QString(buffer) == "pause"){
            if(programIsRunning){
                emit pauseProgram();
                //send feedback
                socket->write("Script pause");
                LogFile::logFileCreate("TCP reply", "Script pause"); //save log
                socket->waitForBytesWritten(500);
            }else{
                //send feedback
                socket->write("Already stopped");
                LogFile::logFileCreate("TCP reply", "Already stopped"); //save log
                socket->waitForBytesWritten(500);
            }
            return;
        }

        if(QString(buffer) == "resume"){
            if(adsWriteStruct.progPause && adsReadStruct.pauseDone){
                emit resumeProgram();
                //send feedback
                socket->write("Script resume");
                LogFile::logFileCreate("TCP reply", "Script resume"); //save log
                socket->waitForBytesWritten(500);
            }else{
                //send feedback
                socket->write("Resume error");
                LogFile::logFileCreate("TCP reply", "Resume error"); //save log
                socket->waitForBytesWritten(500);
            }
            return;
        }



        if(QString(buffer) == "status"){
            //send feedback
            QString status_msg = QString::number(adsReadStruct.currentFlangePosition[0]) + ";" + QString::number(adsReadStruct.currentFlangePosition[1]) + ";" + \
                    QString::number(adsReadStruct.currentFlangePosition[2]) + ";" + QString::number(adsReadStruct.currentFlangePosition[3]) + ";" + \
                    QString::number(adsReadStruct.currentFlangePosition[4]) + ";" + QString::number(adsReadStruct.currentFlangePosition[5]) + ";" + \
                    QString::number(adsReadStruct.axisCurrentAngle[0]) + ";" + QString::number(adsReadStruct.axisCurrentAngle[1]) + ";" + \
                    QString::number(adsReadStruct.axisCurrentAngle[2]) + ";" +QString::number(adsReadStruct.axisCurrentAngle[3]) + ";" +\
                    QString::number(adsReadStruct.axisCurrentAngle[4]) + ";" +QString::number(adsReadStruct.axisCurrentAngle[5]) + ";" +\
                    QString::number(adsReadStruct.axisCurrentAngle[6]);

            socket->write(status_msg.toStdString().c_str());
            LogFile::logFileCreate("TCP reply", status_msg); //save log
            socket->waitForBytesWritten(500);
            return;
        }

        if(!programIsRunning){
            //write received script into the file
            QFile runScript("C:/SiasunRobot/Remote_Control_Mode");
            runScript.open(QIODevice::ReadWrite | QIODevice::Truncate);
            QTextStream stream( &runScript );
            stream << QString(buffer);
            runScript.close();

            WarningWindow::getInstance()->close(); //close current warnning window
            //run script
            emit runProgram("C:/SiasunRobot/Remote_Control_Mode");
            //send feedback
            socket->write("Script start");
            LogFile::logFileCreate("TCP reply", "Script start"); //save log
            socket->waitForBytesWritten(500);
        }else{
            socket->write("System busy");
            LogFile::logFileCreate("TCP reply", "System busy"); //save log
            socket->waitForBytesWritten(500);
        }
    }

}

void SocketThread::disconnected()
{
    if(socket->localPort() == PORT_STATUS){
        StatusFeedback = false;
        connected2001 = false;
    }

    if(socket->localPort() == PORT_RECEIVE){
        connected2000 = false;
    }

    qDebug() << socketDescriptor << "disconnected";
    LogFile::logFileCreate("disconnected ", QString::number(socketDescriptor)); //save log
    isConnecting = false;
    socket->deleteLater();
    emit connected(connected2000, connected2001);
    exit(0);
}

void SocketThread::sendMessage(char msg)
{
    socket->write(&msg);
}

void SocketThread::sendRunStopSignal()
{
    //send feedback
    socket->write("Script finish");
    LogFile::logFileCreate("TCP reply", "Script finish"); //save log
    socket->waitForBytesWritten(500);
}

void SocketThread::sendRunErrorSignal()
{//qDebug() << "***error";
    //socket->write("error");

    //socket->waitForBytesWritten(500);
}

void SocketThread::sendErrorMsg2Client(QString msg)
{
    //send error message
    QByteArray ba = msg.toLatin1();
    const char *c_str = ba.data();
    socket->write(c_str);
    LogFile::logFileCreate("TCP reply", QString::fromUtf8(c_str)); //save log
    socket->waitForBytesWritten(500);
}
