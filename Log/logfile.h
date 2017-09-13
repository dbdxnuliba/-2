/*****************************************************
 *  use this class like this:
 *
 *  #include "logfile.h"
    LogFile::logFileCreate(type, message);

    It will create a file named siasun_log_(time) under C:\
******************************************************/
#ifndef LOGFILE_H
#define LOGFILE_H
#include <QtCore>
#include "dataupdate.h"

class LogFile
{
public:
    LogFile();
    static int logFileCreate(QString type, QString message);    //create log file, save error message
    static void currentStatusSave();                            //save current status into a file
};

#endif // LOGFILE_H
