#include "movetothread.h"
#include <QDebug>

extern AdsMessage_Write adsWriteStruct;
extern AdsMessage_Read  adsReadStruct;
extern bool programIsRunning;
extern double programAxisMoveTo[7];
extern int jogSpeed;

MoveToThread::MoveToThread(){}

void MoveToThread::run()
{
    //send axis angles to the PLC
    for(int i = 0; i < 7; i++) adsWriteStruct.progMoveJAxis[i] = programAxisMoveTo[i];

    adsWriteStruct.progRunSpeed = jogSpeed;//default moveto speed

    if(endPositionMove) adsWriteStruct.moveToType = 2; //enter end position move
    else adsWriteStruct.moveToType = 1; //joint move

    adsWriteStruct.moveTo = true;
    msleep(300);    //wait for signal to sync

    //wait for complete
    while(!adsReadStruct.moveToDone && !adsWriteStruct.reset){ msleep(50); }// && adsWriteStruct.moveTo

    adsWriteStruct.moveTo = false;
    endPositionMove = false;
    adsWriteStruct.moveToType = 0;
}

