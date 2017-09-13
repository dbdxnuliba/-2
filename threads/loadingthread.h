#ifndef LOADINGTHREAD_H
#define LOADINGTHREAD_H
#include <QThread>
#include <QObject>
#include "dataupdate.h"

class LoadingThread : public QThread
{
    Q_OBJECT
public:
    LoadingThread();

private:
    void run();

signals:
    void loadDone();    //load gif image and init system done
};

#endif // LOADINGTHREAD_H
