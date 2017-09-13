#include "logfile.h"
#include <QDebug>

extern AdsMessage_Read adsReadStruct;
extern QString fileSavePath;

LogFile::LogFile(){}

int LogFile::logFileCreate(QString type, QString message)
{
    if(!QDir(fileSavePath + "\\log").exists()) QDir().mkdir(fileSavePath + "\\log");
    QFile logFile(fileSavePath + "\\log" + logFileName + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".txt");

    logFile.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream( &logFile );

    stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "; ";
    stream << type + "; ";
    stream << message + "\n";

    logFile.close();
    return 0;
}

void LogFile::currentStatusSave()
{
    if(!QDir(fileSavePath + "\\status").exists()) QDir().mkdir(fileSavePath + "\\status");
    QFile logFile(fileSavePath + "\\status\\robotStatusSave.txt");

    logFile.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream( &logFile );

    stream << QString::number(adsReadStruct.currentFlangePosition[0]) + ";" + QString::number(adsReadStruct.currentFlangePosition[1]) + ";" + \
            QString::number(adsReadStruct.currentFlangePosition[2]) + ";" + QString::number(adsReadStruct.currentFlangePosition[3]) + ";" + \
            QString::number(adsReadStruct.currentFlangePosition[4]) + ";" + QString::number(adsReadStruct.currentFlangePosition[5]) + ";" + \
            QString::number(adsReadStruct.axisCurrentAngle[0]) + ";" + QString::number(adsReadStruct.axisCurrentAngle[1]) + ";" + \
            QString::number(adsReadStruct.axisCurrentAngle[2]) + ";" +QString::number(adsReadStruct.axisCurrentAngle[3]) + ";" +\
            QString::number(adsReadStruct.axisCurrentAngle[4]) + ";" +QString::number(adsReadStruct.axisCurrentAngle[5]) + ";" +\
            QString::number(adsReadStruct.axisCurrentAngle[6]) +"\n";

    logFile.close();
}

