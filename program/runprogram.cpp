#include "runprogram.h"
#include <QDebug>
#include "../Log/logfile.h"

extern AdsMessage_Write adsWriteStruct;
extern AdsMessage_Read adsReadStruct;
extern XML_Struct       xmlReadStruct;
extern XML_Struct       xmlWriteStruct;
extern Status_Struct   _status_struct;
extern int currentCmdLine;
extern int oldCurrentCmdLine;
extern bool programIsRunning;
extern bool collisionSignalSend;
extern bool functionFinish;
extern float runSpeedPercent;
extern QString currentProgKeyword;
extern bool stopOrRunEnable;

extern std::vector<QString> programCommand;

#define FILEINPUT_MODE 0x0001

extern "C" int RSI_start(int mode, ...);
extern "C" void* RSI_execNext();
extern "C" char *RSI_getErrMsg();
extern "C" int RSI_getErrSize();
extern "C" int RSI_initEnv();
extern "C" int RSI_end();

struct rsi_media *pm = NULL;
struct rsi_media *pmNext = NULL;
char* error_msg;

RunProgram::RunProgram():
    programName(""),
    stop(false),
    error(false),
    stepRun(false),
    stepRunModeEnable(false)
{
    //init interpretor
    if (RSI_initEnv() != 0) LogFile::logFileCreate("Compilor Error", "robot interperter init error!");
}

RunProgram::~RunProgram(){
    //close interpretor
    if (RSI_end() != 0) LogFile::logFileCreate("Compilor Error", "release compilor space error!");
}

void RunProgram::run()
{
    qDebug() << "program start**********";
    _status_struct.robotCurrentState = 1; //running

    QByteArray ba = programName.toLatin1();
    const char *c_str = ba.data();

    if (RSI_start(FILEINPUT_MODE, c_str) != 0){
        QString msg;
        msg.sprintf("%s",RSI_getErrMsg());
        emit programError(msg);
        error = true;
    }else{
        LogFile::logFileCreate("run program", programName.split('/').value(3)); //save run program log
        runProgram();
    }

    if(error){
        //write robot status to the status struct
        _status_struct.robotCurrentState = 2; //error

        emit runError();
    }

    if((!stop) && (!adsReadStruct.stopClicked) && (!error)) emit stopRun();
    stop = false;
    error = false;

    //emit stopProgramThread();

    //wait for bStopped set to false in mode 3(TwinCAT Auto_active)
    //while(adsReadStruct.stopClicked) msleep(30);

    msleep(200);

    //stop run program mode and jump to jog mode in PLC
    adsWriteStruct.progRun = false;
    adsWriteStruct.progStop = false;
    adsWriteStruct.toolCoordinateProg = false;

    msleep(200);

    //to show the current executing command line on screen
    currentCmdLine = 0;
    oldCurrentCmdLine = -1;

    emit changeRunButtonStatus(true);

    emit checkRepeat(); //check if need to repeat program

    qDebug() << "program stop**********";

    programIsRunning = false;

    if(!error){
        //write robot status to the status struct
        _status_struct.robotCurrentState = 0; //stand-by
    }
}

