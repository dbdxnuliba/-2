#include "ads.h"
#include "qdebug.h"


ADS::ADS(){
    nPort = AdsPortOpen();              //ADS connect, open ADS port
    nErr = AdsGetLocalAddress(pAddr);   //get local port address

    //To connect to other PC, set IP here. Default connect to local address
    //pAddr->netId.b[0] = 192;
    //pAddr->netId.b[1] = 168;
    //pAddr->netId.b[2] = 12;
    //pAddr->netId.b[3] = 11;//173;//68//207
    //pAddr->netId.b[4] = 1;
    //pAddr->netId.b[5] = 1;
    pAddr->port = ADS_PORT; //set pointer to port 851
}

ADS::~ADS(){
    nErr = AdsPortClose();
}


long ADS::adsWrite(char path[], unsigned long pathSize, unsigned long valueSize, void *valueAdd)
{
    AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, pathSize, path);
    return AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, valueSize, valueAdd);
}

long ADS::adsRead(char path[], unsigned long pathSize, unsigned long valueSize, void *valueAdd)
{
    AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, pathSize, path);
    return AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, valueSize, valueAdd);
}
