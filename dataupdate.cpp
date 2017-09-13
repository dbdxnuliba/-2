#include "dataupdate.h"
#include "qfile.h"
#include <QTcpSocket>
#include <QDebug>
#include "Log/logfile.h"
#include "./UI/warningwindow.h"

QString fileSavePath = "C:\\SiasunRobot";
QString systemLanguage = "cn";

int progBackupPtr = 0;

bool robotModel5Kg = true;

bool systemError = true;
bool stopOrRunEnable = false;
int currentCmdLine = 0;
int oldCurrentCmdLine = -1;
int sendStatusFeedbackCycle = 100;

bool oldEndButton = false;
bool oldEndButton2 = false;

bool toolButtonSaveNode = false;

//current angle of axis
double axisCurrentAngle[7] = {0};

//current position of flange
double currentFlangePosition[6] = {0};

//flange/axis move direction
short flangeMoveDirection[6];
short axisAngleMoveDirection[7];

//program is running
bool Running = false;
bool shutDown = false;
bool initial = true;
bool resetRobot = true;

//program Run Speed
float runSpeedPercent = 1;

//for TCP communication
bool programIsRunning = false;

//tcp port connect status
bool connected2000 = false;
bool connected2001 = false;

//a function in program is finished
bool functionFinish = true;

bool virtualWallSetted = false;

//target position of axis in moveTo function
double programAxisMoveTo[7];

short jogModeValue;

int programLineHeight = 35; //height of each program lines
int totalCommandLineNum = 0;
bool locked = false; //used to lock the jog screen

//collision detected
bool collisionSignalSend = false;

bool startReadXML = false;

//expert mode
bool expertMode = false;

//main window full screen
bool mianWindowFullScreen = false;
bool stopProgramShutDownMachine = false;
int maxLineMoveSpeed = 1000;
bool back2InitPoint = false;
bool debugDontCheckEmcBtn = false;

bool progThreadSemaphore = true;

int jogSpeed = 50;

bool joint4Exceed = false; //joint 4 is near 0, exceed limit

bool selfCollision = false; //detect if robot will collide itself
bool oldSelfCollision = false;
bool checkSlaveStatus;
bool oldSaveStatus = false;
bool emcStopOldStatus = true;

bool touchVirtualWall = false; //detect if robot touch the virtual wall
bool oldTouchVirtualWall = false;

bool axisWarningOn = false;

double targetPose1[4] = {0}; //target pose read from camera (x y z RzRot)
double targetPose2[4] = {0};
double targetPose3[4] = {0};
double targetPose4[4] = {0};

std::vector<QString> programCommand;

extern "C" int robot_ioIn[8];
extern "C" int robot_ioOut[8];
extern "C" int tool_ioIn[8];
extern "C" int tool_ioOut[8];
extern "C" double end_pos[6];
extern "C" double joint_pos[7];


//last pressed line move button
QString lastClickedMoveButton = "none";

QString currentProgKeyword;


AdsMessage_Write    adsWriteStruct;
AdsMessage_Read     adsReadStruct;
Status_Struct       _status_struct;
XML_Struct          xmlReadStruct;
XML_Struct          xmlWriteStruct;


DataUpdate::DataUpdate():
    //axisOldAngle{},
    //oldFlangePosition{},
    oldMoveSpeed(0),
    adsErrorWrite(0),
    adsErrorRead(0)
{
    for(int i = 0; i < 7; i++) axisOldAngle[i] = 0;
    for(int i = 0; i < 6; i++) oldFlangePosition[i] = 0;
    for(int i = 0; i < 7; i++) adsWriteStruct.teachSmooth[i] = 50;
}

DataUpdate::~DataUpdate(){}


void DataUpdate::run()
{
    update();
}

bool spiDisabled = false;
bool jointLimitOldStatus = false;
bool workingLimitOldStatus = false;
bool oldPowerOn = false;
bool oldBrakeDone = false;
bool oldAxis_error = false;