double RunProgram::calculatePsi(double *jointIn)
{
    //DH
    double Dbs = xmlWriteStruct.DH[0];
    double Dse = xmlWriteStruct.DH[1];
    double Dew = xmlWriteStruct.DH[2];
    double Dwt = xmlWriteStruct.DH[3];
    double w_norm;
    double sintheta;
    double costheta;
    double ZER =1E-12;
    double psi_cur;
    double joint[7];

    if(abs(Dbs - 0.3117) > 0.1) Dbs = 0.3117;
    if(abs(Dse - 0.4) > 0.1) Dse = 0.4;
    if(abs(Dew - 0.4) > 0.1) Dew = 0.4;
    if(abs(Dwt - 0.3117) > 0.1) Dwt = 0.3117;

    for(int i = 0; i < 7; i++) joint[i] = jointIn[i]*3.1415926/180;

    w_norm = sqrt(pow(Dse, 2) + pow(Dew, 2) + 2 * Dse*Dew*cos(joint[3]));
    sintheta= Dse*Dew*sin(joint[1])*sin(joint[2])*sin(joint[3]) / w_norm;

    costheta= -Dse / pow(w_norm, 2) * (-Dew * Dew * cos(joint[3]) * cos(joint[2]) * sin(joint[1]) * sin(joint[3]) + Dew * Dew * cos(joint[1]) * pow(cos(joint[3]), 2) - Dew * Dse * cos(joint[2]) * sin(joint[1]) * sin(joint[3]) + 2 * Dew * Dse * cos(joint[1]) * cos(joint[3]) + Dse * Dse * cos(joint[1]) - cos(joint[1]) * w_norm * w_norm);
    if(abs(costheta)>1.0)
        costheta=1.0;

    if(abs(sintheta)<ZER){
        ;
    }else if(sintheta > 0)
        psi_cur = acos(costheta / sqrt(pow(sintheta, 2) + pow(costheta, 2)));
    else
        psi_cur = -acos(costheta / sqrt(pow(sintheta, 2) + pow(costheta, 2)));

        qDebug() << "***************" << Dbs<<Dse<<Dew<<Dwt<<psi_cur;
    return psi_cur;
}


