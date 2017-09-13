#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "mainwindow.h"
#include <QApplication>
#include "dataupdate.h"

extern AdsMessage_Write adsWriteStruct;
extern bool mianWindowFullScreen;

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
        case CTRL_SHUTDOWN_EVENT:
            //Computer is shutting down
            adsWriteStruct.reserve[4] = true;
            return true;
        case CTRL_LOGOFF_EVENT:
            //Current user logs off
            adsWriteStruct.reserve[4] = true;
            return true;
        case WM_POWERBROADCAST:
            adsWriteStruct.reserve[4] = true;
            return true;
        case WM_QUERYENDSESSION:
            adsWriteStruct.reserve[4] = true;
            return true;
    }
    return FALSE;
}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    MainWindow w;    

    w.show();

    w.startInit();

    //check computer shutdown
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    //set auto full screen
    if(mianWindowFullScreen) w.setWindowState(Qt::WindowFullScreen);

    return a.exec();
}