void DataUpdate::update()
{    
    //check connection method:local or remote
    //read the init file
    char connectType = 'l'; //l: local, r: remote
    QString tcpAddress;
    QFile file(fileSavePath + "\\config.init");;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //check connect method
            if(keywordPieces.value(0).trimmed() == "REMOTE"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    connectType = 'r';
                    tcpAddress = keywordPieces.value(2).trimmed();
                }
            }
        }
    }


    //method 1: local ads connection
    char adsWritePath [] = "GVL.mes_tp2c";
    char adsReadPath [] = "GVL.mes_c2tp";
    //read XML
    char xmlReadPath [] = "GVL.xml_write";


    adsErrorWrite = 0;
    adsErrorRead = 0;


    //method 2: remote connect to server
    QTcpSocket *fun_socket;
    if(connectType == 'r'){
        fun_socket= new QTcpSocket(this);
        fun_socket->connectToHost(tcpAddress, 573);
        if(!fun_socket->waitForConnected(1000)) emit tcpErrorMessage(tcpAddress);
    }

    //int t = 0;
    while((!stop) && (!adsErrorWrite) && (!adsErrorRead)){

        //timeTest.start(); //cycle time test
        if(connectType == 'l'){
            //local ads connection
            adsErrorWrite = ADS::getAdsInstance()->adsWrite(adsWritePath, sizeof(adsWritePath), sizeof(adsWriteStruct), &adsWriteStruct);
            adsErrorRead = ADS::getAdsInstance()->adsRead(adsReadPath, sizeof(adsReadPath), sizeof(adsReadStruct), &adsReadStruct); //read from PLC

            if(startReadXML) ADS::getAdsInstance()->adsRead(xmlReadPath, sizeof(xmlReadPath), sizeof(xmlReadStruct), &xmlReadStruct); //read from PLC
        }else{
            //connectType == 'r'
            //tcp send message
            fun_socket->write((char*)&adsWriteStruct, (qint64)sizeof(adsWriteStruct));
            //tcp receive message
            fun_socket->waitForBytesWritten(500);
            fun_socket->waitForReadyRead(500);
            fun_socket->read((char*)&adsReadStruct, (qint64)sizeof(adsReadStruct));
        }


        if(adsReadStruct.saveStatus != oldSaveStatus){
            oldSaveStatus = adsReadStruct.saveStatus;
            if(adsReadStruct.saveStatus){
                //save the current robot status
                LogFile::currentStatusSave();
            }
        }



        if(adsReadStruct.motionMachine == 4)
            //on traction teach mode, disable all the window
            teachLockScreen(true);
        else
            teachLockScreen(false);


        //if the value is updated, emit a signal to update UI
        for(int i = 0; i < 7; i++){
            if(axisOldAngle[i] != adsReadStruct.axisCurrentAngle[i])
            {
                emit axisAngleValueChanged(adsReadStruct.axisCurrentAngle[i], i+1);
                emit axisChangedUpdate3DView(adsReadStruct.axisCurrentAngle); //update 3D view
            }
            axisOldAngle[i] = adsReadStruct.axisCurrentAngle[i];
        }

        //&***************************************Flange Position***************************************&//
        //read and update real-time Flange Position status
        //position sync&read data

        for(int i = 0; i < 6; i++){
            //check use flange position with tool coordinate or not
            if(adsWriteStruct.statusUseToolCoordinate){
                if(oldFlangePosition[i] != adsReadStruct.currentFlangePositionTool[i])
                    emit flangePositionChanged(adsReadStruct.currentFlangePositionTool[i], i+1);

                oldFlangePosition[i] = adsReadStruct.currentFlangePositionTool[i];
            }else{
                if(oldFlangePosition[i] != adsReadStruct.currentFlangePosition[i])
                    emit flangePositionChanged(adsReadStruct.currentFlangePosition[i], i+1);

                oldFlangePosition[i] = adsReadStruct.currentFlangePosition[i];
            }

        }



        //highlight the current executing command line
        if(programIsRunning){
            if(currentCmdLine != oldCurrentCmdLine){
                emit showCommandLine(currentCmdLine);
                oldCurrentCmdLine = currentCmdLine;
            }
        }


        //check joint limit
        if(jointLimitOldStatus != adsReadStruct.exceedJointLimit){
            jointLimitOldStatus = adsReadStruct.exceedJointLimit;
            if(jointLimitOldStatus) emit exceedJointLimit();
            else emit exceedJointLimitClear();
        }

        //check working limit
        if(workingLimitOldStatus != adsReadStruct.exceedWorkingLimit){
            workingLimitOldStatus = adsReadStruct.exceedWorkingLimit;
            if(workingLimitOldStatus){
                emit exceedJointLimit();
                joint4Exceed = true;
            }
            else emit exceedJointLimitClear();
        }


        if(abs(adsReadStruct.axisCurrentAngle[3]) > 10 && joint4Exceed){
            joint4Exceed = false;
            emit exceedJointLimitClear();
        }


        //check if joint3 value is near 0, if so, disable the movePsi buttons
        if(abs(adsReadStruct.axisCurrentAngle[2]) < 0.5){
            //disable jog-psi button
            emit switchJogSpiButton(false);
            spiDisabled = true;
        }else if(spiDisabled){
            emit switchJogSpiButton(true);
            spiDisabled = false;
        }


        //box IO
        emit externalIOReadUpdate(adsReadStruct.externalIORead);      
        for(int i = 0; i < 6; i++){
            robot_ioIn[i] = adsReadStruct.externalIORead[i];
            adsWriteStruct.io_output_num[i] = robot_ioOut[i];
        }
        emit externalIOWriteUpdate(adsWriteStruct.io_output_num);

        //tool IO
        emit externalToolIOReadUpdate(adsReadStruct.externalToolIORead);
        for(int i = 0; i < 6; i++) tool_ioIn[i] = adsReadStruct.externalToolIORead[i];
        for(int i = 0; i < 4; i++) adsWriteStruct.tool_io_output_num[i] = tool_ioOut[i];
        emit externalToolIOWriteUpdate(adsWriteStruct.tool_io_output_num);


        //current position use in program
        for(int i = 0; i < 6; i++) end_pos[i] = adsReadStruct.currentFlangePosition[i];
        for(int i = 0; i < 7; i++) joint_pos[i] = adsReadStruct.axisCurrentAngle[i];


        //end tool button pressed
        if(adsReadStruct.endButton != oldEndButton){
            oldEndButton = adsReadStruct.endButton;
            emit endToolButtonStatus(adsReadStruct.endButton);
        }

        //second end tool button pressed
        if(adsReadStruct.endButton2 != oldEndButton2){
            oldEndButton2 = adsReadStruct.endButton2;
            if(adsReadStruct.endButton2) emit pressButtonAddNode();
        }


        if(adsReadStruct.resetSystem) emit resetSystem();


        if(adsReadStruct.powerOn != oldPowerOn){
            oldPowerOn = adsReadStruct.powerOn;
            if(!adsReadStruct.powerOn) emit robotPowerOff();
        }


        if(adsReadStruct.brakeDone != oldBrakeDone){
            oldBrakeDone = adsReadStruct.brakeDone;
            emit robotBreakState(adsReadStruct.brakeDone);
        }

        //emergency stop pressed
        if(emcStopOldStatus != adsReadStruct.emcStopSignal){
            emcStopOldStatus = adsReadStruct.emcStopSignal;
            emit emcBtnStatusChange(adsReadStruct.emcStopSignal);
        }

        if(adsReadStruct.axis_error != oldAxis_error){
            oldAxis_error = adsReadStruct.axis_error;
            if(adsReadStruct.axis_error) emit axisError();

        }

        if(selfCollision != oldSelfCollision){
            oldSelfCollision = selfCollision;
            if(selfCollision) emit selfCollisionDetect();
        }

        if(touchVirtualWall != oldTouchVirtualWall){
            oldTouchVirtualWall = touchVirtualWall;
            if(touchVirtualWall) emit wallCollisionDetect();
        }


        //axis warning
        if(adsReadStruct.warningID[0] != 0 || adsReadStruct.warningID[1] != 0 || \
                adsReadStruct.warningID[2] != 0 || adsReadStruct.warningID[3] != 0 || \
                adsReadStruct.warningID[4] != 0 || adsReadStruct.warningID[5] != 0 || \
                adsReadStruct.warningID[6] != 0){
            if(!axisWarningOn){
                emit axisWarning();
                axisWarningOn = true;
            }

        }

        if(showRunTime) emit showProgramRunTime(timer.elapsed());

        if(checkSlaveStatus) emit updateSlaveStatus(adsReadStruct.slave_ready);

        //if(t < timeTest.elapsed()){
        //    t = timeTest.elapsed();
        //    qDebug() << "Worst data sync cycle:" << timeTest.elapsed() << "ms";
        //}
    }

    if(adsErrorRead || adsErrorWrite){
        emit adsErrorMessage(adsErrorRead);
    }


    if(connectType == 'r') fun_socket->close();
}

void DataUpdate::startTimeCount()
{
    timer.start();
    showRunTime = true;
}

void DataUpdate::stopTimeCount()
{
    showRunTime = false;
}

