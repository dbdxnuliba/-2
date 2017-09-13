#include "loadingthread.h"
extern AdsMessage_Read  adsReadStruct;

LoadingThread::LoadingThread(){}

void LoadingThread::run()
{
    msleep(3000);
    //wait for axis power up
   // while(adsReadStruct.stateMachine != 3){ QThread::msleep(300);}
    emit loadDone();
}

