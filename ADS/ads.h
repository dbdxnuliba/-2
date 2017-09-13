/*****************************************************
 *  How to use:
    char adsPath []= "MAIN.path"; //declare the parameter path
    bool adsValue = true; //declare the value

    ADS::getAdsInstance()->adsWrite(adsPath, sizeof(adsPath), sizeof(adsValue), &adsValue); //write to TwinCAT
    ADS::getAdsInstance()->adsRead(adsPath, sizeof(adsPath), sizeof(adsValue), &adsValue); //read from TwinCAT
******************************************************/
#ifndef ADS_H
#define ADS_H
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

class ADS
{
public:
    static ADS* getAdsInstance()
    {
        //In C++11, the following is guaranteed to perform thread-safe initialisation
        static ADS instance;
        return &instance;
    }

    //return 0 is no error
    long adsWrite(char path[], unsigned long pathSize, unsigned long valueSize, void *valueAdd);
    long adsRead(char path[], unsigned long pathSize, unsigned long valueSize, void *valueAdd);

private:
    ADS();
    ~ADS();

    //parameters for ADS protocol
    long      nErr, nPort;
    AmsAddr   Addr;
    PAmsAddr  pAddr = &Addr;
    unsigned long lHdlVar;
    const int ADS_PORT = 851;


};

#endif // ADS_H
