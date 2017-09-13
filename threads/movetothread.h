#ifndef MOVETOTHREAD_H
#define MOVETOTHREAD_H

#include <QThread>
#include "dataupdate.h"

class MoveToThread : public QThread
{
    Q_OBJECT

public:
    MoveToThread();
    bool endPositionMove = false;

private:
    void run(); //thread process will Start in this function


};

#endif // MOVETOTHREAD_H
