/*****************************************************
 *  the program parameters:
    moveJ(x, y, z, rx, ry, rz, v, a, a1, a2, a3, a4, a5, a6, a7)
******************************************************/
#ifndef RUNPROGRAM_H
#define RUNPROGRAM_H
#include <QThread>
#include "dataupdate.h"
#include <QFileDialog>

const int progCommandWaitTime = 5;
const int progWaitTime = 10;

class RunProgram : public QThread
{
    Q_OBJECT
public:
    explicit RunProgram();
    ~RunProgram();

    QString programName;
    bool stop;              //stop run program
    bool error;             //an error comes, stop the program
    bool stepRun;
    bool stepRunModeEnable;
    bool run_prog;          //run a target program
    QString run_prog_name;  //name of the target program

    void runProgram();
    void setProgramName(QString name){programName = name;}

private:
    void run();             //thread Start here
    double calculatePsi(double *jointIn);

signals:
    void runFinish();
    void stopRun();
    void runError();
    void programError(QString);
    void collisionWarning(int collisionID);
    void checkRepeat();
    void runTargetProg(QString);
    void stopRepeat();

    //functions:
    void fun_socket_open(QString, int, int);
    void fun_socket_send(QString);
    void fun_socket_rcv(int);
    void fun_socket_close();
    void fun_external_hand(QString);
    void threadStart();
    void stopProgramThread();
    void changeToolCoordinate(QStringList);
    void changeRunButtonStatus(bool);
    void cameraGo();
    void hideButtons(bool, bool, bool, bool);//stop run step pause (set buttons visible or not)
};

#endif // RUNPROGRAM_H
