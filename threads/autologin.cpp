#include "autologin.h"
#include "dataupdate.h"

extern AdsMessage_Read  adsReadStruct;

AutoLogin::AutoLogin(){}

void AutoLogin::run()
{
    login();
}

void AutoLogin::login()
{
    //click start button
    emit clickStartButton();

    timer.start();
    //wait for all axis ready
    while(!(adsReadStruct.slave_ready[0] && adsReadStruct.slave_ready[1] && adsReadStruct.slave_ready[2] \
             && adsReadStruct.slave_ready[3] && adsReadStruct.slave_ready[4] && adsReadStruct.slave_ready[5]\
             && adsReadStruct.slave_ready[6])){
        msleep(30);
        if(timer.elapsed() > 60000){
            emit clickPowerOffButton();
            login();
            return;
        }
    }

    emit clickEnableAllButton();
}