void RunProgram::runProgram()
{
    bool passWhile = false;
    programIsRunning = true;
    adsWriteStruct.dataFinish = 0;
    adsWriteStruct.blendStep = 0;
    currentCmdLine = -1;
    bool inBlendMove = false;
    bool inMovejBlend = false;
    bool oldMovejRTS;
    bool stopBlend = false;
    double blendMovejTemp[7];   //save previous blend position
    int lastBlendMove = -1;     //0:moveJ, 1:moveL, 2:moveC
    adsWriteStruct.blendMoveStopClick = false;


    if(stepRunModeEnable) emit hideButtons(true, false, true, false);
    else emit hideButtons(true, false, false, true);


    pm = (rsi_media*)RSI_execNext();    //run first command
    if(pm == NULL) {
        QString msg;
        msg.sprintf("%s",RSI_getErrMsg());
        emit programError(msg);
        error = true;
        return;
    }


    while ((pm->kind != rsi_media::M_END) && (!stop) && (!error) && (!adsReadStruct.stopClicked) && (!adsWriteStruct.progStop || adsReadStruct.RTS_Blends)) {

        if(pm->kind == rsi_media::M_ERROR){
            QString msg;
            msg.sprintf("%s",RSI_getErrMsg());
            emit programError(msg);
            error = true;
            return;
        }


        //pre-check for blend move
        if(pm->kind == rsi_media::M_MOVEJ){
            if(pm->u.movej.rad > 0){
                if(programCommand.size() <= pm->linenum) pm->u.movej.rad = -1;//moveJ can just blend with moveJ
                else{
                    int index = pm->linenum;
                    QString keyword = programCommand.at(index).split('(').value(0);
                    while(keyword.at(0) == '#' || programCommand.at(index) == "\n"){
                        index++;
                        if(programCommand.size() > index)
                            keyword = programCommand.at(index).split('(').value(0);
                        else
                            break;
                    }
                    if(programCommand.size() <= index) pm->u.movej.rad = -1;
                    else if(keyword != "MoveJ" && keyword != "movej")
                        pm->u.movej.rad = -1;
                }
            }
        }


        if(pm->kind == rsi_media::M_MOVEL){
            if(pm->u.movel.rad > 0){
                if(programCommand.size() <= pm->linenum)
                    pm->u.movel.rad = -1;
                else{
                    int index = pm->linenum;
                    QString keyword = programCommand.at(index).split('(').value(0);
                    while(keyword.at(0) == '#' || programCommand.at(index) == "\n"){
                        index++;
                        if(programCommand.size() > index)
                            keyword = programCommand.at(index).split('(').value(0);
                        else
                            break;
                    }
                    if(programCommand.size() <= index) pm->u.movel.rad = -1;
                    else if(keyword != "MoveL" && keyword != "MoveC_1" && keyword != "movel" && keyword != "movec")
                        pm->u.movel.rad = -1;
                }
            }
        }

        if(pm->kind == rsi_media::M_MOVEC){
            if(pm->u.movec.rad > 0){
                if(programCommand.size() <= pm->linenum) pm->u.movec.rad = -1;
                else{
                    int index = pm->linenum;
                    QString keyword = programCommand.at(index).split('(').value(0);
                    while(keyword.at(0) == '#' || programCommand.at(index) == "\n"){
                        index++;
                        if(programCommand.size() > index)
                            keyword = programCommand.at(index).split('(').value(0);
                        else
                            break;
                    }
                    if(programCommand.size() <= index) pm->u.movec.rad = -1;
                    else if(keyword != "MoveL" && keyword != "MoveC_1" && keyword != "movel" && keyword != "movec")
                        pm->u.movec.rad = -1;
                }
            }
        }


        //analyse functions
        if (pm->kind == rsi_media::M_MOVEJ && (pm->u.movej.rad <= 0 || stepRunModeEnable)){
            if(!inMovejBlend){
                //normal movej
                for(int i = 0; i < 7; i++) adsWriteStruct.progMoveJAxis[i] = pm->u.movej.pos[i];
                adsWriteStruct.dataFinish++;
                //qDebug() << "*******normal movej";
            }else{
                //last move of blend movej
                for(int i = 0; i < 7; i++){
                    adsWriteStruct.progMoveJAxis[i] = blendMovejTemp[i];
                    adsWriteStruct.progMoveJAxisBlend[i] = pm->u.movej.pos[i];                   
                }
                adsWriteStruct.movejBlendValueSend = true;
                //qDebug() << "1*******last move of blend movej";
            }
            adsWriteStruct.blockType = 1; //moveJ



            adsWriteStruct.progRunSpeed = pm->u.movej.speed * runSpeedPercent;
            //check program error
            if(adsWriteStruct.progRunSpeed > 100) adsWriteStruct.progRunSpeed = 100;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;


            adsWriteStruct.movejBlend = false;



            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

            if(inMovejBlend){
                msleep(10);
                adsWriteStruct.movejBlendValueSend = false;
                //qDebug() << "2*******last move of blend movej";
            }

            inMovejBlend = false;

        }else if (pm->kind == rsi_media::M_MOVEJ && pm->u.movej.rad > 0){
            //send MoveJ axis

            adsWriteStruct.progRunSpeed = pm->u.movej.speed * runSpeedPercent;
            //check program error
            if(adsWriteStruct.progRunSpeed > 100) adsWriteStruct.progRunSpeed = 100;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;
            adsWriteStruct.blockType = 1; //moveJ

            adsWriteStruct.movejBlend = true;


            if(!inMovejBlend){
                //first node of blend movej
                inMovejBlend = true;
                adsWriteStruct.dataFinish++;
                for(int i = 0; i < 7; i++){
                    adsWriteStruct.progMoveJAxis[i] = pm->u.movej.pos[i];
                    blendMovejTemp[i] = pm->u.movej.pos[i]; //save previous blend position
                }
                //qDebug() << "*******first node of blend movej";
            }else{
                //in blend movej
                for(int i = 0; i < 7; i++){
                    adsWriteStruct.progMoveJAxis[i] = blendMovejTemp[i];
                    adsWriteStruct.progMoveJAxisBlend[i] = pm->u.movej.pos[i];
                    blendMovejTemp[i] = pm->u.movej.pos[i]; //save previous blend position
                }
                //qDebug() << "*******in blend movej";
            }
            adsWriteStruct.movejBlendValueSend = true;
            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

            msleep(15);
            adsWriteStruct.movejBlendValueSend = false;

        }else if (pm->kind == rsi_media::M_MOVEJ_POSE){
            //end position movej
            for(int i = 0; i < 6; i++) adsWriteStruct.enteredXyzValue[i] = pm->u.movej.pos[i];
            adsWriteStruct.reserve[1] = true;

            adsWriteStruct.blockType = 1; //moveJ

            adsWriteStruct.progRunSpeed = pm->u.movej.speed * runSpeedPercent;
            //check program error
            if(adsWriteStruct.progRunSpeed > 100) adsWriteStruct.progRunSpeed = 100;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;

            adsWriteStruct.dataFinish++;
            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

        }else if (pm->kind == rsi_media::CALIB_POSE){
            //end position movej
            adsWriteStruct.reserve[2] = true;

            adsWriteStruct.blockType = 1; //moveJ

            adsWriteStruct.progRunSpeed = 30;

            adsWriteStruct.dataFinish++;
            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

        }else if (pm->kind == rsi_media::M_MOVEL && (pm->u.movel.rad <= 0 || stepRunModeEnable)){//no transition semidiameter
            inBlendMove = false;

            //send end position value
            for(int i = 0; i < 6; i++) adsWriteStruct.progMoveLAxis[i] = pm->u.movel.pos[i];

            //send MoveJ axis
            QString keyword = programCommand.at(pm->linenum-1).split('(').value(0);
            if(keyword == "MoveL"){
                adsWriteStruct.targetPsi = calculatePsi(pm->u.movel.axis);
            }else if(keyword == "movel"){
                if(abs(pm->u.movel.psi) <= 180)
                    adsWriteStruct.targetPsi = pm->u.movel.psi/3.1415926*180;
                else
                    adsWriteStruct.targetPsi = calculatePsi(adsReadStruct.axisCurrentAngle);
            }else{
                adsWriteStruct.targetPsi = calculatePsi(adsReadStruct.axisCurrentAngle);
            }

            adsWriteStruct.moveLPsi = true; //moveL don't fix psi

            //adsWriteStruct.semidiameter = 0; //no blend radius
            if(adsWriteStruct.blendStep > 0){
                //last move of all blend moves
                if(lastBlendMove == 1) adsWriteStruct.blockType = 10; //line to line
                if(lastBlendMove == 2) adsWriteStruct.blockType = 13; //arc to line
                //adsWriteStruct.semidiameter = 0;
                adsWriteStruct.blendStep++;
                stopBlend = true;
                qDebug() << "last moveL blend***";
            }else{
                adsWriteStruct.blockType = 2; //normal moveL
            }

            adsWriteStruct.progRunSpeed = pm->u.movel.speed * runSpeedPercent;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;

            adsWriteStruct.dataFinish++;

            currentCmdLine = pm->linenum - 1;//show current executing command line

            if(adsWriteStruct.blendStep > 0){
                msleep(200);
            }else{
                while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);
            }
            qDebug() << "******movel cmd rcv";

        }else if (pm->kind == rsi_media::M_MOVEL && pm->u.movel.rad > 0){//with blend radius
            currentProgKeyword = "MoveBlend";
            //send end position value

            for(int i = 0; i < 6; i++) adsWriteStruct.progMoveLAxis[i] = pm->u.movel.pos[i];

            adsWriteStruct.moveLPsi = true; //moveL don't fix psi

            adsWriteStruct.semidiameter = pm->u.movel.rad;

            adsWriteStruct.progRunSpeed = pm->u.movel.speed * runSpeedPercent;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;

            adsWriteStruct.dataFinish++;

            if(!inBlendMove){
                //runningBlend = true;
                inBlendMove = true;
                adsWriteStruct.blendStep = 1; //1:START
                adsWriteStruct.blockType = 10; //moveL with blend radius
                qDebug() << "movel blend start***";
            }else{
                adsWriteStruct.blendStep++; //2:RUN
                if(lastBlendMove == 1) adsWriteStruct.blockType = 10; //line to line
                if(lastBlendMove == 2) adsWriteStruct.blockType = 13; //arc to line
                qDebug() << "movel blend continue***";
            }
            lastBlendMove = 1;  //moveL blend
            currentCmdLine = pm->linenum - 1;//show current executing command line

            msleep(200);
            //while(!adsReadStruct.commandReceived && !stop){ msleep(progWaitTime);}

        }else if (pm->kind == rsi_media::M_MOVEC && (pm->u.movec.rad <= 0 || stepRunModeEnable)){ //without blend radius
            inBlendMove = false;
            //send second end position value
            for(int i = 0; i < 6; i++){
                adsWriteStruct.progMoveLAxis[i] = pm->u.movec.center[i];
                adsWriteStruct.progMoveLAxisMoveC[i] = pm->u.movec.end[i];
            }
            adsWriteStruct.progRunSpeed = pm->u.movec.speed * runSpeedPercent;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;
            adsWriteStruct.dataFinish++;

            if(adsWriteStruct.blendStep > 0){
                //last move of all blend moves
                if(lastBlendMove == 1) adsWriteStruct.blockType = 11; //line to arc
                if(lastBlendMove == 2) adsWriteStruct.blockType = 12; //arc to arc
                //adsWriteStruct.semidiameter = 0;
                adsWriteStruct.blendStep++;
                stopBlend = true;
                qDebug() << "last moveC blend";
            }else{
                adsWriteStruct.blockType = 3; //normal moveC
            }
            lastBlendMove = 2;  //moveC blend

            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

        }else if (pm->kind == rsi_media::M_MOVEC && pm->u.movec.rad > 0){ //with blend radius
            currentProgKeyword = "MoveBlend";
            //emit hideButtons(true, false, false, false); //blend move can't pause for now
            //send second end position value
            for(int i = 0; i < 6; i++){
                adsWriteStruct.progMoveLAxis[i] = pm->u.movec.center[i];
                adsWriteStruct.progMoveLAxisMoveC[i] = pm->u.movec.end[i];
            }
            adsWriteStruct.progRunSpeed = pm->u.movec.speed * runSpeedPercent;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;
            adsWriteStruct.semidiameter = pm->u.movec.rad;
            adsWriteStruct.dataFinish++;

            if(!inBlendMove){
                inBlendMove = true;
                adsWriteStruct.blendStep = 1; //1:START
                adsWriteStruct.blockType = 12; //moveL with blend radius
                qDebug() << "movec blend start***";
            }else{
                adsWriteStruct.blendStep++; //2:RUN
                if(lastBlendMove == 1) adsWriteStruct.blockType = 11; //line to arc
                if(lastBlendMove == 2) adsWriteStruct.blockType = 12; //arc to arc
                qDebug() << "movec blend continue***";
            }
            lastBlendMove = 2;


            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

        }else if (pm->kind == rsi_media::M_MOVETCP){
            //send end position value
            for(int i = 0; i < 6; i++) adsWriteStruct.progMoveLAxis[i] = adsReadStruct.currentFlangePosition[i];

            adsWriteStruct.moveLPsi = true; //moveL fix psi

            adsWriteStruct.blockType = 2; //normal moveL

            //for moveL-Z_modify to change the z value
            adsWriteStruct.tcpMove[0] = pm->u.tcpmove.pos[0];
            adsWriteStruct.tcpMove[1] = pm->u.tcpmove.pos[1];
            adsWriteStruct.tcpMove[2] = pm->u.tcpmove.pos[2];

            adsWriteStruct.targetPsi = calculatePsi(adsReadStruct.axisCurrentAngle);

            adsWriteStruct.tcpMoveActive = true;


            adsWriteStruct.progRunSpeed = pm->u.tcpmove.speed * runSpeedPercent;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;

            adsWriteStruct.dataFinish++;

            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

        }else if (pm->kind == rsi_media::M_BD){//temporary function ±ê¶¨
            //send end position value
            for(int i = 0; i < 6; i++) adsWriteStruct.progMoveLAxis[i] = pm->u.bd.pos[i];

            adsWriteStruct.blockType = 8; //biaoding moveL

            adsWriteStruct.progRunSpeed = pm->u.bd.speed * runSpeedPercent;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;

            adsWriteStruct.dataFinish++;

            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

        }else if (pm->kind == rsi_media::M_MOVEPSI){
            //this command will move the psi angle of the arm(end fix move)
            for(int i = 0; i < 7; i++) adsWriteStruct.progMoveJAxis[i] = pm->u.movepsi.pos[i];
            adsWriteStruct.progRunSpeed = pm->u.movepsi.speed * runSpeedPercent;
            //check program error
            if(adsWriteStruct.progRunSpeed > 100) adsWriteStruct.progRunSpeed = 100;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;
            adsWriteStruct.blockType = 5; //movePsi
            adsWriteStruct.dataFinish++;

            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);

        //}else if (currentProgKeyword == "socket_open"){
        //
        //    //QString address = parameterPieces.value(0);
        //    //int port = parameterPieces.value(1).toInt();
        //    //int timeout = parameterPieces.value(2).toInt();
        //    //emit fun_socket_open(address, port, timeout);
        //    currentCmdLine++;//show current executing command line
        //    functionFinish = false; //block the program
        //
        //}else if (currentProgKeyword == "socket_rcv"){
        //
        //    //int timeout = parameterPieces.value(0).toInt();
        //    //emit fun_socket_rcv(timeout);
        //    currentCmdLine++;//show current executing command line
        //    functionFinish = false; //block the program
        //
        //}else if (currentProgKeyword == "socket_send"){
        //
        //    //QString message = parameterPieces.value(0);
        //    //emit fun_socket_send(message);
        //    currentCmdLine++;//show current executing command line
        //    functionFinish = false; //block the program
        //
        //}else if (currentProgKeyword == "socket_close"){
        //
        //    emit fun_socket_close();
        //    currentCmdLine++;//show current executing command line
        //    functionFinish = false; //block the program
        //
        //}else if(currentProgKeyword == "save_status"){
        //    //save the current robot status
        //    LogFile::currentStatusSave();
        //
        //    currentCmdLine++;//show current executing command line
        }else if(pm->kind == rsi_media::M_SETTCP){
            //set the tool coordinate
            adsWriteStruct.toolCoordinate[0] = pm->u.settcp.pos[0];
            adsWriteStruct.toolCoordinate[1] = pm->u.settcp.pos[1];
            adsWriteStruct.toolCoordinate[2] = pm->u.settcp.pos[2];
            adsWriteStruct.toolCoordinateR[0] = pm->u.settcp.pos[3];
            adsWriteStruct.toolCoordinateR[1] = pm->u.settcp.pos[4];
            adsWriteStruct.toolCoordinateR[2] = pm->u.settcp.pos[5];
            msleep(200);
            adsWriteStruct.toolCoordinateProg = true;

            //emit changeToolCoordinate(parameterPieces);

            currentCmdLine = pm->linenum - 1;//show current executing command line

            passWhile = true;
        }else if(pm->kind == rsi_media::M_CLEANTCP){
            adsWriteStruct.toolCoordinateProg = false;

            currentCmdLine = pm->linenum - 1;//show current executing command line

            passWhile = true;
        }else if(pm->kind == rsi_media::GRIPPER){
            //gripper control
            currentCmdLine = pm->linenum - 1;//show current executing command line

            adsWriteStruct.gripperMoveLength = pm->u.gripper.num;

            //adsWriteStruct.tcpMoveActive = true;
            adsWriteStruct.reserve[0] = true;

            while((!adsReadStruct.gripperDone) && (!adsWriteStruct.progStop)) msleep(10);

            //adsWriteStruct.tcpMoveActive = false;
            adsWriteStruct.reserve[0] = false;

            msleep(200);

            passWhile = true;

        }else if(pm->kind == rsi_media::REPLAY){
            //set the tool coordinate
            adsWriteStruct.pathNum = pm->u.replay.num;
            adsWriteStruct.progRunSpeed = pm->u.replay.speed;

            if(adsWriteStruct.progRunSpeed > 100) adsWriteStruct.progRunSpeed = 100;
            if(adsWriteStruct.progRunSpeed <= 0) adsWriteStruct.progRunSpeed = 1;
            adsWriteStruct.blockType = 15; //replay

            adsWriteStruct.dataFinish++;

            currentCmdLine = pm->linenum - 1;//show current executing command line
            while(!adsReadStruct.commandReceived && !stop) msleep(progWaitTime);
        }else{
            //unknow function or suppressed command(currentProgKeyword.at(0) == '#'), just add the command line pointer
            currentCmdLine = pm->linenum - 1;//show current executing command line
            passWhile = true;
        }

        //detect if there is a collision
        if(adsReadStruct.collisionDetected){
            if(!collisionSignalSend){
                emit collisionWarning((int)adsReadStruct.collisionID);
                collisionSignalSend = true;
            }
        }

        //wait for the command finish
        int i = 0;
        qDebug() << "***kind:" << pm->kind;
        if(pm->kind == 5){
            //no action command, can be paused here
            while(adsWriteStruct.progPause && (!stop) && (!error)){
                stopOrRunEnable = true;
                msleep(progCommandWaitTime);
            }
            stopOrRunEnable = false;
        }

        oldMovejRTS = adsReadStruct.movejRTS;
        while(((!adsReadStruct.oneCommandFinish) && (!stop) && (!error) && (!passWhile) && (!adsReadStruct.stopClicked) && (!adsReadStruct.RTS_Blends)) || (!functionFinish)){
            //while waiting for one command finish, detect if there is a collision
            if(adsReadStruct.collisionDetected){
                if(!collisionSignalSend){
                    emit collisionWarning((int)adsReadStruct.collisionID);
                    collisionSignalSend = true;
                }

                if(adsWriteStruct.progStop) break;
            }


            //movej blend, raising edge
            if(adsReadStruct.movejRTS != oldMovejRTS){
                oldMovejRTS = adsReadStruct.movejRTS;
                if(adsReadStruct.movejRTS) break;
            }

            //qDebug() << "**inside loop" << i++ << adsReadStruct.RTS_Blends << adsReadStruct.oneCommandFinish << stop << adsReadStruct.stopClicked << adsReadStruct.movejRTS;
            msleep(progCommandWaitTime);
        }

        qDebug() << "outside loop" << adsReadStruct.RTS_Blends << adsReadStruct.oneCommandFinish << stop << adsReadStruct.stopClicked << adsReadStruct.movejRTS;
        passWhile = false;
        adsWriteStruct.tcpMoveActive = false;
        adsWriteStruct.reserve[1] = false;  //movej use flange position
        adsWriteStruct.reserve[2] = false;

        //wait for blend move stop
        if(stopBlend  && !stop && !adsReadStruct.oneCommandFinish){ //&& !adsWriteStruct.progStop
            //stop the blend move
            stopBlend = false;
            for(int i = 0; i < 6; i++) adsWriteStruct.progMoveLAxis[i] = 0; //any point different to previous one
            for(int i = 0; i < 6; i++) adsWriteStruct.progMoveLAxisMoveC[i] = 1; //any point different to previous one

            adsWriteStruct.semidiameter = 0;
            if(pm->kind == rsi_media::M_MOVEL){
                adsWriteStruct.blockType = 10; //line to line
            }else{
                adsWriteStruct.blockType = 12; //arc to arc
            }

            adsWriteStruct.progRunSpeed = 0;
            currentCmdLine = pm->linenum - 1;//show current executing command line


            adsWriteStruct.blendStep = -1;  //last move

            while(!adsReadStruct.reserve[0] && !stop && !adsReadStruct.moveLCBlendFinish){ msleep(progWaitTime);}

            adsWriteStruct.blendStep = -2; //last blend step

            while(adsReadStruct.reserve[0] && !stop && !adsReadStruct.moveLCBlendFinish) {msleep(progWaitTime);}

            adsWriteStruct.blendStep = -3; //tell PLC last blend step is done, time to init

            while(!adsReadStruct.reserve[0] && !stop && !adsReadStruct.moveLCBlendFinish) {msleep(progWaitTime);}
            qDebug() << "**last blend 5";
            adsWriteStruct.dataFinish++;
        }

        pm = (rsi_media*)RSI_execNext();    //check next command
        if(pm == NULL) {
            QString msg;
            msg.sprintf("%s",RSI_getErrMsg());
            emit programError(msg);
            error = true;
            return;
        }

        //let the program pass the step run while
        if(!stepRunModeEnable)// || runningBlend)
            stepRun = true;
        else
            stepRun = false;

        //if step run mode is actived, wait for next click
        while((!adsWriteStruct.progStop) && (!stepRun)) msleep(progCommandWaitTime);

    }

    //release the running status
    adsWriteStruct.blendMoveStopClick = false;
    adsWriteStruct.blendStep = 0;
    adsWriteStruct.blockType = 4; //program finished
    adsWriteStruct.progRun = false;
    adsWriteStruct.dataFinish = -1;
    msleep(200);

}
