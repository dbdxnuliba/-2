#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <math.h>
#include <QThread>
#include <math.h>
#include "Log/logfile.h"
#include <QTcpSocket>
#include <QTextCodec>
#include <thread>
#include <QMovie>
#include <direct.h>
#include <Windows.h>
#include <QElapsedTimer>
#include <QDomDocument>
#include <QtXml>

extern QString systemLanguage;

extern bool Running;
extern bool shutDown;
extern int totalCommandLineNum;
extern AdsMessage_Write adsWriteStruct;
extern AdsMessage_Read  adsReadStruct;
extern Status_Struct    _status_struct;
extern XML_Struct       xmlReadStruct;
extern XML_Struct       xmlWriteStruct;

extern bool functionFinish;
extern bool systemError;
extern bool startReadXML;
extern bool axisWarningOn;

extern bool stopOrRunEnable;
extern int sendStatusFeedbackCycle;

extern int progBackupPtr;
int progBackupPtrMin = 0;
int progBackupPtrMax = 0;

extern bool programIsRunning;
extern bool collisionSignalSend;
extern double programAxisMoveTo[7];
extern QString fileSavePath;
extern float runSpeedPercent;


extern int programLineHeight;
extern bool expertMode;

extern QString lastClickedMoveButton;

//config.init file
extern bool mianWindowFullScreen;
extern bool stopProgramShutDownMachine;
extern int maxLineMoveSpeed;
extern bool back2InitPoint;

extern bool progThreadSemaphore;

extern bool joint4Exceed;

extern bool checkSlaveStatus;

extern bool toolButtonSaveNode;

extern double targetPose1[4]; //(x y z RzRot)
extern double targetPose2[4]; //(x y z RzRot)
extern double targetPose3[4]; //(x y z RzRot)
extern double targetPose4[4]; //(x y z RzRot)

extern int jogSpeed;

extern QString currentProgKeyword;


bool unlock = true; //used to prevent multiple buttons pressed at the same time

bool copySpeedButtonClicked = false; //for change the speed of multiple nodes quickly

bool programOpened = false; //a value to memory a program is modifying or not
bool hasAdsError = false; //a value to memory an ads error happened or not
bool selectProgramToRun = false; //this value will determine to open the program for edit or for execute
bool repeatProgram = false; //repeat current program
bool selectDefaultProgram = false;
bool autoLoginActive = false;
bool startUpUseTool = false;
QString copiedSpeed;
QString serialPortNum = "com3"; //com3 for 3rd version, com2 for 2nd version
bool ipDHCPMode = true;
extern bool virtualWallSetted;

int toolCoordinateSaveLoop = 0;
double toolCoordinateSave[5][6];

//QString square;
//square = ui->accelerationUnit->text().at(8);
QString createName;
QString copyNode = "";

int oldLineNum = -1;
int runProgramRepeatTime = 0;

bool robotScript = false; //the opened program is a robot script program or not

bool forceTeachMode = false;

bool checkStartPoint = true; //check

bool inCameraProgram = false;   //camera programing now or not

extern std::vector<QString> programCommand;

extern bool robotModel5Kg;

extern "C" int robot_ioIn[8];
extern "C" int robot_ioOut[8];
extern "C" int tool_ioIn[4];
extern "C" int tool_ioOut[4];
extern "C" double movePosition[13][6];

//extern "C" int RSI_initEnv();
//extern "C" int RSI_end();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)                 //main window UI
{
    ui->setupUi(this);

    //set window background
    QPixmap bkgnd(":/images/background.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);



    updateData=new DataUpdate();             //a thread to update real-time data
    runProgram=new RunProgram();             //explain the script program to action commands
    nodeWindow=new AddNodeWindow();          //add node window
    numberBoard=new NumberBoard();           //number board window
    keyboard=new Keyboard();                 //keyboard window
    loadingThread=new LoadingThread();       //load gif image thread
    moveToThread=new MoveToThread();         //move to function in a thread
    cal = new calibration();                 //camera calibration

    connect(loadingThread, SIGNAL(loadDone()), this, SLOT(startUpLoadDone()));
    connect(updateData, SIGNAL(emcBtnStatusChange(bool)), this, SLOT(emcButtonCheckThreadHandler(bool)));

    //slot for updating progress bar
    connect(updateData,SIGNAL(axisAngleValueChanged(double,int)), this, SLOT(updateAxisPosition(double,int)));

    //slot for updating flange position
    connect(updateData,SIGNAL(flangePositionChanged(double,int)), this, SLOT(updateFlangePosition(double,int)));

    //slot for updating 3D view
    connect(updateData,SIGNAL(axisChangedUpdate3DView(double*)), this, SLOT(update3DView(double*)));

    //update axis slave status
    connect(updateData,SIGNAL(updateSlaveStatus(bool*)), this, SLOT(updateSlaveStatus(bool*)));


    //slot for data update error
    connect(updateData,SIGNAL(adsErrorMessage(long)), this, SLOT(updateDataAdsError(long)));
    connect(updateData,SIGNAL(tcpErrorMessage(QString)), this, SLOT(updateDataTcpError(QString)));

    //lock/unlock the jog screen
    connect(updateData,SIGNAL(lockJogScreen(bool)), this, SLOT(lockJogScreen(bool)));
    connect(updateData,SIGNAL(teachLockScreen(bool)), this, SLOT(teachLockScreen(bool)));


    //joint limit warning
    connect(updateData,SIGNAL(exceedJointLimit()), this, SLOT(exceedJointLimit()));
    connect(updateData,SIGNAL(exceedJointLimitClear()), this, SLOT(exceedJointLimitClear()));
    connect(updateData,SIGNAL(switchJogSpiButton(bool)), this, SLOT(switchJogSpiButton(bool)));

    //IO read
    connect(updateData,SIGNAL(externalIOReadUpdate(bool*)), this, SLOT(externalIOReadUpdate(bool*)));
    connect(updateData,SIGNAL(externalToolIOReadUpdate(bool*)), this, SLOT(externalToolIOReadUpdate(bool*)));

    //end button pressed or not
    connect(updateData,SIGNAL(endToolButtonStatus(bool)), this, SLOT(endToolButtonStatus(bool)));

    //reset system
    connect(updateData,SIGNAL(resetSystem()), this, SLOT(resetSystem()));

    //robot power off
    connect(updateData,SIGNAL(robotPowerOff()), this, SLOT(robotPowerOff()));

    //robot break state
    connect(updateData,SIGNAL(robotBreakState(bool)), this, SLOT(robotBreakState(bool)));

    //axis has error
    connect(updateData,SIGNAL(axisError()), this, SLOT(axisError()));

    //axis warning
    connect(updateData,SIGNAL(axisWarning()), this, SLOT(axisWarning()));

    //tool button2 press to save node
    connect(updateData,SIGNAL(pressButtonAddNode()), this, SLOT(pressButtonAddNode()));



    //robot self collision detect
    connect(updateData,SIGNAL(selfCollisionDetect()), this, SLOT(selfCollisionDetect()));
    connect(updateData,SIGNAL(wallCollisionDetect()), this, SLOT(wallCollisionDetect()));


    //start/stop count program run time
    connect(this,SIGNAL(startTimeCount()), updateData, SLOT(startTimeCount()));
    connect(this,SIGNAL(stopTimeCount()), updateData, SLOT(stopTimeCount()));
    connect(updateData,SIGNAL(showProgramRunTime(int)), this, SLOT(showProgramRunTime(int)));

    //slot for adding nodes
    connect(nodeWindow,SIGNAL(moveCSelected()), this, SLOT(addMoveCNode()));
    connect(nodeWindow,SIGNAL(moveLSelected(bool)), this, SLOT(addMoveLNode(bool)));
    connect(nodeWindow,SIGNAL(moveJSelected()), this, SLOT(addMoveJNode()));
    connect(nodeWindow,SIGNAL(movePsiSelected()), this, SLOT(addMovePsiNode()));
    connect(nodeWindow,SIGNAL(functionCreated(QString)), this, SLOT(addProgramFunction(QString)));
    connect(nodeWindow,SIGNAL(threadSelected()), this, SLOT(addThreadNode()));
    connect(nodeWindow,SIGNAL(startTeach()), this, SLOT(startTeach()));
    connect(nodeWindow,SIGNAL(addReplayNode()), this, SLOT(addReplayNode()));

    //slot for run command
    connect(updateData,SIGNAL(showCommandLine(int)), this, SLOT(runCommand(int)));
    connect(updateData,SIGNAL(externalIOWriteUpdate(bool*)), this, SLOT(externalIOWriteUpdate(bool*)));
    connect(updateData,SIGNAL(externalToolIOWriteUpdate(bool*)), this, SLOT(externalToolIOWriteUpdate(bool*)));

    //slot to stop run program
    connect(runProgram,SIGNAL(stopRun()), this, SLOT(stopRunProgram()));
    connect(runProgram,SIGNAL(runError()), this, SLOT(stopRunProgram()));
    connect(runProgram,SIGNAL(programError(QString)), this, SLOT(programError(QString)));
    //connect(runProgram,SIGNAL(stopProgramThread()), this, SLOT(stopRunProgramThread()));
    connect(runProgram,SIGNAL(checkRepeat()), this, SLOT(checkRepeatProgram()));
    connect(runProgram,SIGNAL(runTargetProg(QString)), this, SLOT(runTargetProgram(QString)));
    connect(runProgram,SIGNAL(stopRepeat()), this, SLOT(stopRepeat()));


    //a collision detected while the program is running
    connect(runProgram,SIGNAL(collisionWarning(int)), this, SLOT(collisionDetected(int)));
    connect(runProgram, SIGNAL(changeRunButtonStatus(bool)),this,SLOT(changeRunButtonStatus(bool)));

    //program functions:
    connect(runProgram,SIGNAL(fun_socket_open(QString, int, int)), this, SLOT(fun_socket_open(QString, int, int)));
    connect(runProgram,SIGNAL(fun_socket_send(QString)), this, SLOT(fun_socket_send(QString)));
    connect(runProgram,SIGNAL(fun_socket_rcv(int)), this, SLOT(fun_socket_rcv(int)));
    connect(runProgram,SIGNAL(fun_socket_close()), this, SLOT(fun_socket_close()));
    //connect(runProgram,SIGNAL(threadStart()), this, SLOT(threadStart()));
    connect(runProgram,SIGNAL(changeToolCoordinate(QStringList)), this, SLOT(changeToolCoordinate(QStringList)));
    connect(runProgram,SIGNAL(hideButtons(bool, bool, bool, bool)), this, SLOT(hideRunButtons(bool, bool, bool, bool)));
    //connect(runProgram,SIGNAL(externalIOWriteUpdate(int)), this, SLOT(externalIOWriteUpdate(int)));


    //slot for number board
    connect(numberBoard,SIGNAL(sendNumber(int, QString)), this, SLOT(updateProgramNumber(int, QString)));
    connect(numberBoard,SIGNAL(stopEnterValueMove()), this, SLOT(stopEnterValueMove()));
    connect(numberBoard,SIGNAL(startEnterValueMove()), this, SLOT(startEnterValueMove()));

    //slot for keyboard
    connect(keyboard,SIGNAL(sendText(int, QString)), this, SLOT(updateKeyboardText(int, QString)));


    //set button transparent
    ui->stopJogMove1->setVisible(false);
    //disable all zoom buttons for 3D view
    ui->zoomIn_Run->setVisible(false);
    ui->zoomOut_Run->setVisible(false);
    ui->reset_Run->setVisible(false);
    ui->up3D->setVisible(false);
    ui->down3D->setVisible(false);
    ui->left3D->setVisible(false);
    ui->right3D->setVisible(false);


    //tcp server to communicate with other PC (receive message to run related program)
    Server = new TcpServer(this, PORT_RECEIVE);
    Server->startServer();
    connect(Server, SIGNAL(sendPathAgain(QString)), this, SLOT(runTargetProgram(QString)));
    connect(Server, SIGNAL(setRunProgramSpeed2UI(int)), this, SLOT(setRunProgramSpeed2UI(int)));
    connect(Server, SIGNAL(stopScript()), this, SLOT(stopScript()));
    connect(Server, SIGNAL(pauseScript()), this, SLOT(pauseScript()));
    connect(Server, SIGNAL(resumeScript()), this, SLOT(resumeScript()));
    connect(runProgram, SIGNAL(stopRun()), Server, SLOT(sendRunStopSignal()));
    connect(runProgram, SIGNAL(runError()), Server, SLOT(sendRunErrorSignal()));
    connect(this, SIGNAL(sendErrorMsg2Client(QString)), Server, SLOT(sendErrorMsg2Client(QString)));
    connect(Server, SIGNAL(tcpConnected(bool, bool)), this, SLOT(tcpConnected(bool, bool)));



    //tcp server send status feedback
    StatusFeedback = new TcpServer(this, PORT_STATUS);
    StatusFeedback->startServer();
    connect(StatusFeedback, SIGNAL(tcpConnected(bool, bool)), this, SLOT(tcpConnected(bool, bool)));


    //read the serial port number from init file
    QFile file(fileSavePath + "\\config.init");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');
            if(keywordPieces.value(0).trimmed() == "SERIAL") serialPortNum = keywordPieces.value(1).trimmed();
        }
    }

    file.close();

    //init serial port
    serialPort = new QSerialPort(this);
    serialPort->setPortName(serialPortNum);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    qDebug() << "Serial port" << serialPortNum << serialPort->open(QIODevice::ReadWrite);
    connect(serialPort, SIGNAL(readyRead()),this,SLOT(getSerialPortMsg()));

    //init interpretor
    //if (RSI_initEnv() != 0) qDebug() << "robot interperter init error!";
}



MainWindow::~MainWindow()
{
    //close interpretor
    //if (RSI_end() != 0) qDebug() << "release compilor space error!";
    updateData->stop = true;
    serialPort->close();
    //if(myProcess->isOpen()) myProcess->close();
    delete ui;
    delete updateData;
    delete runProgram;
    delete nodeWindow;
    delete numberBoard;
    delete keyboard;
    delete loadingThread;
    delete moveToThread;
}


void MainWindow::updateAxisPosition(double value, int axisNum)
{
    if(abs(value) < 0.0049) value = 0;

    if(axisNum == 1){
        ui->axis1ProgressBar->setValue(value);
        ui->axis1ProgressBar->setFormat(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis1->setText(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis1_status->setText(QString::number( value, 'f', 2 ));
    }
    if(axisNum == 2){
        ui->axis2ProgressBar->setValue(value);
        ui->axis2ProgressBar->setFormat(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis2->setText(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis2_status->setText(QString::number( value, 'f', 2 ));
    }
    if(axisNum == 3){
        ui->axis3ProgressBar->setValue(value);
        ui->axis3ProgressBar->setFormat(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis3->setText(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis3_status->setText(QString::number( value, 'f', 2 ));
    }
    if(axisNum == 4){
        ui->axis4ProgressBar->setValue(value);
        ui->axis4ProgressBar->setFormat(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis4->setText(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis4_status->setText(QString::number( value, 'f', 2 ));
    }
    if(axisNum == 5){
        ui->axis5ProgressBar->setValue(value);
        ui->axis5ProgressBar->setFormat(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis5->setText(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis5_status->setText(QString::number( value, 'f', 2 ));
    }
    if(axisNum == 6){
        ui->axis6ProgressBar->setValue(value);
        ui->axis6ProgressBar->setFormat(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis6->setText(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis6_status->setText(QString::number( value, 'f', 2 ));
    }
    if(axisNum == 7){
        ui->axis7ProgressBar->setValue(value);
        ui->axis7ProgressBar->setFormat(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis7->setText(QString::number( value, 'f', 2 ));
        ui->jointAngleLcdNumberAxis7_status->setText(QString::number( value, 'f', 2 ));
    }
}

void MainWindow::updateFlangePosition(double value, int positionNum)
{
    if(abs(value) < 0.0049) value = 0;

    if(positionNum == 1){
        ui->positionLcdNumberX->setText(QString::number( value, 'f', 2 )); //mm
        ui->positionLcdNumberX_status->setText(QString::number( value, 'f', 2 ));
        ui->xProgressBar->setValue(value);
        ui->xProgressBar->setFormat(QString::number( value, 'f', 2 ));
    }
    if(positionNum == 2){
        ui->positionLcdNumberY->setText(QString::number( value, 'f', 2 ));
        ui->positionLcdNumberY_status->setText(QString::number( value, 'f', 2 ));
        ui->yProgressBar->setValue(value);
        ui->yProgressBar->setFormat(QString::number( value, 'f', 2 ));
    }
    if(positionNum == 3){
        ui->positionLcdNumberZ->setText(QString::number( value, 'f', 2 ));
        ui->positionLcdNumberZ_status->setText(QString::number( value, 'f', 2 ));
        ui->zProgressBar->setValue(value);
        ui->zProgressBar->setFormat(QString::number( value, 'f', 2 ));
    }
    if(positionNum == 4){
        ui->positionLcdNumberRX->setText(QString::number( value, 'f', 2 )); //degree
        ui->positionLcdNumberRX_status->setText(QString::number( value, 'f', 2 ));
        ui->rxProgressBar->setValue(value);
        ui->rxProgressBar->setFormat(QString::number( value, 'f', 2 ));
    }
    if(positionNum == 5){
        ui->positionLcdNumberRY->setText(QString::number( value, 'f', 2 ));
        ui->positionLcdNumberRY_status->setText(QString::number( value, 'f', 2 ));
        ui->ryProgressBar->setValue(value);
        ui->ryProgressBar->setFormat(QString::number( value, 'f', 2 ));
    }
    if(positionNum == 6){
        ui->positionLcdNumberRZ->setText(QString::number( value, 'f', 2 ));
        ui->positionLcdNumberRZ_status->setText(QString::number( value, 'f', 2 ));
        ui->rzProgressBar->setValue(value);
        ui->rzProgressBar->setFormat(QString::number( value, 'f', 2 ));
    }
}

void MainWindow::update3DView(double *newAngle)
{
    double t[7];
    for(int i=0;i<7;i++)
        t[i]=newAngle[i]/180*3.1415926;
    ui->move3DWidget->rob->update(t);
}

void MainWindow::updateDataAdsError(long adsError)
{
    switch(int(adsError)){
    case 6:
        hasAdsError = true;
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告:底层程序未启动");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Warning:ADS connect error: 6\ntarget port not found");
        }

        WarningWindow::getInstance()->exec();
        break;

    case 1808:
        hasAdsError = true;
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告:底层程序未登录");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Warning:ADS connect error: 1808\nsymbol not found");
        }
        //WarningWindow::getInstance()->exec();
        break;

    default:
        hasAdsError = true;
        QPixmap bkgnd(":/images/background.png");
        bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);

        ui->mainStackWindow->setCurrentIndex(0);

        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告:与机器人断开连接");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Warning: Loss connection to the robot");
        }
        if(!autoLoginActive) WarningWindow::getInstance()->exec();
        break;
    }

    //stop machine
    on_disconnectAction_clicked();



    ui->mainStackWindow->setCurrentIndex(0);
}

void MainWindow::updateDataTcpError(QString tcpAdd)
{
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("警告: 连接到远程地址" + tcpAdd + "失败");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Warning: Fail to connect to remote address:" + tcpAdd);
    }

    WarningWindow::getInstance()->exec();

    //stop machine
    on_disconnectAction_clicked();

    hasAdsError = true;
    ui->mainStackWindow->setCurrentIndex(0);
}


void MainWindow::addMoveLNode(bool psi)
{
    if(psi){
        //auto select psi during moveL
        programAddRoot("MoveL_Psi");
    }else{
        //fix a psi during moveL
        programAddRoot("MoveL();");
    }

    //save the new program tree
    saveTheNewProgramTree();
}

void MainWindow::addMoveJNode()
{
    programAddRoot("MoveJ();");
    //nodeWindow->close();

    //save the new program tree
    saveTheNewProgramTree();
}

void MainWindow::addMoveCNode()
{
    //check if the node above is completed, if not, can't add moveC
    //if(ui->programTree->currentItem()->text(0) == "MoveC"){
    //    //check the moveC end node
    //    if(ui->programTree->currentItem()->child(1)->text(0).split('(').value(1) == ""){
    //        //node not complete
    //        if(systemLanguage == "cn"){
    //            WarningWindow::getInstance()->setWarningText("帮助: 请给上一个节点设定位置");
    //        }else if(systemLanguage == "en"){
    //            WarningWindow::getInstance()->setWarningText("Warning: Current node not complete");
    //        }
    //        WarningWindow::getInstance()->exec();
    //
    //        nodeWindow->close();
    //        return;
    //    }
    //
    //}else{
    //    //check the current node
    //    if(ui->programTree->currentItem()->text(0).split('(').value(1) == ""){
    //        //node not complete
    //        if(systemLanguage == "cn"){
    //            WarningWindow::getInstance()->setWarningText("帮助: 请给上一个节点设定位置");
    //        }else if(systemLanguage == "en"){
    //            WarningWindow::getInstance()->setWarningText("Warning: Current node not complete");
    //        }
    //        WarningWindow::getInstance()->exec();
    //        nodeWindow->close();
    //        return;
    //    }
    //}

    //add moveC node and 2 children
    QTreeWidgetItem *itm = programAddRoot("MoveC");
    QSize size; size.setHeight(programLineHeight);
    itm->setSizeHint(0,size);
    programAddChild(itm, "MoveC_1();");
    programAddChild(itm, "MoveC_2();");

    //nodeWindow->close();

    //save the new program tree
    saveTheNewProgramTree();
}

void MainWindow::addMovePsiNode()
{
    programAddRoot("MovePsi");
    //nodeWindow->close();

    //save the new program tree
    saveTheNewProgramTree();
}


void MainWindow::addThreadNode()
{
    //if there is already a thread, throw warning
    QModelIndex indexSave = ui->programTree->currentIndex(); //save the current index
    ui->programTree->setCurrentIndex(ui->programTree->model()->index(0, 0));
    if(ui->programTree->currentItem()->text(0) == "thread"){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("帮助: 已存在一个thread");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Help: A thread already exists");
        }
        WarningWindow::getInstance()->exec();
        //nodeWindow->close();
        ui->programTree->setCurrentIndex(indexSave); //change back to the previous index
        return;
    }

    programAddRoot("thread");
    nodeWindow->close();

    //save the new program tree
    saveTheNewProgramTree();
}

void MainWindow::addProgramFunction(QString functionCmd)
{
    //add program function

    //check if the current item is thread
    if(ui->programTree->currentItem()->text(0) == "thread"){
        //add functions in thread
        programAddChild(ui->programTree->currentItem(), functionCmd);
        ui->programTree->currentItem()->setExpanded(true);
    }else{

        //check if current item is a child of thread
        if(ui->programTree->currentItem()->parent() != NULL && \
                ui->programTree->currentItem()->parent()->text(0) == "thread"){

            QTreeWidgetItem *itm = new QTreeWidgetItem();
            itm->setText(0, functionCmd);
            QSize size; size.setHeight(programLineHeight);
            itm->setSizeHint(0,size);

            //check if current item is "if", add the function as a child of "if"
            if(ui->programTree->currentItem()->text(0).split('(').value(0) == "if"){
                //add the function as a child of "if"
                ui->programTree->currentItem()->addChild(itm);
                ui->programTree->currentItem()->setExpanded(true);

            }else{
                //add a child node inside thread
                int childNum = ui->programTree->currentItem()->parent()\
                        ->indexOfChild(ui->programTree->currentItem());
                ui->programTree->currentItem()->parent()->insertChild(childNum + 1, itm);

                ui->programTree->setCurrentItem(ui->programTree->currentItem()->parent()->child(childNum + 1));
            }



        }else{
            //check if the function is valid
            //if(functionValidCheck(functionCmd)){
            //add a root node
            programAddRoot(functionCmd);
            //}

            //nodeWindow->close();
        }
    }



    //save the new program tree
    saveTheNewProgramTree();
}

void MainWindow::runCommand(int lineNum)
{
    //check if the line number is valid, no overflow
    if(lineNum < totalCommandLineNum && lineNum >= 0){

        //set old command background to white
        if(oldLineNum < totalCommandLineNum && oldLineNum >= 0){
            ui->runDataListWidget->setCurrentRow(oldLineNum);
            ui->runDataListWidget->currentItem()->setBackground(QBrush(QColor(42, 46, 47)));
        }

        //set current execute line to green
        ui->runDataListWidget->setCurrentRow(lineNum);
        ui->runDataListWidget->currentItem()->setBackground(QBrush(QColor(0, 222, 250)));
        ui->runDataListWidget->setCurrentRow(-1);

        oldLineNum = lineNum;
    }
}

void MainWindow::stopRunProgram()
{
    emit stopTimeCount();   //stop count program run time

    //set step button enable
    ui->stepProgramButton_run->setEnabled(true);
    ui->runProgramButton_run->setEnabled(true);
    ui->pauseProgramButton_run->setDisabled(true);
    ui->stopProgramButton_run->setDisabled(true);

    ui->pauseProgramButton_run->setVisible(false);
    ui->stopProgramButton_run->setVisible(false);
    ui->stepProgramButton_run->setVisible(true);
    ui->runProgramButton_run->setVisible(true);

    //send message to PLC
    adsWriteStruct.progPause = false;
    adsWriteStruct.progStop = true;


    if(systemLanguage == "cn"){
        ui->programStatus->setText("就绪");
    }else if(systemLanguage == "en"){
        ui->programStatus->setText("Ready");
    }


    //set old command background to white
    if(oldLineNum < totalCommandLineNum && oldLineNum >= 0){
        ui->runDataListWidget->setCurrentRow(oldLineNum);
        ui->runDataListWidget->currentItem()->setBackground(QBrush(QColor(42, 46, 47)));
        oldLineNum = -1;
    }

    //back to the -1 row
    ui->runDataListWidget->setCurrentRow(-1);


    //send feedback
    //programIsRunning = false;

    //enable the joint move mode buttons
    ui->lineMoveGroupBox->setEnabled(true);
    ui->jointMoveGroupBox->setEnabled(true);
    ui->positionGroupBox->setEnabled(true);
    ui->jointAngleGroupBox->setEnabled(true);
    ui->saveNodeButton->setEnabled(true);
    ui->jogSpeedSlider->setEnabled(true);
    ui->tractionTeachButton->setEnabled(true);
    ui->moveToButton->setEnabled(true);
    ui->pushButtonMoveToOrigin->setEnabled(true);
    /*
    //check if repeat the program infinitely
    if(repeatProgram){
        //run the current program again
        QThread::msleep(200); //wait for thread initial, otherwise the program will skip the first command

        on_runProgramButton_run_clicked();
    }

    //check if need to repeat the program for several time
    if(runProgramRepeatTime > 1){
        //run the current program again
        QThread::msleep(200); //wait for thread initial, otherwise the program will skip the first command

        on_runProgramButton_run_clicked();
        runProgramRepeatTime--;
        ui->repeatTimeLabel->setText(QString::number(runProgramRepeatTime));

        if(runProgramRepeatTime == 1){
            if(systemLanguage == "cn"){
                QPixmap pixmap(":/images/repeat.png");
                QIcon ButtonIcon(pixmap);
                ui->pushButton->setIcon(ButtonIcon);
                ui->pushButton->setIconSize(pixmap.rect().size());
            }else if(systemLanguage == "en"){
                QPixmap pixmap(":/img_en/img_en/repeat-en.png");
                QIcon ButtonIcon(pixmap);
                ui->pushButton->setIcon(ButtonIcon);
                ui->pushButton->setIconSize(pixmap.rect().size());
            }

            repeatProgram = false;
            runProgramRepeatTime = 0;
            ui->repeatTimeLabel->setText("");
        }
    }



    QThread::msleep(200);//wait for other parameters setup
    */
}






void MainWindow::updateProgramNumber(int whichOne, QString value)
{
    //1:speed, 2:acceleration, 3:tool coordinate X, 4:tool coordinate Y, 5:tool coordinate Z
    switch(whichOne){
    //speed
    case 1 :
        if(value == "" || value.toInt() <= 0){
            return;
        }else{
            //detect if no item is selected
            if(ui->programTree->currentItem() == NULL) {
                numberBoard->windowIsOpened = false;
                numberBoard->close();
                return;
            }

            //for Start, moveJ, moveC, value should be within 1-100%
            QString key = ui->programTree->currentItem()->text(0).split('(').value(0);
            if(value.toDouble() > 100){
                if(key == "MoveL" || key == "MoveL_Psi" || key == "MoveC_1" || key == "MoveC_2"){
                    if(value.toDouble() > 1000) value = "1000";
                    ui->setSpeedButton->setText(value + "mm/s");
                }else if(key == "MoveJ" || key == "Start"){
                    value = "100";
                    ui->setSpeedButton->setText(value + "%");
                }else
                    return;
            }else{
                //value between 0-100
                if(key == "MoveL" || key == "MoveL_Psi" || key == "MoveC_1" || key == "MoveC_2"){
                    if(value.toDouble() > 1000) value = "1000";
                    ui->setSpeedButton->setText(value + "mm/s");
                }else if(key == "MoveJ" || key == "Start"){
                    ui->setSpeedButton->setText(value + "%");
                }else
                    return;
            }
        }
        break;

        //acceleration
    case 2 :
        if(value == ""){
            ui->programAccelerationValue->setText("0");
        }else{
            ui->programAccelerationValue->setText(value);
        }
        break;

        //tool coordinate X
    case 3 :
        if(value == ""){
            ui->toolCoordinateLineX->setText("0");
        }else{
            ui->toolCoordinateLineX->setText(value);
        }

        break;

        //tool coordinate Y
    case 4 :
        if(value == ""){
            ui->toolCoordinateLineY->setText("0");
        }else{
            ui->toolCoordinateLineY->setText(value);
        }
        break;

        //tool coordinate Z
    case 5 :
        if(value == ""){
            ui->toolCoordinateLineZ->setText("0");
        }else{
            ui->toolCoordinateLineZ->setText(value);
        }
        break;


        //blend radius, set 交融半径
    case 6 :
        if(value.toInt() < 0) return;
        break;


        //tool coordinate RX
    case 9 :
        if(value == ""){
            ui->toolCoordinateLineRX->setText("0");
        }else{
            ui->toolCoordinateLineRX->setText(value);
        }

        break;

        //tool coordinate RY
    case 10 :
        if(value == ""){
            ui->toolCoordinateLineRY->setText("0");
        }else{
            ui->toolCoordinateLineRY->setText(value);
        }
        break;

        //tool coordinate RZ
    case 11 :
        if(value == ""){
            ui->toolCoordinateLineRZ->setText("0");
        }else{
            ui->toolCoordinateLineRZ->setText(value);
        }
        break;

        //expert mode switch
    case 12 :
        if(value == "573"){
            //switch expert mode
            expertMode = !expertMode;
            switchExpertMode(expertMode);
        }
        break;

    case 13 :
        //set run program repeat time
        if(value.toInt() <= 0){
            repeatProgram = true;
            runProgramRepeatTime = 0;
            ui->repeatTimeLabel->setText("");
        }
        else{
            repeatProgram = false;
            runProgramRepeatTime = value.toInt();
            if(runProgramRepeatTime > 99) runProgramRepeatTime = 99;
            ui->repeatTimeLabel->setText(QString::number(runProgramRepeatTime));
        }
        break;

        //end tool weight
    case 14 :
        if(value.toDouble() >= 0){
            ui->toolLoad->setText(value);
            adsWriteStruct.tcpLoad = value.toDouble();
        }
        break;

        //tool weight position X
    case 15 :
        ui->toolLoadPositionX->setText(value);
        break;

        //tool weight position Y
    case 16 :
        ui->toolLoadPositionY->setText(value);
        break;

        //tool weight position Z
    case 17 :
        ui->toolLoadPositionZ->setText(value);
        break;

        //tcp calibration rx
    case 18 :
        ui->toolCoordinateLineRX_2->setText(value);
        break;

        //tcp calibration ry
    case 19 :
        ui->toolCoordinateLineRY_2->setText(value);
        break;

        //tcp calibration rz
    case 20 :
        ui->toolCoordinateLineRZ_2->setText(value);
        break;

        //virtual wall
    case 21:
        ui->virtualWall1Length->setText(value);
        break;

    case 22:
        ui->virtualWall2Length->setText(value);
        break;

    case 23:
        ui->virtualWall3Length->setText(value);
        break;

    case 24:
        ui->virtualWall4Length->setText(value);
        break;

    case 25:
        ui->virtualWall5Length->setText(value);
        break;

    case 26:
        ui->virtualWall6Length->setText(value);
        break;

    case 27:
        ui->virtualWall3Rot->setText(value);
        break;

    case 28:
        ui->virtualWall4Rot->setText(value);
        break;

    case 29:
        ui->virtualWall5Rot->setText(value);
        break;

    case 30:
        ui->virtualWall6Rot->setText(value);
        break;
    }

    //update the node, save the new parameter (1:speed, 2:acceleration, 3:transition semidiameter)
    if((whichOne == 1 || whichOne == 2 || whichOne == 6) && ui->programTree->currentItem() != NULL){
        //split '('
        QStringList keywordPieces = ui->programTree->currentItem()->text(0).split( "(" );
        //detect if the current item is a subitem of the node
        QStringList keywordPieces2 = ui->programTree->currentItem()->text(0).split( ":" );

        if(keywordPieces.value(1) != NULL || keywordPieces2.value(1) != NULL){
            QString keyWord;

            if(keywordPieces2.value(1) != NULL){
                //current item is a subitem of the node
                ui->programTree->setCurrentItem(ui->programTree->currentItem()->parent());
                //get keyword
                keywordPieces = ui->programTree->currentItem()->text(0).split( "(" );
                keyWord = keywordPieces.value(0);
            }else{
                //get keyword
                keyWord = keywordPieces.value(0);
            }

            if(keyWord == "Sleep"){
                numberBoard->windowIsOpened = false;
                numberBoard->close();
                return;
            }

            QString programCommand[16];

            //get parameters
            QStringList parameterPieces = keywordPieces.value(1).split( ")" ).value(0).split( "," );
            for(int i = 0; i < parameterPieces.count(); i++) programCommand[i] = parameterPieces.value(i);

            //add position to node string
            QString node;

            if(value == "") value = "0";

            //change speed value
            if(whichOne == 1){
                if(parameterPieces.count() == 15)
                    //no transition semidiameter parameter
                    node = keyWord + "(" + programCommand[0] + "," + programCommand[1]+ ","+ programCommand[2]+\
                            "," + programCommand[3] + "," + programCommand[4]+ "," + programCommand[5] + ", " + \
                            value+ "," + programCommand[7] + "," + programCommand[8] + "," + \
                            programCommand[9] + "," + programCommand[10] + "," + programCommand[11] + "," + \
                            programCommand[12] + "," + programCommand[13] + "," + programCommand[14] + ");";
                else
                    //with transition semidiameter parameter
                    node = keyWord + "(" + programCommand[0] + "," + programCommand[1]+ ","+ programCommand[2]+\
                            "," + programCommand[3] + "," + programCommand[4]+ "," + programCommand[5] + ", " + \
                            value+ "," + programCommand[7] + "," + programCommand[8] + "," + \
                            programCommand[9] + "," + programCommand[10] + "," + programCommand[11] + "," + \
                            programCommand[12] + "," + programCommand[13] + "," + programCommand[14] + "," \
                            + programCommand[15] + ");";
            }

            //change acceleration value
            if(whichOne == 2){
                if(parameterPieces.count() == 15)
                    //no transition semidiameter parameter
                    node = keyWord + "(" + programCommand[0] + "," + programCommand[1]+ ","+ programCommand[2]+\
                            "," + programCommand[3] + "," + programCommand[4]+ "," + programCommand[5] + "," + \
                            programCommand[6]+ "," + value + "," + programCommand[8] + "," + \
                            programCommand[9] + "," + programCommand[10] + "," + programCommand[11] + "," + \
                            programCommand[12] + "," + programCommand[13] + "," + programCommand[14] + ");";
                else
                    //with transition semidiameter parameter
                    node = keyWord + "(" + programCommand[0] + "," + programCommand[1]+ ","+ programCommand[2]+\
                            "," + programCommand[3] + "," + programCommand[4]+ "," + programCommand[5] + "," + \
                            programCommand[6]+ "," + value + "," + programCommand[8] + "," + \
                            programCommand[9] + "," + programCommand[10] + "," + programCommand[11] + "," + \
                            programCommand[12] + "," + programCommand[13] + "," + programCommand[14] + "," \
                            + programCommand[15] + ");";
            }

            //change blend radius parameter
            if(whichOne == 6){
                if(value == "0"){
                    //clean blend radius parameter
                    node = keyWord + "(" + programCommand[0] + "," + programCommand[1]+ ","+ programCommand[2]+\
                            "," + programCommand[3] + "," + programCommand[4]+ "," + programCommand[5] + "," + \
                            programCommand[6]+ "," + value + "," + programCommand[8] + "," + \
                            programCommand[9] + "," + programCommand[10] + "," + programCommand[11] + "," + \
                            programCommand[12] + "," + programCommand[13] + "," + programCommand[14] + ",-1);";

                    ui->blendRadiusValue->setText("N/A");
                }else{
                    //with blend radius parameter
                    node = keyWord + "(" + programCommand[0] + "," + programCommand[1]+ ","+ programCommand[2]+\
                            "," + programCommand[3] + "," + programCommand[4]+ "," + programCommand[5] + "," + \
                            programCommand[6]+ "," + value + "," + programCommand[8] + "," + \
                            programCommand[9] + "," + programCommand[10] + "," + programCommand[11] + "," + \
                            programCommand[12] + "," + programCommand[13] + "," + programCommand[14] + "," \
                            + value + ");";

                    ui->blendRadiusValue->setText(value);
                }
            }

            ui->programTree->currentItem()->setText(0, node);
        }

        //save the new program tree
        saveTheNewProgramTree();
    }

    numberBoard->windowIsOpened = false;
    numberBoard->close();

}

void MainWindow::updateKeyboardText(int keyboardType, QString text)
{
    //for create program, type in a name
    if(keyboardType == 1) createName = text;

    //for add functions in program
    if(keyboardType == 2) {

        //check function validation
        if(functionValidCheck(text)){
            //change current node
            ui->programTree->currentItem()->setText(0, text);

            //save the new program tree
            saveTheNewProgramTree();

            //refresh the description page
            //QModelIndex index;
            //on_programTree_clicked(index);
            QTreeWidgetItem *current = NULL;
            on_programTree_currentItemChanged(current, current);
        }
    }

    //modify robot script language
    if(keyboardType == 3) {

        //change current node
        ui->programTree->currentItem()->setText(0, text);

        //save the new program tree
        saveTheNewProgramTree();

        ui->programStack->setCurrentIndex(5);
        ui->scriptLabel->setText(text);
        ui->scriptLabel->setAlignment(Qt::AlignCenter);
    }

    //ip address
    if(keyboardType == 4){
        ui->ipAddressLineEdit->setText(text);
        //ui->gatewayLineEdit->setText(text.split('.').value(0)+"."+text.split('.').value(1)+"."+text.split('.').value(2)+".1");
    }

    //netmask
    if(keyboardType == 5) ui->netmaskLineEdit->setText(text);

    //gateway
    if(keyboardType == 6) ui->gatewayLineEdit->setText(text);

    keyboard->close();
}

void MainWindow::addNode2Simulator(QString func)
{
    QStringList keywordPieces = func.split('(');
    QString keyWord = keywordPieces.value(0);
    QStringList parameterPieces = keywordPieces.value(1).split( ")" ).value(0).split( "," );

    if(keyWord == "Start"){
        char txt[] = "Start";
        ui->move3DWidget->rob->createNode(parameterPieces.value(1).toDouble(),
                                          -parameterPieces.value(0).toDouble(),
                                          parameterPieces.value(2).toDouble(), txt);
    }

    if(keyWord == "MoveJ"){
        char txt[] = "MoveJ";
        ui->move3DWidget->rob->createNode(parameterPieces.value(1).toDouble(),
                                          -parameterPieces.value(0).toDouble(),
                                          parameterPieces.value(2).toDouble(), txt);
    }

    if(keyWord == "MoveL"){
        char txt[] = "MoveL";
        ui->move3DWidget->rob->createNode(parameterPieces.value(1).toDouble(),
                                          -parameterPieces.value(0).toDouble(),
                                          parameterPieces.value(2).toDouble(), txt);
    }

}


void MainWindow::moveToSelectedNode()
{
    if(ui->programTree->currentItem() == NULL || ui->programTree->currentItem()->text(0) == "MoveC")
        return;

    if(ui->programTree->currentIndex().row() < 0 || ui->programTree->currentItem()->parent() != NULL){
        //detect if the selected node is one of the 2 nodes of moveC
        if(ui->programTree->currentItem()->parent()->text(0) != "MoveC"){
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("帮助: 请选择一个根节点");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Help: Please select a root node");
            }
            WarningWindow::getInstance()->exec();
            return;
        }
    }

    QString text = ui->programTree->currentItem()->text(0).split( "(" ).value(1);
    QString moveType = ui->programTree->currentItem()->text(0).split( "(" ).value(0);
    //check if the node is valid
    if(text != ""){
        adsWriteStruct.progStop = false;

        if(moveType == "MoveJ" || moveType == "Start" || moveType == "MoveL" || moveType == "MoveL_Psi" || moveType == "MoveC_1" || moveType == "MoveC_2"){
            //moveJ(x, y, z, rx, ry, rz, v, a, a1, a2, a3, a4, a5, a6, a7)

            //set all the axis
            for(int i = 8; i < 15; i++)
                programAxisMoveTo[i-8] = text.split( ")" ).value(0).split( "," ).value(i).toDouble();

            //ui->stopJogMove1->setVisible(true);

            //Start moveto thread
            moveToThread->start();
        }
    }



}

void MainWindow::moveToOriginalPoint()
{
    adsWriteStruct.progStop = false;

    //set all axis to 0 if init file doesn't exist
    for(int i = 0; i < 7; i++) programAxisMoveTo[i] = 0;

    //read the init file
    QFile file(fileSavePath + "\\config.init");;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){

    }else{
        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //set full screen
            if(keywordPieces.value(0).trimmed() == "INITPOINT"){
                for(int i = 0; i < 7; i++) programAxisMoveTo[i] = keywordPieces.value(i+1).toDouble();

            }
        }
    }

    file.close();

    //Start moveto thread
    moveToThread->start();

}



void MainWindow::runTargetProgram(QString programPath)
{
    if(!programIsRunning){
        //Start the correspond program
        //load program
        if(selectProgramHandler(programPath)){
            emit sendErrorMsg2Client("Can't find program");
            qDebug() << "Program doesn't exist" << programPath;
            return;
        }

        checkStartPoint = false;

        //run program
        on_runProgramButton_run_clicked();
    }

}

void MainWindow::setRunProgramSpeed2UI(int speedPercentage)
{
    if(speedPercentage > 0 && speedPercentage <= 100){
        on_runSpeedSlider_valueChanged(speedPercentage);
        ui->runSpeedSlider->setValue(speedPercentage);
    }
}

void MainWindow::collisionDetected(int collisionID)
{
    on_pauseProgramButton_run_clicked();

    LogFile::logFileCreate("collision detect", "axis " + QString::number(collisionID)); //save run program log

    //read the init file
    QFile file(fileSavePath + "\\config.init");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "init config file doesn't exist";

        //open a warning window
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告: 关节" + QString::number(collisionID) + "检测到碰撞！");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Warning: axis " + QString::number(collisionID) + " Collision detected!");
        }
        WarningWindow::getInstance()->exec();

    }else{
        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //collision auto recover
            if(keywordPieces.value(0).trimmed() == "COLLISIONREC"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    //sleep target time
                    QThread::msleep(keywordPieces.value(2).trimmed().toInt());

                    //auto recover
                    on_runProgramButton_run_clicked();
                }else{
                    //open a warning window
                    if(systemLanguage == "cn"){
                        WarningWindow::getInstance()->setWarningText("警告: 关节" + QString::number(collisionID) + "检测到碰撞！");
                    }else if(systemLanguage == "en"){
                        WarningWindow::getInstance()->setWarningText("Warning: axis " + QString::number(collisionID) + " Collision detected!");
                    }
                    WarningWindow::getInstance()->exec();

                }
            }
        }
    }

    file.close();

}


void MainWindow::lockJogScreen(bool lock)
{
    ui->lineMoveGroupBox->setDisabled(lock);
    ui->jointMoveGroupBox->setDisabled(lock);
    ui->positionGroupBox->setDisabled(lock);
    ui->jointAngleGroupBox->setDisabled(lock);
    ui->saveNodeButton->setDisabled(lock);
    ui->jogSpeedSlider->setDisabled(lock);
    ui->tractionTeachButton->setDisabled(lock);
    ui->moveToButton->setDisabled(lock);
    ui->pushButtonMoveToOrigin->setDisabled(lock);
}

void MainWindow::teachLockScreen(bool lock)
{
    ui->mainStackWindow->setDisabled(lock);
}


void MainWindow::jogAxisMoveButtonStatus(bool status, bool status2)
{
    ui->axis1MoveBackward->setEnabled(status);
    ui->axis1MoveForward->setEnabled(status);
    ui->axis2MoveBackward->setEnabled(status);
    ui->axis2MoveForward->setEnabled(status);
    ui->axis3MoveBackward->setEnabled(status);
    ui->axis3MoveForward->setEnabled(status);
    ui->axis4MoveBackward->setEnabled(status);
    ui->axis4MoveForward->setEnabled(status);
    ui->axis5MoveBackward->setEnabled(status);
    ui->axis5MoveForward->setEnabled(status);
    ui->axis6MoveBackward->setEnabled(status);
    ui->axis6MoveForward->setEnabled(status);
    ui->axis7MoveBackward->setEnabled(status);
    ui->axis7MoveForward->setEnabled(status);
    ui->lineMoveBackward->setEnabled(status2);
    ui->lineMoveDown->setEnabled(status2);
    ui->lineMoveForward->setEnabled(status2);
    ui->lineMoveLeft->setEnabled(status2);
    ui->lineMoveRight->setEnabled(status2);
    ui->lineMoveUp->setEnabled(status2);
    ui->angleMoveBackward->setEnabled(status2);
    ui->angleMoveDown->setEnabled(status2);
    ui->angleMoveForward->setEnabled(status2);
    ui->angleMoveLeft->setEnabled(status2);
    ui->angleMoveRight->setEnabled(status2);
    ui->angleMoveUp->setEnabled(status2);
    ui->fixPositionMoveBackward->setEnabled(status2);
    ui->fixPositionMoveForward->setEnabled(status2);
}

void MainWindow::stopJogAxis()
{
    //release jog axis buttons
    adsWriteStruct.jogMode = 0;
    for(int i = 0; i < 7; i++){
        adsWriteStruct.axisAngleMoveDirection[i] = 0;
    }
}

void MainWindow::stopJogLine()
{
    //release jog line buttons
    for(int i = 0; i < 6; i++){
        adsWriteStruct.flangeMoveDirection[i] = 0;
    }

    // sync&write jogmode
    adsWriteStruct.jogMode = 0;
}

void MainWindow::exceedJointLimit()
{
    QString jointNum;
    jogAxisMoveButtonStatus(false, false);

    stopJogAxis();
    stopJogLine();
    repeatProgram = false; //stop repeat program

    if(adsReadStruct.exceedWorkingLimit) {
        //qDebug() << "******" << adsReadStruct.exceedWorkingLimit;
        //jogAxisMoveButtonStatus(true, false);
        jointNum = "joint4";
        ui->moveStackedWidget->setCurrentIndex(1);
    }
    else if(adsReadStruct.axisCurrentAngle[0] <= -xmlWriteStruct.limits_Pos[0]+2){ui->axis1MoveForward->setEnabled(true);  jointNum = '1';}
    else if(adsReadStruct.axisCurrentAngle[0] >= xmlWriteStruct.limits_Pos[0]-2) {ui->axis1MoveBackward->setEnabled(true); jointNum = '1';}
    else if(adsReadStruct.axisCurrentAngle[1] <= -xmlWriteStruct.limits_Pos[1]+2) {ui->axis2MoveForward->setEnabled(true);  jointNum = '2';}
    else if(adsReadStruct.axisCurrentAngle[1] >= xmlWriteStruct.limits_Pos[1]-2)  {ui->axis2MoveBackward->setEnabled(true); jointNum = '2';}
    else if(adsReadStruct.axisCurrentAngle[2] <= -xmlWriteStruct.limits_Pos[2]+2){ui->axis3MoveForward->setEnabled(true);  jointNum = '3';}
    else if(adsReadStruct.axisCurrentAngle[2] >= xmlWriteStruct.limits_Pos[2]-2) {ui->axis3MoveBackward->setEnabled(true); jointNum = '3';}
    else if(adsReadStruct.axisCurrentAngle[3] <= -xmlWriteStruct.limits_Pos[3]+2) {ui->axis4MoveForward->setEnabled(true);  jointNum = '4';}
    else if(adsReadStruct.axisCurrentAngle[3] >= xmlWriteStruct.limits_Pos[3]-2)  {ui->axis4MoveBackward->setEnabled(true); jointNum = '4';}
    else if(adsReadStruct.axisCurrentAngle[4] <= -xmlWriteStruct.limits_Pos[4]+2){ui->axis5MoveForward->setEnabled(true);  jointNum = '5';}
    else if(adsReadStruct.axisCurrentAngle[4] >= xmlWriteStruct.limits_Pos[4]-2) {ui->axis5MoveBackward->setEnabled(true); jointNum = '5';}
    else if(adsReadStruct.axisCurrentAngle[5] <= -xmlWriteStruct.limits_Pos[5]+2) {ui->axis6MoveForward->setEnabled(true);  jointNum = '6';}
    else if(adsReadStruct.axisCurrentAngle[5] >= xmlWriteStruct.limits_Pos[5]-2)  {ui->axis6MoveBackward->setEnabled(true); jointNum = '6';}
    else if(adsReadStruct.axisCurrentAngle[6] <= -xmlWriteStruct.limits_Pos[6]+2){ui->axis7MoveForward->setEnabled(true);  jointNum = '7';}
    else if(adsReadStruct.axisCurrentAngle[6] >= xmlWriteStruct.limits_Pos[6]-2) {ui->axis7MoveBackward->setEnabled(true); jointNum = '7';}

    if(!(adsReadStruct.exceedWorkingLimit)){
        if(lastClickedMoveButton == "lineMoveBackward")            ui->lineMoveForward->setEnabled(true);
        else if(lastClickedMoveButton == "lineMoveDown")           ui->lineMoveUp->setEnabled(true);
        else if(lastClickedMoveButton == "lineMoveForward")        ui->lineMoveBackward->setEnabled(true);
        else if(lastClickedMoveButton == "lineMoveLeft")           ui->lineMoveRight->setEnabled(true);
        else if(lastClickedMoveButton == "lineMoveRight")          ui->lineMoveLeft->setEnabled(true);
        else if(lastClickedMoveButton == "lineMoveUp")             ui->lineMoveDown->setEnabled(true);
        else if(lastClickedMoveButton == "angleMoveBackward")      ui->angleMoveForward->setEnabled(true);
        else if(lastClickedMoveButton == "angleMoveDown")          ui->angleMoveUp->setEnabled(true);
        else if(lastClickedMoveButton == "angleMoveForward")       ui->angleMoveBackward->setEnabled(true);
        else if(lastClickedMoveButton == "angleMoveLeft")          ui->angleMoveRight->setEnabled(true);
        else if(lastClickedMoveButton == "angleMoveRight")         ui->angleMoveLeft->setEnabled(true);
        else if(lastClickedMoveButton == "angleMoveUp")            ui->angleMoveDown->setEnabled(true);
        else if(lastClickedMoveButton == "fixPositionMoveBackward")ui->fixPositionMoveForward->setEnabled(true);
        else if(lastClickedMoveButton == "fixPositionMoveForward") ui->fixPositionMoveBackward->setEnabled(true);
    }


    if(jointNum != "joint4"){
        LogFile::logFileCreate("axis beyond limit", "axis " + jointNum); //save run program log
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告: " + jointNum + "关节超出限位！");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Warning: Exceed joint limit!");
        }
        WarningWindow::getInstance()->exec();
    }else{
        if(systemLanguage == "cn"){
            if(abs(adsReadStruct.axisCurrentAngle[3]) < 10){
                WarningWindow::getInstance()->setWarningText("警告: 关节力矩过大\n请手动调整4关节角度");
                WarningWindow::getInstance()->exec();
                //disable line move
                ui->lineMoveGroupBox->setEnabled(false);
            }else{
                WarningWindow::getInstance()->setWarningText("警告: 关节力矩过大");
                WarningWindow::getInstance()->exec();

                if(systemLanguage == "cn"){
                    QPixmap pixmap(":/images/repeat.png");
                    QIcon ButtonIcon(pixmap);
                    ui->pushButton->setIcon(ButtonIcon);
                    ui->pushButton->setIconSize(pixmap.rect().size());
                }else if(systemLanguage == "en"){
                    QPixmap pixmap(":/img_en/img_en/repeat-en.png");
                    QIcon ButtonIcon(pixmap);
                    ui->pushButton->setIcon(ButtonIcon);
                    ui->pushButton->setIconSize(pixmap.rect().size());
                }

                //exceed limit not caused by joint4, clear all disable
                exceedJointLimitClear();
            }
        }else if(systemLanguage == "en"){
            if(abs(adsReadStruct.axisCurrentAngle[3]) < 10){
                WarningWindow::getInstance()->setWarningText("Warning: Exceed working limit!\nAdjust joint 4");
                WarningWindow::getInstance()->exec();

                //disable line move
                ui->lineMoveGroupBox->setEnabled(false);
            }else{
                WarningWindow::getInstance()->setWarningText("Warning: Exceed working limit!");
                WarningWindow::getInstance()->exec();

                //exceed limit not caused by joint4, clear all disable
                exceedJointLimitClear();
            }
        }

        on_switchToJointMove_clicked();
    }

    robotWindowChangePosition(1);
    on_jogMoveButton_clicked();

    //if the joint exceed limit when program is running, active all the buttons
    if(programIsRunning) jogAxisMoveButtonStatus(true, true);


}

void MainWindow::exceedJointLimitClear()
{
    jogAxisMoveButtonStatus(true, true);
    ui->lineMoveGroupBox->setEnabled(true);
}

void MainWindow::getSerialPortMsg()
{
    QByteArray ba;
    ba = serialPort->readAll();
    //qDebug()<< ba.at(0);// << "**********" << ba.count()<<"*****" << ba.data();

    //qDebug()<< ba.at(0);
    //qDebug()<< ba.at(1);
    //qDebug()<< ba.at(2);
    //qDebug()<< ba.at(3);
    //qDebug()<< ba.at(4);
    //qDebug()<< ba.at(5);
    //qDebug()<< ba.at(6);
    //qDebug()<< ba.at(7);
    //qDebug()<< ba.at(8);
    //qDebug()<< ba.at(9);
    //qDebug()<< ba;


    //2nd initial window
    if(ba.at(4) == 'i' && ba.at(3) == 0x31)     {if(ui->axis7MoveBackward->isEnabled()) on_axis7MoveBackward_pressed();    }
    else if(ba.at(4) == 'i' && ba.at(3) == 0x30){if(ui->axis7MoveBackward->isEnabled()) on_axis7MoveBackward_released();   }
    else if(ba.at(4) == 'j' && ba.at(3) == 0x31){if(ui->axis7MoveForward->isEnabled()) on_axis7MoveForward_pressed();     }
    else if(ba.at(4) == 'j' && ba.at(3) == 0x30){if(ui->axis7MoveForward->isEnabled()) on_axis7MoveForward_released();    }
    else if(ba.at(4) == '=' && ba.at(3) == 0x31){
        if(numberBoard->windowIsOpened){
            //enter value move
            numberBoard->on_pressToMove_pressed();
        }else if(ui->functionStackWindow->currentIndex() == 1){
            //program move to selected node
            on_moveToButton_pressed();
        }else if(ui->functionStackWindow->currentIndex() == 2){
            //move to Start point
            on_moveToStartPoint_pressed();
        }
        else{
            //move to original point
            on_pushButtonMoveToOrigin_pressed();
        }
    }
    else if(ba.at(4) == '=' && ba.at(3) == 0x30){
        on_pushButtonMoveToOrigin_released();
        numberBoard->on_pressToMove_released();
        on_moveToButton_released();
        on_moveToStartPoint_released();
    }

    //if achieve joint/working limit, return;
    //if(adsReadStruct.exceedJointLimit || adsReadStruct.exceedWorkingLimit || programIsRunning) return;
    //if achieve joint/working limit, return;
    if( programIsRunning) return;

    //if serial message receive error(ba.count() != 9), stop move

    if(ui->moveStackedWidget->currentIndex() == 1){
        //joint move
        if(ba.at(4) == 30 && ba.at(3) == 0x31){if(ui->axis1MoveBackward->isEnabled()) on_axis1MoveBackward_pressed();}
        else if(ba.at(4) == 30 && ba.at(3) == 0x30){if(ui->axis1MoveBackward->isEnabled()) on_axis1MoveBackward_released();}
        else if(ba.at(4) == 48 && ba.at(3) == 0x31){if(ui->axis1MoveForward->isEnabled()) on_axis1MoveForward_pressed();  }
        else if(ba.at(4) == 48 && ba.at(3) == 0x30){if(ui->axis1MoveForward->isEnabled()) on_axis1MoveForward_released(); }
        else if(ba.at(4) == 46 && ba.at(3) == 0x31){if(ui->axis2MoveBackward->isEnabled()) on_axis2MoveBackward_pressed(); }
        else if(ba.at(4) == 46 && ba.at(3) == 0x30){if(ui->axis2MoveBackward->isEnabled()) on_axis2MoveBackward_released();}
        else if(ba.at(4) == 32 && ba.at(3) == 0x31){if(ui->axis2MoveForward->isEnabled()) on_axis2MoveForward_pressed();  }
        else if(ba.at(4) == 32 && ba.at(3) == 0x30){if(ui->axis2MoveForward->isEnabled()) on_axis2MoveForward_released(); }
        else if(ba.at(4) == 18 && ba.at(3) == 0x31){if(ui->axis3MoveBackward->isEnabled()) on_axis3MoveBackward_pressed(); }
        else if(ba.at(4) == 18 && ba.at(3) == 0x30){if(ui->axis3MoveBackward->isEnabled()) on_axis3MoveBackward_released();}
        else if(ba.at(4) == 33 && ba.at(3) == 0x31){if(ui->axis3MoveForward->isEnabled()) on_axis3MoveForward_pressed();  }
        else if(ba.at(4) == 33 && ba.at(3) == 0x30){if(ui->axis3MoveForward->isEnabled()) on_axis3MoveForward_released(); }
        else if(ba.at(4) == 34 && ba.at(3) == 0x31){if(ui->axis4MoveBackward->isEnabled()) on_axis4MoveBackward_pressed(); }
        else if(ba.at(4) == 34 && ba.at(3) == 0x30){if(ui->axis4MoveBackward->isEnabled()) on_axis4MoveBackward_released();}
        else if(ba.at(4) == 35 && ba.at(3) == 0x31){if(ui->axis4MoveForward->isEnabled()) on_axis4MoveForward_pressed();  }
        else if(ba.at(4) == 35 && ba.at(3) == 0x30){if(ui->axis4MoveForward->isEnabled()) on_axis4MoveForward_released(); }
        else if(ba.at(4) == 23 && ba.at(3) == 0x31){if(ui->axis5MoveBackward->isEnabled()) on_axis5MoveBackward_pressed(); }
        else if(ba.at(4) == 23 && ba.at(3) == 0x30){if(ui->axis5MoveBackward->isEnabled()) on_axis5MoveBackward_released();}
        else if(ba.at(4) == 36 && ba.at(3) == 0x31){if(ui->axis5MoveForward->isEnabled()) on_axis5MoveForward_pressed();  }
        else if(ba.at(4) == 36 && ba.at(3) == 0x30){if(ui->axis5MoveForward->isEnabled()) on_axis5MoveForward_released(); }
        else if(ba.at(4) == 37 && ba.at(3) == 0x31){if(ui->axis6MoveBackward->isEnabled()) on_axis6MoveBackward_pressed(); }
        else if(ba.at(4) == 37 && ba.at(3) == 0x30){if(ui->axis6MoveBackward->isEnabled()) on_axis6MoveBackward_released();}
        else if(ba.at(4) == 38 && ba.at(3) == 0x31){if(ui->axis6MoveForward->isEnabled()) on_axis6MoveForward_pressed();  }
        else if(ba.at(4) == 38 && ba.at(3) == 0x30){if(ui->axis6MoveForward->isEnabled()) on_axis6MoveForward_released(); }
    }



    if(ui->moveStackedWidget->currentIndex() == 0){
        //end move
        if(!joint4Exceed){
            //not at joint limit, keep moving
            if(ba.at(4) == 30 && ba.at(3) == 0x31 && unlock){
                if(ui->lineMoveForward->isEnabled()){
                    on_lineMoveForward_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 48 && ba.at(3) == 0x31 && unlock){
                if(ui->lineMoveBackward->isEnabled()){
                    on_lineMoveBackward_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 46 && ba.at(3) == 0x31 && unlock){
                if(ui->lineMoveLeft->isEnabled()){
                    on_lineMoveLeft_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 32 && ba.at(3) == 0x31 && unlock){
                if(ui->lineMoveRight->isEnabled()){
                    on_lineMoveRight_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 18 && ba.at(3) == 0x31 && unlock){
                if(ui->lineMoveUp->isEnabled()){
                    on_lineMoveUp_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 33 && ba.at(3) == 0x31 && unlock){
                if(ui->lineMoveDown->isEnabled()){
                    on_lineMoveDown_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 34 && ba.at(3) == 0x31 && unlock){
                if(ui->angleMoveUp->isEnabled()){
                    on_angleMoveUp_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 35 && ba.at(3) == 0x31 && unlock){
                if(ui->angleMoveDown->isEnabled()){
                    on_angleMoveDown_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 23 && ba.at(3) == 0x31 && unlock){
                if(ui->angleMoveForward->isEnabled()){
                    on_angleMoveForward_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 36 && ba.at(3) == 0x31 && unlock){
                if(ui->angleMoveBackward->isEnabled()){
                    on_angleMoveBackward_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 37 && ba.at(3) == 0x31 && unlock){
                if(ui->angleMoveLeft->isEnabled()){
                    on_angleMoveLeft_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 38 && ba.at(3) == 0x31 && unlock){
                if(ui->angleMoveRight->isEnabled()){
                    on_angleMoveRight_pressed();unlock = false;
                }
            }
            else if(ba.at(4) == 30 && ba.at(3) == 0x30){on_lineMoveForward_released();unlock = true;}
            else if(ba.at(4) == 48 && ba.at(3) == 0x30){on_lineMoveBackward_released();unlock = true;}
            else if(ba.at(4) == 46 && ba.at(3) == 0x30){on_lineMoveLeft_released();unlock = true;}
            else if(ba.at(4) == 32 && ba.at(3) == 0x30){on_lineMoveRight_released();unlock = true;}
            else if(ba.at(4) == 18 && ba.at(3) == 0x30){on_lineMoveUp_released();unlock = true;}
            else if(ba.at(4) == 33 && ba.at(3) == 0x30){on_lineMoveDown_released();unlock = true;}
            else if(ba.at(4) == 34 && ba.at(3) == 0x30){on_angleMoveUp_released();unlock = true;}
            else if(ba.at(4) == 35 && ba.at(3) == 0x30){on_angleMoveDown_released();unlock = true;}
            else if(ba.at(4) == 23 && ba.at(3) == 0x30){on_angleMoveForward_released();unlock = true;}
            else if(ba.at(4) == 36 && ba.at(3) == 0x30){on_angleMoveBackward_released();unlock = true;}
            else if(ba.at(4) == 37 && ba.at(3) == 0x30){on_angleMoveLeft_released();unlock = true;}
            else if(ba.at(4) == 38 && ba.at(3) == 0x30){on_angleMoveRight_released();unlock = true;}
        }else{
            //at joint limit, move back
            if(ba.at(3) == 0x31){
                //a button is pressed
                if(lastClickedMoveButton == "lineMoveBackward")        on_lineMoveForward_pressed();
                else if(lastClickedMoveButton == "lineMoveDown")       on_lineMoveUp_pressed();
                else if(lastClickedMoveButton == "lineMoveForward")    on_lineMoveBackward_pressed();
                else if(lastClickedMoveButton == "lineMoveLeft")       on_lineMoveRight_pressed();
                else if(lastClickedMoveButton == "lineMoveRight")      on_lineMoveLeft_pressed();
                else if(lastClickedMoveButton == "lineMoveUp")         on_lineMoveDown_pressed();
                else if(lastClickedMoveButton == "angleMoveBackward")  on_angleMoveForward_pressed();
                else if(lastClickedMoveButton == "angleMoveDown")      on_angleMoveUp_pressed();
                else if(lastClickedMoveButton == "angleMoveForward")   on_angleMoveBackward_pressed();
                else if(lastClickedMoveButton == "angleMoveLeft")      on_angleMoveRight_pressed();
                else if(lastClickedMoveButton == "angleMoveRight")     on_angleMoveLeft_pressed();
                else if(lastClickedMoveButton == "angleMoveUp")        on_angleMoveDown_pressed();
            }else{
                //a button is released
                if(lastClickedMoveButton == "lineMoveBackward")        on_lineMoveForward_released();
                else if(lastClickedMoveButton == "lineMoveDown")       on_lineMoveUp_released();
                else if(lastClickedMoveButton == "lineMoveForward")    on_lineMoveBackward_released();
                else if(lastClickedMoveButton == "lineMoveLeft")       on_lineMoveRight_released();
                else if(lastClickedMoveButton == "lineMoveRight")      on_lineMoveLeft_released();
                else if(lastClickedMoveButton == "lineMoveUp")         on_lineMoveDown_released();
                else if(lastClickedMoveButton == "angleMoveBackward")  on_angleMoveForward_released();
                else if(lastClickedMoveButton == "angleMoveDown")      on_angleMoveUp_released();
                else if(lastClickedMoveButton == "angleMoveForward")   on_angleMoveBackward_released();
                else if(lastClickedMoveButton == "angleMoveLeft")      on_angleMoveRight_released();
                else if(lastClickedMoveButton == "angleMoveRight")     on_angleMoveLeft_released();
                else if(lastClickedMoveButton == "angleMoveUp")        on_angleMoveDown_released();
            }

        }
    }
}
/*
void MainWindow::autoRunProgram()
{
    QFile file(fileSavePath + "\\config.init");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //run program when Start
            if(keywordPieces.value(0).trimmed() == "RUNAUTO"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    int digit = keywordPieces.value(3).toInt();
                    bool status;
                    if(keywordPieces.value(4).toInt() == 0) status = false;
                    else status = true;

                    qDebug() << "***" << digit << status;
                    if(adsReadStruct.externalIORead[digit-1] == status){
                        on_pushButton_clicked(); //repeat
                        updateProgramNumber(0,"");   //close numberboard
                        runTargetProgram("C:\\SiasunRobot\\program\\" + keywordPieces.value(2).trimmed());
                        ui->functionStackWindow->setCurrentIndex(2);
                    }
                }
            }

        }
    }
}
*/
void MainWindow::emcButtonCheckThreadHandler(bool status)
{
    if(!status){
        //when button pushed down
        //ui->functionStackWindow->setDisabled(true);

        //stop the current program
        //if(adsWriteStruct.progRun){
        //    on_stopProgramButton_run_clicked();
        //    while(adsWriteStruct.progRun) QThread::msleep(50);
        //}
    }else{
        //when button released up
        //ui->functionStackWindow->setEnabled(true);
        WarningWindow::getInstance()->enableOKButton(true);
    }
}

void MainWindow::changeRunButtonStatus(bool status)
{
    ui->runProgramButton_run->setEnabled(status);
    ui->runProgramButton_run->setVisible(status);
    ui->stepProgramButton_run->setEnabled(status);
    ui->stepProgramButton_run->setVisible(status);
    ui->stopProgramButton_run->setVisible(!status);
    ui->pauseProgramButton_run->setVisible(!status);
}




void MainWindow::fun_socket_open(QString address, int port, int timeout)
{

    //connect to server
    fun_socket = new QTcpSocket(this);

    fun_socket->connectToHost(address, port);

    if(timeout == 0) timeout = 1000;//if timeout parameter is not set in function

    if(fun_socket->waitForConnected(timeout)){
        ui->debugPrint->addItem(address + " connected");
    }
    else {
        ui->debugPrint->addItem(address + " not connected");


    }

    functionFinish = true; //release the program

}

void MainWindow::fun_socket_send(QString message)
{
    //send data
    QByteArray ba = message.toLatin1();
    char* sendBuf;
    sendBuf=ba.data();
    fun_socket->write(sendBuf);

    functionFinish = true; //release the program
}


void MainWindow::fun_socket_rcv(int timeout)
{
    //------------------------------------------------------
    int time_counter = 0;
    if(timeout == 0) timeout = 3000;

    fun_socket->waitForBytesWritten(timeout);
    while((fun_socket->bytesAvailable() < 131) && (time_counter++ < 30)){
        qDebug() << "time_counter:" << time_counter;
        fun_socket->waitForReadyRead(100);
        qDebug() << "fun_socket_available:" << fun_socket->bytesAvailable();
        //QThread::msleep(100);
    }
    qDebug() << "msg size:" << fun_socket->bytesAvailable()<< "time_counter:" << time_counter ;
    if(time_counter > 30)
        qDebug() << "error: no message recive----------------------";

    QByteArray buffer = fun_socket->readAll();
    qDebug() << "fun_socket_rvc: " << buffer.data();
    QString msg = (QString)buffer;
    QStringList parameters = msg.split(',');
    if(parameters.size() == 16){
        for(int i = 1; i < 5; i++){
            targetPose1[i-1] = parameters.value(i-1).toDouble();
            qDebug() << "**pose1:" << targetPose1[i-1];
            ui->debugPrint->addItem("pose1:" + QString::number(targetPose1[i-1]));
        }
        for(int i = 5; i < 9; i++){
            targetPose2[i-5] = parameters.value(i-1).toDouble();
            qDebug() << "**pose2:" << targetPose2[i-5];
            ui->debugPrint->addItem("pose2:" + QString::number(targetPose2[i-5]));
        }
        for(int i = 9; i < 13; i++){
            targetPose3[i-9] = parameters.value(i-1).toDouble();
            qDebug() << "**pose3:" << targetPose3[i-9];
            ui->debugPrint->addItem("pose3:" + QString::number(targetPose3[i-9]));
        }
        for(int i = 13; i < 17; i++){
            targetPose4[i-13] = parameters.value(i-1).toDouble();
            qDebug() << "**pose4:" << targetPose4[i-13];
            ui->debugPrint->addItem("pose4:" + QString::number(targetPose4[i-13]));
        }
    }else{
        ui->debugPrint->addItem("read message error");
        runProgram->error = true; //an error comes, stop the program

    }
    functionFinish = true; //release the program
}

void MainWindow::fun_socket_close()
{
    fun_socket->close();

    functionFinish = true; //release the program
}



void MainWindow::changeToolCoordinate(QStringList toolCoordinate)
{
    ui->toolCoordinateLineX->setText(toolCoordinate.value(0));
    ui->toolCoordinateLineY->setText(toolCoordinate.value(1));
    ui->toolCoordinateLineZ->setText(toolCoordinate.value(2));
    ui->toolCoordinateLineRX->setText(toolCoordinate.value(3));
    ui->toolCoordinateLineRY->setText(toolCoordinate.value(4));
    ui->toolCoordinateLineRZ->setText(toolCoordinate.value(5));
}

void MainWindow::switchExpertMode(bool status)
{
    ui->pushButtonMin->setVisible(status);
    ui->setJoint0Points->setVisible(status);
    ui->writeDynamicParameters->setVisible(status);
    ui->analogIO->setVisible(status);
    ui->safeSetting->setVisible(status);
    ui->installSetting->setVisible(status);
    ui->pushButton_2->setVisible(status);
    ui->clearInfo->setVisible(status);
}

bool MainWindow::functionValidCheck(QString func)
{
    if(func == ";\n") return true;

    QStringList keywordPieces = func.split('(');
    QString keyWord = keywordPieces.value(0);
    QStringList parameterPieces = keywordPieces.value(1).split( ")" ).value(0).split( "," );

    if(func.at(0) == '#' || keyWord.contains("*")) return true;

    if(keyWord == "gripper" || keyWord == "biaoding" || keyWord == "IO_in" || keyWord == "IO_out" || keyWord == "external_hand" || keyWord == "MovePsi" || keyWord == "moveL-Z" || keyWord == "moveCenter" || keyWord == "camera" || keyWord == "replay" || keyWord == "save_status") return true;

    if(keyWord == "MoveJ" || keyWord == "Start"){
        if(parameterPieces.length() != 15){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("moveJ 参数个数错误");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("moveJ Parameter number error");
            }

            return false;
        }

        //check if parameters are all numbers
        for(int i = 0; i < 15; i++){
            //trimmed() will remove whitespace
            if(parameterPieces.value(i).toDouble() == 0 && parameterPieces.value(i).trimmed() != "0"){

                if(parameterPieces.value(i).trimmed().split('.').value(0) == "0") return true; //for 0.0000
                if(parameterPieces.value(i).trimmed().split('.').value(0) == "-0") return true; //for -0.0000
                ui->debugPrint->addItem("moveJ parameters are not all numbers");
                return false;
            }
        }

        //check axis 1 3 5 7 range
        for(int i = 8; i < 15; i+=2){
            if(parameterPieces.value(i).toDouble() < -360 || parameterPieces.value(i).toDouble() > 360){
                ui->debugPrint->addItem("moveJ axis 1 3 5 7 range wrong");
                return false;
            }
        }

        //check axis 2 4 6 range
        for(int i = 9; i < 15; i+=2){
            if(parameterPieces.value(i).toDouble() < -110 || parameterPieces.value(i).toDouble() > 110){
                ui->debugPrint->addItem("moveJ check axis 2 4 6 range wrong");
                return false;
            }
        }

        return true;

    }else if(keyWord == "Sleep" || keyWord == "sleep"){
        if(parameterPieces.length() != 1){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("sleep 参数个数错误");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("sleep Parameter number error");
            }
            return false;
        }

        if(parameterPieces.value(0).toInt() <= 0){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("sleep 参数无效");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("sleep Parameter invalid");
            }
            return false;
        }
    }else if(keyWord == "set_tcp"){
        if(parameterPieces.length() != 6){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("set_tcp 参数个数错误");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("set_tcp Parameter number error");
            }
            return false;
        }

        for(int i = 0; i < 6; i++){
            if(parameterPieces.value(i).toDouble() == 0 && parameterPieces.value(i).trimmed() != "0"){
                if(systemLanguage == "cn"){
                    ui->debugPrint->addItem("set_tcp 参数" + QString::number(i+1) + "无效");
                }else if(systemLanguage == "en"){
                    ui->debugPrint->addItem("set_tcp Parameter" + QString::number(i+1) + "invalid");
                }
                return false;
            }
        }
    }else if(keyWord == "socket_open"){
        if(parameterPieces.length() != 3){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("socket_open 参数个数错误");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("socket_open Parameter number error");
            }
            return false;
        }

        for(int i = 1; i < 2; i++){
            if(parameterPieces.value(i).toInt() == 0 && parameterPieces.value(i).trimmed() != "0"){
                if(systemLanguage == "cn"){
                    ui->debugPrint->addItem("socket_open 参数" + QString::number(i+1) + "无效");
                }else if(systemLanguage == "en"){
                    ui->debugPrint->addItem("socket_open Parameter" + QString::number(i+1) + "invalid");
                }
                return false;
            }
        }

        QStringList socketAdd = parameterPieces.value(0).split('.');
        if(socketAdd.length() != 4){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("socket_open socket地址错误");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("socket_open Socket address error");
            }
            return false;
        }

        for(int i = 0; i < 4; i++){
            if(socketAdd.value(i).toInt() == 0 && socketAdd.value(i).trimmed() != "0"){
                if(systemLanguage == "cn"){
                    ui->debugPrint->addItem("socket_open socket地址错误");
                }else if(systemLanguage == "en"){
                    ui->debugPrint->addItem("socket_open Socket address error");
                }
                return false;
            }
        }

    }else if(keyWord == "socket_send"){
        if(keywordPieces.value(1).split( ")" ).value(0) == ""){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("socket_send 无参数");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("socket_send No parameter");
            }
            return false;
        }
    }else if(keyWord == "socket_rcv"){
        if(parameterPieces.length() > 1){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("socket_rcv 参数个数错误");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("socket_rcv Parameter number error");
            }
            return false;
        }

        if(keywordPieces.value(1).split( ")" ).value(0).trimmed() != "" && parameterPieces.value(0).toInt() <= 0){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("socket_rcv 参数无效");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("socket_rcv Parameter invalid");
            }
            return false;
        }
    }else if(keyWord == "socket_close"){
        if(keywordPieces.value(1).split( ")" ).value(0).trimmed() != ""){
            if(systemLanguage == "cn"){
                ui->debugPrint->addItem("socket_close 参数无效");
            }else if(systemLanguage == "en"){
                ui->debugPrint->addItem("socket_close Parameter invalid");
            }
            return false;
        }
    }else if(keyWord == "MoveL" || keyWord == "MoveL_Psi" || keyWord == "MoveC_1" || keyWord == "MoveC_2"){
        //check if speed is in valid range
        if(parameterPieces.value(6).toDouble() > maxLineMoveSpeed || parameterPieces.value(6).toDouble() <= 0){
            ui->debugPrint->addItem("moveL/moveC 速度超限");
            return false;
        }
        return true;
    }else{
        if(systemLanguage == "cn"){
            ui->debugPrint->addItem("函数无效");
        }else if(systemLanguage == "en"){
            ui->debugPrint->addItem("Invalid function");
        }

        return false;
    }


    return true;
}

void MainWindow::switchJogSpiButton(bool status)
{
    ui->fixPositionMoveBackward->setEnabled(status);
    ui->fixPositionMoveForward->setEnabled(status);
}

void MainWindow::endToolButtonStatus(bool status)
{
    if(status || forceTeachMode){
        //end tool button pressed
        //on traction teach mode, disable all the window
        ui->mainStackWindow->setDisabled(true);

        //set traction teach FLAG to true
        adsWriteStruct.tractionTeach = true;

    }else{
        //set traction teach FLAG to false
        adsWriteStruct.tractionTeach = false;

        //enable all the window
        ui->mainStackWindow->setEnabled(true);
    }
}

void MainWindow::resetSystem()
{
    qDebug() << "**system reset";
    emit stopTimeCount();   //stop count program run time

    robotWindowChangePosition(1);

    //stop the current program
    on_stopProgramButton_run_clicked();

    adsWriteStruct.enableAllAxis = false;

    //sleep 200ms to garantee power=false can be written to the PLC
    QThread::msleep(200);

    ui->mainStackWindow->setCurrentIndex(0);

    QPixmap bkgnd(":/images/background.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //on_startStopButton_clicked
    Running = false;
    QPixmap pixmap(":/images/icon-star.png");
    QIcon ButtonIcon(pixmap);
    ui->startStopButton->setIcon(ButtonIcon);
    //adsWriteStruct.power = Running;

    //updateData->stop = true;

    runProgram->stop = true;

    adsWriteStruct.tcpIdStart = false;

    //jog
    adsWriteStruct.jogMode = -1;
    adsWriteStruct.toolCoordinateJog = false;
    ui->move3DWidget->rob->setTool(adsWriteStruct.toolCoordinateJog, adsWriteStruct.toolCoordinate[0], adsWriteStruct.toolCoordinate[1],\
            adsWriteStruct.toolCoordinate[2], adsWriteStruct.toolCoordinateR[0], adsWriteStruct.toolCoordinateR[1], adsWriteStruct.toolCoordinateR[2]);

    //run
    adsWriteStruct.progRun = false;
    adsWriteStruct.progPause = false;
    adsWriteStruct.progStop = false;
    adsWriteStruct.blockType = 4; //stop program

    adsWriteStruct.shutDown = false;
    adsWriteStruct.dataFinish = 0;
    adsWriteStruct.blendStep = 0;
    adsWriteStruct.toolCoordinateProg = false;
    adsWriteStruct.tractionTeach = false;
    adsWriteStruct.moveTo = false;
    adsWriteStruct.moveToType = 0;

    //enable the joint move mode buttons
    ui->lineMoveGroupBox->setEnabled(true);
    ui->jointMoveGroupBox->setEnabled(true);
    ui->positionGroupBox->setEnabled(true);
    ui->jointAngleGroupBox->setEnabled(true);
    ui->saveNodeButton->setEnabled(true);
    ui->jogSpeedSlider->setEnabled(true);
    ui->tractionTeachButton->setEnabled(true);
    ui->moveToButton->setEnabled(true);
    ui->pushButtonMoveToOrigin->setEnabled(true);

    adsWriteStruct.tcpCalibrationStart = false;

    //if(systemLanguage == "cn"){
    //    WarningWindow::getInstance()->setWarningText("点击系统复位");
    //}else if(systemLanguage == "en"){
    //    WarningWindow::getInstance()->setWarningText("Click to reset system");
    //}
    //WarningWindow::getInstance()->exec();

    //switch to base coordinate
    on_baseCoordinateBtn_clicked();

    adsWriteStruct.reset = true;

    QThread::msleep(200);

    adsWriteStruct.reset = false;
}

void MainWindow::startEnterValueMove()
{
    //show 3D robot
    //on_threeDViewSwitch_clicked();
}

void MainWindow::startTeach()
{
    on_tractionTeachButton_clicked();
}

void MainWindow::addReplayNode()
{        
    programAddRoot("replay(" + QString::number(adsWriteStruct.pathNum) + ", 50);");

    //save the new program tree
    saveTheNewProgramTree();
}

void MainWindow::stopEnterValueMove()
{
    //show value info
    //on_threeDViewSwitch_clicked();
    on_stopJogMove1_clicked();
}

void MainWindow::robotPowerOff()
{
    on_disconnectAction_clicked();

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("警告: 机械臂已断电");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Warning: Robot power off");
    }
    WarningWindow::getInstance()->exec();
}

void MainWindow::robotBreakState(bool state)
{
    if(state){
        ui->functionStackWindow->setDisabled(false);
    }else{
        ui->functionStackWindow->setDisabled(true);
    }
}

bool msgLock = false;
void MainWindow::   axisError()
{
    emit stopTimeCount();   //stop count program run time
    systemError = true;
    QString errorMsg;

    switch(adsReadStruct.errorID[0]){
    case 16:
        errorMsg = "读取初始化文件错误";
        break;
    case 32:
        errorMsg = "写入编码器值错误";
        break;
    case 48:
        errorMsg = "编码器读数超过180度，写入值时出错";
        break;
    case 64:
        errorMsg = "使能错误";
        break;
    case 80:
        errorMsg = "松抱闸错误";
        break;
    case 96:
        errorMsg = "轴复位错误";
        break;
    case 4097:
        errorMsg = "Jog Error\npsi set position error";
        break;
    case 4098:
        errorMsg = "Jog Error\npsi power";
        break;
    case 4099:
        errorMsg = "Jog Error\nbase psi set position error";
        break;
    case 4100:
        errorMsg = "Jog Error\nbase psi power";
        break;
    case 4101:
        errorMsg = "Jog Error\njog psi set position";
        break;
    case 4102:
        errorMsg = "Jog Error\njog psi power";
        break;
    case 4103:
        errorMsg = "Jog Error\nACT jog ";
        break;
    case 8193:
        errorMsg = "moveabs error";
        break;
    case 8194:
        errorMsg = "moveabs aborted";
        break;
    case 8195:
        errorMsg = "pause halt error";
        break;
    case 8196:
        errorMsg = "stop halt error";
        break;
    case 12289:
        errorMsg = "moveL error";
        break;
    case 12290:
        errorMsg = "moveC error";
        break;
    case 12291:
        errorMsg = "movepsi set position";
        break;
    case 12292:
        errorMsg = "movepsi power";
        break;
    case 12293:
        errorMsg = "movepsi moveabs";
        break;
    case 12294:
        errorMsg = "movepsi moveabs aborted";
        break;
    case 12295:
        errorMsg = "movepsi halt";
        break;
    case 12296:
        errorMsg = "biaoding movel error";
        break;
    case 12297:
        errorMsg = "biaoding return error";
        break;
    case 16385:
        errorMsg = "disable poslag monitor error";
        break;
    case 16386:
        errorMsg = "axis reset error";
        break;
    case 16387:
        errorMsg = "enable poslag monitor error";
        break;
    case 20481:
        errorMsg = "规划错误";
        break;
    case 20482:
        errorMsg = "stop2情况下超过速度限制1.1倍";
        break;
    case 20483:
        errorMsg = "超过加速度限制1.2倍";
        break;
    case 20484:
        errorMsg = "stop2情况下超过246物理极限位置";
        break;
    case 20485:
        errorMsg = "stop1运行100ms后仍然超过安全速度";
        break;
    case 20486:
        errorMsg = "超过246物理极限";
        break;
    case 20487:
        errorMsg = "stop1中mc_halt错误";
        break;
    case 20736:
        WarningWindow::getInstance()->enableOKButton(false);
        errorMsg = "特殊状态下按软急停\n请松开急停";
        break;
    case 20992:
        WarningWindow::getInstance()->enableOKButton(false);
        errorMsg = "常规运动状态下按软急停\n请松开急停";
        break;
    case 21248:
        errorMsg = "断通讯";
        break;
    default:
        errorMsg = "发生未知错误" + QString::number(adsReadStruct.errorID[0]);
    }


    WarningWindow::getInstance()->setWarningText(errorMsg);


    //log
    switch(adsReadStruct.errorID[0]){
    case 16:
        errorMsg = "read init file error";
        break;
    case 32:
        errorMsg = "write to encoder error";
        break;
    case 48:
        errorMsg = "encoder value exceed 180";
        break;
    case 64:
        errorMsg = "enable error";
        break;
    case 80:
        errorMsg = "brake error";
        break;
    case 96:
        errorMsg = "axis reset error";
        break;
    case 4097:
        errorMsg = "Jog Error\npsi set position error";
        break;
    case 4098:
        errorMsg = "Jog Error\npsi power";
        break;
    case 4099:
        errorMsg = "Jog Error\nbase psi set position error";
        break;
    case 4100:
        errorMsg = "Jog Error\nbase psi power";
        break;
    case 4101:
        errorMsg = "Jog Error\njog psi set position";
        break;
    case 4102:
        errorMsg = "Jog Error\njog psi power";
        break;
    case 4103:
        errorMsg = "Jog Error\nACT jog ";
        break;
    case 8193:
        errorMsg = "moveabs error";
        break;
    case 8194:
        errorMsg = "moveabs aborted";
        break;
    case 8195:
        errorMsg = "pause halt error";
        break;
    case 8196:
        errorMsg = "stop halt error";
        break;
    case 12289:
        errorMsg = "moveL error";
        break;
    case 12290:
        errorMsg = "moveC error";
        break;
    case 12291:
        errorMsg = "movepsi set position";
        break;
    case 12292:
        errorMsg = "movepsi power";
        break;
    case 12293:
        errorMsg = "movepsi moveabs";
        break;
    case 12294:
        errorMsg = "movepsi moveabs aborted";
        break;
    case 12295:
        errorMsg = "movepsi halt";
        break;
    case 12296:
        errorMsg = "biaoding movel error";
        break;
    case 12297:
        errorMsg = "biaoding return error";
        break;
    case 16385:
        errorMsg = "disable poslag monitor error";
        break;
    case 16386:
        errorMsg = "axis reset error";
        break;
    case 16387:
        errorMsg = "enable poslag monitor error";
        break;
    case 20481:
        errorMsg = "planning error";
        break;
    case 20482:
        errorMsg = "stop2 exceed speed limit 10%";
        break;
    case 20483:
        errorMsg = "exceed acceleration limit 20%";
        break;
    case 20484:
        errorMsg = "stop2 exceed axis 2/4/6 physic limit";
        break;
    case 20485:
        errorMsg = "stop1 run 100ms, still exceed safe speed";
        break;
    case 20486:
        errorMsg = "exceed axis 2/4/6 physic limit";
        break;
    case 20487:
        errorMsg = "stop1 mc_halt error";
        break;
    case 20736:
        errorMsg = "push Emergency STOP button in special situation";
        break;
    case 20992:
        errorMsg = "push Emergency STOP button in normal move situation";
        break;
    case 21248:
        errorMsg = "communication disconnect";
        break;
    default:
        errorMsg = "unknow error" + QString::number(adsReadStruct.errorID[0]);
    }
    LogFile::logFileCreate("axis error", errorMsg); //save run program log
    LogFile::logFileCreate("slave status: ", QString::number((int)adsReadStruct.slave_ready[0]) + " " +\
            QString::number((int)adsReadStruct.slave_ready[1]) + " " +\
            QString::number((int)adsReadStruct.slave_ready[2]) + " " +\
            QString::number((int)adsReadStruct.slave_ready[3]) + " " +\
            QString::number((int)adsReadStruct.slave_ready[4]) + " " +\
            QString::number((int)adsReadStruct.slave_ready[5]) + " " +\
            QString::number((int)adsReadStruct.slave_ready[6])); //save run program log


    WarningWindow::getInstance()->exec();

    resetSystem();
}

void MainWindow::axisWarning()
{
    stopJogAxis();



    //uncheck the repeat button
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/repeat.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/repeat-en.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }

    repeatProgram = false;
    runProgramRepeatTime = 0;
    ui->repeatTimeLabel->setText("");


    bool hide = false;
    QString errorMsg = "";
    for(int i = 1; i <=7; i++){
        switch((int)adsReadStruct.warningID[i-1]){
        case 0: break;
        case 1: errorMsg = errorMsg + "关节" + QString::number(i) + "超速\n";break;
        case 2: errorMsg = errorMsg + "关节" + QString::number(i) + "加速度超速\n";break;
        case 3: errorMsg = errorMsg + "关节" + QString::number(i) + "超过正限位\n";hide = true;break;
        case 4: errorMsg = errorMsg + "关节" + QString::number(i) + "超过负限位\n";hide = true;break;
        default: errorMsg = errorMsg + "关节" + QString::number(i) + "未知错误\n";
        }
    }

    if(!hide){
        QPixmap bkgnd(":/images/background3.png");
        bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);
        ui->mainStackWindow->setCurrentIndex(5);
        ui->warningText->setText(errorMsg);
    }
    hide = false;



    //for log file
    errorMsg = "";
    for(int i = 1; i <=7; i++){
        switch((int)adsReadStruct.warningID[i-1]){
        case 0: break;
        case 1: errorMsg = errorMsg + "axis " + QString::number(i) + " exceed the speed limit ";break;
        case 2: errorMsg = errorMsg + "axis " + QString::number(i) + " exceed the acceleration limit ";break;
        case 3: errorMsg = errorMsg + "axis " + QString::number(i) + " exceed the positive physic limit ";break;
        case 4: errorMsg = errorMsg + "axis " + QString::number(i) + " exceed the negative physic limit ";break;
        default: errorMsg = errorMsg + "axis " + QString::number(i) + " unknow error ";
        }
    }
    LogFile::logFileCreate("axis warning", errorMsg); //save run program log


    adsWriteStruct.clearWarning = true;
    QThread::msleep(200);
    adsWriteStruct.clearWarning = false;

    axisWarningOn = false; //open axis warning


}

void MainWindow::pressButtonAddNode()
{
    if(ui->functionStackWindow->currentIndex() == 1 && toolButtonSaveNode && adsWriteStruct.tractionTeach){
        //in program window and in teach mode
        addMoveJNode(); //add a movej node
        on_saveNodeButton_clicked(); //save current position
        updateProgramNumber(6, "1");  //add blend
    }
}

void MainWindow::selfCollisionDetect()
{
    if(programIsRunning) on_pauseProgramButton_run_clicked();
    WarningWindow::getInstance()->setWarningText("警告: 机械臂自身将要碰撞\n请注意机械臂姿态");
    WarningWindow::getInstance()->exec();
}

void MainWindow::wallCollisionDetect()
{
    if(programIsRunning) on_pauseProgramButton_run_clicked();
    WarningWindow::getInstance()->setWarningText("警告: 机械臂碰撞到虚拟边界");
    WarningWindow::getInstance()->exec();
}

void MainWindow::programError(QString msg)
{
    //uncheck the repeat button
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/repeat.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/repeat-en.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }

    repeatProgram = false;
    runProgramRepeatTime = 0;
    ui->repeatTimeLabel->setText("");

    if(msg == "") emit sendErrorMsg2Client("Syntax error");
    else emit sendErrorMsg2Client(msg);
    adsWriteStruct.dataFinish = -2;
    adsWriteStruct.blockType = 4; //stop program
    QThread::msleep(500);

    if(systemLanguage == "cn"){
        if(msg == "")msg = "请检查函数或变量是否有效";
        WarningWindow::getInstance()->setWarningText("发现以下错误:\n" + msg);
    }else if(systemLanguage == "en"){
        if(msg == "")msg = "Unknown error";
        WarningWindow::getInstance()->setWarningText("Find following error:\n" + msg);
    }
    WarningWindow::getInstance()->exec();
    LogFile::logFileCreate("program syntax error", msg); //save run program log

    adsWriteStruct.progStop = false;
}


void MainWindow::startJogAxis(int num, int direction)
{
    if(Running){
        adsWriteStruct.axisAngleMoveDirection[num] = direction;
        adsWriteStruct.jogMode = 2;
        lastClickedMoveButton = "none";
    }
}


//&********************************axis1********************************&//
void MainWindow::on_axis1MoveBackward_pressed()
{
    //move backward
    startJogAxis(0, -1);
}

void MainWindow::on_axis1MoveBackward_released()
{
    stopJogAxis();
}

void MainWindow::on_axis1MoveForward_pressed()
{
    //move forward
    startJogAxis(0, 1);
}

void MainWindow::on_axis1MoveForward_released()
{
    stopJogAxis();
}


//&********************************axis2********************************&//
void MainWindow::on_axis2MoveBackward_pressed()
{
    //move backward
    startJogAxis(1, -1);
}

void MainWindow::on_axis2MoveBackward_released()
{
    stopJogAxis();
}

void MainWindow::on_axis2MoveForward_pressed()
{
    //move forward
    startJogAxis(1, 1);
}


void MainWindow::on_axis2MoveForward_released()
{
    stopJogAxis();
}

//&********************************axis3********************************&//
void MainWindow::on_axis3MoveBackward_pressed()
{
    //move backward
    startJogAxis(2, -1);
}

void MainWindow::on_axis3MoveBackward_released()
{
    stopJogAxis();
}

void MainWindow::on_axis3MoveForward_pressed()
{
    //move forward
    startJogAxis(2, 1);
}

void MainWindow::on_axis3MoveForward_released()
{
    stopJogAxis();
}

//&********************************axis4********************************&//
void MainWindow::on_axis4MoveBackward_pressed()
{
    //move backward
    startJogAxis(3, -1);
}

void MainWindow::on_axis4MoveBackward_released()
{
    stopJogAxis();
}

void MainWindow::on_axis4MoveForward_pressed()
{
    //move forward
    startJogAxis(3, 1);
}

void MainWindow::on_axis4MoveForward_released()
{
    stopJogAxis();
}

//&********************************axis5********************************&//
void MainWindow::on_axis5MoveBackward_pressed()
{
    //move backward
    startJogAxis(4, -1);
}

void MainWindow::on_axis5MoveBackward_released()
{
    stopJogAxis();
}

void MainWindow::on_axis5MoveForward_pressed()
{
    //move forward
    startJogAxis(4, 1);
}

void MainWindow::on_axis5MoveForward_released()
{
    stopJogAxis();
}

//&********************************axis6********************************&//
void MainWindow::on_axis6MoveBackward_pressed()
{
    //move backward
    startJogAxis(5, -1);
}

void MainWindow::on_axis6MoveBackward_released()
{
    stopJogAxis();
}

void MainWindow::on_axis6MoveForward_pressed()
{
    //move forward
    startJogAxis(5, 1);
}

void MainWindow::on_axis6MoveForward_released()
{
    stopJogAxis();
}

//&********************************axis7********************************&//
void MainWindow::on_axis7MoveBackward_pressed()
{
    //move backward
    startJogAxis(6, -1);
}

void MainWindow::on_axis7MoveBackward_released()
{
    stopJogAxis();
}

void MainWindow::on_axis7MoveForward_pressed()
{
    //move forward
    startJogAxis(6, 1);
}

void MainWindow::on_axis7MoveForward_released()
{
    stopJogAxis();
}



//&********************************Flange position X********************************&//
void MainWindow::on_lineMoveForward_pressed()
{
    //line move forward
    if(Running){
        adsWriteStruct.flangeMoveDirection[0] = -1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "lineMoveForward";
    }

}

void MainWindow::on_lineMoveForward_released()
{
    stopJogLine();
}

void MainWindow::on_lineMoveBackward_pressed()
{
    //line move backward
    if(Running){
        adsWriteStruct.flangeMoveDirection[0] = 1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "lineMoveBackward";
    }
}

void MainWindow::on_lineMoveBackward_released()
{
    stopJogLine();
}

//&********************************Flange position Y********************************&//
void MainWindow::on_lineMoveLeft_pressed()
{
    //line move left
    if(Running){
        adsWriteStruct.flangeMoveDirection[1] = -1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "lineMoveLeft";
    }
}

void MainWindow::on_lineMoveLeft_released()
{
    stopJogLine();
}

void MainWindow::on_lineMoveRight_pressed()
{
    //line move right
    if(Running){
        adsWriteStruct.flangeMoveDirection[1] = 1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "lineMoveRight";
    }
}

void MainWindow::on_lineMoveRight_released()
{
    stopJogLine();
}

//&********************************Flange position Z********************************&//
void MainWindow::on_lineMoveUp_pressed()
{
    //line move up
    if(Running){
        adsWriteStruct.flangeMoveDirection[2] = -1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "lineMoveUp";
    }
}

void MainWindow::on_lineMoveUp_released()
{
    stopJogLine();
}

void MainWindow::on_lineMoveDown_pressed()
{
    //line move down
    if(Running){
        adsWriteStruct.flangeMoveDirection[2] = 1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "lineMoveDown";
    }
}

void MainWindow::on_lineMoveDown_released()
{
    stopJogLine();
}

//&********************************Flange position RX********************************&//
void MainWindow::on_angleMoveUp_pressed()
{
    //angle move up
    if(Running){
        adsWriteStruct.flangeMoveDirection[3] = -1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "angleMoveUp";
    }
}

void MainWindow::on_angleMoveUp_released()
{
    stopJogLine();
}

void MainWindow::on_angleMoveDown_pressed()
{
    //angle move down
    if(Running){
        adsWriteStruct.flangeMoveDirection[3] = 1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "angleMoveDown";
    }
}

void MainWindow::on_angleMoveDown_released()
{
    stopJogLine();
}

//&********************************Flange position RY********************************&//
void MainWindow::on_angleMoveForward_pressed()
{
    //angle move forward
    if(Running){
        adsWriteStruct.flangeMoveDirection[4] = -1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "angleMoveForward";
    }
}

void MainWindow::on_angleMoveForward_released()
{
    stopJogLine();
}

void MainWindow::on_angleMoveBackward_pressed()
{
    //angle move backward
    if(Running){
        adsWriteStruct.flangeMoveDirection[4] = 1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "angleMoveBackward";
    }
}

void MainWindow::on_angleMoveBackward_released()
{
    stopJogLine();
}

//&********************************Flange position RZ********************************&//
void MainWindow::on_angleMoveLeft_pressed()
{
    //angle move left
    if(Running){
        adsWriteStruct.flangeMoveDirection[5] = -1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "angleMoveLeft";
    }
}

void MainWindow::on_angleMoveLeft_released()
{
    stopJogLine();
}

void MainWindow::on_angleMoveRight_pressed()
{
    //angle move right
    if(Running){
        adsWriteStruct.flangeMoveDirection[5] = 1;
        adsWriteStruct.jogMode = 1;
        lastClickedMoveButton = "angleMoveRight";
    }
}

void MainWindow::on_angleMoveRight_released()
{
    stopJogLine();
}


int MainWindow::selectProgramHandler(QString name){

    //stop the ancient program first
    if(adsWriteStruct.progRun) on_stopProgramButton_run_clicked();

    if(name == ""){
        QDir myDir(fileSavePath + "\\program");
        QStringList allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

        ui->programNameList->clear();

        if(allFiles.count() == 0) return 1;

        for(int i = 0; i < allFiles.count(); i++){
            ui->programNameList->addItem(allFiles.at(i));
        }

        ui->programChooseOK->setVisible(true);
        ui->programChooseDelete->setVisible(false);
        ui->importProgram->setVisible(true);
        ui->functionStackWindow->setCurrentIndex(8);

        ui->programNameList->setCurrentRow(0);//delete will cause null pointer problem
        on_programNameList_itemClicked(ui->programNameList->currentItem());

        selectProgramToRun = true;

        return 0;
    }else{
        runProgramName = name;
    }


    QFile file(runProgramName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return 1;

    //show program name on table
    QStringList pieces = runProgramName.split("/");
    ui->selectedProgramName->setText(pieces.value(pieces.size() - 1));

    totalCommandLineNum = 0;

    //show program on the list widget
    ui->runDataListWidget->clear();


    bool fileValid = true;

    //clear existing nodes in 3D simulator
    ui->move3DWidget->rob->clearNodes();
    programCommand.clear();

    int i = 1;
    while (!file.atEnd()) {
        //read each line
        QString line = file.readLine();


        //blank line
        //if(line != "\n") {
        totalCommandLineNum++;
        ui->runDataListWidget->addItem(QString::number(i++) + " " + line);
        programCommand.push_back(line);

        //check if the file is valid
        //if(!functionValidCheck(line)) fileValid = false;

        //add program node to the 3D model
        //if(fileValid) addNode2Simulator(line);

        //}
    }


    if(fileValid){
        if(systemLanguage == "cn"){
            ui->programStatus->setText("就绪");
        }else if(systemLanguage == "en"){
            ui->programStatus->setText("Ready");
        }

        ui->runProgramButton_run->setEnabled(true);
        ui->stepProgramButton_run->setEnabled(true);
        ui->stepProgramButton_run->setVisible(true);
        ui->runProgramButton_run->setVisible(true);
    }else{
        ui->runDataListWidget->clear();
        adsWriteStruct.blockType = 0;
        ui->selectedProgramName->setText("");
        totalCommandLineNum = 0;
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告: 此程序有错误，请检查");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Warning: Errors find in this program");
        }
        WarningWindow::getInstance()->exec();
    }

    file.close();

    return 0;
}


void MainWindow::on_selectProgramButton_clicked()
{
    if(adsWriteStruct.progRun){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告: 是否要停止当前运行的程序?");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Stop the current program?");
        }

        WarningWindow::getInstance()->showCancelButton(true);
        WarningWindow::getInstance()->exec();
        WarningWindow::getInstance()->showCancelButton(false);


        if (!WarningWindow::getInstance()->yesOrNo) return;
    }

    selectProgramHandler();

}


void MainWindow::on_runProgramButton_run_clicked()
{

    if(adsWriteStruct.progPause && !adsReadStruct.pauseDone && !stopOrRunEnable) return;
    //QThread::msleep(200);


    if(!programIsRunning){

        QFile checkFile(runProgramName);

        //nearly 1M
        if(checkFile.size() > 1000000){
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("警告: 程序过大");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Program too large");
            }
            WarningWindow::getInstance()->exec();

            checkFile.close();
            return;
        }

        if(!checkFile.exists()){
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("警告: 程序不存在");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Program doesn't exist");
            }
            WarningWindow::getInstance()->exec();

            checkFile.close();
            return;
        }
        checkFile.close();

        //check if need to move to the START point manually
        if(!adsWriteStruct.progPause && !repeatProgram && checkStartPoint && (runProgramRepeatTime == 0)){
            QFile file(runProgramName);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
            QString line = file.readLine();
            while(line == "" || line == "\n" || line.at(0) == '#'){
                line = file.readLine();
                if(file.atEnd()) break;
            }
            QString keyword = line.split('(').value(0);

            if(keyword == "Start" || keyword == "MoveJ" || keyword == "MoveL"){
                QStringList parameterPieces = line.split('(').value(1).split( ")" ).value(0).split( "," );
                double variance = 0;
                for(int i = 0; i < 7; i++){
                    variance+=abs(parameterPieces.value(i + 8).toDouble() - adsReadStruct.axisCurrentAngle[i]);
                }
                if(variance > 1){
                    //set all the axis
                    for(int i = 8; i < 15; i++)
                        programAxisMoveTo[i-8] = parameterPieces.value(i).toDouble();

                    //current position is not the START position
                    ui->stackedWidgetRunProg->setCurrentIndex(1);
                    return;
                }
            }
            file.close();
        }
    }

    checkStartPoint = true;

    ui->stackedWidgetRunProg->setCurrentIndex(0);

    //send Start program message to PLC
    adsWriteStruct.progRun = true;
    adsWriteStruct.progPause = false;
    adsWriteStruct.progStop = false;

    if(systemLanguage == "cn"){
        ui->programStatus->setText("运行中");
    }else if(systemLanguage == "en"){
        ui->programStatus->setText("Run");
    }

    runProgram->stop = false;
    runProgram->setProgramName(runProgramName);
    runProgram->start();
    runProgram->stepRunModeEnable = false;

    //start run program time count
    ui->runTimeLabel->setVisible(true);
    emit startTimeCount();

    //send feedback
    programIsRunning = true;



    ui->stepProgramButton_run->setDisabled(true);
    ui->runProgramButton_run->setDisabled(true);
    ui->stopProgramButton_run->setEnabled(true);
    ui->pauseProgramButton_run->setEnabled(true);
    ui->stepProgramButton_run->setDisabled(true);

    ui->stepProgramButton_run->setVisible(false);
    ui->runProgramButton_run->setVisible(false);
    ui->pauseProgramButton_run->setVisible(true);
    ui->stopProgramButton_run->setVisible(true);


    //disable the joint move mode buttons
    ui->lineMoveGroupBox->setDisabled(true);
    ui->jointMoveGroupBox->setDisabled(true);
    ui->positionGroupBox->setDisabled(true);
    ui->jointAngleGroupBox->setDisabled(true);
    ui->saveNodeButton->setDisabled(true);
    ui->jogSpeedSlider->setDisabled(true);
    ui->tractionTeachButton->setDisabled(true);
    ui->moveToButton->setDisabled(true);
    ui->pushButtonMoveToOrigin->setDisabled(true);

    //let adsWriteStruct.progRun signal be true and PLC clear the collision signal
    if(collisionSignalSend) QThread::msleep(500);

    collisionSignalSend = false;

}

void MainWindow::on_pauseProgramButton_run_clicked()
{
    emit stopTimeCount();   //stop count program run time

    //send message to PLC
    adsWriteStruct.progRun = false;
    adsWriteStruct.progPause = true;
    adsWriteStruct.progStop = false;


    QThread::msleep(500);

    ui->pauseProgramButton_run->setDisabled(true);
    ui->runProgramButton_run->setEnabled(true);
    ui->stepProgramButton_run->setDisabled(true);
    ui->stopProgramButton_run->setEnabled(true);

    ui->stepProgramButton_run->setVisible(false);
    ui->runProgramButton_run->setVisible(true);
    ui->pauseProgramButton_run->setVisible(false);
    ui->stopProgramButton_run->setVisible(true);

    if(systemLanguage == "cn"){
        ui->programStatus->setText("暂停");
    }else if(systemLanguage == "en"){
        ui->programStatus->setText("Pause");
    }
}

void MainWindow::on_disconnect_clicked()
{
    ui->functionStackWindow->setEnabled(true);
    ui->functionStackWindow->setCurrentIndex(6);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/nav-icon-move1.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-program1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-setting1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-disconnect.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/images/nav-icon-about1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/nav-icon-move1-en.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-program1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-setting1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-disconnect.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/img_en/img_en/nav-icon-about1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_disconnectAction_clicked()
{
    QPixmap bkgnd(":/images/background.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    adsWriteStruct.enableAllAxis = false;   //disable all axis

    //stop the current program
    if(adsWriteStruct.progRun){
        on_stopProgramButton_run_clicked();
        //wait for stop finish
        while(adsWriteStruct.progRun) QThread::msleep(50);
    }

    on_stopJogMove1_clicked();
    QThread::msleep(100);


    Running = false;
    //QPixmap pixmap(":/images/icon-star.png");
    //QIcon ButtonIcon(pixmap);
    //ui->startStopButton->setIcon(ButtonIcon);
    //ui->startStopButton->setIconSize(QSize(70,70));
    adsWriteStruct.power = Running;

    //stop data update thread
    //sleep 200ms to garantee power=false can be written to the PLC
    QThread::msleep(200);
    updateData->stop = true;


    ui->mainStackWindow->setCurrentIndex(0);
}

void MainWindow::on_startStopButton_clicked()
{
    readXMLFile();  //load XML files

    hasAdsError = false;
    checkSlaveStatus = true;
    adsWriteStruct.tcpIdStart = false;

    Running = true;
    adsWriteStruct.power = Running;
    runProgram->stop = false;

    //Start data update thread
    updateData->stop = false;
    updateData->start();

    QPixmap bkgnd(":/images/background3.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //joint active page
    ui->mainStackWindow->setCurrentIndex(4);
}


void MainWindow::startUpLoadDone()
{
    if(!hasAdsError){
        QPixmap bkgnd(":/images/background2.png");
        bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);

        //change the main stack index to 2: control page
        ui->mainStackWindow->setCurrentIndex(2);
        ui->functionStackWindow->setCurrentIndex(0);
        ui->functionStackWindow->setCurrentIndex(2);
        ui->functionStackWindow->setCurrentIndex(0);

        on_jogMoveButton_clicked();

        systemError = false;

        //仿真界面需激活一下，否则ShapeDrawable无法显示
        on_threeDViewSwitch_clicked();

        if(startUpUseTool && virtualWallSetted) on_setToolCoordinateButton_clicked();
    }else{
        ui->mainStackWindow->setCurrentIndex(0);

        if(systemLanguage == "cn"){
            if(adsWriteStruct.tcpLoad == 0)
                WarningWindow::getInstance()->setWarningText("末端无工具");
            else
                WarningWindow::getInstance()->setWarningText("末端工具质量:" + QString::number(adsWriteStruct.tcpLoad)+"kg\n"+\
                                                             "质心位置:"+ QString::number(adsWriteStruct.tcpLoadPos[0]) + " "+ QString::number(adsWriteStruct.tcpLoadPos[1]) + " "+ QString::number(adsWriteStruct.tcpLoadPos[2]));
        }else if(systemLanguage == "en"){
            if(adsWriteStruct.tcpLoad == 0)
                WarningWindow::getInstance()->setWarningText("No tool installed");
            else
                WarningWindow::getInstance()->setWarningText("Tool weight:" + QString::number(adsWriteStruct.tcpLoad)+"kg\n"+\
                                                             "Weight position:"+ QString::number(adsWriteStruct.tcpLoadPos[0]) + " "+ QString::number(adsWriteStruct.tcpLoadPos[1]) + " "+ QString::number(adsWriteStruct.tcpLoadPos[2]));
        }
        WarningWindow::getInstance()->exec();
    }
}

void MainWindow::hideRunButtons(bool stop, bool run, bool step, bool pause)
{
    ui->stopProgramButton_run->setVisible(stop);
    ui->runProgramButton_run->setVisible(run);
    ui->stepProgramButton_run->setVisible(step);
    ui->pauseProgramButton_run->setVisible(pause);
}

void MainWindow::externalIOReadUpdate(bool *ioStatus)
{
    QPixmap pixmapOK(":/images/node_ok.png");
    QPixmap pixmapWarning(":/images/node_warning.png");

    //if(ioStatus[0]) ui->ioInput1->setPixmap(pixmapOK);
    //else ui->ioInput1->setPixmap(pixmapWarning);
    //if(ioStatus[1]) ui->ioInput2->setPixmap(pixmapOK);
    //else ui->ioInput2->setPixmap(pixmapWarning);
    if(ioStatus[0]) ui->ioInput3->setPixmap(pixmapOK);
    else ui->ioInput3->setPixmap(pixmapWarning);
    if(ioStatus[1]) ui->ioInput4->setPixmap(pixmapOK);
    else ui->ioInput4->setPixmap(pixmapWarning);
    if(ioStatus[2]) ui->ioInput5->setPixmap(pixmapOK);
    else ui->ioInput5->setPixmap(pixmapWarning);
    if(ioStatus[3]) ui->ioInput6->setPixmap(pixmapOK);
    else ui->ioInput6->setPixmap(pixmapWarning);
    if(ioStatus[4]) ui->ioInput7->setPixmap(pixmapOK);
    else ui->ioInput7->setPixmap(pixmapWarning);
    if(ioStatus[5]) ui->ioInput8->setPixmap(pixmapOK);
    else ui->ioInput8->setPixmap(pixmapWarning);
}

void MainWindow::externalIOWriteUpdate(bool* ioStatus)
{
    QPixmap pixmapOK(":/images/node_ok.png");
    QPixmap pixmapWarning(":/images/node_warning.png");
    QIcon iconOK(pixmapOK);
    QIcon iconWarning(pixmapWarning);


    //if(ioStatus[0]){ ui->ioOutput1->setIcon(iconOK);ui->ioOutput1->setIconSize(pixmapOK.rect().size());}
    //else{ ui->ioOutput1->setIcon(iconWarning);ui->ioOutput1->setIconSize(pixmapWarning.rect().size());}
    //if(ioStatus[1]){ ui->ioOutput2->setIcon(iconOK);ui->ioOutput2->setIconSize(pixmapOK.rect().size());}
    //else{ ui->ioOutput2->setIcon(iconWarning);ui->ioOutput2->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[0]){ ui->ioOutput3->setIcon(iconOK);ui->ioOutput3->setIconSize(pixmapOK.rect().size());}
    else{ ui->ioOutput3->setIcon(iconWarning);ui->ioOutput3->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[1]){ ui->ioOutput4->setIcon(iconOK);ui->ioOutput4->setIconSize(pixmapOK.rect().size());}
    else{ ui->ioOutput4->setIcon(iconWarning);ui->ioOutput4->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[2]){ ui->ioOutput5->setIcon(iconOK);ui->ioOutput5->setIconSize(pixmapOK.rect().size());}
    else{ ui->ioOutput5->setIcon(iconWarning);ui->ioOutput5->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[3]){ ui->ioOutput6->setIcon(iconOK);ui->ioOutput6->setIconSize(pixmapOK.rect().size());}
    else{ ui->ioOutput6->setIcon(iconWarning);ui->ioOutput6->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[4]){ ui->ioOutput7->setIcon(iconOK);ui->ioOutput7->setIconSize(pixmapOK.rect().size());}
    else{ ui->ioOutput7->setIcon(iconWarning);ui->ioOutput7->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[5]){ ui->ioOutput8->setIcon(iconOK);ui->ioOutput8->setIconSize(pixmapOK.rect().size());}
    else{ ui->ioOutput8->setIcon(iconWarning);ui->ioOutput8->setIconSize(pixmapWarning.rect().size());}


}

void MainWindow::externalToolIOReadUpdate(bool *ioStatus)
{
    QPixmap pixmapOK(":/images/node_ok.png");
    QPixmap pixmapWarning(":/images/node_warning.png");

    if(ioStatus[0]) ui->toolInput1->setPixmap(pixmapOK);
    else ui->toolInput1->setPixmap(pixmapWarning);
    if(ioStatus[1]) ui->toolInput2->setPixmap(pixmapOK);
    else ui->toolInput2->setPixmap(pixmapWarning);
    if(ioStatus[2]) ui->toolInput3->setPixmap(pixmapOK);
    else ui->toolInput3->setPixmap(pixmapWarning);
    if(ioStatus[3]) ui->toolInput4->setPixmap(pixmapOK);
    else ui->toolInput4->setPixmap(pixmapWarning);
}

void MainWindow::externalToolIOWriteUpdate(bool *ioStatus)
{
    QPixmap pixmapOK(":/images/node_ok.png");
    QPixmap pixmapWarning(":/images/node_warning.png");
    QIcon iconOK(pixmapOK);
    QIcon iconWarning(pixmapWarning);

    if(ioStatus[0]){ ui->toolOutput1->setIcon(iconOK);ui->toolOutput1->setIconSize(pixmapOK.rect().size());}
    else{ ui->toolOutput1->setIcon(iconWarning);ui->toolOutput1->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[1]){ ui->toolOutput2->setIcon(iconOK);ui->toolOutput2->setIconSize(pixmapOK.rect().size());}
    else{ ui->toolOutput2->setIcon(iconWarning);ui->toolOutput2->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[2]){ ui->toolOutput3->setIcon(iconOK);ui->toolOutput3->setIconSize(pixmapOK.rect().size());}
    else{ ui->toolOutput3->setIcon(iconWarning);ui->toolOutput3->setIconSize(pixmapWarning.rect().size());}
    if(ioStatus[3]){ ui->toolOutput4->setIcon(iconOK);ui->toolOutput4->setIconSize(pixmapOK.rect().size());}
    else{ ui->toolOutput4->setIcon(iconWarning);ui->toolOutput4->setIconSize(pixmapWarning.rect().size());}
}


void MainWindow::on_jogMoveButton_clicked()
{
    //move tab
    robotWindowChangePosition(1);

    ui->functionStackWindow->setCurrentIndex(0);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/nav-icon-move2.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-program1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-setting1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-move.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/images/nav-icon-about1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/nav-icon-move2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-program1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-setting1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-move.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/img_en/img_en/nav-icon-about1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }

}

void MainWindow::on_programButton_clicked()
{
    if(inCameraProgram)
        ui->functionStackWindow->setCurrentIndex(9);
    else if(programOpened){
        ui->functionStackWindow->setCurrentIndex(1);

        //ui->editButton->setVisible(true);
        //ui->moveButton->setVisible(false);
        //ui->moveToButton->setVisible(false);
        //ui->transitionButton->setVisible(false);

        QTreeWidgetItem *current, *previous;
        on_programTree_currentItemChanged(current, previous);
    }
    else
        ui->functionStackWindow->setCurrentIndex(7);



    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/nav-icon-move1.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-program2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-setting1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-program.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/images/nav-icon-about1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/nav-icon-move1-en.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-program2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-setting1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-program.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/img_en/img_en/nav-icon-about1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_runButton_clicked()
{
    //not running, refresh the program
    //if(!adsWriteStruct.progRun) on_refreshProgram_clicked();

    //run tab
    robotWindowChangePosition(3);

    ui->stackedWidgetRunProg->setCurrentIndex(0);
    ui->functionStackWindow->setCurrentIndex(2);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/nav-icon-move1.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-program1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-setting1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-run.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/images/nav-icon-about1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/nav-icon-move1-en.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-program1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-setting1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-run.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/img_en/img_en/nav-icon-about1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_statusButton_clicked()
{
    ui->functionStackWindow->setCurrentIndex(3);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/nav-icon-move1.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-program1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-setting1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-status.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/images/nav-icon-about1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/nav-icon-move1-en.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-program1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-setting1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-status.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/img_en/img_en/nav-icon-about1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_settingButton_clicked()
{
    ui->functionStackWindow->setCurrentIndex(4);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/nav-icon-move1.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-program1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-setting2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-setting.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/images/nav-icon-about1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/nav-icon-move1-en.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-program1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-setting2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-setting.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/img_en/img_en/nav-icon-about1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_aboutButton_clicked()
{
    ui->functionStackWindow->setCurrentIndex(5);

    if(systemLanguage == "cn"){
        ui->version_label->setText("版本: " + appVersion + "\n\nCopyright 2017 - 中科新松有限公司\n\nwww.siasun-in.com");
    }else if(systemLanguage == "en"){
        ui->version_label->setText("Version: " + appVersion + "\n\nCopyright 2017 - SIASUN\n\nwww.siasun-in.com");
    }

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/nav-icon-move1.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-program1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-setting1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav_about.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/images/nav-icon-about2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/nav-icon-move1-en.png");
        QIcon ButtonIcon(pixmap);
        ui->jogMoveButton->setIcon(ButtonIcon);
        ui->jogMoveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-program1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-setting1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->settingButton->setIcon(ButtonIcon);
        ui->settingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav_about.png");
        ui->status_label->setPixmap(pixmap);
        pixmap.load(":/img_en/img_en/nav-icon-about2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_shutDownButton_clicked()
{
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("确认关闭系统?");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Shut down the machine?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo) {
        if(!Running){
            //Start data update thread
            updateData->stop = false;
            updateData->start();
            QThread::msleep(300);
        }

        //make the TwinCAT into config mode
        adsWriteStruct.configTwinCAT = true;
        QThread::msleep(1000);

        shutDown = true;

        adsWriteStruct.shutDown = shutDown;

        //sleep 200ms to garantee power=false can be written to the PLC
        QThread::msleep(200);

        QApplication::quit();

        if(stopProgramShutDownMachine) system("shutdown -s -t 1");
    }
}


void MainWindow::on_createButton_clicked()
{
    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();

    createName = "";
    //show keyboard to enter the name
    keyboard->setModal(true);
    keyboard->expertModeSwitch(expertMode);
    keyboard->cleanEditLine();
    keyboard->keyboardType = 1;
    keyboard->exec();

    QDir myDir(fileSavePath + "\\program");
    QStringList allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    for(int i = 0; i < allFiles.count(); i++){
        if(allFiles.at(i).split(".spf").value(0) == createName){
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("程序：" + createName + " 已经存在\n是否覆盖原程序?");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Program:" + createName + " already exists\nreplace it?");
            }

            WarningWindow::getInstance()->showCancelButton(true);
            WarningWindow::getInstance()->exec();
            WarningWindow::getInstance()->showCancelButton(false);


            if (!WarningWindow::getInstance()->yesOrNo) return;
        }
    }

    if(createName!=""){
        //delete all the existing items
        ui->programTree->clear();

        //create a new program
        programName = "C:/SiasunRobot/program/" + createName + ".spf";

        QFile file(programName);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) return;

        //show program name
        QStringList pieces = programName.split("/");
        ui->programNameLabel->setText(pieces.value(pieces.size() - 1));


        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->programTree);
        itm->setText(0, "Start");
        QSize size; size.setHeight(programLineHeight);
        itm->setSizeHint(0,size);

        //set warning icon
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        itm->setIcon(0,ButtonIcon);

        ui->programTree->addTopLevelItem(itm);


        ui->addNodeButton->setEnabled(true);
        ui->editButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->programBackward->setEnabled(true);
        ui->programForward->setEnabled(true);
        ui->moveButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->moveToButton->setEnabled(true);
        ui->transitionButton->setEnabled(true);
        ui->setSpeedButton->setEnabled(true);
        ui->suppressButton->setEnabled(true);

        //save the program in the tree into a file
        QTextStream stream( &file );
        stream << ui->programTree->topLevelItem(0)->text(0) + ";" << endl;


        //clean the ancient recover files in SiasunRobot folder(old_X)
        progBackupPtr = 0;
        for(int i = 0; i < 10; i++){
            QFile file(fileSavePath + "\\old_" + QString::number(i));
            file.remove();
        }
        progBackupPtrMax = 0;

        //change the current tree index to 0 (if no element selected, it can cause qTreeWidget crash)
        ui->programTree->setCurrentIndex(ui->programTree->model()->index(0, 0));

        //save the current position to the Start node
        on_saveNodeButton_clicked();

        programOpened = true;

        on_programButton_clicked();

        //save the program
        saveProgramHandler(programName);
    }
}


void MainWindow::openProgramHandler(QString name)
{
    QFile file(name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    //delete all the existing items
    ui->programTree->clear();

    //show program name
    QStringList pieces = programName.split("/");
    ui->programNameLabel->setText(pieces.value(pieces.size() - 1));


    int moveCcount = 0;
    QTreeWidgetItem *itmSave;
    QTreeWidgetItem *threadItm;
    bool firstmoveCValid = false;
    bool threadHeaderNode = true;

    if(pieces.value(pieces.size() - 1).split('.').value(1) == "rs")
        robotScript = true;
    else
        robotScript = false;

    while (!file.atEnd()) {
        //read each line
        QString line = file.readLine();

        if(line != "\n"){ //filt blank line

            //split ';'
            //QStringList keywordPieces = line.split( ";" );

            //get command line
            QString commandLine = line;//keywordPieces.value(0);


            QString keyWord = commandLine.split("(").value(0);

            if(threadHeaderNode && (keyWord.split('*').value(0) == "thread" || keyWord.split('*').value(0) == "#thread")){

                //for thread functions, add thread header node
                threadItm = new QTreeWidgetItem(ui->programTree);

                if(keyWord.split('*').value(0) == "thread")
                    threadItm->setText(0, "thread");
                else{
                    threadItm->setText(0, "#thread");
                    threadItm->setTextColor(0, Qt::gray);
                }

                //change font
                //QFont font = threadItm->font(0); font.setBold(true);
                //threadItm->setFont(0, font);

                QPixmap pixmap(":/images/node_ok.png");
                QIcon ButtonIcon(pixmap);
                threadItm->setIcon(0,ButtonIcon);

                threadItm->setExpanded(true);

                threadHeaderNode = false;
            }

            if(robotScript){
                //robot script language
                QTreeWidgetItem *itm = new QTreeWidgetItem(ui->programTree);
                itm->setText(0, line);
                QSize size; size.setHeight(programLineHeight);
                itm->setSizeHint(0,size);

                if(commandLine != "\n"){
                    QPixmap pixmap(":/images/node_ok.png");
                    QIcon ButtonIcon(pixmap);
                    itm->setIcon(0,ButtonIcon);
                }
            }
            else if(keyWord.split('*').value(0) == "thread" || keyWord.split('*').value(0) == "#thread"){

                //check if the function is "if"
                if(keyWord.split('*').value(1).split('(').value(0) == "if"){
                    //add the "if" function
                    QTreeWidgetItem *ifItm = programAddChild(threadItm, commandLine.split("*").value(1));
                    QSize size; size.setHeight(programLineHeight);
                    ifItm->setSizeHint(0, size);

                    //add all the children of "if"
                    QStringList ifChild = commandLine.split("*");

                    for(int i = 2; i < ifChild.count(); i++) programAddChild(ifItm, ifChild.value(i));

                    ifItm->setExpanded(true);

                }else{
                    //for other thread functions, add thread functions to the header
                    programAddChild(threadItm, commandLine.split("*").value(1));
                }

            }
            else if(keyWord != "MoveC_1" && keyWord != "MoveC_2" && keyWord != "#MoveC_1" && keyWord != "#MoveC_2"){

                //for normal nodes
                //add program to the tree widget
                QTreeWidgetItem *itm = new QTreeWidgetItem(ui->programTree);
                itm->setText(0, commandLine);
                QSize size; size.setHeight(programLineHeight);
                itm->setSizeHint(0,size);

                //change font for functions
                if(keyWord != "MoveL" && keyWord != "MoveJ"){
                    if(keyWord.at(0) == '#'){
                        QString realKeyWord = keyWord.split('#').value(1);
                        if(realKeyWord != "MoveL" && realKeyWord != "MoveJ"){
                            QFont font = itm->font(0); font.setBold(true);
                            //itm->setFont(0, font);
                        }
                    }else{
                        QFont font = itm->font(0); font.setBold(true);
                        //itm->setFont(0, font);
                    }

                }


                if(keyWord == "MoveL" || keyWord == "MoveJ" || keyWord == "Start"){
                    //check if the node is valid
                    if(commandLine.split("(").value(1).split(")").value(0) == "" && commandLine.at(0) != '#'){
                        //set warning icon
                        QPixmap pixmap(":/images/node_warning.png");
                        QIcon ButtonIcon(pixmap);
                        itm->setIcon(0,ButtonIcon);
                    }else{
                        QPixmap pixmap(":/images/node_ok.png");
                        QIcon ButtonIcon(pixmap);
                        itm->setIcon(0,ButtonIcon);

                        if(keyWord.contains('#')) itm->setTextColor(0, Qt::gray);
                    }
                }else{
                    QPixmap pixmap(":/images/node_ok.png");
                    QIcon ButtonIcon(pixmap);
                    itm->setIcon(0,ButtonIcon);

                    if(keyWord.contains('#')) itm->setTextColor(0, Qt::gray);
                }

            }
            else{
                //for moveC nodes: 2 moveC nodes will combine to one node
                if(moveCcount == 0) {
                    //add program to the tree widget
                    QTreeWidgetItem *itm = new QTreeWidgetItem(ui->programTree);
                    QSize size; size.setHeight(programLineHeight);
                    itm->setSizeHint(0,size);
                    itmSave = itm;

                    if(keyWord.contains('#')){
                        itm->setText(0, "#MoveC");
                        itm->setTextColor(0, Qt::gray);
                        programAddChild(itm, commandLine.split('#').value(1));
                    }
                    else{
                        itm->setText(0, "MoveC");
                        programAddChild(itm, commandLine);
                    }

                    //check if the node is valid
                    if(commandLine.split("(").value(1).split(")").value(0) == ""){
                        //set warning icon
                        QPixmap pixmap(":/images/node_warning.png");
                        QIcon ButtonIcon(pixmap);
                        itm->child(0)->setIcon(0,ButtonIcon);
                        itm->setIcon(0,ButtonIcon);
                    }else{
                        QPixmap pixmap(":/images/node_ok.png");
                        QIcon ButtonIcon(pixmap);
                        itm->child(0)->setIcon(0,ButtonIcon);
                        firstmoveCValid = true;
                    }

                    moveCcount++;
                }else{
                    //moveC_2
                    QSize size; size.setHeight(programLineHeight);
                    itmSave->setSizeHint(0,size);

                    if(keyWord.contains('#')){
                        programAddChild(itmSave, commandLine.split('#').value(1));
                    }
                    else{
                        programAddChild(itmSave, commandLine);
                    }

                    //check if the node is valid
                    if(commandLine.split("(").value(1).split(")").value(0) == ""){
                        //set warning icon
                        QPixmap pixmap(":/images/node_warning.png");
                        QIcon ButtonIcon(pixmap);
                        itmSave->child(1)->setIcon(0,ButtonIcon);
                        itmSave->setIcon(0,ButtonIcon);

                        firstmoveCValid = false;
                    }else{
                        QPixmap pixmap(":/images/node_ok.png");
                        QIcon ButtonIcon(pixmap);
                        itmSave->child(1)->setIcon(0,ButtonIcon);


                        if(firstmoveCValid){
                            itmSave->setIcon(0,ButtonIcon);

                        }

                        firstmoveCValid = false;

                    }


                    moveCcount = 0;

                }

            }
        }
    }

    ui->addNodeButton->setEnabled(true);
    ui->editButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->programBackward->setEnabled(true);
    ui->programForward->setEnabled(true);
    ui->moveButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
    ui->moveToButton->setEnabled(true);
    ui->transitionButton->setEnabled(true);
    ui->setSpeedButton->setEnabled(true);
    ui->suppressButton->setEnabled(true);
    ui->setAccelerationButton->setEnabled(true);
    ui->transitionButton->setEnabled(false);

}

void MainWindow::saveTheNewProgramTree()
{
    if(progBackupPtrMax != progBackupPtr) progBackupPtr++;
    progBackupPtrMax = progBackupPtr;//Start from current status

    saveProgramHandler(fileSavePath + "\\old_" + QString::number(progBackupPtr++));

    progBackupPtrMax++;
    if(progBackupPtr > 9){
        progBackupPtr = 9; //shift all the files
        progBackupPtrMax = 9;
        for(int i = 0; i < progBackupPtrMax; i++){
            QFile file(fileSavePath + "\\old_" + QString::number(i));
            file.remove();
            QFile file2(fileSavePath + "\\old_" + QString::number(i+1));
            file2.rename(fileSavePath + "\\old_" + QString::number(i));
        }
    }
}


void MainWindow::on_openButton_clicked()
{
    selectProgramToRun = false;

    if(!QDir(fileSavePath + "\\program").exists()) QDir().mkdir(fileSavePath + "\\program");

    QDir myDir(fileSavePath + "\\program");
    QStringList allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    ui->programNameList->clear();

    if(allFiles.count() == 0) {on_createButton_clicked(); return;}

    for(int i = 0; i < allFiles.count(); i++){
        ui->programNameList->addItem(allFiles.at(i));
    }

    ui->programChooseOK->setVisible(true);
    ui->programChooseDelete->setVisible(false);
    ui->importProgram->setVisible(true);
    ui->functionStackWindow->setCurrentIndex(8);

    ui->programNameList->setCurrentRow(0);//delete will cause null pointer problem
    on_programNameList_itemClicked(ui->programNameList->currentItem());
}


QTreeWidgetItem* MainWindow::programAddRoot(QString name)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem;
    QSize size; size.setHeight(programLineHeight);
    itm->setSizeHint(0,size);

    if(ui->programTree->currentIndex().row() < 0 || ui->programTree->currentItem()->parent() != NULL){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("帮助: 请选择根节点进行操作");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Help: Please select a root node");
        }
        WarningWindow::getInstance()->exec();
        return itm;
    }

    itm->setText(0, name);

    QString keyword = name.split("(").value(0);

    if(keyword == "Start" || keyword == "MoveL" || keyword == "MoveJ" || keyword == "MoveC"){
        if(name.split("(").value(1).split(")").value(0) != "" || name.at(0) == '#'){
            QPixmap pixmap(":/images/node_ok.png");
            QIcon ButtonIcon(pixmap);
            itm->setIcon(0,ButtonIcon);
        }else{
            if(!robotScript){
                //not a robot script blank line
                QPixmap pixmap(":/images/node_warning.png");
                QIcon ButtonIcon(pixmap);
                itm->setIcon(0,ButtonIcon);
            }
        }
    }else{
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        itm->setIcon(0,ButtonIcon);
    }

    //if the inserted node is thread, put it to the top
    if(name == "thread")
        ui->programTree->insertTopLevelItem(0, itm);
    else
        ui->programTree->insertTopLevelItem(ui->programTree->currentIndex().row() + 1, itm);

    //change current item to the new item
    if(name != "end")
        ui->programTree->setCurrentItem(itm);

    return itm;
}

QTreeWidgetItem *MainWindow::programAddChild(QTreeWidgetItem *currentItm, QString name)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0, name);
    currentItm->addChild(itm);

    if(name == "MoveC_1();" || name == "MoveC_2();"){
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        itm->setIcon(0,ButtonIcon);
    }

    return itm;
}

void MainWindow::startInit()
{
    //version
    ui->version_label_2->setText(appVersion);

    ui->cameraSetting->setVisible(false);

    adsWriteStruct.reserve[3] = true; //default use teach pendant

    ui->selectToolCoordinate1->setVisible(false);
    ui->selectToolCoordinate2->setVisible(false);
    ui->selectToolCoordinate3->setVisible(false);
    ui->selectToolCoordinate4->setVisible(false);
    ui->selectToolCoordinate5->setVisible(false);

    QFile toolCoordinateFile(fileSavePath + "\\tool_coordinates");

    if (toolCoordinateFile.open(QIODevice::ReadOnly)){
        QString toolCoordinates = toolCoordinateFile.readLine();
        QStringList keywordPieces = toolCoordinates.split(';');

        QStringList numbers = keywordPieces.value(0).split(':');

        ui->selectToolCoordinate1->setText("X: " + numbers.value(0) + " " + \
                                           "Y: " + numbers.value(1) + " " + \
                                           "Z: " + numbers.value(2) + " " + \
                                           "RX: " + numbers.value(3) + " " + \
                                           "RY: " + numbers.value(4) + " " + \
                                           "RZ: " + numbers.value(5));
        ui->selectToolCoordinate1->setVisible(true);
        for(int i = 0; i < 5; i++) toolCoordinateSave[0][i] = numbers.value(i).toDouble();



        numbers = keywordPieces.value(1).split(':');
        ui->selectToolCoordinate2->setText("X: " + numbers.value(0) + " " + \
                                           "Y: " + numbers.value(1) + " " + \
                                           "Z: " + numbers.value(2) + " " + \
                                           "RX: " + numbers.value(3) + " " + \
                                           "RY: " + numbers.value(4) + " " + \
                                           "RZ: " + numbers.value(5));
        ui->selectToolCoordinate2->setVisible(true);
        for(int i = 0; i < 5; i++) toolCoordinateSave[1][i] = numbers.value(i).toDouble();

        numbers = keywordPieces.value(2).split(':');
        ui->selectToolCoordinate3->setText("X: " + numbers.value(0) + " " + \
                                           "Y: " + numbers.value(1) + " " + \
                                           "Z: " + numbers.value(2) + " " + \
                                           "RX: " + numbers.value(3) + " " + \
                                           "RY: " + numbers.value(4) + " " + \
                                           "RZ: " + numbers.value(5));
        ui->selectToolCoordinate3->setVisible(true);
        for(int i = 0; i < 5; i++) toolCoordinateSave[2][i] = numbers.value(i).toDouble();

        numbers = keywordPieces.value(3).split(':');
        ui->selectToolCoordinate4->setText("X: " + numbers.value(0) + " " + \
                                           "Y: " + numbers.value(1) + " " + \
                                           "Z: " + numbers.value(2) + " " + \
                                           "RX: " + numbers.value(3) + " " + \
                                           "RY: " + numbers.value(4) + " " + \
                                           "RZ: " + numbers.value(5));
        ui->selectToolCoordinate4->setVisible(true);
        for(int i = 0; i < 5; i++) toolCoordinateSave[3][i] = numbers.value(i).toDouble();

        numbers = keywordPieces.value(4).split(':');
        ui->selectToolCoordinate5->setText("X: " + numbers.value(0) + " " + \
                                           "Y: " + numbers.value(1) + " " + \
                                           "Z: " + numbers.value(2) + " " + \
                                           "RX: " + numbers.value(3) + " " + \
                                           "RY: " + numbers.value(4) + " " + \
                                           "RZ: " + numbers.value(5));
        ui->selectToolCoordinate5->setVisible(true);
        for(int i = 0; i < 5; i++) toolCoordinateSave[4][i] = numbers.value(i).toDouble();


    }
    toolCoordinateFile.close();

    //read the init file
    QFile file(fileSavePath + "\\config.init");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("配置文件不存在");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("init config file doesn't exist");
        }
        WarningWindow::getInstance()->exec();
    }else{
        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //set full screen
            if(keywordPieces.value(0).trimmed() == "FULLSCREEN"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    mianWindowFullScreen = true;
                }
            }

            //stop program will shutdown machine
            if(keywordPieces.value(0).trimmed() == "SHUTDOWN"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    stopProgramShutDownMachine = true;
                }
            }

            //camera
            if(keywordPieces.value(0).trimmed() == "CAMERA"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    ui->cameraSetting->setVisible(true);
                }
            }


            //change maxLineMoveSpeed
            if(keywordPieces.value(0).trimmed() == "MAXSPEED"){
                if(keywordPieces.value(1).toInt() == 0 && keywordPieces.value(1).trimmed() != "0") return;
                maxLineMoveSpeed = keywordPieces.value(1).toInt();
            }

            //back to init point before stop connect to the robot
            if(keywordPieces.value(0).trimmed() == "BACK2ZERO"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    back2InitPoint = true;
                }
            }

            if(keywordPieces.value(0).trimmed() == "USETP"){
                if(keywordPieces.value(1).trimmed() == "off"){
                    adsWriteStruct.reserve[3] = false;
                }
            }

            //status feedback cycle
            if(keywordPieces.value(0).trimmed() == "FBCYCLE"){
                sendStatusFeedbackCycle = keywordPieces.value(1).trimmed().toInt();
            }




            //back to init point before stop connect to the robot
            if(keywordPieces.value(0).trimmed() == "LANGUAGE"){
                if(keywordPieces.value(1).trimmed() == "cn"){
                    systemLanguage = "cn";
                }

                if(keywordPieces.value(1).trimmed() == "en"){
                    systemLanguage = "en";
                    QPixmap pixmap(":/images/english.png");
                    QIcon ButtonIcon(pixmap);
                    ui->languageSwitch->setIcon(ButtonIcon);
                    ui->languageSwitch->setIconSize(pixmap.rect().size());
                }

            }

            //ignore the emc button when using simulation
            if(keywordPieces.value(0).trimmed() == "COLLISIONAUTO"){
                if(keywordPieces.value(1).trimmed() == "off"){
                    on_collisionDetectActive_2_clicked();
                }

                switch(keywordPieces.value(2).trimmed().toInt()){
                case 1:
                    ui->collisionSensitiveLabel->setText("碰撞检测灵敏度: 高");
                    adsWriteStruct.collisionSensitive = 1;
                    ui->collisionSensitive->setValue(1);
                    break;
                case 2:
                    ui->collisionSensitiveLabel->setText("碰撞检测灵敏度: 中");
                    adsWriteStruct.collisionSensitive = 2;
                    ui->collisionSensitive->setValue(2);
                    break;
                case 3:
                    ui->collisionSensitiveLabel->setText("碰撞检测灵敏度: 低");
                    adsWriteStruct.collisionSensitive = 3;
                    ui->collisionSensitive->setValue(3);
                    break;
                default: break;
                }
            }

            if(keywordPieces.value(0).trimmed() == "DYNAMICTHRESH"){
                if(keywordPieces.value(1).trimmed() == "off"){
                    on_dynamicThreshActive_clicked();
                    ui->dynamicThreshStackedWidget->setCurrentIndex(0);
                }else{
                    ui->dynamicThreshStackedWidget->setCurrentIndex(1);
                }

                adsWriteStruct.dynamicThreshJoint[0] = keywordPieces.value(2).trimmed().toDouble();
                adsWriteStruct.dynamicThreshJoint[1] = keywordPieces.value(3).trimmed().toDouble();
                adsWriteStruct.dynamicThreshJoint[2] = keywordPieces.value(4).trimmed().toDouble();
                adsWriteStruct.dynamicThreshJoint[3] = keywordPieces.value(5).trimmed().toDouble();
                adsWriteStruct.dynamicThreshJoint[4] = keywordPieces.value(6).trimmed().toDouble();
                adsWriteStruct.dynamicThreshJoint[5] = keywordPieces.value(7).trimmed().toDouble();
                adsWriteStruct.dynamicThreshJoint[6] = keywordPieces.value(8).trimmed().toDouble();

                ui->dynamicThread_1->setText(keywordPieces.value(2).trimmed());
                ui->dynamicThread_2->setText(keywordPieces.value(3).trimmed());
                ui->dynamicThread_3->setText(keywordPieces.value(4).trimmed());
                ui->dynamicThread_4->setText(keywordPieces.value(5).trimmed());
                ui->dynamicThread_5->setText(keywordPieces.value(6).trimmed());
                ui->dynamicThread_6->setText(keywordPieces.value(7).trimmed());
                ui->dynamicThread_7->setText(keywordPieces.value(8).trimmed());

            }


            //end tool weight and weight point position
            if(keywordPieces.value(0).trimmed() == "TOOLWPOS"){
                ui->toolLoad->setText(keywordPieces.value(1).trimmed());
                ui->toolLoadPositionX->setText(keywordPieces.value(2).trimmed());
                ui->toolLoadPositionY->setText(keywordPieces.value(3).trimmed());
                ui->toolLoadPositionZ->setText(keywordPieces.value(4).trimmed());
                adsWriteStruct.tcpLoad = keywordPieces.value(1).trimmed().toDouble();
                adsWriteStruct.tcpLoadPos[0] = keywordPieces.value(2).trimmed().toDouble();
                adsWriteStruct.tcpLoadPos[1] = keywordPieces.value(3).trimmed().toDouble();
                adsWriteStruct.tcpLoadPos[2] = keywordPieces.value(4).trimmed().toDouble();
            }


            if(keywordPieces.value(0).trimmed() == "AUTOLOGIN"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    autoLoginActive = true;

                    QPixmap pixmap(":/images/node_ok.png");
                    QIcon ButtonIcon(pixmap);
                    ui->autoLogin->setIcon(ButtonIcon);
                    ui->autoLogin->setIconSize(pixmap.rect().size());
                    if(systemLanguage == "cn"){
                        ui->autoLogin->setText("自动登录 开");
                    }else if(systemLanguage == "en"){
                        ui->autoLogin->setText("Auto login: Enable");
                    }
                }
            }


            if(keywordPieces.value(0).trimmed() == "VIRTUALWALL"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    ui->virtualWallCheckbox1->setChecked(true);
                    ui->virtualWall1Length->setText(keywordPieces.value(2).trimmed());
                }

                if(keywordPieces.value(3).trimmed() == "on"){
                    ui->virtualWallCheckbox2->setChecked(true);
                    ui->virtualWall2Length->setText(keywordPieces.value(4).trimmed());
                }

                if(keywordPieces.value(5).trimmed() == "on"){
                    ui->virtualWallCheckbox3->setChecked(true);
                    ui->virtualWall3Length->setText(keywordPieces.value(6).trimmed());
                    ui->virtualWall3Rot->setText(keywordPieces.value(7).trimmed());
                }

                if(keywordPieces.value(8).trimmed() == "on"){
                    ui->virtualWallCheckbox4->setChecked(true);
                    ui->virtualWall4Length->setText(keywordPieces.value(9).trimmed());
                    ui->virtualWall4Rot->setText(keywordPieces.value(10).trimmed());
                }

                if(keywordPieces.value(11).trimmed() == "on"){
                    ui->virtualWallCheckbox5->setChecked(true);
                    ui->virtualWall5Length->setText(keywordPieces.value(12).trimmed());
                    ui->virtualWall5Rot->setText(keywordPieces.value(13).trimmed());
                }

                if(keywordPieces.value(14).trimmed() == "on"){
                    ui->virtualWallCheckbox6->setChecked(true);
                    ui->virtualWall6Length->setText(keywordPieces.value(15).trimmed());
                    ui->virtualWall6Rot->setText(keywordPieces.value(16).trimmed());
                }

                on_setVirtualWall_clicked();

            }


            if(keywordPieces.value(0).trimmed() == "TOOL"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    ui->toolCoordinateLineX->setText(keywordPieces.value(2).trimmed());
                    ui->toolCoordinateLineY->setText(keywordPieces.value(3).trimmed());
                    ui->toolCoordinateLineZ->setText(keywordPieces.value(4).trimmed());
                    ui->toolCoordinateLineRX->setText(keywordPieces.value(5).trimmed());
                    ui->toolCoordinateLineRY->setText(keywordPieces.value(6).trimmed());
                    ui->toolCoordinateLineRZ->setText(keywordPieces.value(7).trimmed());

                    startUpUseTool = true;
                }

            }

        }
    }

    file.close();

    //set system language
    setSystemLanguage(systemLanguage);

    //set main stack index to 0
    ui->mainStackWindow->setCurrentIndex(0);

    ui->statusStackedWidget->setCurrentIndex(0);

    ui->moveParameterStackWindow->setCurrentIndex(0);

    ui->tcpConnectStatus->setVisible(false);

    //change setting stack index to 0
    ui->settingStack->setCurrentIndex(0);

    //change move stack index to 1
    ui->moveStackedWidget->setCurrentIndex(1);

    ui->programStack->setCurrentIndex(0);

    //set expert mode to none by default
    switchExpertMode(expertMode);

    ui->saveNodeButton->setVisible(false);
    ui->stopProgramButton_run->setVisible(false);
    ui->runProgramButton_run->setVisible(false);
    ui->stepProgramButton_run->setVisible(false);
    ui->pauseProgramButton_run->setVisible(false);

    ui->functionStackWindow->setDisabled(true);

    ui->installGroupBox->setVisible(false);


    //auto login
    if(autoLoginActive){
        autoLogin = new AutoLogin();       //auto login thread
        connect(autoLogin,SIGNAL(clickStartButton()), this, SLOT(on_startStopButton_clicked()));
        connect(autoLogin,SIGNAL(clickEnableAllButton()), this, SLOT(on_robotActive_clicked()));
        connect(autoLogin,SIGNAL(clickPowerOffButton()), this, SLOT(on_return2Start_clicked()));
        autoLogin->start();
    }

}





void MainWindow::robotWindowChangePosition(int type)
{
    int basex, basey;
    switch (type){
    case 1:
        //determine whether show the "save node" button or not
        if(ui->programTree->currentItem() != NULL){
            ui->saveNodeButton->setVisible(true);
        }else{
            ui->saveNodeButton->setVisible(false);
        }

        //move tab
        ui->move3DWidget->setParent(ui->moveParameterStackWindow->widget(1));
        ui->move3DWidget->move(1,1);
        ui->move3DWidget->setFixedSize(370,400);
        break;
    case 2:
        //program tab
        ui->move3DWidget->setParent(ui->programStack->widget(3));
        ui->move3DWidget->move(1,1);
        ui->move3DWidget->setFixedSize(380,500);
        break;
    case 3:
        //run tab
        ui->move3DWidget->setParent(ui->functionStackWindow->widget(2));
        ui->move3DWidget->move(360,40);
        ui->move3DWidget->setFixedSize(440,420);
        ui->zoomIn_Run->setParent(ui->functionStackWindow->widget(2));
        ui->zoomOut_Run->setParent(ui->functionStackWindow->widget(2));
        ui->reset_Run->setParent(ui->functionStackWindow->widget(2));
        ui->up3D->setParent(ui->functionStackWindow->widget(2));
        ui->down3D->setParent(ui->functionStackWindow->widget(2));
        ui->left3D->setParent(ui->functionStackWindow->widget(2));
        ui->right3D->setParent(ui->functionStackWindow->widget(2));
        basex = 370;basey = 420;
        ui->zoomIn_Run->move(basex,basey);
        ui->zoomOut_Run->move(basex + 35,basey);
        ui->reset_Run->move(basex + 35*2,basey);
        ui->up3D->move(basex + 35*3,basey);
        ui->down3D->move(basex + 35*4,basey);
        ui->left3D->move(basex + 35*5,basey);
        ui->right3D->move(basex + 35*6,basey);
        break;
    }
}


void MainWindow::on_addNodeButton_clicked()
{
    if(robotScript){
        //robot script program, add a blank line
        programAddRoot("");

        //save the new program tree
        saveTheNewProgramTree();

        return;
    }


    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();

    if(ui->programTree->currentItem() == NULL) return;

    //a root node must be selected
    if(ui->programTree->currentIndex().row() < 0 || ui->programTree->currentItem()->parent() != NULL){

        if(ui->programTree->currentItem()->parent()->text(0) != "thread"){
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("帮助: 请选择根节点进行操作");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Help: Please select a root node");
            }
            WarningWindow::getInstance()->exec();


            return;

        }else{
            //show the add node window
            nodeWindow->setModal(true);

            nodeWindow->functionsForThread(true);

            nodeWindow->exec();

            return;
        }
    }


    //show the add node window
    nodeWindow->setModal(true);

    //switch expert mode
    nodeWindow->functionWindowExpertMode(expertMode);

    //change language
    nodeWindow->changeLanguage();

    nodeWindow->exec();
}

void MainWindow::on_deleteButton_clicked()
{
    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();

    if(ui->programTree->currentItem() != NULL && ui->programTree->currentItem()->parent() == NULL){

        QStringList keywordPieces = ui->programTree->currentItem()->text(0).split('(');

        //can't delete Start node
        //if(keywordPieces.value(0) != "Start" && keywordPieces.value(0).split('#').value(1) != "Start"){
        if(ui->programTree->topLevelItemCount() > 1){

            if(ui->programTree->topLevelItemCount() == 1){
                //if there is just one item, delete will crash, not solved yet, use clear() for now.
                ui->programTree->clear();

                //save the new program tree
                saveTheNewProgramTree();
            }else{
                //delete here
                delete ui->programTree->currentItem();

                //save the new program tree
                saveTheNewProgramTree();
            }

        }else
        {
            return;
            if(expertMode){
                if(ui->programTree->topLevelItemCount() == 1){
                    //if there is just one item, delete will crash, not solved yet, use clear() for now.
                    ui->programTree->clear();

                    //save the new program tree
                    saveTheNewProgramTree();
                }else{
                    //delete here
                    delete ui->programTree->currentItem();

                    //save the new program tree
                    saveTheNewProgramTree();
                }
            }else{
                if(systemLanguage == "cn"){
                    WarningWindow::getInstance()->setWarningText("注意: 不能删除程序起始点");
                }else if(systemLanguage == "en"){
                    WarningWindow::getInstance()->setWarningText("Help: Can't delete the Start node");
                }
                WarningWindow::getInstance()->exec();
            }
        }
    }
    else
    {
        //check if the child is in thread
        if(ui->programTree->currentItem() != NULL && ui->programTree->currentItem()->parent()->text(0) == "thread"){
            //delete here
            delete ui->programTree->currentItem();

            //save the new program tree
            saveTheNewProgramTree();

            return;
        }

        //check if the child is in "if"
        if(ui->programTree->currentItem() != NULL && ui->programTree->currentItem()->parent()->text(0).split('(').value(0) == "if"){
            //delete here
            delete ui->programTree->currentItem();

            //save the new program tree
            saveTheNewProgramTree();

            return;
        }


        //else, throw a warning
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("帮助: 请选择根节点进行操作");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Help: Please select a root node");
        }
        WarningWindow::getInstance()->exec();
    }
}

void MainWindow::saveProgramHandler(QString programName)
{
    //save the program in the tree into a file
    QFile file(programName);
    file.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream stream( &file );

    int index = ui->programTree->topLevelItemCount();
    for (int i = 0; i < index; i++){
        QString itmName = ui->programTree->topLevelItem(i)->text(0);
        if(itmName == "MoveC"){
            //moveC will add 2 nodes
            QString child1 = ui->programTree->topLevelItem(i)->child(0)->text(0);
            QString child2 = ui->programTree->topLevelItem(i)->child(1)->text(0);
            if(child1.contains("\n"))
                stream << child1.split("\n").value(0) << endl;
            else
                stream << child1 << endl;

            if(child2.contains("\n"))
                stream << child2.split("\n").value(0) << endl;
            else
                stream << child2 << endl;

        }else if(itmName == "#MoveC"){
            //moveC will add 2 nodes
            QString child1 = "#" + ui->programTree->topLevelItem(i)->child(0)->text(0);
            QString child2 = "#" + ui->programTree->topLevelItem(i)->child(1)->text(0);
            if(child1.contains("\n"))
                stream << child1.split("\n").value(0) << endl;
            else
                stream << child1 << endl;

            if(child2.contains("\n"))
                stream << child2.split("\n").value(0) << endl;
            else
                stream << child2 << endl;
        }
        else if(itmName == "thread"){

            for(int j = 0; j < ui->programTree->topLevelItem(i)->childCount(); j++){

                //for if function and its children
                if(ui->programTree->topLevelItem(i)->child(j)->text(0).split('(').value(0) == "if"){
                    QString childrenItm = "";

                    for(int k = 0; k < ui->programTree->topLevelItem(i)->child(j)->childCount(); k++){
                        childrenItm = childrenItm + "*" + ui->programTree->topLevelItem(i)->child(j)->child(k)->text(0) ;

                    }

                    stream <<"thread*" + ui->programTree->topLevelItem(i)->child(j)->text(0) + childrenItm + ";" << endl;

                }else{

                    stream <<"thread*" + ui->programTree->topLevelItem(i)->child(j)->text(0) + ";" << endl;
                }
            }

        }
        else if(itmName == "#thread"){

            for(int j = 0; j < ui->programTree->topLevelItem(i)->childCount(); j++){

                //for if function and its children
                if(ui->programTree->topLevelItem(i)->child(j)->text(0).split('(').value(0) == "if"){
                    QString childrenItm = "";

                    for(int k = 0; k < ui->programTree->topLevelItem(i)->child(j)->childCount(); k++){
                        childrenItm = childrenItm + "*" + ui->programTree->topLevelItem(i)->child(j)->child(k)->text(0) ;

                    }

                    stream <<"#thread*" + ui->programTree->topLevelItem(i)->child(j)->text(0) + childrenItm + ";" << endl;

                }else{

                    stream <<"#thread*" + ui->programTree->topLevelItem(i)->child(j)->text(0) + ";" << endl;
                }
            }

        }
        else{
            if(robotScript){
                //robot script language
                if(itmName.contains("\n"))
                    if(ui->programTree->topLevelItem(i)->text(0).split("\n").value(1) == "")
                        //  aaa\n
                        stream << ui->programTree->topLevelItem(i)->text(0).split("\n").value(0) << endl;
                    else
                        //  \naaa
                        stream << ui->programTree->topLevelItem(i)->text(0).split("\n").value(1) << endl;
                else
                    stream << ui->programTree->topLevelItem(i)->text(0) << endl;
            }
            else{
                //if(itmName.at(0) == '#')
                //    stream << ui->programTree->topLevelItem(i)->text(0) << endl;
                //else{

                if(itmName.contains("\n")){
                    stream << ui->programTree->topLevelItem(i)->text(0).split("\n").value(0) << endl;
                }else{
                    stream << ui->programTree->topLevelItem(i)->text(0) << endl;
                }
                //}
            }
        }

    }

}


void MainWindow::on_saveButton_clicked()
{
    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();

    saveProgramHandler(programName);

    ui->mainStackWindow->setCurrentIndex(3);
}

void MainWindow::on_moveButton_clicked()
{
    if(ui->programTree->currentItem() != NULL && ui->programTree->currentItem()->parent() == NULL){

        if(ui->programTree->currentItem()->text(0) != "MoveC"){
            QStringList keywordPieces = ui->programTree->currentItem()->text(0).split( "(" );

            //get keyword
            QString keyWord = keywordPieces.value(0);

            //this function just work for Start/moveJ/moveL
            if(keyWord != "Start" && keyWord != "MoveJ" && keyWord != "MoveL" && keyWord != "MovePsi" && keyWord != "MoveL_Psi" && keyWord != "biaoding"){
                //not a move function, edit it
                on_editButton_clicked();
                return;
            }

            //for moveL, change jog move to line move
            if(keyWord == "MoveL") on_switchToEndMove_clicked();

            //move tab
            robotWindowChangePosition(1);

            on_jogMoveButton_clicked();
        }

    }
    else if(ui->programTree->currentItem() != NULL && ui->programTree->currentItem()->parent() != NULL){
        if(ui->programTree->currentItem()->parent()->text(0) == "MoveC"){
            //move tab
            robotWindowChangePosition(1);

            on_jogMoveButton_clicked();
        }
    }

}


bool check3PointsInOneLine(double x1, double y1, double z1, double x2, double y2, double z2,double x3, double y3, double z3){
    double a = x2 - x1; double b = y2 - y1; double c = z2 - z1;
    double d = x3 - x1; double e = y3 - y1; double f = z3 - z1;

    if((a*e == b*d) && (a*f == d*c) && (b*f == e*c)){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告: MoveC三点共线");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Warning: MoveC 3 points are in one line");
        }
        WarningWindow::getInstance()->exec();
        return true;
    }

    return false;
}


void MainWindow:: on_saveNodeButton_clicked()
{
    if(ui->programTree->currentItem() != NULL){
        //remove the ancient position
        //split '('
        QStringList keywordPieces = ui->programTree->currentItem()->text(0).split( "(" );

        //get keyword
        QString keyWord = keywordPieces.value(0);


        //this function just work for Start/moveJ/moveL
        if(keyWord != "Start" && keyWord != "MoveJ" && keyWord != "MoveL" && keyWord != "MovePsi" && keyWord != "MoveC_1" && keyWord != "MoveC_2" && keyWord != "MoveL_Psi" && keyWord != "biaoding") return;


        //move back to program ui
        on_programButton_clicked();


        //set the default speed
        QString speed;
        //check if the node has a speed before
        QStringList valuesList = keywordPieces.value(1).split(',');

        if(keyWord == "MoveL" || keyWord == "MoveL_Psi" || keyWord == "MoveC_1" || keyWord == "MoveC_2" || keyWord == "biaoding"){
            if(valuesList.value(6).toInt() > 0)
                speed = valuesList.value(6);
            else
                speed = defaultMoveLSpeed;
        }
        if(keyWord == "MoveJ" || keyWord == "Start" || keyWord == "MovePsi"){
            if(valuesList.value(6).toInt() > 0)
                speed = valuesList.value(6);
            else
                speed = defaultMoveJSpeed;
        }


        QString blendRadius;
        if(valuesList.value(15).split(')').value(0).toInt() > 0)
            blendRadius = valuesList.value(15);
        else
            blendRadius = "-1";


        //add position to node string
        QString node;
        if(adsWriteStruct.statusUseToolCoordinate){
            //if use tool coordinate
            //if(keyWord != "MoveL")
            //    node = keyWord + "(" + QString::number(adsReadStruct.currentFlangePositionTool[0], 'f', 3) + ", " + \
            //    QString::number(adsReadStruct.currentFlangePositionTool[1], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePositionTool[2], 'f', 3) + \
            //    ", " + QString::number(adsReadStruct.currentFlangePositionTool[3], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePositionTool[4], 'f', 3)\
            //    + ", " + QString::number(adsReadStruct.currentFlangePositionTool[5], 'f', 3) + ", " + speed+ ", " \
            //    + ui->programAccelerationValue->text() + ", " + QString::number(adsReadStruct.axisCurrentAngle[0], 'f', 3)\
            //    + ", " + QString::number(adsReadStruct.axisCurrentAngle[1], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[2], 'f', 3) \
            //    + ", " + QString::number(adsReadStruct.axisCurrentAngle[3], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[4], 'f', 3) + \
            //    ", " + QString::number(adsReadStruct.axisCurrentAngle[5], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[6], 'f', 3) + ");";
            //else //add blend radias default -1
            node = keyWord + "(" + QString::number(adsReadStruct.currentFlangePositionTool[0], 'f', 3) + ", " + \
                    QString::number(adsReadStruct.currentFlangePositionTool[1], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePositionTool[2], 'f', 3) + \
                    ", " + QString::number(adsReadStruct.currentFlangePositionTool[3], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePositionTool[4], 'f', 3)\
                    + ", " + QString::number(adsReadStruct.currentFlangePositionTool[5], 'f', 3) + ", " + speed+ ", " \
                    + ui->programAccelerationValue->text() + ", " + QString::number(adsReadStruct.axisCurrentAngle[0], 'f', 3)\
                    + ", " + QString::number(adsReadStruct.axisCurrentAngle[1], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[2], 'f', 3) \
                    + ", " + QString::number(adsReadStruct.axisCurrentAngle[3], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[4], 'f', 3) + \
                    ", " + QString::number(adsReadStruct.axisCurrentAngle[5], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[6], 'f', 3) + ", " + blendRadius + ");";

        }else{
            //if not using tool coordinate
            //if(keyWord != "MoveL")
            //    node = keyWord + "(" + QString::number(adsReadStruct.currentFlangePosition[0], 'f', 3) + ", " + \
            //    QString::number(adsReadStruct.currentFlangePosition[1], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePosition[2], 'f', 3) + \
            //    ", " + QString::number(adsReadStruct.currentFlangePosition[3], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePosition[4], 'f', 3)\
            //    + ", " + QString::number(adsReadStruct.currentFlangePosition[5], 'f', 3) + ", " + speed+ ", " \
            //    + ui->programAccelerationValue->text() + ", " + QString::number(adsReadStruct.axisCurrentAngle[0], 'f', 3)\
            //    + ", " + QString::number(adsReadStruct.axisCurrentAngle[1], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[2], 'f', 3) \
            //    + ", " + QString::number(adsReadStruct.axisCurrentAngle[3], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[4], 'f', 3) + \
            //    ", " + QString::number(adsReadStruct.axisCurrentAngle[5], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[6], 'f', 3) + ");";
            //else
            node = keyWord + "(" + QString::number(adsReadStruct.currentFlangePosition[0], 'f', 3) + ", " + \
                    QString::number(adsReadStruct.currentFlangePosition[1], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePosition[2], 'f', 3) + \
                    ", " + QString::number(adsReadStruct.currentFlangePosition[3], 'f', 3) + ", " + QString::number(adsReadStruct.currentFlangePosition[4], 'f', 3)\
                    + ", " + QString::number(adsReadStruct.currentFlangePosition[5], 'f', 3) + ", " + speed+ ", " \
                    + ui->programAccelerationValue->text() + ", " + QString::number(adsReadStruct.axisCurrentAngle[0], 'f', 3)\
                    + ", " + QString::number(adsReadStruct.axisCurrentAngle[1], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[2], 'f', 3) \
                    + ", " + QString::number(adsReadStruct.axisCurrentAngle[3], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[4], 'f', 3) + \
                    ", " + QString::number(adsReadStruct.axisCurrentAngle[5], 'f', 3) + ", " + QString::number(adsReadStruct.axisCurrentAngle[6], 'f', 3) + ", " + blendRadius + ");";
        }
        ui->programTree->currentItem()->setText(0, node);

        //remove all the existing child first
        for(int i = ui->programTree->currentItem()->childCount() - 1; i >= 0; i--)
            delete ui->programTree->currentItem()->child(i);

        //change status image
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->programTree->currentItem()->setIcon(0,ButtonIcon);

        //check for MoveC if all the 2 nodes are valid
        if (ui->programTree->currentItem()->parent() != NULL){
            //current item is moveC
            if(ui->programTree->currentItem()->parent()->child(0)->text(0).split("(").value(1) == "" || \
                    ui->programTree->currentItem()->parent()->child(1)->text(0).split("(").value(1) == ""){
                QPixmap pixmap(":/images/node_warning.png");
                QIcon ButtonIcon(pixmap);
                ui->programTree->currentItem()->parent()->setIcon(0,ButtonIcon);
            }else{
                QPixmap pixmap(":/images/node_ok.png");
                QIcon ButtonIcon(pixmap);
                ui->programTree->currentItem()->parent()->setIcon(0,ButtonIcon);
            }

        }

        //set the speed of the node
        //on_setSpeedButton_clicked();

        //save the new program tree
        saveTheNewProgramTree();

        //refresh the parameters in program window
        //QModelIndex index;
        //on_programTree_clicked(index);
        QTreeWidgetItem *current = NULL;
        on_programTree_currentItemChanged(current, current);
    }
}


void MainWindow::on_resetButton_clicked()
{
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("确定复位系统?");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Be sure to reset the system?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo) resetSystem();
}

void MainWindow::on_stopProgramButton_run_clicked()
{
    if(!programIsRunning) return;

    //pause button clicked, wait for pause done
    if(adsWriteStruct.progPause && !adsReadStruct.pauseDone && !stopOrRunEnable) return;

    //uncheck the repeat button
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/repeat.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/repeat-en.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }

    repeatProgram = false;
    runProgramRepeatTime = 0;
    ui->repeatTimeLabel->setText("");

    ui->pauseProgramButton_run->setDisabled(true);
    ui->stopProgramButton_run->setDisabled(true);
    ui->pauseProgramButton_run->setVisible(false);
    ui->stopProgramButton_run->setVisible(false);
    ui->runProgramButton_run->setVisible(false);
    ui->stepProgramButton_run->setVisible(false);


    //send message to PLC
    if(adsWriteStruct.blendStep != 0) adsWriteStruct.blendMoveStopClick = true;
    adsWriteStruct.progStop = true;
    adsWriteStruct.movejBlend = false;

    if(adsWriteStruct.progPause){
        //if program is currently paused, wait PLC to stop the program and then\
        //quit the runProgram thread by runProgram->stop = true;
        adsWriteStruct.progPause = false;
        while(!adsReadStruct.pauseThenStopped){QThread::msleep(30);}
        runProgram->stop = true;
    }


    if(systemLanguage == "cn"){
        ui->programStatus->setText("就绪");
    }else if(systemLanguage == "en"){
        ui->programStatus->setText("Ready");
    }


    //set old command background to white
    if(oldLineNum < totalCommandLineNum && oldLineNum >= 0){
        ui->runDataListWidget->setCurrentRow(oldLineNum);
        ui->runDataListWidget->currentItem()->setBackground(QBrush(QColor(42, 46, 47)));
        oldLineNum = -1;
    }

    //back to the -1 row
    ui->runDataListWidget->setCurrentRow(-1);


    //send feedback
    programIsRunning = false;



    //enable the joint move mode buttons
    ui->lineMoveGroupBox->setEnabled(true);
    ui->jointMoveGroupBox->setEnabled(true);
    ui->positionGroupBox->setEnabled(true);
    ui->jointAngleGroupBox->setEnabled(true);
    ui->saveNodeButton->setEnabled(true);
    ui->jogSpeedSlider->setEnabled(true);
    ui->tractionTeachButton->setEnabled(true);
    ui->moveToButton->setEnabled(true);
    ui->pushButtonMoveToOrigin->setEnabled(true);
}



void MainWindow::on_stepProgramButton_run_clicked()
{
    QFile checkFile(runProgramName);
    if(!checkFile.exists()){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("警告: 程序不存在");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Program doesn't exist");
        }
        WarningWindow::getInstance()->exec();
        return;
    }
    checkFile.close();

    if(!programIsRunning){

        ui->runTimeLabel->setVisible(false);

        collisionSignalSend = false;

        //send Start program message to PLC
        adsWriteStruct.progRun = true;
        adsWriteStruct.progPause = false;
        adsWriteStruct.progStop = false;


        if(systemLanguage == "cn"){
            ui->programStatus->setText("单步运行");
        }else if(systemLanguage == "en"){
            ui->programStatus->setText("Step");
        }

        runProgram->stop = false;
        runProgram->setProgramName(runProgramName);
        runProgram->start();
        runProgram->stepRunModeEnable = true;

        //send feedback
        programIsRunning = true;

        //set step button enable
        ui->runProgramButton_run->setDisabled(true);
        ui->pauseProgramButton_run->setDisabled(true);
        ui->stopProgramButton_run->setEnabled(true);

        ui->runProgramButton_run->setVisible(false);
        ui->pauseProgramButton_run->setVisible(false);
        ui->stopProgramButton_run->setVisible(true);
        ui->stepProgramButton_run->setVisible(true);

        //disable the joint move mode buttons
        ui->lineMoveGroupBox->setDisabled(true);
        //ui->angleMoveGroupBox->setDisabled(true);
        ui->jointMoveGroupBox->setDisabled(true);
        ui->tractionTeachButton->setDisabled(true);
        ui->moveToButton->setDisabled(true);
        ui->saveNodeButton->setDisabled(true);
        ui->pushButtonMoveToOrigin->setDisabled(true);
    }

    //uncheck the repeat button
    QPixmap pixmap(":/images/repeat.png");
    QIcon ButtonIcon(pixmap);
    ui->pushButton->setIcon(ButtonIcon);
    ui->pushButton->setIconSize(pixmap.rect().size());
    repeatProgram = false;
    runProgramRepeatTime = 0;
    ui->repeatTimeLabel->setText("");


    runProgram->stepRun = true; //active the step run mode
}

void MainWindow::on_jogSpeedSlider_valueChanged(int value)
{
    adsWriteStruct.jogSpeed = value;
    jogSpeed = value;
}


void MainWindow::on_setSpeedButton_clicked()
{
    //if the command line is a function, no need to set speed, exit
    QString keyWord = ui->programTree->currentItem()->text(0).split( "(" ).value(0);
    if(keyWord != "MoveL" && keyWord != "MoveL_Psi" && keyWord != "MoveJ" && keyWord != "Start" && keyWord != "MoveC_1" && keyWord != "MoveC_2") return;

    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入移动速度，单位：\nMoveJ：%百分比\nMoveL，MoveC：mm/s");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter move speed，Unit：\nMoveJ：%Percentage\nMoveL，MoveC：mm/s");
    numberBoard->editLineNum = 1; //for speed edit line
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_setAccelerationButton_clicked()
{
    //if the command line is a function, no need to set speed, exit
    QString keyWord = ui->programTree->currentItem()->text(0).split( "(" ).value(0);
    if(keyWord != "MoveL" && keyWord != "MoveJ" && keyWord != "Start") return;

    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    numberBoard->editLineNum = 2; //for acceleration edit line
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();

}


void MainWindow::on_deleteProgramButton_clicked()
{
    if(programIsRunning){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("请停止正在运行的程序");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Please stop the program");
        }
        WarningWindow::getInstance()->exec();
        return;
    }

    if(!QDir(fileSavePath + "\\program").exists()) QDir().mkdir(fileSavePath + "\\program");
    QDir myDir(fileSavePath + "\\program");
    QStringList allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    ui->programNameList->clear();

    if(allFiles.count() == 0) return;

    for(int i = 0; i < allFiles.count(); i++){
        ui->programNameList->addItem(allFiles.at(i));
    }

    ui->programNameList->setCurrentRow(0);//delete will cause null pointer bug
    on_programNameList_itemClicked(ui->programNameList->currentItem());

    ui->programChooseOK->setVisible(false);
    ui->programChooseDelete->setVisible(true);
    ui->importProgram->setVisible(false);
    ui->functionStackWindow->setCurrentIndex(8);
}


void MainWindow::on_runSpeedSlider_valueChanged(int value)
{
    runSpeedPercent = (float)value/100;
}


void MainWindow::on_zoomIn_Run_pressed()
{
    ui->move3DWidget->ZoomIn(true);
}

void MainWindow::on_zoomIn_Run_released()
{
    ui->move3DWidget->ZoomIn(false);
}

void MainWindow::on_zoomOut_Run_pressed()
{
    ui->move3DWidget->ZoomOut(true);
}

void MainWindow::on_zoomOut_Run_released()
{
    ui->move3DWidget->ZoomOut(false);
}

void MainWindow::on_reset_Run_clicked()
{
    ui->move3DWidget->ZoomHome();
}

void MainWindow::on_up3D_pressed()
{
    ui->move3DWidget->MoveTop(true);
}

void MainWindow::on_up3D_released()
{
    ui->move3DWidget->MoveTop(false);
}

void MainWindow::on_down3D_pressed()
{
    ui->move3DWidget->MoveBottom(true);
}

void MainWindow::on_down3D_released()
{
    ui->move3DWidget->MoveBottom(false);
}

void MainWindow::on_left3D_pressed()
{
    ui->move3DWidget->MoveLeft(true);
}

void MainWindow::on_left3D_released()
{
    ui->move3DWidget->MoveLeft(false);
}

void MainWindow::on_right3D_pressed()
{
    ui->move3DWidget->MoveRight(true);
}

void MainWindow::on_right3D_released()
{
    ui->move3DWidget->MoveRight(false);
}


void MainWindow::on_pushButtonMin_clicked()
{
    /** bug here **/
    //minimize the main window
    this->setWindowState(Qt::WindowMinimized);

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("解锁最小化");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Resume from minimal");
    }
    WarningWindow::getInstance()->exec();


    //maximize the main window
    this->setWindowState(Qt::WindowMaximized);
    this->setWindowState(Qt::WindowFullScreen);

}

void MainWindow::on_tractionTeachButton_clicked()
{
    //on traction teach mode, disable all the window
    ui->mainStackWindow->setDisabled(true);

    //set traction teach FLAG to true
    adsWriteStruct.tractionTeach = true;

    forceTeachMode = true;

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("点击终止牵引模式");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Traction Teach Mode\nClick to finish Traction Teach Mode");
    }
    WarningWindow::getInstance()->exec();


    //set traction teach FLAG to false
    adsWriteStruct.tractionTeach = false;

    //enable all the window
    ui->mainStackWindow->setEnabled(true);

    forceTeachMode = false;
}

void MainWindow::on_fixPositionMoveBackward_pressed()
{
    //use jog mode for fixed end position move (1:end move, 2:joint move, 3: fixed end position move)
    adsWriteStruct.jogMode = 3;

    //use axis angle move parameter for fixed end position move
    adsWriteStruct.axisAngleMoveDirection[0] = -1;

    lastClickedMoveButton = "fixPositionMoveBackward";

}

void MainWindow::on_fixPositionMoveBackward_released()
{
    //use jog mode for fixed end position move (1:end move, 2:joint move, 3: fixed end position move)
    adsWriteStruct.jogMode = 0;

    //use axis angle move parameter for fixed end position move
    adsWriteStruct.axisAngleMoveDirection[0] = 0;


}

void MainWindow::on_fixPositionMoveForward_pressed()
{
    //use jog mode for fixed end position move (1:end move, 2:joint move, 3: fixed end position move)
    adsWriteStruct.jogMode = 3;

    //use axis angle move parameter for fixed end position move
    adsWriteStruct.axisAngleMoveDirection[0] = 1;

    lastClickedMoveButton = "fixPositionMoveForward";

}

void MainWindow::on_fixPositionMoveForward_released()
{
    //use jog mode for fixed end position move (1:end move, 2:joint move, 3: fixed end position move)
    adsWriteStruct.jogMode = 0;

    //use axis angle move parameter for fixed end position move
    adsWriteStruct.axisAngleMoveDirection[0] = 0;

}


void MainWindow::on_refreshProgram_clicked()
{
    if(adsWriteStruct.progRun){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("是否要停止当前运行的程序?");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Stop the current program?");
        }

        WarningWindow::getInstance()->showCancelButton(true);
        WarningWindow::getInstance()->exec();
        WarningWindow::getInstance()->showCancelButton(false);


        if (WarningWindow::getInstance()->yesOrNo) selectProgramHandler(runProgramName);
    }else
        selectProgramHandler(runProgramName);
}

void MainWindow::on_programBackward_clicked()
{
    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();

    if(progBackupPtr > progBackupPtrMin){
        progBackupPtr--;
        if(progBackupPtr == progBackupPtrMax-1) progBackupPtr--;
        if(progBackupPtr == -1) progBackupPtr++;
        openProgramHandler(fileSavePath + "\\old_" + QString::number(progBackupPtr));

    }

    //qDebug() << "**************" << progBackupPtr << "min" << progBackupPtrMin;

    //change the current tree index to 0
    //if no element selected, there is a bug can cause qTreeWidget to crash
    ui->programTree->setCurrentIndex(ui->programTree->model()->index(0, 0));
}

void MainWindow::on_programForward_clicked()
{
    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();

    if(progBackupPtr < progBackupPtrMax-1){
        progBackupPtr++;
        openProgramHandler(fileSavePath + "\\old_" + QString::number(progBackupPtr));
    }

    //qDebug() << "**************" << progBackupPtr << "max" << progBackupPtrMax;

    //change the current tree index to 0
    //if no element selected, there is a bug can cause qTreeWidget to crash
    ui->programTree->setCurrentIndex(ui->programTree->model()->index(0, 0));
}



void MainWindow::on_editButton_clicked()
{
    //a root node must be selected
    if(ui->programTree->currentIndex().row() < 0 || ui->programTree->currentItem()->parent() != NULL){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("帮助: 请选择一个根节点");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Help: Please select a root node");
        }
        WarningWindow::getInstance()->exec();

        return;
    }

    QStringList keywordPieces = ui->programTree->currentItem()->text(0).split( "(" );
    //get keyword
    QString keyWord = keywordPieces.value(0);

    //this function just work for functions, not for Start/moveJ/moveL
    if(keyWord == "Start" || keyWord == "MoveJ" || keyWord == "MoveL" || keyWord == "thread") return;


    //show keyboard to enter the function
    keyboard->setModal(true);
    keyboard->expertModeSwitch(expertMode);
    keyboard->setText(ui->programTree->currentItem()->text(0));
    //if(robotScript)
    keyboard->keyboardType = 3; //modify robot script language
    //else
    //    keyboard->keyboardType = 2; //add function type
    keyboard->exec();
}

void MainWindow::setSystemLanguage(QString language)
{
    QPixmap pixmap;
    QIcon ButtonIcon;
    ui->moveStackedWidget->setCurrentIndex(1);
    int index;
    if(language == "cn") index = 0;
    else if(language == "en") index = 1;
    switch(index){
    case 0:
        //Chinese
        ui->axis1Label->setText("1 关节");
        ui->axis2Label->setText("2 关节");
        ui->axis3Label->setText("3 关节");
        ui->axis4Label->setText("4 关节");
        ui->axis5Label->setText("5 关节");
        ui->axis6Label->setText("6 关节");
        ui->axis7Label->setText("7 关节");
        ui->positionGroupBox->setTitle("位置 / mm");
        ui->jointAngleGroupBox->setTitle("关节角度 / °");
        ui->jointAngleAxis1Panel->setText("1 关节");
        ui->jointAngleAxis2Panel->setText("2 关节");
        ui->jointAngleAxis3Panel->setText("3 关节");
        ui->jointAngleAxis4Panel->setText("4 关节");
        ui->jointAngleAxis5Panel->setText("5 关节");
        ui->jointAngleAxis6Panel->setText("6 关节");
        ui->jointAngleAxis7Panel->setText("7 关节");
        ui->jointAngleAxis1Panel_3->setText("1 关节");
        ui->jointAngleAxis2Panel_3->setText("2 关节");
        ui->jointAngleAxis3Panel_3->setText("3 关节");
        ui->jointAngleAxis4Panel_3->setText("4 关节");
        ui->jointAngleAxis5Panel_3->setText("5 关节");
        ui->jointAngleAxis6Panel_3->setText("6 关节");
        ui->jointAngleAxis7Panel_3->setText("7 关节");
        ui->programTree->setHeaderLabel("程序树");
        ui->programSpeedValueLabel->setText("速度");
        ui->setAccelerationButton->setText("加速度");
        ui->positionGroupBox_2->setTitle("位置 / mm");
        ui->jointAngleGroupBox_2->setTitle("关节角度 / °");
        ui->jointAngleAxis1Panel_2->setText("1 关节");
        ui->jointAngleAxis2Panel_2->setText("2 关节");
        ui->jointAngleAxis3Panel_2->setText("3 关节");
        ui->jointAngleAxis4Panel_2->setText("4 关节");
        ui->jointAngleAxis5Panel_2->setText("5 关节");
        ui->jointAngleAxis6Panel_2->setText("6 关节");
        ui->jointAngleAxis7Panel_2->setText("7 关节");

        ui->runSpeedLabel->setText("速度");
        ui->endFixMoveLabel->setText("末端固定 变换位姿");
        ui->toolCoordinateLabel->setTitle("设置工具坐标系");
        ui->progMoveParametersGroupBox->setTitle("参数");
        ui->progFunctionGroupBox->setTitle("表达式");
        ui->progFuncIntroGroupBox->setTitle("函数介绍");
        ui->setJointValue->setText("输入末端位置");
        ui->setEndValue->setText("输入关节位置");
        ui->moveManualLabel->setText("手动将机械臂移动到程序初始位置");
        ui->AdvancedSetting->setText("高级设置");
        if(adsWriteStruct.toolCoordinateJog){
            pixmap.load(":/images/baseCoordinateSelect.png");
            ButtonIcon.addPixmap(pixmap);
            ui->baseCoordinateBtn->setIcon(ButtonIcon);
            ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
            pixmap.load(":/images/toolCoordinateSelect_2.png");
            ButtonIcon.addPixmap(pixmap);
            ui->toolCoordinateBtn->setIcon(ButtonIcon);
            ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
            ui->coordinateChooseLabel->setText("数据显示及编程使用 工具坐标系");
        }
        else{
            pixmap.load(":/images/baseCoordinateSelect_2.png");
            ButtonIcon.addPixmap(pixmap);
            ui->baseCoordinateBtn->setIcon(ButtonIcon);
            ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
            pixmap.load(":/images/toolCoordinateSelect.png");
            ButtonIcon.addPixmap(pixmap);
            ui->toolCoordinateBtn->setIcon(ButtonIcon);
            ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
            ui->coordinateChooseLabel->setText("数据显示及编程使用 基座坐标系");
        }

        ui->teachSmoothLabel->setText("牵引柔顺度 " + QString::number(ui->teachSmooth->value()));
        ui->tractionTeachButton->setText("牵引");
        ui->copyNode_2->setText("关节柔顺度设置");
        ui->copyNode_3->setText("整体柔顺度设置");
        ui->copyNode->setText("复制节点");
        ui->pasteNode->setText("粘贴节点");
        ui->resetToolCoordinateButton->setText("取消工具坐标");
        ui->setToolCoordinateButton->setText("设定工具坐标系");

        ui->jointEndValue->setText("关节及末端数据");
        ui->log->setText("日志");
        ui->digitalIO->setText("控制柜数字I/O");
        ui->analogIO->setText("控制柜模拟I/O");
        ui->toolIO->setText("工具I/O");
        ui->clearInfo->setText("清除信息");
        ui->toolCoordinate->setText("TCP设置");
        ui->defaultProgram->setText("默认程序");
        ui->ipConfig->setText("IP地址修改");
        ui->safeSetting->setText("安全设置");
        ui->installSetting->setText("安装设置");
        ui->toolLoadRecognize->setText("负载自动辨识");
        ui->tcpCalibration->setText("TCP标定");
        ui->staticIP->setText("静态地址");
        ui->ipAddressLabel_2->setText("当前IP地址:");
        ui->ipSettingGroupBox->setTitle("IP地址设定");
        ui->ipAddressLabel->setText("IP地址:");
        ui->netmaskLabel->setText("子网掩码:");
        ui->gatewayLabel->setText("网关:");
        ui->ipSettingOK->setText("修改IP地址");
        ui->loadDefaultProgram->setText("加载默认程序");
        ui->defaultProgramLabel1_2->setText("当");
        ui->defaultProgramLabel1_3->setText("为");
        ui->defaultProgramLabel1_4->setText("时");
        ui->defaultProgramLabel1->setText("重复运行此程序");
        ui->cancelDefaultProgram->setText("取消默认程序");
        ui->saveDefaultProgram->setText("确认设定");
        ui->setInitPosition->setText("设定当前位置为归零点");
        if(adsWriteStruct.collisionDetectActive){
            ui->collisionDetectActive->setText("碰撞检测中");
            ui->collisionDetectActive_2->setText("碰撞检测 开");
        }
        else{
            ui->collisionDetectActive->setText("无碰撞检测");
            ui->collisionDetectActive_2->setText("碰撞检测 关");
        }

        ui->cameraSetting->setText("相机编程");
        ui->importProgram->setText("导入外部程序");
        ui->callOsk->setText("打开软键盘");
        ui->commandLinkButton_calibration->setText("一键标定");
        ui->label_calPlate->setText("标定板设置");
        ui->coordinateSwitch->setText("坐标系");
        ui->threeDViewSwitch->setText("3D仿真");
        ui->blendRadiusLabel->setText("融合半径");
        ui->moveToStartPoint->setText("按住移动");
        ui->jointInitLabel->setText("关节初始化");
        ui->jointActiveStateLabel1->setText("关节 1");
        ui->jointActiveStateLabel2->setText("关节 2");
        ui->jointActiveStateLabel3->setText("关节 3");
        ui->jointActiveStateLabel4->setText("关节 4");
        ui->jointActiveStateLabel5->setText("关节 5");
        ui->jointActiveStateLabel6->setText("关节 6");
        ui->jointActiveStateLabel7->setText("关节 7");
        ui->return2Start->setText("断电");
        ui->robotActive->setText("全部使能");
        ui->virtualWall->setText("虚拟边界");
        if(toolButtonSaveNode) ui->teachReplayMode->setText("末端记录按钮 开");
        else ui->teachReplayMode->setText("末端记录按钮 关");

        if(autoLoginActive) ui->autoLogin->setText("自动登录 开");
        else ui->autoLogin->setText("自动登录 关");

        ui->toolLoadLabel->setText("质量(kg)");
        ui->tcpPositionGroup->setTitle("质心位置(m)");
        ui->readToolWeightPosition->setText("读取辨识位置");
        ui->setToolWeightPosition->setText("使用此位置");
        ui->toolLoadAutoRecognizeReset->setText("复位");
        ui->toolLoadAutoRecognize->setText("开始辨识");
        ui->toolLoadAutoRecognizePause->setText("暂停");
        ui->tcpCalibrationTypeLabel->setText("选择标定种类");
        ui->tcpCalibration4p->setText("4点");
        ui->tcpCalibration6p->setText("6点");
        ui->tcpCalibrationP1Label->setText("请移动到第一个点后点击下一步");
        ui->tcpCalibrationP1Next->setText("下一步");
        ui->tcpCalibrationP2Label->setText("请移动到第二个点后点击下一步");
        ui->tcpCalibrationP2Next->setText("下一步");
        ui->tcpCalibrationP3Label->setText("请移动到第三个点后点击下一步");
        ui->tcpCalibrationP3Next->setText("下一步");
        ui->tcpCalibrationP4Label->setText("请移动到第四个点后点击下一步");
        ui->tcpCalibrationP4Next->setText("下一步");
        ui->tcpCalibrationP5Label->setText("请沿工具X轴正方向移动后点击下一步");
        ui->tcpCalibrationP5Next->setText("下一步");
        ui->tcpCalibrationP6Label->setText("请沿工具Y轴正方向移动后点击下一步");
        ui->tcpCalibrationP6Next->setText("下一步");
        ui->tcpCalibrationRxyzLabel->setText("请输入工具姿态后点击下一步");
        ui->tcpCalibrationRxyzNext->setText("下一步");
        ui->virtualWallLabel->setText("设置虚拟边界");
        ui->virtualWallCheckbox1->setText("水平边界1 与XY平面距离mm");
        ui->virtualWallCheckbox2->setText("水平边界2 与XY平面距离mm");
        ui->virtualWallCheckbox3->setText("垂直边界1 与Z轴距离mm");
        ui->virtualWallCheckbox4->setText("垂直边界2 与Z轴距离mm");
        ui->virtualWallCheckbox5->setText("垂直边界3 与Z轴距离mm");
        ui->virtualWallCheckbox6->setText("垂直边界4 与Z轴距离mm");
        ui->virtualWall3Label->setText("绕Z轴旋转°");
        ui->virtualWall4Label->setText("绕Z轴旋转°");
        ui->virtualWall5Label->setText("绕Z轴旋转°");
        ui->virtualWall6Label->setText("绕Z轴旋转°");
        ui->cancelVirtualWall->setText("取消虚拟边界");
        ui->setVirtualWall->setText("设定虚拟边界");
        ui->tcpConnectStatus->setText("无外部连接");


        //change button image
        if(ui->functionStackWindow->currentIndex() == 4){
            pixmap.load(":/images/nav-icon-move1.png");
            ButtonIcon.addPixmap(pixmap);
            ui->jogMoveButton->setIcon(ButtonIcon);
            ui->jogMoveButton->setIconSize(pixmap.rect().size());
            pixmap.load(":/images/nav-icon-setting2.png");
            ButtonIcon.addPixmap(pixmap);
            ui->settingButton->setIcon(ButtonIcon);
            ui->settingButton->setIconSize(pixmap.rect().size());
        }else{
            pixmap.load(":/images/nav-icon-move2.png");
            ButtonIcon.addPixmap(pixmap);
            ui->jogMoveButton->setIcon(ButtonIcon);
            ui->jogMoveButton->setIconSize(pixmap.rect().size());
            pixmap.load(":/images/nav-icon-setting1.png");
            ButtonIcon.addPixmap(pixmap);
            ui->settingButton->setIcon(ButtonIcon);
            ui->settingButton->setIconSize(pixmap.rect().size());
        }

        pixmap.load(":/images/nav-icon-program1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-run1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-status1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-about1.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/nav-icon-disconnect.png");
        ButtonIcon.addPixmap(pixmap);
        ui->disconnect->setIcon(ButtonIcon);
        ui->disconnect->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/disconnect.png");
        ButtonIcon.addPixmap(pixmap);
        ui->disconnectAction->setIcon(ButtonIcon);
        ui->disconnectAction->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_joint2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_line.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_teach.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_speed.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/back2zero.png");
        ButtonIcon.addPixmap(pixmap);
        ui->pushButtonMoveToOrigin->setIcon(ButtonIcon);
        ui->pushButtonMoveToOrigin->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Floppy Disk 2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->saveNodeButton->setIcon(ButtonIcon);
        ui->saveNodeButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/openProgram.png");
        ButtonIcon.addPixmap(pixmap);
        ui->selectProgramButton->setIcon(ButtonIcon);
        ui->selectProgramButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Refresh 2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->refreshProgram->setIcon(ButtonIcon);
        ui->refreshProgram->setIconSize(pixmap.rect().size());
        if(repeatProgram){
            pixmap.load(":/images/repeat_2.png");
            ButtonIcon.addPixmap(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }else{
            pixmap.load(":/images/repeat.png");
            ButtonIcon.addPixmap(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }
        //pixmap.load(":/images/Stop@2x.png");
        //ButtonIcon.addPixmap(pixmap);
        //ui->stopProgramButton_run->setIcon(ButtonIcon);
        //ui->stopProgramButton_run->setIconSize(pixmap.rect().size());
        //pixmap.load(":/images/Play@2x.png");
        //ButtonIcon.addPixmap(pixmap);
        //ui->runProgramButton_run->setIcon(ButtonIcon);
        //ui->runProgramButton_run->setIconSize(pixmap.rect().size());
        //pixmap.load(":/images/Next@2x.png");
        //ButtonIcon.addPixmap(pixmap);
        //ui->stepProgramButton_run->setIcon(ButtonIcon);
        //ui->stepProgramButton_run->setIconSize(pixmap.rect().size());
        //pixmap.load(":/images/Pause@2x.png");
        //ButtonIcon.addPixmap(pixmap);
        //ui->pauseProgramButton_run->setIcon(ButtonIcon);
        //ui->pauseProgramButton_run->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/icon-openProg.png");
        ButtonIcon.addPixmap(pixmap);
        ui->openButton->setIcon(ButtonIcon);
        ui->openButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Folder More.png");
        ButtonIcon.addPixmap(pixmap);
        ui->createButton->setIcon(ButtonIcon);
        ui->createButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/deleteProgram.png");
        ButtonIcon.addPixmap(pixmap);
        ui->deleteProgramButton->setIcon(ButtonIcon);
        ui->deleteProgramButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/goBack.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programChooseCancel->setIcon(ButtonIcon);
        ui->programChooseCancel->setIconSize(pixmap.rect().size());
        ui->pushButton_exit->setIcon(ButtonIcon);
        ui->pushButton_exit->setIconSize(pixmap.rect().size());
        ui->back2Program->setIcon(ButtonIcon);
        ui->back2Program->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/numOK.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programChooseOK->setIcon(ButtonIcon);
        ui->programChooseOK->setIconSize(pixmap.rect().size());
        ui->saveSuccessOK->setIcon(ButtonIcon);
        ui->saveSuccessOK->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/finishProg.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programBackBtn->setIcon(ButtonIcon);
        ui->programBackBtn->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/save.png");
        ButtonIcon.addPixmap(pixmap);
        ui->saveButton->setIcon(ButtonIcon);
        ui->saveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/icon-modifyFunc.png");
        ButtonIcon.addPixmap(pixmap);
        ui->editButton->setIcon(ButtonIcon);
        ui->editButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Toggles 3.png");
        ButtonIcon.addPixmap(pixmap);
        ui->moveButton->setIcon(ButtonIcon);
        ui->moveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Paper Airplane.png");
        ButtonIcon.addPixmap(pixmap);
        ui->moveToButton->setIcon(ButtonIcon);
        ui->moveToButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/transition.png");
        ButtonIcon.addPixmap(pixmap);
        ui->transitionButton->setIcon(ButtonIcon);
        ui->transitionButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Add.png");
        ButtonIcon.addPixmap(pixmap);
        ui->addNodeButton->setIcon(ButtonIcon);
        ui->addNodeButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Graduation.png");
        ButtonIcon.addPixmap(pixmap);
        ui->suppressButton->setIcon(ButtonIcon);
        ui->suppressButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Trash.png");
        ButtonIcon.addPixmap(pixmap);
        ui->deleteButton->setIcon(ButtonIcon);
        ui->deleteButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Curved Arrow Left.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programBackward->setIcon(ButtonIcon);
        ui->programBackward->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/Curved Arrow Right.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programForward->setIcon(ButtonIcon);
        ui->programForward->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/icon-star.png");
        ButtonIcon.addPixmap(pixmap);
        ui->startStopButton->setIcon(ButtonIcon);
        ui->startStopButton->setIconSize(pixmap.rect().size());


        break;
    case 1:
        //english
        ui->axis1Label->setText("Axis1");
        ui->axis2Label->setText("Axis2");
        ui->axis3Label->setText("Axis3");
        ui->axis4Label->setText("Axis4");
        ui->axis5Label->setText("Axis5");
        ui->axis6Label->setText("Axis6");
        ui->axis7Label->setText("Axis7");

        ui->positionGroupBox->setTitle("Position / mm");
        ui->jointAngleGroupBox->setTitle("Joint Angle / °");
        ui->jointAngleAxis1Panel->setText("Axis1");
        ui->jointAngleAxis2Panel->setText("Axis2");
        ui->jointAngleAxis3Panel->setText("Axis3");
        ui->jointAngleAxis4Panel->setText("Axis4");
        ui->jointAngleAxis5Panel->setText("Axis5");
        ui->jointAngleAxis6Panel->setText("Axis6");
        ui->jointAngleAxis7Panel->setText("Axis7");
        ui->jointAngleAxis1Panel_3->setText("Axis1");
        ui->jointAngleAxis2Panel_3->setText("Axis2");
        ui->jointAngleAxis3Panel_3->setText("Axis3");
        ui->jointAngleAxis4Panel_3->setText("Axis4");
        ui->jointAngleAxis5Panel_3->setText("Axis5");
        ui->jointAngleAxis6Panel_3->setText("Axis6");
        ui->jointAngleAxis7Panel_3->setText("Axis7");
        ui->programTree->setHeaderLabel("Program Tree");
        ui->programSpeedValueLabel->setText("Speed");
        ui->setAccelerationButton->setText("Acc");
        ui->positionGroupBox_2->setTitle("Position / mm");
        ui->jointAngleGroupBox_2->setTitle("Joint Angle / °");
        ui->jointAngleAxis1Panel_2->setText("Axis1");
        ui->jointAngleAxis2Panel_2->setText("Axis2");
        ui->jointAngleAxis3Panel_2->setText("Axis3");
        ui->jointAngleAxis4Panel_2->setText("Axis4");
        ui->jointAngleAxis5Panel_2->setText("Axis5");
        ui->jointAngleAxis6Panel_2->setText("Axis6");
        ui->jointAngleAxis7Panel_2->setText("Axis7");
        ui->runSpeedLabel->setText("Speed");
        ui->endFixMoveLabel->setText("End-Fix Move");
        ui->toolCoordinateLabel->setTitle("Set Tool Coordinate");
        ui->progMoveParametersGroupBox->setTitle("Parameters");
        ui->progFunctionGroupBox->setTitle("Function");
        ui->progFuncIntroGroupBox->setTitle("Introduce");
        ui->setJointValue->setText("Enter end value");
        ui->setEndValue->setText("Enter joint value");
        if(adsWriteStruct.toolCoordinateJog){
            pixmap.load(":/img_en/img_en/baseCoordinate-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->baseCoordinateBtn->setIcon(ButtonIcon);
            ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
            pixmap.load(":/img_en/img_en/toolCoordinate2-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->toolCoordinateBtn->setIcon(ButtonIcon);
            ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
            ui->coordinateChooseLabel->setText("Parameters in tool coordinate\nProgram in tool coordinate");
        }
        else{
            pixmap.load(":/img_en/img_en/baseCoordinate2-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->baseCoordinateBtn->setIcon(ButtonIcon);
            ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
            pixmap.load(":/img_en/img_en/toolCoordinate-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->toolCoordinateBtn->setIcon(ButtonIcon);
            ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
            ui->coordinateChooseLabel->setText("Parameters in base coordinate\nProgram in base coordinate");
        }

        ui->teachSmoothLabel->setText("Teach smooth: " + QString::number(ui->teachSmooth->value()));
        ui->tractionTeachButton->setText("Teach");
        ui->copyNode_2->setText("Joint Force control Setting");
        ui->copyNode_3->setText("Entire Force control Setting");
        ui->copyNode->setText("Copy Node");
        ui->pasteNode->setText("Paste Node");
        ui->resetToolCoordinateButton->setText("Cancel Setting");
        ui->setToolCoordinateButton->setText("Set tool coordinate");

        ui->jointEndValue->setText("Joint End Value");
        ui->log->setText("Log");
        ui->digitalIO->setText("Digital I/O");
        ui->analogIO->setText("Analog I/O");
        ui->toolIO->setText("Tool I/O");
        ui->clearInfo->setText("Clear info");
        ui->toolCoordinate->setText("TCP Setting");
        ui->defaultProgram->setText("Default Program");
        ui->ipConfig->setText("IP Address");
        ui->safeSetting->setText("Safety Setting");
        ui->installSetting->setText("Install Setting");
        ui->toolLoadRecognize->setText("Auto recognize load");
        ui->tcpCalibration->setText("TCP Calibration");
        ui->staticIP->setText("Static");
        ui->ipAddressLabel_2->setText("Current IP:");
        ui->ipSettingGroupBox->setTitle("Set IP address");
        ui->ipAddressLabel->setText("IP:");
        ui->netmaskLabel->setText("Netmask:");
        ui->gatewayLabel->setText("Gateway:");
        ui->ipSettingOK->setText("Modify IP");
        ui->loadDefaultProgram->setText("Load program");
        ui->defaultProgramLabel1_2->setText("When");
        ui->defaultProgramLabel1_3->setText("is");
        ui->defaultProgramLabel1_4->setText("");
        ui->defaultProgramLabel1->setText("Repeat run this program");
        ui->cancelDefaultProgram->setText("Cancel default program");
        ui->saveDefaultProgram->setText("Set default program");
        ui->setInitPosition->setText("Set Home position");
        ui->moveManualLabel->setText("Move robot manually\nto the Start point");
        ui->moveToStartPoint->setText("Press to Move");
        ui->AdvancedSetting->setText("Advanced Setting");
        if(adsWriteStruct.collisionDetectActive){
            ui->collisionDetectActive->setText("Detecting");
            ui->collisionDetectActive_2->setText("Detecting");
        }
        else{
            ui->collisionDetectActive->setText("No detect");
            ui->collisionDetectActive_2->setText("No detect");
        }

        ui->cameraSetting->setText("Camera Program");
        ui->importProgram->setText("Import");
        ui->callOsk->setText("Keyboard");
        ui->commandLinkButton_calibration->setText("Calibration");
        ui->label_calPlate->setText("Panel Setting");
        ui->coordinateSwitch->setText("Coordinate");
        ui->threeDViewSwitch->setText("3D View");
        ui->blendRadiusLabel->setText("Blend");
        ui->jointInitLabel->setText("Joint Initialization");
        ui->jointActiveStateLabel1->setText("Joint 1");
        ui->jointActiveStateLabel2->setText("Joint 2");
        ui->jointActiveStateLabel3->setText("Joint 3");
        ui->jointActiveStateLabel4->setText("Joint 4");
        ui->jointActiveStateLabel5->setText("Joint 5");
        ui->jointActiveStateLabel6->setText("Joint 6");
        ui->jointActiveStateLabel7->setText("Joint 7");
        ui->return2Start->setText("Power Off");
        ui->robotActive->setText("Enable All");
        ui->virtualWall->setText("Virtual Wall");
        if(toolButtonSaveNode) ui->teachReplayMode->setText("Tool button Save node: ON");
        else ui->teachReplayMode->setText("Tool button Save node: OFF");

        if(autoLoginActive) ui->autoLogin->setText("Auto Login: ON");
        else ui->autoLogin->setText("Auto Login: OFF");

        ui->toolLoadLabel->setText("Load(kg)");
        ui->tcpPositionGroup->setTitle("Load center position(m)");
        ui->readToolWeightPosition->setText("Read recognized position");
        ui->setToolWeightPosition->setText("Use this position");
        ui->toolLoadAutoRecognizeReset->setText("Reset");
        ui->toolLoadAutoRecognize->setText("Start Recognize");
        ui->toolLoadAutoRecognizePause->setText("Pause");
        ui->tcpCalibrationTypeLabel->setText("Select Calibration Methode");
        ui->tcpCalibration4p->setText("4 Points");
        ui->tcpCalibration6p->setText("6 Points");
        ui->tcpCalibrationP1Label->setText("Please move to the 1st point\nThen click Next");
        ui->tcpCalibrationP1Next->setText("Next");
        ui->tcpCalibrationP2Label->setText("Please move to the 2nd point\nThen click Next");
        ui->tcpCalibrationP2Next->setText("Next");
        ui->tcpCalibrationP3Label->setText("Please move to the 3rd point\nThen click Next");
        ui->tcpCalibrationP3Next->setText("Next");
        ui->tcpCalibrationP4Label->setText("Please move to the 4th point\nThen click Next");
        ui->tcpCalibrationP4Next->setText("Next");
        ui->tcpCalibrationP5Label->setText("Move along the X axis of the tool coordinate\nThen click Next");
        ui->tcpCalibrationP5Next->setText("Next");
        ui->tcpCalibrationP6Label->setText("Move along the Y axis of the tool coordinate\nThen click Next");
        ui->tcpCalibrationP6Next->setText("Next");
        ui->tcpCalibrationRxyzLabel->setText("Please enter the tool posture\nThen click Next");
        ui->tcpCalibrationRxyzNext->setText("Next");
        ui->virtualWallLabel->setText("Set Virtual Wall");
        ui->virtualWallCheckbox1->setText("Horizental wall 1 to XY plane(mm)");
        ui->virtualWallCheckbox2->setText("Horizental wall 2 to XY plane(mm)");
        ui->virtualWallCheckbox3->setText("Vertical wall 1 to Z axis(mm)");
        ui->virtualWallCheckbox4->setText("Vertical wall 2 to Z axis(mm)");
        ui->virtualWallCheckbox5->setText("Vertical wall 3 to Z axis(mm)");
        ui->virtualWallCheckbox6->setText("Vertical wall 4 to Z axis(mm)");
        ui->virtualWall3Label->setText("Rot on Z axis°");
        ui->virtualWall4Label->setText("Rot on Z axis°");
        ui->virtualWall5Label->setText("Rot on Z axis°");
        ui->virtualWall6Label->setText("Rot on Z axis°");
        ui->cancelVirtualWall->setText("Cancel Virtual Wall");
        ui->setVirtualWall->setText("Set Virtual Wall");
        ui->tcpConnectStatus->setText("No connection");


        //change button image
        if(ui->functionStackWindow->currentIndex() == 4){
            pixmap.load(":/img_en/img_en/nav-icon-move1-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->jogMoveButton->setIcon(ButtonIcon);
            ui->jogMoveButton->setIconSize(pixmap.rect().size());
            pixmap.load(":/img_en/img_en/nav-icon-setting2-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->settingButton->setIcon(ButtonIcon);
            ui->settingButton->setIconSize(pixmap.rect().size());
        }else{
            pixmap.load(":/img_en/img_en/nav-icon-move2-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->jogMoveButton->setIcon(ButtonIcon);
            ui->jogMoveButton->setIconSize(pixmap.rect().size());
            pixmap.load(":/img_en/img_en/nav-icon-setting1-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->settingButton->setIcon(ButtonIcon);
            ui->settingButton->setIconSize(pixmap.rect().size());
        }

        pixmap.load(":/img_en/img_en/nav-icon-program1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programButton->setIcon(ButtonIcon);
        ui->programButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-run1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runButton->setIcon(ButtonIcon);
        ui->runButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-status1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->statusButton->setIcon(ButtonIcon);
        ui->statusButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-about1-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->aboutButton->setIcon(ButtonIcon);
        ui->aboutButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/nav-icon-disconnect-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->disconnect->setIcon(ButtonIcon);
        ui->disconnect->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/disconnect-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->disconnectAction->setIcon(ButtonIcon);
        ui->disconnectAction->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_joint2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_line-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_teach-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_speed-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/back2zero-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->pushButtonMoveToOrigin->setIcon(ButtonIcon);
        ui->pushButtonMoveToOrigin->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/savePosition-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->saveNodeButton->setIcon(ButtonIcon);
        ui->saveNodeButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/openProgram-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->selectProgramButton->setIcon(ButtonIcon);
        ui->selectProgramButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Refresh 2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->refreshProgram->setIcon(ButtonIcon);
        ui->refreshProgram->setIconSize(pixmap.rect().size());
        if(repeatProgram){
            pixmap.load(":/img_en/img_en/repeat_2-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }else{
            pixmap.load(":/img_en/img_en/repeat-en.png");
            ButtonIcon.addPixmap(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }
        pixmap.load(":/img_en/img_en/Stop-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->stopProgramButton_run->setIcon(ButtonIcon);
        ui->stopProgramButton_run->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Play-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->runProgramButton_run->setIcon(ButtonIcon);
        ui->runProgramButton_run->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Next-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->stepProgramButton_run->setIcon(ButtonIcon);
        ui->stepProgramButton_run->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Pause-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->pauseProgramButton_run->setIcon(ButtonIcon);
        ui->pauseProgramButton_run->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/icon-openProg.png");
        ButtonIcon.addPixmap(pixmap);
        ui->openButton->setIcon(ButtonIcon);
        ui->openButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Folder More_2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->createButton->setIcon(ButtonIcon);
        ui->createButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/deleteProgram.png");
        ButtonIcon.addPixmap(pixmap);
        ui->deleteProgramButton->setIcon(ButtonIcon);
        ui->deleteProgramButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/goBack.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programChooseCancel->setIcon(ButtonIcon);
        ui->programChooseCancel->setIconSize(pixmap.rect().size());
        ui->pushButton_exit->setIcon(ButtonIcon);
        ui->pushButton_exit->setIconSize(pixmap.rect().size());
        ui->back2Program->setIcon(ButtonIcon);
        ui->back2Program->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/numOK.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programChooseOK->setIcon(ButtonIcon);
        ui->programChooseOK->setIconSize(pixmap.rect().size());
        ui->saveSuccessOK->setIcon(ButtonIcon);
        ui->saveSuccessOK->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/finishProg.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programBackBtn->setIcon(ButtonIcon);
        ui->programBackBtn->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/save.png");
        ButtonIcon.addPixmap(pixmap);
        ui->saveButton->setIcon(ButtonIcon);
        ui->saveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/icon-modifyFunc.png");
        ButtonIcon.addPixmap(pixmap);
        ui->editButton->setIcon(ButtonIcon);
        ui->editButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Toggles 3.png");
        ButtonIcon.addPixmap(pixmap);
        ui->moveButton->setIcon(ButtonIcon);
        ui->moveButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Paper Airplane.png");
        ButtonIcon.addPixmap(pixmap);
        ui->moveToButton->setIcon(ButtonIcon);
        ui->moveToButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/transition.png");
        ButtonIcon.addPixmap(pixmap);
        ui->transitionButton->setIcon(ButtonIcon);
        ui->transitionButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Add.png");
        ButtonIcon.addPixmap(pixmap);
        ui->addNodeButton->setIcon(ButtonIcon);
        ui->addNodeButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Graduation.png");
        ButtonIcon.addPixmap(pixmap);
        ui->suppressButton->setIcon(ButtonIcon);
        ui->suppressButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Trash.png");
        ButtonIcon.addPixmap(pixmap);
        ui->deleteButton->setIcon(ButtonIcon);
        ui->deleteButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Curved Arrow Left.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programBackward->setIcon(ButtonIcon);
        ui->programBackward->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/Curved Arrow Right.png");
        ButtonIcon.addPixmap(pixmap);
        ui->programForward->setIcon(ButtonIcon);
        ui->programForward->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/icon-star-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->startStopButton->setIcon(ButtonIcon);
        ui->startStopButton->setIconSize(pixmap.rect().size());

        break;
    default:
        break;
    }

}


void MainWindow::on_suppressButton_clicked()
{
    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();

    //a root node must be selected
    if(ui->programTree->currentIndex().row() < 0 || ui->programTree->currentItem()->parent() != NULL){
        return;
    }

    QStringList keywordPieces = ui->programTree->currentItem()->text(0).split('(');
    if(keywordPieces.value(0) == "Start") return; //can't comment Start point

    //check if the node is the last position of a blend move
    //QStringList parameterPieces = keywordPieces.value(1).split( ")" ).value(0).split( "," );
    //if(keywordPieces.value(0) == "MoveL"){
    //    if(parameterPieces.count() == 15){
    //        //no blend or last position of the blend
    //        QAbstractItemModel* model = ui->programTree->model();
    //        QModelIndex index1 = model->index(ui->programTree->currentIndex().row(), 0); //current index
    //
    //        QModelIndex index2 = model->index(ui->programTree->currentIndex().row() - 1, 0); //previous index
    //        if(!index2.isValid()) return;
    //
    //        ui->programTree->setCurrentIndex(index2);
    //
    //        if(ui->programTree->currentItem()->text(0).split('(').value(1).split( ")" ).value(0).split( "," ).count() == 16){
    //            ui->programTree->setCurrentIndex(index1);
    //
    //            //previous move has blend
    //            if(systemLanguage == "cn"){
    //                WarningWindow::getInstance()->setWarningText("帮助: 融合段终点不能注释");
    //            }else if(systemLanguage == "en"){
    //                WarningWindow::getInstance()->setWarningText("Help: Can't comment last node of blend move");
    //            }
    //            WarningWindow::getInstance()->exec();
    //            return;
    //        }
    //
    //        ui->programTree->setCurrentIndex(index1);
    //
    //    }
    //}

    if(ui->programTree->currentItem()->text(0).at(0) == '#'){
        //change current node
        ui->programTree->currentItem()->setText(0, ui->programTree->currentItem()->text(0).split("#").value(1));

        ui->programTree->currentItem()->setTextColor(0, Qt::white);

        //save the new program tree
        saveTheNewProgramTree();

        //move current item to next item
        if(ui->programTree->itemBelow(ui->programTree->currentItem()) != NULL)
            ui->programTree->setCurrentIndex(ui->programTree->model()->index(ui->programTree->currentIndex().row() + 1, 0));
    }
    else{
        //change current node
        ui->programTree->currentItem()->setText(0, "#" + ui->programTree->currentItem()->text(0));

        ui->programTree->currentItem()->setTextColor(0, Qt::gray);

        //save the new program tree
        saveTheNewProgramTree();

        //move current item to next item
        if(ui->programTree->itemBelow(ui->programTree->currentItem()) != NULL)
            ui->programTree->setCurrentIndex(ui->programTree->model()->index(ui->programTree->currentIndex().row() + 1, 0));
    }

    //QModelIndex index;
    //on_programTree_clicked(index);
    QTreeWidgetItem *current = NULL;
    on_programTree_currentItemChanged(current, current);
}



void MainWindow::on_AdvancedSetting_clicked()
{
    //advanced tool setting
    //if(ui->settingStack->currentIndex() == 1)
    //    ui->settingStack->setCurrentIndex(0);
    //else
    ui->settingStack->setCurrentIndex(3);
}



void MainWindow::on_toolCoordinate_clicked()
{
    //Tool coordinate setting
    //if(ui->settingStack->currentIndex() == 2)
    //    ui->settingStack->setCurrentIndex(0);
    //else
    ui->settingStack->setCurrentIndex(2);
}



void MainWindow::on_setToolCoordinateButton_clicked()
{
    adsWriteStruct.toolCoordinate[0] = ui->toolCoordinateLineX->text().toDouble();
    adsWriteStruct.toolCoordinate[1] = ui->toolCoordinateLineY->text().toDouble();
    adsWriteStruct.toolCoordinate[2] = ui->toolCoordinateLineZ->text().toDouble();
    adsWriteStruct.toolCoordinateR[0] = ui->toolCoordinateLineRX->text().toDouble();
    adsWriteStruct.toolCoordinateR[1] = ui->toolCoordinateLineRY->text().toDouble();
    adsWriteStruct.toolCoordinateR[2] = ui->toolCoordinateLineRZ->text().toDouble();

    //find if the new tool coordinate is already saved
    bool findCoordinateSaved = false;
    for(int i = 0; i < 5; i++){
        if(toolCoordinateSave[i][0] == adsWriteStruct.toolCoordinate[0] && \
                toolCoordinateSave[i][1] == adsWriteStruct.toolCoordinate[1] && \
                toolCoordinateSave[i][2] == adsWriteStruct.toolCoordinate[2] && \
                toolCoordinateSave[i][3] == adsWriteStruct.toolCoordinateR[0] && \
                toolCoordinateSave[i][4] == adsWriteStruct.toolCoordinateR[1] && \
                toolCoordinateSave[i][5] == adsWriteStruct.toolCoordinateR[2]){
            findCoordinateSaved = true;
        }
    }

    if(!findCoordinateSaved){
        toolCoordinateSave[toolCoordinateSaveLoop][0] = adsWriteStruct.toolCoordinate[0];
        toolCoordinateSave[toolCoordinateSaveLoop][1] = adsWriteStruct.toolCoordinate[1];
        toolCoordinateSave[toolCoordinateSaveLoop][2] = adsWriteStruct.toolCoordinate[2];
        toolCoordinateSave[toolCoordinateSaveLoop][3] = adsWriteStruct.toolCoordinateR[0];
        toolCoordinateSave[toolCoordinateSaveLoop][4] = adsWriteStruct.toolCoordinateR[1];
        toolCoordinateSave[toolCoordinateSaveLoop][5] = adsWriteStruct.toolCoordinateR[2];


        switch(toolCoordinateSaveLoop){
            case 0:
                ui->selectToolCoordinate1->setText("X: " + ui->toolCoordinateLineX->text() + " " + \
                                                   "Y: " + ui->toolCoordinateLineY->text() + " " + \
                                                   "Z: " + ui->toolCoordinateLineZ->text() + " " + \
                                                   "RX: " + ui->toolCoordinateLineRX->text() + " " + \
                                                   "RY: " + ui->toolCoordinateLineRY->text() + " " + \
                                                   "RZ: " + ui->toolCoordinateLineRZ->text());
                ui->selectToolCoordinate1->setVisible(true);
                break;
            case 1:
                ui->selectToolCoordinate2->setText("X: " + ui->toolCoordinateLineX->text() + " " + \
                                                   "Y: " + ui->toolCoordinateLineY->text() + " " + \
                                                   "Z: " + ui->toolCoordinateLineZ->text() + " " + \
                                                   "RX: " +ui->toolCoordinateLineRX->text() + " " + \
                                                   "RY: " +ui->toolCoordinateLineRY->text() + " " + \
                                                   "RZ: " +ui->toolCoordinateLineRZ->text());
                ui->selectToolCoordinate2->setVisible(true);
                break;
            case 2:
                ui->selectToolCoordinate3->setText("X: " + ui->toolCoordinateLineX->text() + " " + \
                                                   "Y: " + ui->toolCoordinateLineY->text() + " " + \
                                                   "Z: " + ui->toolCoordinateLineZ->text() + " " + \
                                                   "RX: " +ui->toolCoordinateLineRX->text() + " " + \
                                                   "RY: " +ui->toolCoordinateLineRY->text() + " " + \
                                                   "RZ: " +ui->toolCoordinateLineRZ->text());
                ui->selectToolCoordinate3->setVisible(true);
                break;
            case 3:
                ui->selectToolCoordinate4->setText("X: " + ui->toolCoordinateLineX->text() + " " + \
                                                   "Y: " + ui->toolCoordinateLineY->text() + " " + \
                                                   "Z: " + ui->toolCoordinateLineZ->text() + " " + \
                                                   "RX: " +ui->toolCoordinateLineRX->text() + " " + \
                                                   "RY: " +ui->toolCoordinateLineRY->text() + " " + \
                                                   "RZ: " +ui->toolCoordinateLineRZ->text());
                ui->selectToolCoordinate4->setVisible(true);
                break;
            case 4:
                ui->selectToolCoordinate5->setText("X: " + ui->toolCoordinateLineX->text() + " " + \
                                                   "Y: " + ui->toolCoordinateLineY->text() + " " + \
                                                   "Z: " + ui->toolCoordinateLineZ->text() + " " + \
                                                   "RX: " +ui->toolCoordinateLineRX->text() + " " + \
                                                   "RY: " +ui->toolCoordinateLineRY->text() + " " + \
                                                   "RZ: " +ui->toolCoordinateLineRZ->text());
                ui->selectToolCoordinate5->setVisible(true);
                break;
        }


        QFile toolCoordinateFile(fileSavePath + "\\tool_coordinates");
        toolCoordinateFile.open(QIODevice::ReadWrite);
        QTextStream stream( &toolCoordinateFile );
        char output[300];
        sprintf(output, "%0.2f:%0.2f:%0.2f:%0.2f:%0.2f:%0.2f;%0.2f:%0.2f:%0.2f:%0.2f:%0.2f:%0.2f;%0.2f:%0.2f:%0.2f:%0.2f:%0.2f:%0.2f;%0.2f:%0.2f:%0.2f:%0.2f:%0.2f:%0.2f;%0.2f:%0.2f:%0.2f:%0.2f:%0.2f:%0.2f", \
                toolCoordinateSave[0][0],toolCoordinateSave[0][1],toolCoordinateSave[0][2],\
                toolCoordinateSave[0][3],toolCoordinateSave[0][4],toolCoordinateSave[0][5],\
                toolCoordinateSave[1][0],toolCoordinateSave[1][1],toolCoordinateSave[1][2],\
                toolCoordinateSave[1][3],toolCoordinateSave[1][4],toolCoordinateSave[1][5],\
                toolCoordinateSave[2][0],toolCoordinateSave[2][1],toolCoordinateSave[2][2],\
                toolCoordinateSave[2][3],toolCoordinateSave[2][4],toolCoordinateSave[2][5],\
                toolCoordinateSave[3][0],toolCoordinateSave[3][1],toolCoordinateSave[3][2],\
                toolCoordinateSave[3][3],toolCoordinateSave[3][4],toolCoordinateSave[3][5],\
                toolCoordinateSave[4][0],toolCoordinateSave[4][1],toolCoordinateSave[4][2],\
                toolCoordinateSave[4][3],toolCoordinateSave[4][4],toolCoordinateSave[4][5]);

        stream << output;
        toolCoordinateFile.close();

        toolCoordinateSaveLoop++;
        if(toolCoordinateSaveLoop == 5) toolCoordinateSaveLoop = 0;
    }

    adsWriteStruct.toolCoordinateJog = true;
    //move back to move page
    on_jogMoveButton_clicked();

    //current status show with tool coordinate or not
    if(adsWriteStruct.toolCoordinate[0] == 0 && adsWriteStruct.toolCoordinate[1] == 0 && \
            adsWriteStruct.toolCoordinate[2] == 0 && adsWriteStruct.toolCoordinateR[0] == 0 \
            && adsWriteStruct.toolCoordinateR[1] == 0 && adsWriteStruct.toolCoordinateR[2] == 0){
        if(systemLanguage == "cn"){
            ui->positionGroupBox_2->setTitle("位置 / mm");
            ui->positionGroupBox->setTitle("位置 / mm");
        }else if(systemLanguage == "en"){
            ui->positionGroupBox_2->setTitle("Position / mm");
            ui->positionGroupBox->setTitle("Position / mm");
        }
        adsWriteStruct.statusUseToolCoordinate = false;
    }else{
        if(systemLanguage == "cn"){
            ui->positionGroupBox_2->setTitle("位置 / mm (含工具坐标)");
            ui->positionGroupBox->setTitle("位置 / mm (含工具坐标)");
        }else if(systemLanguage == "en"){
            ui->positionGroupBox_2->setTitle("Position / mm (with tool)");
            ui->positionGroupBox->setTitle("Position / mm (with tool)");
        }
        adsWriteStruct.statusUseToolCoordinate = true;
    }

    //clear old tool point
    ui->move3DWidget->rob->setTool(false, adsWriteStruct.toolCoordinate[0], adsWriteStruct.toolCoordinate[1],\
            adsWriteStruct.toolCoordinate[2], adsWriteStruct.toolCoordinateR[0], adsWriteStruct.toolCoordinateR[1], adsWriteStruct.toolCoordinateR[2]);
    //create new
    ui->move3DWidget->rob->setTool(true, adsWriteStruct.toolCoordinate[0], adsWriteStruct.toolCoordinate[1],\
            adsWriteStruct.toolCoordinate[2], adsWriteStruct.toolCoordinateR[0], adsWriteStruct.toolCoordinateR[1], adsWriteStruct.toolCoordinateR[2]);


    //update robot position
    update3DView(adsReadStruct.axisCurrentAngle);

    //recreate virtual wall
    if(virtualWallSetted) on_setVirtualWall_clicked();

    //save tool coordinate to config file
    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "TOOL"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        text.replace(replaceTarget, "TOOL	:	on  :" + ui->toolCoordinateLineX->text() + ":" + \
                     ui->toolCoordinateLineY->text() + ":" + \
                     ui->toolCoordinateLineZ->text() + ":" + \
                     ui->toolCoordinateLineRX->text() + ":" + \
                     ui->toolCoordinateLineRY->text() + ":" + \
                     ui->toolCoordinateLineRZ->text() + "\n");
    }else{
        text.append("\nTOOL	:	on  :" + ui->toolCoordinateLineX->text() + ":" + \
                    ui->toolCoordinateLineY->text() + ":" + \
                    ui->toolCoordinateLineZ->text() + ":" + \
                    ui->toolCoordinateLineRX->text() + ":" + \
                    ui->toolCoordinateLineRY->text() + ":" + \
                    ui->toolCoordinateLineRZ->text() + "\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_switchToEndMove_clicked()
{
    ui->moveStackedWidget->setCurrentIndex(0);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/move_joint.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_line2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_teach.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_speed.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/move_joint-en.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_line2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_teach-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_speed-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_switchToJointMove_clicked()
{
    ui->moveStackedWidget->setCurrentIndex(1);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/move_joint2.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_line.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_teach.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_speed.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/move_joint2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_line-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_teach-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_speed-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }
}


void MainWindow::on_jointAngleLcdNumberAxis1_clicked()
{
    if(programIsRunning) return;

    //enter the target joint position
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = true;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    numberBoard->changeLanguage();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入关节目标位置");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter joint Values");
    numberBoard->editLineNum = 9; //for jod mode enter joint value
    numberBoard->windowIsOpened = true;
    numberBoard->exec();
}

void MainWindow::on_jointAngleLcdNumberAxis2_clicked(){on_jointAngleLcdNumberAxis1_clicked();}
void MainWindow::on_jointAngleLcdNumberAxis3_clicked(){on_jointAngleLcdNumberAxis1_clicked();}
void MainWindow::on_jointAngleLcdNumberAxis4_clicked(){on_jointAngleLcdNumberAxis1_clicked();}
void MainWindow::on_jointAngleLcdNumberAxis5_clicked(){on_jointAngleLcdNumberAxis1_clicked();}
void MainWindow::on_jointAngleLcdNumberAxis6_clicked(){on_jointAngleLcdNumberAxis1_clicked();}
void MainWindow::on_jointAngleLcdNumberAxis7_clicked(){on_jointAngleLcdNumberAxis1_clicked();}

void MainWindow::on_positionLcdNumberX_clicked()
{
    if(programIsRunning) return;

    //enter the target xyz position
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = true;
    numberBoard->changeWindowSize();
    numberBoard->changeLanguage();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入末端目标位置\n基坐标系");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter End Values\nIn base coordinate");
    //numberBoard->editLineNum = 6; //for jod mode position(x y z rx ry rz)
    numberBoard->windowIsOpened = true;
    numberBoard->exec();
}

void MainWindow::on_positionLcdNumberY_clicked(){on_positionLcdNumberX_clicked();}

void MainWindow::on_positionLcdNumberZ_clicked(){on_positionLcdNumberX_clicked();}

void MainWindow::on_positionLcdNumberRX_clicked(){on_positionLcdNumberX_clicked();}

void MainWindow::on_positionLcdNumberRY_clicked(){on_positionLcdNumberX_clicked();}

void MainWindow::on_positionLcdNumberRZ_clicked(){on_positionLcdNumberX_clicked();}

void MainWindow::on_toolCoordinateLineX_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具坐标系X");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate X");
    numberBoard->editLineNum = 3; //for tool coordinate X
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolCoordinateLineY_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具坐标系Y");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate Y");
    numberBoard->editLineNum = 4; //for tool coordinate Y
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolCoordinateLineZ_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具坐标系Z");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate Z");
    numberBoard->editLineNum = 5; //for tool coordinate Z
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolCoordinateLineRX_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具坐标系RX");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate RX");
    numberBoard->editLineNum = 9; //for tool coordinate RX
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolCoordinateLineRY_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具坐标系RY");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate RY");
    numberBoard->editLineNum = 10; //for tool coordinate RY
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolCoordinateLineRZ_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具坐标系RZ");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate RZ");
    numberBoard->editLineNum = 11; //for tool coordinate RZ
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_stopJogMove1_clicked()
{
    //stop move
    adsWriteStruct.progStop = true;


    //wait for stop
    QThread::msleep(100);

    //reset
    adsWriteStruct.moveTo = false;
    adsWriteStruct.progStop = false;

}


void MainWindow::on_resetToolCoordinateButton_clicked()
{
    ui->toolCoordinateLineX->setText("0");
    ui->toolCoordinateLineY->setText("0");
    ui->toolCoordinateLineZ->setText("0");
    ui->toolCoordinateLineRX->setText("0");
    ui->toolCoordinateLineRY->setText("0");
    ui->toolCoordinateLineRZ->setText("0");

    adsWriteStruct.toolCoordinate[0] = 0;
    adsWriteStruct.toolCoordinate[1] = 0;
    adsWriteStruct.toolCoordinate[2] = 0;
    adsWriteStruct.toolCoordinateR[0] = 0;
    adsWriteStruct.toolCoordinateR[1] = 0;
    adsWriteStruct.toolCoordinateR[2] = 0;

    //move back to move page
    on_jogMoveButton_clicked();

    if(systemLanguage == "cn"){
        ui->positionGroupBox_2->setTitle("位置 / mm");
        ui->positionGroupBox->setTitle("位置 / mm");
    }else if(systemLanguage == "en"){
        ui->positionGroupBox_2->setTitle("Position / mm");
        ui->positionGroupBox->setTitle("Position / mm");
    }
    on_baseCoordinateBtn_clicked();


    //save tool coordinate to config file
    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "TOOL"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        text.replace(replaceTarget, "TOOL	:	off\n");
    }else{
        text.append("\nTOOL	:	off\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_programTree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    item;column;
    on_moveButton_clicked();
}

void MainWindow::on_expertMode_clicked()
{
    if(expertMode){
        expertMode = !expertMode;
        switchExpertMode(expertMode);

    }else{
        //show number board
        numberBoard->setModal(true);
        numberBoard->cleanLineEdit();
        numberBoard->axisGroupVisible = false;
        numberBoard->positionGroupVisible = false;
        numberBoard->changeWindowSize();
        if(systemLanguage == "cn")
            numberBoard->setLabelText("专家模式");
        else if(systemLanguage == "en")
            numberBoard->setLabelText("Expert Mode");
        numberBoard->editLineNum = 12; //for expert mode
        //numberBoard->windowIsOpened = true;
        numberBoard->changeLanguage();
        numberBoard->exec();

    }
}


void MainWindow::on_copySpeedButton_clicked()
{
    copySpeedButtonClicked = !copySpeedButtonClicked;
    //change shift button view

    if(copySpeedButtonClicked){
        QPixmap pixmap(":/images/icon-copy_2.png");
        QIcon ButtonIcon(pixmap);
        ui->copySpeedButton->setIcon(ButtonIcon);
        ui->copySpeedButton->setIconSize(pixmap.rect().size());

        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("速度已复制，单击左侧节点\n来批量改变移动速度");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Speed copied, click the target node\nto paste the speed");
        }
        WarningWindow::getInstance()->exec();

        if(ui->setSpeedButton->text().contains('%')) copiedSpeed = ui->setSpeedButton->text().split('%').value(0);
        else copiedSpeed = ui->setSpeedButton->text().split("mm").value(0);
    }
    else{
        QPixmap pixmap(":/images/icon-copy.png");
        QIcon ButtonIcon(pixmap);
        ui->copySpeedButton->setIcon(ButtonIcon);
        ui->copySpeedButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_transitionButton_clicked()
{
    //if current command line is not moveL or moveC, exit
    QString keyWord = ui->programTree->currentItem()->text(0).split( "(" ).value(0);
    if(keyWord != "MoveL" && keyWord != "MoveC_2" && keyWord != "MoveJ" && keyWord != "Start") return;
    //QAbstractItemModel* model = ui->programTree->model();
    //QModelIndex index1 = model->index(ui->programTree->currentIndex().row(), 0); //current index
    //
    //QModelIndex index2 = model->index(ui->programTree->currentIndex().row() + 1, 0); //next index
    ////moveL is the last line, return
    //if(!index2.isValid()) return;
    //
    //ui->programTree->setCurrentIndex(index2);

    //if next command line is not moveL or moveC, exit
    //keyWord = ui->programTree->currentItem()->text(0).split( "(" ).value(0);
    //ui->programTree->setCurrentIndex(index1); //change back to current index
    //
    //if(keyWord != "MoveL") return;

    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();

    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入融合半径，单位mm\n直接点击确定则取消融合半径");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter blend radias, unit:mm\nJust click OK to cancel\nblend radias");
    numberBoard->editLineNum = 6; //transition semidiameter
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_speedSettingButton_clicked()
{
    ui->moveStackedWidget->setCurrentIndex(3);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/move_joint.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_line.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_teach.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_speed2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/move_joint-en.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_line-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_teach-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_speed2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_teachButton_clicked()
{
    ui->moveStackedWidget->setCurrentIndex(2);

    //change button image
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/move_joint.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_line.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_teach2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/move_speed.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/move_joint-en.png");
        QIcon ButtonIcon(pixmap);
        ui->switchToJointMove->setIcon(ButtonIcon);
        ui->switchToJointMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_line-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->switchToEndMove->setIcon(ButtonIcon);
        ui->switchToEndMove->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_teach2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->teachButton->setIcon(ButtonIcon);
        ui->teachButton->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/move_speed-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->speedSettingButton->setIcon(ButtonIcon);
        ui->speedSettingButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_threeDViewSwitch_clicked()
{
    QPixmap pixmap(":/images/move_coordinate.png");
    QIcon ButtonIcon(pixmap);
    QPixmap pixmap2(":/images/move_3D.png");
    QIcon ButtonIcon2(pixmap2);
    QPixmap pixmap3(":/images/move_coordinate.png");
    QIcon ButtonIcon3(pixmap3);
    QPixmap pixmap4(":/images/move_3D2.png");
    QIcon ButtonIcon4(pixmap4);

    //switch to 3D view
    switch(ui->moveParameterStackWindow->currentIndex()){
    case 1:
        ui->moveParameterStackWindow->setCurrentIndex(0);

        //change button image
        ui->coordinateSwitch->setIcon(ButtonIcon);
        ui->coordinateSwitch->setIconSize(pixmap.rect().size());
        ui->threeDViewSwitch->setIcon(ButtonIcon2);
        ui->threeDViewSwitch->setIconSize(pixmap2.rect().size());
        break;
    default:
        ui->moveParameterStackWindow->setCurrentIndex(1);

        //change button image
        ui->coordinateSwitch->setIcon(ButtonIcon3);
        ui->coordinateSwitch->setIconSize(pixmap3.rect().size());
        ui->threeDViewSwitch->setIcon(ButtonIcon4);
        ui->threeDViewSwitch->setIconSize(pixmap4.rect().size());

        //change 3D view position
        robotWindowChangePosition(1);

        break;
    }


}

void MainWindow::on_coordinateSwitch_clicked()
{
    QPixmap pixmap(":/images/move_coordinate.png");
    QIcon ButtonIcon(pixmap);
    QPixmap pixmap2(":/images/move_3D.png");
    QIcon ButtonIcon2(pixmap2);
    QPixmap pixmap3(":/images/move_coordinate2.png");
    QIcon ButtonIcon3(pixmap3);
    QPixmap pixmap4(":/images/move_3D.png");
    QIcon ButtonIcon4(pixmap4);

    //switch to tool coordinate setting
    switch(ui->moveParameterStackWindow->currentIndex()){
    case 2:
        ui->moveParameterStackWindow->setCurrentIndex(0);

        //change button image
        ui->coordinateSwitch->setIcon(ButtonIcon);
        ui->coordinateSwitch->setIconSize(pixmap.rect().size());
        ui->threeDViewSwitch->setIcon(ButtonIcon2);
        ui->threeDViewSwitch->setIconSize(pixmap2.rect().size());
        break;
    default:
        ui->moveParameterStackWindow->setCurrentIndex(2);

        //change button image
        ui->coordinateSwitch->setIcon(ButtonIcon3);
        ui->coordinateSwitch->setIconSize(pixmap3.rect().size());
        ui->threeDViewSwitch->setIcon(ButtonIcon4);
        ui->threeDViewSwitch->setIconSize(pixmap4.rect().size());
        break;
    }
}

void MainWindow::on_programBackBtn_clicked()
{
    copyNode = "";  //clear the copied node

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("是否保存程序？");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Save program?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo){
        //detect if the speed copy button is on, if so, turn it off
        if(copySpeedButtonClicked) on_copySpeedButton_clicked();

        saveProgramHandler(programName);
    }

    ui->functionStackWindow->setCurrentIndex(7);

    programOpened = false;
}

void MainWindow::on_programNameList_itemClicked(QListWidgetItem *item)
{
    QFile file(fileSavePath + "\\program\\" + item->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    //show program on the list widget
    ui->programDetail->clear();

    int i = 1;
    while (!file.atEnd()) {
        //read each line
        QString line = file.readLine();
        ui->programDetail->addItem(QString::number(i++) + " " + line);
    }

}

void MainWindow::on_programChooseOK_clicked()
{
    if(selectDefaultProgram){
        //select default program
        selectDefaultProgram = false;
        ui->defaultProgramName->setText(ui->programNameList->currentItem()->text());
        ui->functionStackWindow->setCurrentIndex(4);
        return;
    }

    if(selectProgramToRun){
        //run program
        selectProgramToRun = false;
        QString programNameRun;
        if(ui->programNameList->currentItem()->text().split('.').value(1) == "rs"){
            qDebug() << "a robot script program";

            QString command = "robotc C:/SiasunRobot/program/" + ui->programNameList->currentItem()->text() + " > t.txt";
            QByteArray ba = command.toLatin1();
            const char *c_str2 = ba.data();
            _chdir("C:/SiasunRobot/c_code");
            //system(c_str2);
            WinExec(c_str2, SW_HIDE);
            programNameRun = "C:/SiasunRobot/c_code/result/actionlist";

        }else{

            programNameRun = "C:/SiasunRobot/program/" + ui->programNameList->currentItem()->text();
        }
        selectProgramHandler(programNameRun);

        ui->runTimeLabel->setVisible(false);

        on_runButton_clicked();

    }else{
        copyNode = "";  //clear the copied node

        //modify program
        //detect if the speed copy button is on, if so, turn it off
        if(copySpeedButtonClicked) on_copySpeedButton_clicked();

        programName = "C:/SiasunRobot/program/" + ui->programNameList->currentItem()->text();
        openProgramHandler(programName);

        //clean the ancient recover files in SiasunRobot folder(old_X)
        progBackupPtr = 0;
        for(int i = 0; i < 10; i++){
            QFile file(fileSavePath + "\\old_" + QString::number(i));
            file.remove();
        }
        progBackupPtrMax = 0;


        //save the new program tree
        saveTheNewProgramTree();

        //change the current tree index to 0 (if no element selected, there is a bug can cause qTreeWidget crash)
        ui->programTree->setCurrentIndex(ui->programTree->model()->index(0, 0));

        programOpened = true;

        on_programButton_clicked(); //switch the stack page

        //show detail of the first command line
        //QModelIndex index;
        //on_programTree_clicked(index);
        QTreeWidgetItem *current = NULL;
        on_programTree_currentItemChanged(current, current);
    }
}

void MainWindow::on_programChooseCancel_clicked()
{
    if(selectDefaultProgram){
        selectDefaultProgram = false;
        ui->functionStackWindow->setCurrentIndex(4);
        return;
    }

    if(selectProgramToRun){
        selectProgramToRun = false;
        on_runButton_clicked();
    }
    else ui->functionStackWindow->setCurrentIndex(7);
}

void MainWindow::on_programNameList_itemDoubleClicked(QListWidgetItem *item)
{
    item;
    if(!ui->programChooseDelete->isVisible()) on_programChooseOK_clicked();
}

void MainWindow::on_programChooseDelete_clicked()
{
    //detect if the speed copy button is on, if so, turn it off
    if(copySpeedButtonClicked) on_copySpeedButton_clicked();
    QString programNameDelete;
    programNameDelete = "C:/SiasunRobot/program/" + ui->programNameList->currentItem()->text();

    QFile file(programNameDelete);


    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("确定要删除程序：" + programNameDelete.split('/').value(programNameDelete.split('/').size()-1) + "?");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Delete program:" + programNameDelete.split('/').value(programNameDelete.split('/').size()-1) + "?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo){
        file.remove();

        on_deleteProgramButton_clicked();
    }

}

void MainWindow::on_baseCoordinateBtn_clicked()
{
    adsWriteStruct.toolCoordinateJog = false;
    ui->move3DWidget->rob->setTool(adsWriteStruct.toolCoordinateJog, adsWriteStruct.toolCoordinate[0], adsWriteStruct.toolCoordinate[1],\
            adsWriteStruct.toolCoordinate[2], adsWriteStruct.toolCoordinateR[0], adsWriteStruct.toolCoordinateR[1], adsWriteStruct.toolCoordinateR[2]);

    adsWriteStruct.statusUseToolCoordinate = false;

    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/baseCoordinateSelect_2.png");
        QIcon ButtonIcon(pixmap);
        ui->baseCoordinateBtn->setIcon(ButtonIcon);
        ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/toolCoordinateSelect.png");
        ButtonIcon.addPixmap(pixmap);
        ui->toolCoordinateBtn->setIcon(ButtonIcon);
        ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
        ui->coordinateChooseLabel->setText("数据显示及编程使用 基座坐标系");
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/baseCoordinate2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->baseCoordinateBtn->setIcon(ButtonIcon);
        ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/toolCoordinate-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->toolCoordinateBtn->setIcon(ButtonIcon);
        ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
        ui->coordinateChooseLabel->setText("Parameters in base coordinate\nProgram in base coordinate");
    }

    //update robot position
    update3DView(adsReadStruct.axisCurrentAngle);

    //recreate virtual wall
    if(virtualWallSetted) on_setVirtualWall_clicked();
}

void MainWindow::on_toolCoordinateBtn_clicked()
{
    //Tool coordinate setting
    on_settingButton_clicked();
    ui->settingStack->setCurrentIndex(2);
    adsWriteStruct.toolCoordinateJog = true;


    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/baseCoordinateSelect.png");
        QIcon ButtonIcon(pixmap);
        ui->baseCoordinateBtn->setIcon(ButtonIcon);
        ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/toolCoordinateSelect_2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->toolCoordinateBtn->setIcon(ButtonIcon);
        ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
        ui->coordinateChooseLabel->setText("数据显示及编程使用 工具坐标系");
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/baseCoordinate-en.png");
        QIcon ButtonIcon(pixmap);
        ui->baseCoordinateBtn->setIcon(ButtonIcon);
        ui->baseCoordinateBtn->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/toolCoordinate2-en.png");
        ButtonIcon.addPixmap(pixmap);
        ui->toolCoordinateBtn->setIcon(ButtonIcon);
        ui->toolCoordinateBtn->setIconSize(pixmap.rect().size());
        ui->coordinateChooseLabel->setText("Parameters in tool coordinate\nProgram in tool coordinate");
    }
}

void MainWindow::on_pushButtonMoveToOrigin_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/back2zero_2.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButtonMoveToOrigin->setIcon(ButtonIcon);
        ui->pushButtonMoveToOrigin->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/back2zero_2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButtonMoveToOrigin->setIcon(ButtonIcon);
        ui->pushButtonMoveToOrigin->setIconSize(pixmap.rect().size());
    }

    //detect if a program is running
    if(!programIsRunning){
        //move to original point
        moveToOriginalPoint();
    }
}

void MainWindow::on_pushButtonMoveToOrigin_released()
{
    //detect if a program is running
    if(!programIsRunning){
        //stop current move
        on_stopJogMove1_clicked();
    }

    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/back2zero.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButtonMoveToOrigin->setIcon(ButtonIcon);
        ui->pushButtonMoveToOrigin->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/back2zero-en.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButtonMoveToOrigin->setIcon(ButtonIcon);
        ui->pushButtonMoveToOrigin->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_saveNodeButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Floppy Disk 2_2.png");
        QIcon ButtonIcon(pixmap);
        ui->saveNodeButton->setIcon(ButtonIcon);
        ui->saveNodeButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/savePosition2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->saveNodeButton->setIcon(ButtonIcon);
        ui->saveNodeButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_saveNodeButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Floppy Disk 2.png");
        QIcon ButtonIcon(pixmap);
        ui->saveNodeButton->setIcon(ButtonIcon);
        ui->saveNodeButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/savePosition-en.png");
        QIcon ButtonIcon(pixmap);
        ui->saveNodeButton->setIcon(ButtonIcon);
        ui->saveNodeButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_stopJogMove1_pressed()
{
    QPixmap pixmap(":/images/stop_move_2.jpg");
    QIcon ButtonIcon(pixmap);
    ui->stopJogMove1->setIcon(ButtonIcon);
    ui->stopJogMove1->setIconSize(pixmap.rect().size());
}

void MainWindow::on_stopJogMove1_released()
{
    QPixmap pixmap(":/images/stop_move.png");
    QIcon ButtonIcon(pixmap);
    ui->stopJogMove1->setIcon(ButtonIcon);
    ui->stopJogMove1->setIconSize(pixmap.rect().size());
}

void MainWindow::on_openButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Folder.png");
        QIcon ButtonIcon(pixmap);
        ui->openButton->setIcon(ButtonIcon);
        ui->openButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Folder.png");
        QIcon ButtonIcon(pixmap);
        ui->openButton->setIcon(ButtonIcon);
        ui->openButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_openButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/icon-openProg.png");
        QIcon ButtonIcon(pixmap);
        ui->openButton->setIcon(ButtonIcon);
        ui->openButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/icon-openProg.png");
        QIcon ButtonIcon(pixmap);
        ui->openButton->setIcon(ButtonIcon);
        ui->openButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_createButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Folder More_2.png");
        QIcon ButtonIcon(pixmap);
        ui->createButton->setIcon(ButtonIcon);
        ui->createButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Folder More.png");
        QIcon ButtonIcon(pixmap);
        ui->createButton->setIcon(ButtonIcon);
        ui->createButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_createButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Folder More.png");
        QIcon ButtonIcon(pixmap);
        ui->createButton->setIcon(ButtonIcon);
        ui->createButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Folder More_2.png");
        QIcon ButtonIcon(pixmap);
        ui->createButton->setIcon(ButtonIcon);
        ui->createButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programChooseCancel_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/goBack_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseCancel->setIcon(ButtonIcon);
        ui->programChooseCancel->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/goBack_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseCancel->setIcon(ButtonIcon);
        ui->programChooseCancel->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programChooseCancel_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/goBack.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseCancel->setIcon(ButtonIcon);
        ui->programChooseCancel->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/goBack.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseCancel->setIcon(ButtonIcon);
        ui->programChooseCancel->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programChooseOK_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseOK->setIcon(ButtonIcon);
        ui->programChooseOK->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseOK->setIcon(ButtonIcon);
        ui->programChooseOK->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programChooseOK_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseOK->setIcon(ButtonIcon);
        ui->programChooseOK->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->programChooseOK->setIcon(ButtonIcon);
        ui->programChooseOK->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programBackBtn_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/finishProg_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackBtn->setIcon(ButtonIcon);
        ui->programBackBtn->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/finishProg_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackBtn->setIcon(ButtonIcon);
        ui->programBackBtn->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programBackBtn_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/finishProg.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackBtn->setIcon(ButtonIcon);
        ui->programBackBtn->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/finishProg.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackBtn->setIcon(ButtonIcon);
        ui->programBackBtn->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_saveButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/save_2.png");
        QIcon ButtonIcon(pixmap);
        ui->saveButton->setIcon(ButtonIcon);
        ui->saveButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/save_2.png");
        QIcon ButtonIcon(pixmap);
        ui->saveButton->setIcon(ButtonIcon);
        ui->saveButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_saveButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/save.png");
        QIcon ButtonIcon(pixmap);
        ui->saveButton->setIcon(ButtonIcon);
        ui->saveButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/save.png");
        QIcon ButtonIcon(pixmap);
        ui->saveButton->setIcon(ButtonIcon);
        ui->saveButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_saveSuccessOK_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->saveSuccessOK->setIcon(ButtonIcon);
        ui->saveSuccessOK->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->saveSuccessOK->setIcon(ButtonIcon);
        ui->saveSuccessOK->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_saveSuccessOK_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->saveSuccessOK->setIcon(ButtonIcon);
        ui->saveSuccessOK->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->saveSuccessOK->setIcon(ButtonIcon);
        ui->saveSuccessOK->setIconSize(pixmap.rect().size());
    }

    ui->mainStackWindow->setCurrentIndex(2);

}

void MainWindow::on_deleteProgramButton_pressed()
{
    //QPixmap pixmap(":/images/Trash@2x.png");
    //QIcon ButtonIcon(pixmap);
    //ui->deleteProgramButton->setIcon(ButtonIcon);
    //ui->deleteProgramButton->setIconSize(pixmap.rect().size());
}

void MainWindow::on_deleteProgramButton_released()
{
    //QPixmap pixmap(":/images/deleteProgram.png");
    //QIcon ButtonIcon(pixmap);
    //ui->deleteProgramButton->setIcon(ButtonIcon);
    //ui->deleteProgramButton->setIconSize(pixmap.rect().size());
}

void MainWindow::on_editButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/icon-modifyFunc_2.png");
        QIcon ButtonIcon(pixmap);
        ui->editButton->setIcon(ButtonIcon);
        ui->editButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/icon-modifyFunc_2.png");
        QIcon ButtonIcon(pixmap);
        ui->editButton->setIcon(ButtonIcon);
        ui->editButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_editButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/icon-modifyFunc.png");
        QIcon ButtonIcon(pixmap);
        ui->editButton->setIcon(ButtonIcon);
        ui->editButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/icon-modifyFunc.png");
        QIcon ButtonIcon(pixmap);
        ui->editButton->setIcon(ButtonIcon);
        ui->editButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_moveButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Toggles 3_2.png");
        QIcon ButtonIcon(pixmap);
        ui->moveButton->setIcon(ButtonIcon);
        ui->moveButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Toggles 3_2.png");
        QIcon ButtonIcon(pixmap);
        ui->moveButton->setIcon(ButtonIcon);
        ui->moveButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_moveButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Toggles 3.png");
        QIcon ButtonIcon(pixmap);
        ui->moveButton->setIcon(ButtonIcon);
        ui->moveButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Toggles 3.png");
        QIcon ButtonIcon(pixmap);
        ui->moveButton->setIcon(ButtonIcon);
        ui->moveButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_moveToButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Paper Airplane_2.png");
        QIcon ButtonIcon(pixmap);
        ui->moveToButton->setIcon(ButtonIcon);
        ui->moveToButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Paper Airplane_2.png");
        QIcon ButtonIcon(pixmap);
        ui->moveToButton->setIcon(ButtonIcon);
        ui->moveToButton->setIconSize(pixmap.rect().size());
    }

    //detect if a program is running
    if(!programIsRunning){
        robotWindowChangePosition(2);
        ui->programStack->setCurrentIndex(3);

        moveToSelectedNode();
    }

}

void MainWindow::on_moveToButton_released()
{
    //stop current move
    on_stopJogMove1_clicked();
    ui->programStack->setCurrentIndex(1);


    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Paper Airplane.png");
        QIcon ButtonIcon(pixmap);
        ui->moveToButton->setIcon(ButtonIcon);
        ui->moveToButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Paper Airplane.png");
        QIcon ButtonIcon(pixmap);
        ui->moveToButton->setIcon(ButtonIcon);
        ui->moveToButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_transitionButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/transition_2.png");
        QIcon ButtonIcon(pixmap);
        ui->transitionButton->setIcon(ButtonIcon);
        ui->transitionButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/transition_2.png");
        QIcon ButtonIcon(pixmap);
        ui->transitionButton->setIcon(ButtonIcon);
        ui->transitionButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_transitionButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/transition.png");
        QIcon ButtonIcon(pixmap);
        ui->transitionButton->setIcon(ButtonIcon);
        ui->transitionButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/transition.png");
        QIcon ButtonIcon(pixmap);
        ui->transitionButton->setIcon(ButtonIcon);
        ui->transitionButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_addNodeButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Add_2.png");
        QIcon ButtonIcon(pixmap);
        ui->addNodeButton->setIcon(ButtonIcon);
        ui->addNodeButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Add_2.png");
        QIcon ButtonIcon(pixmap);
        ui->addNodeButton->setIcon(ButtonIcon);
        ui->addNodeButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_addNodeButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Add.png");
        QIcon ButtonIcon(pixmap);
        ui->addNodeButton->setIcon(ButtonIcon);
        ui->addNodeButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Add.png");
        QIcon ButtonIcon(pixmap);
        ui->addNodeButton->setIcon(ButtonIcon);
        ui->addNodeButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_suppressButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Graduation_2.png");
        QIcon ButtonIcon(pixmap);
        ui->suppressButton->setIcon(ButtonIcon);
        ui->suppressButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Graduation_2.png");
        QIcon ButtonIcon(pixmap);
        ui->suppressButton->setIcon(ButtonIcon);
        ui->suppressButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_suppressButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Graduation.png");
        QIcon ButtonIcon(pixmap);
        ui->suppressButton->setIcon(ButtonIcon);
        ui->suppressButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Graduation.png");
        QIcon ButtonIcon(pixmap);
        ui->suppressButton->setIcon(ButtonIcon);
        ui->suppressButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_deleteButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Trash_2.png");
        QIcon ButtonIcon(pixmap);
        ui->deleteButton->setIcon(ButtonIcon);
        ui->deleteButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Trash_2.png");
        QIcon ButtonIcon(pixmap);
        ui->deleteButton->setIcon(ButtonIcon);
        ui->deleteButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_deleteButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Trash.png");
        QIcon ButtonIcon(pixmap);
        ui->deleteButton->setIcon(ButtonIcon);
        ui->deleteButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Trash.png");
        QIcon ButtonIcon(pixmap);
        ui->deleteButton->setIcon(ButtonIcon);
        ui->deleteButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programBackward_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Curved Arrow Left_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackward->setIcon(ButtonIcon);
        ui->programBackward->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Curved Arrow Left_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackward->setIcon(ButtonIcon);
        ui->programBackward->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programBackward_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Curved Arrow Left.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackward->setIcon(ButtonIcon);
        ui->programBackward->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Curved Arrow Left.png");
        QIcon ButtonIcon(pixmap);
        ui->programBackward->setIcon(ButtonIcon);
        ui->programBackward->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programForward_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Curved Arrow Right_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programForward->setIcon(ButtonIcon);
        ui->programForward->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Curved Arrow Right_2.png");
        QIcon ButtonIcon(pixmap);
        ui->programForward->setIcon(ButtonIcon);
        ui->programForward->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_programForward_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Curved Arrow Right.png");
        QIcon ButtonIcon(pixmap);
        ui->programForward->setIcon(ButtonIcon);
        ui->programForward->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Curved Arrow Right.png");
        QIcon ButtonIcon(pixmap);
        ui->programForward->setIcon(ButtonIcon);
        ui->programForward->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_selectProgramButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/openProgram_2.png");
        QIcon ButtonIcon(pixmap);
        ui->selectProgramButton->setIcon(ButtonIcon);
        ui->selectProgramButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/openProgram2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->selectProgramButton->setIcon(ButtonIcon);
        ui->selectProgramButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_selectProgramButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/openProgram.png");
        QIcon ButtonIcon(pixmap);
        ui->selectProgramButton->setIcon(ButtonIcon);
        ui->selectProgramButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/openProgram-en.png");
        QIcon ButtonIcon(pixmap);
        ui->selectProgramButton->setIcon(ButtonIcon);
        ui->selectProgramButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_refreshProgram_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Refresh 2_2.png");
        QIcon ButtonIcon(pixmap);
        ui->refreshProgram->setIcon(ButtonIcon);
        ui->refreshProgram->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Refresh 2_2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->refreshProgram->setIcon(ButtonIcon);
        ui->refreshProgram->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_refreshProgram_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/Refresh 2.png");
        QIcon ButtonIcon(pixmap);
        ui->refreshProgram->setIcon(ButtonIcon);
        ui->refreshProgram->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/Refresh 2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->refreshProgram->setIcon(ButtonIcon);
        ui->refreshProgram->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_pushButton_clicked()
{
    //click repeat button
    repeatProgram = !repeatProgram;

    if(repeatProgram){
        if(systemLanguage == "cn"){
            QPixmap pixmap(":/images/repeat_2.png");
            QIcon ButtonIcon(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }else if(systemLanguage == "en"){
            QPixmap pixmap(":/img_en/img_en/repeat_2-en.png");
            QIcon ButtonIcon(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }


        //show number board
        numberBoard->setModal(true);
        numberBoard->cleanLineEdit();
        numberBoard->axisGroupVisible = false;
        numberBoard->positionGroupVisible = false;
        numberBoard->changeWindowSize();
        if(systemLanguage == "cn")
            numberBoard->setLabelText("输入重复次数\n直接点击确定为无限循环");
        else if(systemLanguage == "en")
            numberBoard->setLabelText("Enter repeat time\nJust click OK to repeat\ninfinitely");
        numberBoard->editLineNum = 13; //for run program repeat time
        //numberBoard->windowIsOpened = true;
        numberBoard->changeLanguage();
        numberBoard->exec();

    }else{
        if(systemLanguage == "cn"){
            QPixmap pixmap(":/images/repeat.png");
            QIcon ButtonIcon(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }else if(systemLanguage == "en"){
            QPixmap pixmap(":/img_en/img_en/repeat-en.png");
            QIcon ButtonIcon(pixmap);
            ui->pushButton->setIcon(ButtonIcon);
            ui->pushButton->setIconSize(pixmap.rect().size());
        }

        repeatProgram = false;
        runProgramRepeatTime = 0;
        ui->repeatTimeLabel->setText("");
    }
}

void MainWindow::on_languageSwitch_clicked()
{
    //change system language
    //chinese
    if(systemLanguage == "cn"){
        systemLanguage = "en";
        QPixmap pixmap(":/images/english.png");
        QIcon ButtonIcon(pixmap);
        ui->languageSwitch->setIcon(ButtonIcon);
        ui->languageSwitch->setIconSize(pixmap.rect().size());
    }
    else{
        systemLanguage = "cn";
        QPixmap pixmap(":/images/chinese.png");
        QIcon ButtonIcon(pixmap);
        ui->languageSwitch->setIcon(ButtonIcon);
        ui->languageSwitch->setIconSize(pixmap.rect().size());
    }

    setSystemLanguage(systemLanguage);

    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "LANGUAGE"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        text.replace(replaceTarget, "LANGUAGE	:   " + systemLanguage + "\n");
    }else{
        text.append("\nLANGUAGE	:   " + systemLanguage + "\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_toolCoordinate_pressed()
{
    //QPixmap pixmap(":/images/Wrench_2.png");
    //QIcon ButtonIcon(pixmap);
    //ui->toolCoordinate->setIcon(ButtonIcon);
    //ui->toolCoordinate->setIconSize(pixmap.rect().size());
}

void MainWindow::on_toolCoordinate_released()
{
    //QPixmap pixmap(":/images/Wrench.png");
    //QIcon ButtonIcon(pixmap);
    //ui->toolCoordinate->setIcon(ButtonIcon);
    //ui->toolCoordinate->setIconSize(pixmap.rect().size());
}

void MainWindow::on_resetButton_pressed()
{
    QPixmap pixmap(":/images/resetSystem_2.png");
    QIcon ButtonIcon(pixmap);
    ui->resetButton->setIcon(ButtonIcon);
    ui->resetButton->setIconSize(pixmap.rect().size());
}

void MainWindow::on_resetButton_released()
{
    QPixmap pixmap(":/images/resetSystem.png");
    QIcon ButtonIcon(pixmap);
    ui->resetButton->setIcon(ButtonIcon);
    ui->resetButton->setIconSize(pixmap.rect().size());
}


void MainWindow::on_startStopButton_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/icon-star_2.png");
        QIcon ButtonIcon(pixmap);
        ui->startStopButton->setIcon(ButtonIcon);
        ui->startStopButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/icon-star_2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->startStopButton->setIcon(ButtonIcon);
        ui->startStopButton->setIconSize(pixmap.rect().size());
    }
}

void MainWindow::on_startStopButton_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/icon-star.png");
        QIcon ButtonIcon(pixmap);
        ui->startStopButton->setIcon(ButtonIcon);
        ui->startStopButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/icon-star-en.png");
        QIcon ButtonIcon(pixmap);
        ui->startStopButton->setIcon(ButtonIcon);
        ui->startStopButton->setIconSize(pixmap.rect().size());
    }
}


void MainWindow::on_expertMode_pressed()
{
    QPixmap pixmap(":/images/expertMode_2.png");
    QIcon ButtonIcon(pixmap);
    ui->expertMode->setIcon(ButtonIcon);
    ui->expertMode->setIconSize(pixmap.rect().size());
}

void MainWindow::on_expertMode_released()
{
    QPixmap pixmap(":/images/expertMode.png");
    QIcon ButtonIcon(pixmap);
    ui->expertMode->setIcon(ButtonIcon);
    ui->expertMode->setIconSize(pixmap.rect().size());
}

void MainWindow::on_setInitPosition_clicked()
{
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("确定修改归零点?");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Be sure to change\n init point?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo){
        //set the "move to home position" position
        //read the init file
        bool find = false;
        QString replaceTarget;
        QFile file(fileSavePath + "\\config.init");;
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //set full screen
            if(keywordPieces.value(0).trimmed() == "INITPOINT"){

                replaceTarget = initCom;
                find = true;
            }
        }
        file.close();

        QByteArray fileData;
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        fileData = file.readAll();
        QString text(fileData);

        if(find){
            text.replace(replaceTarget, "INITPOINT	:	" + QString::number(adsReadStruct.axisCurrentAngle[0]) + ":" + \
                    QString::number(adsReadStruct.axisCurrentAngle[1]) + ":" + QString::number(adsReadStruct.axisCurrentAngle[2]) + ":" + \
                    QString::number(adsReadStruct.axisCurrentAngle[3]) + ":" + QString::number(adsReadStruct.axisCurrentAngle[4]) + ":" + \
                    QString::number(adsReadStruct.axisCurrentAngle[5]) + ":" + QString::number(adsReadStruct.axisCurrentAngle[6]) + "\n");

        }else{
            text.append("\nINITPOINT	:	" + QString::number(adsReadStruct.axisCurrentAngle[0]) + ":" + \
                    QString::number(adsReadStruct.axisCurrentAngle[1]) + ":" + QString::number(adsReadStruct.axisCurrentAngle[2]) + ":" + \
                    QString::number(adsReadStruct.axisCurrentAngle[3]) + ":" + QString::number(adsReadStruct.axisCurrentAngle[4]) + ":" + \
                    QString::number(adsReadStruct.axisCurrentAngle[5]) + ":" + QString::number(adsReadStruct.axisCurrentAngle[6]) + "\n");

        }

        file.seek(0); // go to the beginning of the file
        file.resize(0); //clear all the old text
        file.write(text.toUtf8()); // write the new text back to the file
        file.close(); // close the file handle.
    }
}

void MainWindow::on_blendRadiusValue_clicked()
{
    on_transitionButton_clicked();
}

void MainWindow::on_pushButton_2_clicked()
{
    //save the current robot status
    LogFile::currentStatusSave();
}


void MainWindow::on_clearInfo_clicked()
{
    ui->debugPrint->clear();
}



void MainWindow::on_ioOutput3_clicked()
{
    robot_ioOut[0] = !robot_ioOut[0];

    if(robot_ioOut[0]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput3->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput3->setIcon(ButtonIcon);
    }

}

void MainWindow::on_ioOutput4_clicked()
{
    robot_ioOut[1] = !robot_ioOut[1];

    if(robot_ioOut[1]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput4->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput4->setIcon(ButtonIcon);
    }
}

void MainWindow::on_ioOutput5_clicked()
{
    robot_ioOut[2] = !robot_ioOut[2];

    if(robot_ioOut[2]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput5->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput5->setIcon(ButtonIcon);
    }
}

void MainWindow::on_ioOutput6_clicked()
{
    robot_ioOut[3] = !robot_ioOut[3];

    if(robot_ioOut[3]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput6->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput6->setIcon(ButtonIcon);
    }
}

void MainWindow::on_ioOutput7_clicked()
{
    robot_ioOut[4] = !robot_ioOut[4];

    if(robot_ioOut[4]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput7->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput7->setIcon(ButtonIcon);
    }
}

void MainWindow::on_ioOutput8_clicked()
{
    robot_ioOut[5] = !robot_ioOut[5];

    if(robot_ioOut[5]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput8->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->ioOutput8->setIcon(ButtonIcon);
    }
}



void MainWindow::on_programTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    current;previous;
    QString keyWord;
    QStringList list;
    QString base;

    //if copy speed button clicked, paste the speed to the selected node
    if(copySpeedButtonClicked) updateProgramNumber(1, copiedSpeed);

    if(ui->programTree->currentItem() == NULL) return;

    //update speed and acceleration value on the UI
    if(ui->programTree->currentItem()->parent() == NULL || ui->programTree->currentItem()->parent()->text(0) == "MoveC"\
            || ui->programTree->currentItem()->parent()->text(0) == "#MoveC" || \
            ui->programTree->currentItem()->parent()->text(0) == "thread" || \
            ui->programTree->currentItem()->parent()->parent()->text(0) == "thread"){

        base = ui->programTree->currentItem()->text(0);
        keyWord = base.split( "(" ).value(0);
        list = base.split( "(" ).value(1).split( ")" ).value(0).split( "," );
    }


    //set move command parameters
    if(keyWord == "MoveL" || keyWord == "Start" || keyWord == "MoveJ" || keyWord == "MoveC_1" || keyWord == "MoveC_2" || keyWord == "MovePsi" || keyWord == "MoveL_Psi" || keyWord == "biaoding"){
        ui->progMoveParamX->setText(QString::number( list.value(0).toDouble(), 'f', 2 ));
        ui->progMoveParamY->setText(QString::number( list.value(1).toDouble(), 'f', 2 ));
        ui->progMoveParamZ->setText(QString::number( list.value(2).toDouble(), 'f', 2 ));
        ui->progMoveParamRX->setText(QString::number( list.value(3).toDouble(), 'f', 2 ));
        ui->progMoveParamRY->setText(QString::number( list.value(4).toDouble(), 'f', 2 ));
        ui->progMoveParamRZ->setText(QString::number( list.value(5).toDouble(), 'f', 2 ));
        ui->progMoveParamAxis1->setText(QString::number( list.value(8).toDouble(), 'f', 3 ));
        ui->progMoveParamAxis2->setText(QString::number( list.value(9).toDouble(), 'f', 3 ));
        ui->progMoveParamAxis3->setText(QString::number( list.value(10).toDouble(), 'f', 3 ));
        ui->progMoveParamAxis4->setText(QString::number( list.value(11).toDouble(), 'f', 3 ));
        ui->progMoveParamAxis5->setText(QString::number( list.value(12).toDouble(), 'f', 3 ));
        ui->progMoveParamAxis6->setText(QString::number( list.value(13).toDouble(), 'f', 3 ));
        ui->progMoveParamAxis7->setText(QString::number( list.value(14).toDouble(), 'f', 3 ));
        ui->programStack->setCurrentIndex(1);

        ui->editButton->setVisible(false);
        ui->moveButton->setVisible(true);
        ui->moveToButton->setVisible(true);
        if(keyWord == "MoveL" || keyWord == "MoveC_2" || keyWord == "MoveJ" || keyWord == "Start")
            ui->transitionButton->setVisible(true);
        else
            ui->transitionButton->setVisible(false);

        //init blend radius value
        ui->blendRadiusValue->setText("N/A");
    }

    ui->transitionButton->setEnabled(false);
    if(keyWord == "Start" || keyWord == "MoveJ" || keyWord == "MovePsi"){
        ui->transitionButton->setEnabled(true);
        ui->accelerationUnit->setText("       %");
        ui->setSpeedButton->setText(QString::number(list.value(6).toDouble()) + "%");

        if(list.count() == 16){
            if(list.value(15).toDouble() >= 0){
                //has blend radius
                ui->blendRadiusValue->setText(QString::number( list.value(15).toDouble(), 'f', 1 ));
            }
        }
    }


    if(keyWord == "MoveL" || keyWord == "MoveL_Psi" || keyWord == "biaoding"){
        ui->transitionButton->setEnabled(true);

        //ui->accelerationUnit->setText("mm/s" + square);

        ui->setSpeedButton->setText(QString::number(list.value(6).toDouble()) + "mm/s");

        ui->progMoveTypeGroupBox->setTitle(keyWord);
        if(systemLanguage == "cn"){
            ui->progMoveTypeDesc->setText("末端直线移动到此点");
        }else if(systemLanguage == "en"){
            ui->progMoveTypeDesc->setText("Move linearly to this point");
        }

        //blend radius
        ui->blendRadiusValue->setText("N/A");

        if(list.count() == 16){
            if(list.value(15).toDouble() >= 0){
                //has blend radius
                ui->blendRadiusValue->setText(QString::number( list.value(15).toDouble(), 'f', 1 ));
            }
        }


    }else if(keyWord == "MoveJ"){
        ui->progMoveTypeGroupBox->setTitle(keyWord);
        if(systemLanguage == "cn"){
            ui->progMoveTypeDesc->setText("末端以任意轨迹移动到此点");
        }else if(systemLanguage == "en"){
            ui->progMoveTypeDesc->setText("Move to this point in a random track");
        }
    }else if(keyWord == "Start"){
        ui->progMoveTypeGroupBox->setTitle(keyWord);
        if(systemLanguage == "cn"){
            ui->progMoveTypeDesc->setText("起始点，末端以任意轨迹移动到此点");
        }else if(systemLanguage == "en"){
            ui->progMoveTypeDesc->setText("The Start point of the program");
        }
    }else if(keyWord == "MoveC"){
        ui->blendRadiusValue->setText("");
        ui->transitionButton->setVisible(false);
        ui->progMoveTypeGroupBox->setTitle(keyWord);
        ui->programStack->setCurrentIndex(1);
        if(systemLanguage == "cn"){
            ui->progMoveTypeDesc->setText("弧线运动");
        }else if(systemLanguage == "en"){
            ui->progMoveTypeDesc->setText("Curve movement");
        }
        ui->progMoveParamX->setText("");
        ui->progMoveParamY->setText("");
        ui->progMoveParamZ->setText("");
        ui->progMoveParamRX->setText("");
        ui->progMoveParamRY->setText("");
        ui->progMoveParamRZ->setText("");
        ui->progMoveParamAxis1->setText("");
        ui->progMoveParamAxis2->setText("");
        ui->progMoveParamAxis3->setText("");
        ui->progMoveParamAxis4->setText("");
        ui->progMoveParamAxis5->setText("");
        ui->progMoveParamAxis6->setText("");
        ui->progMoveParamAxis7->setText("");
        ui->setSpeedButton->setText("");
    }else if(keyWord == "MoveC_1"){
        ui->progMoveTypeGroupBox->setTitle(keyWord);
        ui->setSpeedButton->setText(QString::number(list.value(6).toDouble()) + "mm/s");
        if(systemLanguage == "cn"){
            ui->progMoveTypeDesc->setText("弧线运动子节点一，圆弧轨迹中间点");
        }else if(systemLanguage == "en"){
            ui->progMoveTypeDesc->setText("Middle point of the curve movement");
        }
    }else if(keyWord == "MoveC_2"){
        ui->transitionButton->setEnabled(true);
        ui->progMoveTypeGroupBox->setTitle(keyWord);
        ui->setSpeedButton->setText(QString::number(list.value(6).toDouble()) + "mm/s");
        if(systemLanguage == "cn"){
            ui->progMoveTypeDesc->setText("弧线运动子节点二，圆弧轨迹结束点");
        }else if(systemLanguage == "en"){
            ui->progMoveTypeDesc->setText("End point of the curve movement");
        }

        if(list.count() == 16){
            if(list.value(15).toDouble() >= 0){
                //has blend radius
                ui->blendRadiusValue->setText(QString::number( list.value(15).toDouble(), 'f', 1 ));
            }
        }
    }else if(keyWord == "Sleep"){
        ui->programStack->setCurrentIndex(2);
        //ui->progFunctionGroupBox->setTitle(keyWord);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("Sleep(time)\n程序暂停一段时间\ntime以毫秒为单位");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("Sleep(time)\nex: Sleep(5000)\ntime is in ms");
        }

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }else if(keyWord == "socket_open"){
        ui->programStack->setCurrentIndex(2);
        //ui->progFunctionGroupBox->setTitle(keyWord);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("socket_open(address, port, timeout)\n创建socket连接\n例：\nsocket_open(192.168.1.10, 6000, 3000)");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("socket_open(address, port, timeout)\nex: \nsocket_open(192.168.1.10, 6000, 3000)\ntimeout is in ms (maximum 3000ms)\nif not set, timeout will be 1000ms by default. ");
        }

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }else if(keyWord == "socket_send"){
        ui->programStack->setCurrentIndex(2);
        //ui->progFunctionGroupBox->setTitle(keyWord);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("socket_send(message)\n发送socket信息\n例：\nsocket_send(this is a test message)");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("socket_send(message)\nex: \nsocket_send(this is a test message)");
        }

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }else if(keyWord == "socket_rcv"){
        ui->programStack->setCurrentIndex(2);
        //ui->progFunctionGroupBox->setTitle(keyWord);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("socket_rcv(timeout)\n接收socket信息，\n例：socket_rcv(3000)\ntimeout为最大等待时间\n若不设定则默认为3000毫秒");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("socket_rcv(timeout)\nif timeout not set, \ntimeout will be 3000ms by default.");
        }

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }else if(keyWord == "socket_close"){
        ui->programStack->setCurrentIndex(2);
        //ui->progFunctionGroupBox->setTitle(keyWord);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("socket_close()\n关闭socket");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("socket_close()\nClose the socket");
        }

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }else if(keyWord == "set_tcp"){
        ui->programStack->setCurrentIndex(2);
        //ui->progFunctionGroupBox->setTitle(keyWord);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("set_tcp(x,y,z,rx,ry,rz)\n设置程序中使用的工具坐标系\nex:set_tcp(0,0,170.3,0,0,0)");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("set_tcp(x,y,z,rx,ry,rz)\nset the tool coordinate used in program\nex:set_tcp(0,0,170.3,0,0,0)");
        }

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }else if(keyWord == "save_status"){
        ui->programStack->setCurrentIndex(2);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("save_status()\n保存当前状态信息到日志文件中");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("save_status()\nSave the current position\nand all axis angles into a file");
        }

    }else if(keyWord == "thread"){
        ui->programStack->setCurrentIndex(2);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("");
        }
    }else if(keyWord == "if"){
        ui->programStack->setCurrentIndex(2);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("");
        }
    }else if(keyWord == "stop_program"){
        ui->programStack->setCurrentIndex(2);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("stop_program()\n停止当前程序");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("stop_program()\nstop current program");
        }
    }else if(keyWord == "run_program"){
        ui->programStack->setCurrentIndex(2);
        ui->progFunctionShow->setText(base);
        if(systemLanguage == "cn"){
            ui->progFunctionDesc->setText("run_program(name)\n运行指定程序，name是程序名");
        }else if(systemLanguage == "en"){
            ui->progFunctionDesc->setText("run_program(name)\nrun target program\nname is the target program's name");
        }

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }
    else{
        ui->programStack->setCurrentIndex(5);
        ui->scriptLabel->setText(base);
        ui->scriptLabel->setAlignment(Qt::AlignCenter);

        ui->editButton->setVisible(true);
        ui->moveButton->setVisible(false);
        ui->moveToButton->setVisible(false);
        ui->transitionButton->setVisible(false);
    }

}

void MainWindow::on_moveToStartPoint_pressed()
{
    adsWriteStruct.progStop = false;

    //Start moveto thread
    moveToThread->start();
}

void MainWindow::on_moveToStartPoint_released()
{
    on_stopJogMove1_clicked();
}

void MainWindow::on_back2Program_clicked()
{
    ui->stackedWidgetRunProg->setCurrentIndex(0);
}

void MainWindow::on_importProgram_clicked()
{
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("选择外部设备中的程序，导入系统中");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Select program from external devices\nImport to system");
    }
    WarningWindow::getInstance()->exec();

    QString filePath = QFileDialog::getOpenFileName(this, tr("导入文件"), "/", tr("SPF Files (*.spf)"));

    if(filePath == "") return;

    if(QFile::copy(filePath, "C:/SiasunRobot/program/" + filePath.split('/').value(filePath.split('/').count()-1))){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("导入成功");
        }else{
            WarningWindow::getInstance()->setWarningText("Import success");
        }
    }else{
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("导入失败，已存在同名程序");
        }else{
            WarningWindow::getInstance()->setWarningText("Import fail\nAlready have a program with same name");
        }
    }
    WarningWindow::getInstance()->exec();

    if(!QDir(fileSavePath + "\\program").exists()) QDir().mkdir(fileSavePath + "\\program");
    QDir myDir(fileSavePath + "\\program");
    QStringList allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    ui->programNameList->clear();

    if(allFiles.count() == 0) return;

    for(int i = 0; i < allFiles.count(); i++){
        ui->programNameList->addItem(allFiles.at(i));
    }

    ui->programNameList->setCurrentRow(0);//delete will cause null pointer bug
    on_programNameList_itemClicked(ui->programNameList->currentItem());
}

void MainWindow::on_setJoint0Points_clicked()
{
    startReadXML = true;
    adsWriteStruct.joint0ValueRead = true;
    QThread::msleep(1000);
    adsWriteStruct.joint0ValueRead = false;


    //read XML
    //char xmlWritePath [] = "GVL.xml_write";
    //
    //ADS::getAdsInstance()->adsRead(xmlWritePath, sizeof(xmlWritePath), sizeof(xmlReadStruct), &xmlReadStruct); //read from PLC
    //check read error
    //while(xmlReadStruct.axisBias[0] == 0 || xmlReadStruct.axisBias[1] == 0 || \
    xmlReadStruct.axisBias[2] == 0 || xmlReadStruct.axisBias[3] == 0 || \
            xmlReadStruct.axisBias[4] == 0 || xmlReadStruct.axisBias[5] == 0 || \
            xmlReadStruct.axisBias[6] == 0)
            //while(ADS::getAdsInstance()->adsRead(xmlWritePath, sizeof(xmlWritePath), sizeof(xmlReadStruct), &xmlReadStruct) != 0);

            QDomDocument document;
    QDomElement root = document.createElement("AxisBias");
    document.appendChild(root);

    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("axis");
        node.setAttribute("bias", xmlReadStruct.axisBias[i]);
        node.setAttribute("index", i+1);
        root.appendChild(node);
    }

    QFile file ("C:\\SiasunRobot\\xml\\POS_BIAS.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream stream(&file);
    stream << document.toString();
    file.close();

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("关节零位偏移设置完成");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Axis bias set success");
    }
    WarningWindow::getInstance()->exec();

    startReadXML = false;
}

void MainWindow::on_digitalIO_clicked()
{
    ui->statusStackedWidget->setCurrentIndex(2);
}

void MainWindow::on_ipConfig_clicked()
{
    ui->settingStack->setCurrentIndex(4);
    if(ipDHCPMode) on_dhcpIP_clicked();
    else on_staticIP_clicked();
    showSystemIP();
}


void MainWindow::on_dhcpIP_clicked()
{
    QPixmap pixmap(":/images/node_ok.png");
    QIcon ButtonIcon(pixmap);
    ui->dhcpIP->setIcon(ButtonIcon);

    QPixmap pixmap2(":/images/node_warning.png");
    QIcon ButtonIcon2(pixmap2);
    ui->staticIP->setIcon(ButtonIcon2);

    ui->ipSettingGroupBox->setVisible(false);
}

void MainWindow::on_staticIP_clicked()
{
    QPixmap pixmap(":/images/node_ok.png");
    QIcon ButtonIcon(pixmap);
    ui->staticIP->setIcon(ButtonIcon);

    QPixmap pixmap2(":/images/node_warning.png");
    QIcon ButtonIcon2(pixmap2);
    ui->dhcpIP->setIcon(ButtonIcon2);

    ui->ipSettingGroupBox->setVisible(true);
}

void MainWindow::on_ipAddressLineEdit_selectionChanged()
{
    //show keyboard to enter the function
    keyboard->setModal(true);
    keyboard->expertModeSwitch(expertMode);
    keyboard->setText("192.168.");
    keyboard->keyboardType = 4;
    keyboard->exec();
}

void MainWindow::on_netmaskLineEdit_selectionChanged()
{
    //show keyboard to enter the function
    keyboard->setModal(true);
    keyboard->expertModeSwitch(expertMode);
    keyboard->setText("255.255.255.0");
    keyboard->keyboardType = 5;
    keyboard->exec();
}

void MainWindow::on_gatewayLineEdit_selectionChanged()
{
    //show keyboard to enter the function
    keyboard->setModal(true);
    keyboard->expertModeSwitch(expertMode);
    keyboard->setText("192.168.");
    keyboard->keyboardType = 6;
    keyboard->exec();
}

void MainWindow::on_ipSettingOK_clicked()
{
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("确定修改IP地址?");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Be sure to change IP address?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo){
        if(ui->ipSettingGroupBox->isVisible()){
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("修改IP\n可能会花费几分钟\n请耐心等待窗口自动关闭");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("This may take several minuts");
            }
            WarningWindow::getInstance()->exec();

            //static address
            QString command = "netsh interface ipv4 set address name=\"local\" static " + ui->ipAddressLineEdit->text() + " " \
                    + ui->netmaskLineEdit->text() + " " + ui->gatewayLineEdit->text();
            QByteArray ba = command.toLatin1();
            const char *c_str = ba.data();
            //qDebug() << WinExec(c_str, SW_HIDE);
            system(c_str);
            ipDHCPMode = false;
        }else{
            //qDebug() << WinExec("netsh interface ipv4 set address name=\"local\" source=dhcp", SW_HIDE);
            //QThread::msleep(1500);
            system("netsh interface ipv4 set address name=\"local\" source=dhcp");
            ipDHCPMode = true;
        }

        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("IP地址已修改");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("IP address changed");
        }
        WarningWindow::getInstance()->exec();

        showSystemIP();
    }
}

void MainWindow::showSystemIP()
{
    //WinExec("ipconfig > C:/SiasunRobot/ip.txt", SW_HIDE);
    //QThread::msleep(1500);
    system("ipconfig > C:/SiasunRobot/ip");
    bool find = false;
    int i = 0;
    QFile file("C:/SiasunRobot/ip");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    while(!file.atEnd()){
        QString line = file.readLine();
        if(find) i++;
        if(line.contains("local")){find = true; }
        if(i == 4){
            QString ip = line.split(':').value(1).split("\n").value(0);
            if(ip == ""){
                if(systemLanguage == "cn"){
                    ip = "网线已拔出";
                }else if(systemLanguage == "en"){
                    ip = "Ethernet cable disconnect";
                }
            }
            ui->currentIPAddress->setText(ip);
            return;
        }
    }
    return;
}

void MainWindow::checkRepeatProgram()
{
    //check if repeat the program infinitely
    if(repeatProgram){
        //run the current program again
        QThread::msleep(200); //wait for thread initial, otherwise the program will skip the first command

        on_runProgramButton_run_clicked();
    }

    //check if need to repeat the program for several time
    if(runProgramRepeatTime > 1){
        //run the current program again
        QThread::msleep(200); //wait for thread initial, otherwise the program will skip the first command

        on_runProgramButton_run_clicked();
        runProgramRepeatTime--;
        ui->repeatTimeLabel->setText(QString::number(runProgramRepeatTime));

        if(runProgramRepeatTime == 1){
            if(systemLanguage == "cn"){
                QPixmap pixmap(":/images/repeat.png");
                QIcon ButtonIcon(pixmap);
                ui->pushButton->setIcon(ButtonIcon);
                ui->pushButton->setIconSize(pixmap.rect().size());
            }else if(systemLanguage == "en"){
                QPixmap pixmap(":/img_en/img_en/repeat-en.png");
                QIcon ButtonIcon(pixmap);
                ui->pushButton->setIcon(ButtonIcon);
                ui->pushButton->setIconSize(pixmap.rect().size());
            }

            repeatProgram = false;
            runProgramRepeatTime = 0;
            ui->repeatTimeLabel->setText("");
        }
    }



    QThread::msleep(200);//wait for other parameters setup
}

void MainWindow::showProgramRunTime(int time)
{
    int x = time / 1000;
    int ms = time % 1000;
    int seconds = x % 60;
    int minutes = x / 60;
    QString runTime;
    if(minutes == 0)
        runTime = QString::number(seconds) + "." + QString::number(ms) + "s";
    else
        runTime = QString::number(minutes) + "m" + QString::number(seconds) + "." + QString::number(ms) + "s";

    ui->runTimeLabel->setText(runTime);
}

void MainWindow::stopScript()
{
    on_stopProgramButton_run_clicked();
}

void MainWindow::pauseScript()
{
    on_pauseProgramButton_run_clicked();
}

void MainWindow::resumeScript()
{
    on_runProgramButton_run_clicked();
}

void MainWindow::readXMLFile()
{
    //joint initial position bias
    QDomDocument document;
    QFile file("C:\\SiasunRobot\\xml\\POS_BIAS.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("POS_BIAS参数文件加载失败");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Parameter config fail");
        }
        WarningWindow::getInstance()->exec();
        return;
    }

    if(!document.setContent(&file))return;
    file.close();

    //get the root element
    QDomElement root = document.firstChildElement();

    QDomNodeList items = root.childNodes();

    for(int i = 0; i < 7; i++){
        QDomElement node = items.at(i).toElement();
        xmlWriteStruct.axisBias[i] = node.attribute("bias").toDouble();
    }




    //install orientation
    file.setFileName("C:\\SiasunRobot\\xml\\INSTALL.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("INSTALL参数文件加载失败");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Parameter config fail");
        }
        WarningWindow::getInstance()->exec();
        return;
    }

    if(!document.setContent(&file))return;
    file.close();

    //get the root element
    root = document.firstChildElement();

    items = root.childNodes();


    QDomElement node = items.at(0).toElement();
    xmlWriteStruct.theta_G = node.attribute("rx").toDouble();
    node = items.at(1).toElement();
    xmlWriteStruct.alpha_G = node.attribute("ry").toDouble();

    if(xmlWriteStruct.theta_G == 90 && xmlWriteStruct.alpha_G == 0){
        ui->acrossInstall->setChecked(true);
        ui->verticalInstall->setChecked(false);
        ui->anywayInstall->setChecked(false);
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("机械臂安装方向为：竖装\n末端负载 " + QString::number(adsWriteStruct.tcpLoad) + " kg");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Robot install: Across\nTCP load " + QString::number(adsWriteStruct.tcpLoad) + " kg");
        }
        if(!autoLoginActive) WarningWindow::getInstance()->exec();
    }
    else if(xmlWriteStruct.theta_G == 0 && xmlWriteStruct.alpha_G == 0){
        ui->acrossInstall->setChecked(false);
        ui->verticalInstall->setChecked(true);
        ui->anywayInstall->setChecked(false);
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("机械臂安装方向为：横装\n末端负载 " + QString::number(adsWriteStruct.tcpLoad) + " kg");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Robot install: Vertical\nTCP load " + QString::number(adsWriteStruct.tcpLoad) + " kg");
        }
        if(!autoLoginActive) WarningWindow::getInstance()->exec();
    }
    else{
        ui->acrossInstall->setChecked(false);
        ui->verticalInstall->setChecked(false);
        ui->anywayInstall->setChecked(true);
        ui->installRx->setText(QString::number(xmlWriteStruct.theta_G));
        ui->installRy->setText(QString::number(xmlWriteStruct.alpha_G));
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("机械臂安装方向为：Rx:" + QString::number(xmlWriteStruct.theta_G) + " Ry:" + QString::number(xmlWriteStruct.alpha_G) + "\n末端负载 " + QString::number(adsWriteStruct.tcpLoad) + " kg");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Robot install: Rx:" + QString::number(xmlWriteStruct.theta_G) + " Ry:" + QString::number(xmlWriteStruct.alpha_G) + "\nTCP load " + QString::number(adsWriteStruct.tcpLoad) + " kg");
        }
        WarningWindow::getInstance()->exec();
    }


    //read DH
    file.setFileName("C:\\SiasunRobot\\xml\\DH.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("DH参数文件加载失败");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Parameter config fail");
        }
        WarningWindow::getInstance()->exec();

        //default value
        xmlWriteStruct.DH[0] = 0.3117;
        xmlWriteStruct.DH[1] = 0.4;
        xmlWriteStruct.DH[2] = 0.4;
        xmlWriteStruct.DH[3] = 0.1753;
        return;
    }

    if(!document.setContent(&file))return;
    file.close();

    //get the root element
    root = document.firstChildElement();

    items = root.childNodes();

    node = items.at(0).toElement();
    xmlWriteStruct.DH[0] = node.attribute("Dbs").toDouble();
    node = items.at(1).toElement();
    xmlWriteStruct.DH[1] = node.attribute("Dse").toDouble();
    node = items.at(2).toElement();
    xmlWriteStruct.DH[2] = node.attribute("Dew").toDouble();
    node = items.at(3).toElement();
    xmlWriteStruct.DH[3] = node.attribute("Dwt").toDouble();


    qDebug() << "***DH" << xmlWriteStruct.DH[0] << xmlWriteStruct.DH[1] << xmlWriteStruct.DH[2] << xmlWriteStruct.DH[3];


    //force control dynamic parameters
    file.setFileName("C:\\SiasunRobot\\xml\\DYNAMIC.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("DYNAMIC参数文件加载失败");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Parameter config fail");
        }
        WarningWindow::getInstance()->exec();
        return;
    }

    if(!document.setContent(&file))return;
    file.close();

    //get the root element
    root = document.firstChildElement();

    items = root.childNodes();


    //Kt
    for(int i = 0; i < 7; i++){
        node= items.at(i).toElement();
        xmlWriteStruct.Kt[i] = node.attribute("Kt" + QString::number(i+1)).toDouble();
    }


    //FC
    for(int i = 0; i < 7; i++){
        node= items.at(i+7).toElement();
        xmlWriteStruct.FC[i] = node.attribute("FC" + QString::number(i+1)).toDouble();
    }


    //FV
    for(int i = 0; i < 7; i++){
        node= items.at(i+14).toElement();
        xmlWriteStruct.FV[i] = node.attribute("FV" + QString::number(i+1)).toDouble();
    }


    //FVV
    for(int i = 0; i < 7; i++){
        node= items.at(i+21).toElement();
        xmlWriteStruct.FVV[i] = node.attribute("FVV" + QString::number(i+1)).toDouble();
    }


    //FVVV
    for(int i = 0; i < 7; i++){
        node= items.at(i+28).toElement();
        xmlWriteStruct.FVVV[i] = node.attribute("FVVV" + QString::number(i+1)).toDouble();
    }


    //TFv
    for(int i = 0; i < 7; i++){
        node= items.at(i+35).toElement();
        xmlWriteStruct.TFv[i] = node.attribute("TFv" + QString::number(i+1)).toDouble();
    }


    //TFc
    for(int i = 0; i < 7; i++){
        node= items.at(i+42).toElement();
        xmlWriteStruct.TFc[i] = node.attribute("TFc" + QString::number(i+1)).toDouble();
    }


    //Th
    for(int i = 0; i < 7; i++){
        node= items.at(i+49).toElement();
        xmlWriteStruct.Th[i] = node.attribute("Th" + QString::number(i+1)).toDouble();
    }





    //axis limit parameters
    file.setFileName("C:\\SiasunRobot\\xml\\LIMITS.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("关节限制参数文件加载失败");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Parameter config fail");
        }
        WarningWindow::getInstance()->exec();
        return;
    }

    if(!document.setContent(&file))return;
    file.close();

    //get the root element
    root = document.firstChildElement();

    items = root.childNodes();


    //pos
    for(int i = 0; i < 7; i++){
        node= items.at(i).toElement();
        xmlWriteStruct.limits_Pos[i] = node.attribute("PosJ" + QString::number(i+1)).toDouble();
    }


    //speed
    for(int i = 0; i < 7; i++){
        node= items.at(i+7).toElement();
        xmlWriteStruct.limits_velo[i] = node.attribute("SpeedJ" + QString::number(i+1)).toDouble();
    }


    //acc
    for(int i = 0; i < 7; i++){
        node= items.at(i+14).toElement();
        xmlWriteStruct.limits_acc[i] = node.attribute("AccJ" + QString::number(i+1)).toDouble();
    }


    //write XML
    char xmlWritePath [] = "GVL.xml_read_data";
    ADS::getAdsInstance()->adsWrite(xmlWritePath, sizeof(xmlWritePath), sizeof(xmlWriteStruct), &xmlWriteStruct); //read from PLC


    //save paremeters to log
    LogFile::logFileCreate("read init parameters", "axis bias: " + QString::number(xmlWriteStruct.axisBias[0])+" "+\
            QString::number(xmlWriteStruct.axisBias[1])+" "+QString::number(xmlWriteStruct.axisBias[2])+" "+\
            QString::number(xmlWriteStruct.axisBias[3])+" "+QString::number(xmlWriteStruct.axisBias[4])+" "+\
            QString::number(xmlWriteStruct.axisBias[5])+" "+QString::number(xmlWriteStruct.axisBias[6]));
    LogFile::logFileCreate("read init parameters", "DH: " + QString::number(xmlWriteStruct.DH[0])+" "+\
            QString::number(xmlWriteStruct.DH[1])+" "+QString::number(xmlWriteStruct.DH[2])+" "+\
            QString::number(xmlWriteStruct.DH[3]));
    LogFile::logFileCreate("read init parameters", "install: " + QString::number(xmlWriteStruct.theta_G)+" "+\
                           QString::number(xmlWriteStruct.alpha_G));
    LogFile::logFileCreate("read init parameters", "limits_velo: " + QString::number(xmlWriteStruct.limits_velo[0])+" "+\
            QString::number(xmlWriteStruct.limits_velo[1])+" "+QString::number(xmlWriteStruct.limits_velo[2])+" "+\
            QString::number(xmlWriteStruct.limits_velo[3])+" "+QString::number(xmlWriteStruct.limits_velo[4])+" "+\
            QString::number(xmlWriteStruct.limits_velo[5])+" "+QString::number(xmlWriteStruct.limits_velo[6]));
    LogFile::logFileCreate("read init parameters", "limits_acc: " + QString::number(xmlWriteStruct.limits_acc[0])+" "+\
            QString::number(xmlWriteStruct.limits_acc[1])+" "+QString::number(xmlWriteStruct.limits_acc[2])+" "+\
            QString::number(xmlWriteStruct.limits_acc[3])+" "+QString::number(xmlWriteStruct.limits_acc[4])+" "+\
            QString::number(xmlWriteStruct.limits_acc[5])+" "+QString::number(xmlWriteStruct.limits_acc[6]));
    LogFile::logFileCreate("read init parameters", "limits_Pos: " + QString::number(xmlWriteStruct.limits_Pos[0])+" "+\
            QString::number(xmlWriteStruct.limits_Pos[1])+" "+QString::number(xmlWriteStruct.limits_Pos[2])+" "+\
            QString::number(xmlWriteStruct.limits_Pos[3])+" "+QString::number(xmlWriteStruct.limits_Pos[4])+" "+\
            QString::number(xmlWriteStruct.limits_Pos[5])+" "+QString::number(xmlWriteStruct.limits_Pos[6]));

}

void MainWindow::stopRepeat()
{
    //uncheck the repeat button
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/repeat.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/repeat-en.png");
        QIcon ButtonIcon(pixmap);
        ui->pushButton->setIcon(ButtonIcon);
        ui->pushButton->setIconSize(pixmap.rect().size());
    }

    repeatProgram = false;
    runProgramRepeatTime = 0;
    ui->repeatTimeLabel->setText("");
}

void MainWindow::updateSlaveStatus(bool *status)
{
    QPixmap OK(":/images/node_ok.png");
    QPixmap Warning(":/images/node_warning.png");
    QIcon pixmapOK(OK);
    QIcon pixmapWarning(Warning);

    if(status[0]) ui->jointActiveState1->setIcon(pixmapOK);
    else ui->jointActiveState1->setIcon(pixmapWarning);
    if(status[1]) ui->jointActiveState2->setIcon(pixmapOK);
    else ui->jointActiveState2->setIcon(pixmapWarning);
    if(status[2]) ui->jointActiveState3->setIcon(pixmapOK);
    else ui->jointActiveState3->setIcon(pixmapWarning);
    if(status[3]) ui->jointActiveState4->setIcon(pixmapOK);
    else ui->jointActiveState4->setIcon(pixmapWarning);
    if(status[4]) ui->jointActiveState5->setIcon(pixmapOK);
    else ui->jointActiveState5->setIcon(pixmapWarning);
    if(status[5]) ui->jointActiveState6->setIcon(pixmapOK);
    else ui->jointActiveState6->setIcon(pixmapWarning);
    if(status[6]) ui->jointActiveState7->setIcon(pixmapOK);
    else ui->jointActiveState7->setIcon(pixmapWarning);
}

void MainWindow::tcpConnected(bool port2000, bool port2001)
{
    if(port2000){
        ui->tcpConnectStatus->setVisible(true);
        if(port2001){
            if(systemLanguage == "cn"){
                ui->tcpConnectStatus->setText("2000 2001端口已连接");
            }else if(systemLanguage == "en"){
                ui->tcpConnectStatus->setText("2000 2001 port connected");
            }
        }else{
            if(systemLanguage == "cn"){
                ui->tcpConnectStatus->setText("2000 端口已连接");
            }else if(systemLanguage == "en"){
                ui->tcpConnectStatus->setText("2000 port connected");
            }
        }
    }else{
        if(port2001){
            ui->tcpConnectStatus->setVisible(true);
            if(systemLanguage == "cn"){
                ui->tcpConnectStatus->setText("2001 端口已连接");
            }else if(systemLanguage == "en"){
                ui->tcpConnectStatus->setText("2001 port connected");
            }
        }else{
            ui->tcpConnectStatus->setVisible(false);
            if(systemLanguage == "cn"){
                ui->tcpConnectStatus->setText("无外部连接");
            }else if(systemLanguage == "en"){
                ui->tcpConnectStatus->setText("No connection");
            }
        }
    }
}


void MainWindow::on_defaultProgram_clicked()
{
    ui->settingStack->setCurrentIndex(5);
}

void MainWindow::on_loadDefaultProgram_clicked()
{
    selectDefaultProgram = true;

    if(!QDir(fileSavePath + "\\program").exists()) QDir().mkdir(fileSavePath + "\\program");

    QDir myDir(fileSavePath + "\\program");
    QStringList allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    ui->programNameList->clear();

    if(allFiles.count() == 0) {on_createButton_clicked(); return;}

    for(int i = 0; i < allFiles.count(); i++){
        ui->programNameList->addItem(allFiles.at(i));
    }

    ui->programChooseOK->setVisible(true);
    ui->programChooseDelete->setVisible(false);
    ui->importProgram->setVisible(true);
    ui->functionStackWindow->setCurrentIndex(8);

    ui->programNameList->setCurrentRow(0);//delete will cause null pointer problem
    on_programNameList_itemClicked(ui->programNameList->currentItem());
}

void MainWindow::on_saveDefaultProgram_clicked()
{
    QString defaultProgramName = ui->defaultProgramName->text();
    if(defaultProgramName.split('.').value(1) != "spf") return;


    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        if(keywordPieces.value(0).trimmed() == "RUNAUTO"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        text.replace(replaceTarget, "RUNAUTO	:	on  :   " + defaultProgramName + ":" + \
                     QString::number(ui->digitalInCombo->currentIndex()+1) + ":" +\
                     QString::number(ui->digitalStatusCombo->currentIndex()) + "\n");
    }else{
        text.append("\nRUNAUTO	:	on  :   " + defaultProgramName + ":" + \
                    QString::number(ui->digitalInCombo->currentIndex()+1) + ":" +\
                    QString::number(ui->digitalStatusCombo->currentIndex()) + "\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("默认程序已设定");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Default program enabled");
    }
    WarningWindow::getInstance()->exec();

    QPixmap pixmap(":/images/node_ok.png");
    QIcon ButtonIcon(pixmap);
    ui->defaultProgram->setIcon(ButtonIcon);
}

void MainWindow::on_cancelDefaultProgram_clicked()
{
    QString defaultProgramName = ui->defaultProgramName->text();
    if(defaultProgramName.split('.').value(1) != "spf") return;

    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        if(keywordPieces.value(0).trimmed() == "RUNAUTO"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        text.replace(replaceTarget, "RUNAUTO	:	off\n");
    }else{
        text.append("\nRUNAUTO	:	off\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("默认程序已取消");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Default program disabled");
    }
    WarningWindow::getInstance()->exec();

    ui->defaultProgramName->setText("未设定默认程序");

    QPixmap pixmap(":/images/node_warning.png");
    QIcon ButtonIcon(pixmap);
    ui->defaultProgram->setIcon(ButtonIcon);
}

void MainWindow::on_cameraSetting_clicked()
{
    inCameraProgram = true;

    QString program = "C:/SiasunRobot/opencv/ISDK_Display_Control.exe";
    myProcess = new QProcess();

    myProcess->start(program);

    HWND www = NULL;

    while(www == NULL) www = FindWindow(L"WindowsForms10.Window.8.app.0.378734a",L"Cognex In-Sight Display Demo - Not Connected");

    WId wid = (WId)FindWindow(L"WindowsForms10.Window.8.app.0.378734a",L"Cognex In-Sight Display Demo - Not Connected");

    m_window = QWindow::fromWinId(wid);
    m_widget = QWidget::createWindowContainer(m_window,ui->widget4Camera,Qt::Widget);
    m_widget->setGeometry(0, 0, ui->widget4Camera->width(), ui->widget4Camera->height());

    cal->ReadInParametersFromFile();

    ui->functionStackWindow->setCurrentIndex(9);
}

void MainWindow::on_pushButton_exit_clicked()
{
    ui->functionStackWindow->setCurrentIndex(7);
    myProcess->close();
    inCameraProgram = false;
}

void MainWindow::on_callOsk_clicked()
{
    WinExec("osk", SW_HIDE);
}

char IP_address[] = "169.254.253.12";
void MainWindow::on_commandLinkButton_manualTrigger_clicked()
{
    QTcpSocket *client;
    QByteArray buffer_rec;

    client = new QTcpSocket(this);
    client->connectToHost(QHostAddress(cal->Camera_IP),60003);
    client->waitForConnected(1000);
    int connectedState = client->state();
    QString str;
    if(connectedState == 3){
        qDebug() << cal->Camera_IP << " connected";
        str = cal->Camera_IP;
        str += " connected.";
        ui->label_statusText->setText(str);
        client->write("[go0]");

        int time_counter = 0;

        //client->waitForBytesWritten(3000);
        while((client->bytesAvailable() < 164) && (time_counter++ < 30)){
            //qDebug() << "time_counter:" << time_counter;
            client->waitForReadyRead(100);
            //qDebug() << "client_available:" << client->bytesAvailable();
        }
        qDebug() << "msg size:" << client->bytesAvailable()<< "time_counter:" << time_counter ;
        if(time_counter > 30)
        {
            qDebug() << "error: no message recive----------------------";
            ui->label_statusText->setText("Error: no message received.");
        }
        buffer_rec = client->readAll();
        qDebug() << "client_rvc:";
        qDebug() << buffer_rec.data();
        //str = "client_rcv:  ";
        //str += buffer_rec.data();
        //ui->label_statusText->setText(str);

        QString msg = (QString)buffer_rec;

        if(msg != "")
        {
            str += '\n';
            str += "Buffer received: ";
            str += '\n';
            str += msg;

            ui->label_statusText->setText(str);
        }

    }
    else {
        qDebug() << cal->Camera_IP << " not connected";
        str = cal->Camera_IP;
        str += " not connected.";
        cal->error_type = 1;
        ui->label_statusText->setText(str);
    }

    client->close();
    delete client;

    Sleep(1000);
}

void MainWindow::on_commandLinkButton_calibration_clicked()
{
    int calPlate = 1;
    calPlate = ui->comboBox_calibPlate->currentIndex() + 1;

    //init
    cal->InsightDataRead = false;
    cal->calibrated = false;
    cal->imageSize.height = cal->ImageSize_Height;
    cal->imageSize.width = cal->ImageSize_Width;
    cal->num = cal->Calibrate_NrOfFrameToUse;
    cal->outputFileName = cal->Write_outputFileName;
    cal->outputFileName += QString::number(ui->comboBox_cameraPosition->currentIndex(), 10);
    cal->outputFileName += ".txt";
    if(calPlate == 1)
    {
        cal->boardSize.width = 7;
        cal->boardSize.height = 8;
        cal->squareSize = 12.5;
    }
    else if(calPlate == 2)
    {
        cal->boardSize.width = 7;
        cal->boardSize.height = 8;
        cal->squareSize = 6;
    }
    else if(calPlate == 3)
    {
        cal->boardSize.width = cal->BoardSize_Width;
        cal->boardSize.height = cal->BoardSize_Height;
        cal->squareSize = cal->Square_Size;
    }
    cal->calPlate = calPlate;
    cal->camPosNum = ui->comboBox_cameraPosition->currentIndex();
    //

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("是否确认\n从当前位置开始自动标定？");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Be sure to auto-calibrate\nfrom current position?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo) {
        if(cal->camPosNum == 0)
        {
            on_runButton_clicked();

            vector<Mat> movePos = cal->CalculateRobotMovePosition(adsReadStruct.currentFlangePosition[0], adsReadStruct.currentFlangePosition[1], \
                    adsReadStruct.currentFlangePosition[2], adsReadStruct.currentFlangePosition[3], \
                    adsReadStruct.currentFlangePosition[4], adsReadStruct.currentFlangePosition[5], ui->comboBox_cameraDistance->currentIndex());

            for(int i = 0; i < 13; i++){
                for(int j = 0; j < 6; j++){
                    movePosition[i][j] = movePos[i].at<double>(j,0);
                }
                qDebug() << "***" << movePosition[i][0] <<  movePosition[i][1] <<  movePosition[i][2] << \
                            movePosition[i][3] <<  movePosition[i][4] <<  movePosition[i][5];
            }

            //run calibration program
            runTargetProgram("C:/SiasunRobot/Camera_Calibration");

            ui->commandLinkButton_calibration->setDisabled(true);
            ui->commandLinkButton_manualTrigger->setDisabled(true);
            ui->comboBox_calibPlate->setDisabled(true);
            ui->comboBox_cameraDistance->setDisabled(true);
            ui->comboBox_cameraPosition->setDisabled(true);
            //ui->pushButton_exit->setDisabled(true);
            ui->label_statusText->setText("Hand-eye Calibration Running...\nPlease wait...");




            //if(cal->HandEye_Calib(cal->calPlate, cal->Camera_IP))
            cal->start();
            while(cal->isRunning())
            {
                QCoreApplication::processEvents();
            }
            if(cal->calibSucceeded)
            {
                ui->label_statusText->setText(cal->str);
                cal->InsightDataRead = true;
                cal->calibrated = true;
            }
            else{
                ui->stopProgramButton_run->click();
                if(systemLanguage == "cn"){
                    WarningWindow::getInstance()->setWarningText("一键标定失败");
                }else if(systemLanguage == "en"){
                    WarningWindow::getInstance()->setWarningText("Auto calibration failed");
                }
                WarningWindow::getInstance()->exec();

                ui->label_statusText->setText(cal->str);
                cal->InsightDataRead = false;
                cal->calibrated = false;
            }



            ui->commandLinkButton_calibration->setEnabled(true);
            ui->commandLinkButton_manualTrigger->setEnabled(true);
            ui->comboBox_calibPlate->setEnabled(true);
            ui->comboBox_cameraDistance->setEnabled(true);
            ui->comboBox_cameraPosition->setEnabled(true);
            ui->pushButton_exit->setEnabled(true);

        }
        else
        {
            ui->commandLinkButton_calibration->setDisabled(true);
            ui->commandLinkButton_manualTrigger->setDisabled(true);
            ui->comboBox_calibPlate->setDisabled(true);
            ui->comboBox_cameraDistance->setDisabled(true);
            ui->comboBox_cameraPosition->setDisabled(true);
            //ui->pushButton_exit->setDisabled(true);
            ui->label_statusText->setText("Calibration Running...\nPlease wait...");
            cal->start();
            while(cal->isRunning())
            {
                QCoreApplication::processEvents();
            }
            if(cal->calibSucceeded)
            {
                ui->label_statusText->setText(cal->str);
            }
            else{
                if(systemLanguage == "cn"){
                    WarningWindow::getInstance()->setWarningText("一键标定失败");
                }else if(systemLanguage == "en"){
                    WarningWindow::getInstance()->setWarningText("Auto calibration failed");
                }
                WarningWindow::getInstance()->exec();

                ui->label_statusText->setText(cal->str);
            }

            ui->commandLinkButton_calibration->setEnabled(true);
            ui->commandLinkButton_manualTrigger->setEnabled(true);
            ui->comboBox_calibPlate->setEnabled(true);
            ui->comboBox_cameraDistance->setEnabled(true);
            ui->comboBox_cameraPosition->setEnabled(true);
            ui->pushButton_exit->setEnabled(true);

            //Mat Objpos(3, 1, CV_64FC1);//机器人坐标系下（x,y,z)
            //Objpos = cal->CalculatePosition(340.76, 137.73, 3);
            //for(int i = 0; i < 3; i++)
            //{
            //    qDebug() << Objpos.at<double>(i,0);
            //}
        }

    }
}

void MainWindow::on_jointEndValue_clicked()
{
    ui->statusStackedWidget->setCurrentIndex(0);
}

void MainWindow::on_log_clicked()
{
    ui->statusStackedWidget->setCurrentIndex(1);

    ui->debugPrint->clear();

    ui->logList->clear();

    QDir myDir(fileSavePath + "\\log");
    QStringList allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

    if(allFiles.count() == 0) return;

    for(int i = allFiles.count()-1; i >= 0; i--){
        ui->logList->addItem(allFiles.at(i));
    }

    QFile logFile(fileSavePath + "\\log" + logFileName + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".txt");
    if(logFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!logFile.atEnd()) {
            //read each line
            QString line = logFile.readLine();

            ui->debugPrint->addItem(line);
        }
    }

    logFile.close();
}

void MainWindow::on_analogIO_clicked()
{
    ui->statusStackedWidget->setCurrentIndex(4);
}

void MainWindow::on_toolIO_clicked()
{
    ui->statusStackedWidget->setCurrentIndex(3);
}

void MainWindow::on_teachSmooth_valueChanged(int value)
{
    adsWriteStruct.teachSmoothAll = (char)value;

    if(systemLanguage == "cn"){
        ui->teachSmoothLabel->setText("牵引柔顺度: " + QString::number(value));
    }else if(systemLanguage == "en"){
        ui->teachSmoothLabel->setText("Teach smooth: " + QString::number(value));
    }
}

void MainWindow::on_safeSetting_clicked()
{
    ui->settingStack->setCurrentIndex(1);
}

void MainWindow::on_installSetting_clicked()
{
    ui->settingStack->setCurrentIndex(6);
}

void MainWindow::on_setJointValue_clicked()
{
    on_positionLcdNumberX_clicked();
}

void MainWindow::on_setEndValue_clicked()
{
    on_jointAngleLcdNumberAxis1_clicked();
}

void MainWindow::on_collisionDetectActive_2_clicked()
{

    if(adsWriteStruct.collisionDetectActive){
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->collisionDetectActive_2->setIcon(ButtonIcon);
        ui->collisionDetectActive_2->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->collisionDetectActive_2->setText("碰撞检测 关");
        }else if(systemLanguage == "en"){
            ui->collisionDetectActive_2->setText("No detect");
        }

    }else{
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->collisionDetectActive_2->setIcon(ButtonIcon);
        ui->collisionDetectActive_2->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->collisionDetectActive_2->setText("碰撞检测 开");
        }else if(systemLanguage == "en"){
            ui->collisionDetectActive_2->setText("Detecting");
        }
    }

    if(adsWriteStruct.collisionDetectActive){
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->collisionDetectActive->setIcon(ButtonIcon);
        ui->collisionDetectActive->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->collisionDetectActive->setText("无碰撞检测");
        }else if(systemLanguage == "en"){
            ui->collisionDetectActive->setText("No detect");
        }
    }else{
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->collisionDetectActive->setIcon(ButtonIcon);
        ui->collisionDetectActive->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->collisionDetectActive->setText("碰撞检测中");
        }else if(systemLanguage == "en"){
            ui->collisionDetectActive->setText("Detecting");
        }
    }

    adsWriteStruct.collisionDetectActive = !adsWriteStruct.collisionDetectActive;

    //set the "force control" activation
    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "COLLISIONAUTO"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        if(adsWriteStruct.collisionDetectActive)
            text.replace(replaceTarget, "COLLISIONAUTO	:	on  :" + QString::number(ui->collisionSensitive->value()) + "\n");
        else
            text.replace(replaceTarget, "COLLISIONAUTO	:	off :" + QString::number(ui->collisionSensitive->value()) + "\n");
    }else{
        if(adsWriteStruct.collisionDetectActive)
            text.append("\nCOLLISIONAUTO	:	on  :" + QString::number(ui->collisionSensitive->value()) + "\n");
        else
            text.append("\nCOLLISIONAUTO	:	off :" + QString::number(ui->collisionSensitive->value()) + "\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_robotActive_clicked()
{
    if(!(adsReadStruct.slave_ready[0] && adsReadStruct.slave_ready[1] && adsReadStruct.slave_ready[2] \
         && adsReadStruct.slave_ready[3] && adsReadStruct.slave_ready[4] && adsReadStruct.slave_ready[5]\
         && adsReadStruct.slave_ready[6])){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("请等待机械臂初始化");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Please wait for init");
        }
        WarningWindow::getInstance()->exec();

        return;
    }

    adsWriteStruct.enableAllAxis = true;
    checkSlaveStatus = false;

    //change the main stack index to 1: loading gif page
    ui->mainStackWindow->setCurrentIndex(1);
    QMovie *movie = new QMovie(":/images/load.gif");
    ui->gif_label->setMovie(movie);
    movie->start();

    //Start loading thread
    loadingThread->start();

    //wait for sync
    QThread::msleep(200);
    //read the init file
    //if(adsReadStruct.sysInitDone){
    QFile file(fileSavePath + "\\config.init");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //run program when Start
            if(keywordPieces.value(0).trimmed() == "RUNAUTO"){
                if(keywordPieces.value(1).trimmed() == "on"){
                    QPixmap pixmap(":/images/node_ok.png");
                    QIcon ButtonIcon(pixmap);
                    ui->defaultProgram->setIcon(ButtonIcon);

                    int digit = keywordPieces.value(3).toInt();
                    bool status;
                    if(keywordPieces.value(4).toInt() == 0) status = false;
                    else status = true;

                    ui->defaultProgramName->setText(keywordPieces.value(2).trimmed());
                    ui->digitalInCombo->setCurrentIndex(digit-1);
                    ui->digitalStatusCombo->setCurrentIndex(keywordPieces.value(4).toInt());

                    if(adsReadStruct.externalIORead[digit-1] == status){
                        runTargetProgram("C:\\SiasunRobot\\program\\" + keywordPieces.value(2).trimmed());

                        //click repeat button
                        repeatProgram = !repeatProgram;
                        QPixmap pixmap(":/images/repeat_2.png");
                        QIcon ButtonIcon(pixmap);
                        ui->pushButton->setIcon(ButtonIcon);
                        ui->pushButton->setIconSize(pixmap.rect().size());


                    }
                }else{
                    QPixmap pixmap(":/images/node_warning.png");
                    QIcon ButtonIcon(pixmap);
                    ui->defaultProgram->setIcon(ButtonIcon);
                }
            }

        }
    }
    file.close();
    //}
}

void MainWindow::on_return2Start_clicked()
{
    checkSlaveStatus = false;
    on_disconnectAction_clicked();
    QThread::msleep(300);
    updateData->stop = true;
}

void MainWindow::on_copyNode_clicked()
{
    if(ui->programTree->currentItem() == NULL || ui->programTree->currentItem()->text(0) == "MoveC" ||\
            ui->programTree->currentItem()->text(0).split('(').value(0) == "MoveC_1"|| ui->programTree->currentItem()->text(0).split('(').value(0) == "MoveC_2") return;
    copyNode = ui->programTree->currentItem()->text(0);

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("节点已复制");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Node copied");
    }
    WarningWindow::getInstance()->exec();
}

void MainWindow::on_pasteNode_clicked()
{
    if(ui->programTree->currentItem() == NULL || copyNode == "") return;
    programAddRoot(copyNode);
    //save the new program tree
    saveTheNewProgramTree();
}

void MainWindow::on_acrossInstall_toggled(bool checked)
{
    //竖装
    if(checked){
        ui->installGroupBox->setVisible(false);
        xmlWriteStruct.theta_G = 90;
        xmlWriteStruct.alpha_G = 0;
    }
}

void MainWindow::on_verticalInstall_toggled(bool checked)
{
    //横装
    if(checked){
        ui->installGroupBox->setVisible(false);
        xmlWriteStruct.theta_G = 0;
        xmlWriteStruct.alpha_G = 0;
    }
}

void MainWindow::on_anywayInstall_toggled(bool checked)
{
    if(checked){
        ui->installGroupBox->setVisible(true);
    }
}

void MainWindow::on_copyNode_3_clicked()
{
    ui->moveStackedWidget->setCurrentIndex(2);
}

void MainWindow::on_copyNode_2_clicked()
{
    ui->moveStackedWidget->setCurrentIndex(4);
}

void MainWindow::on_teachSmooth1_valueChanged(int value)
{
    adsWriteStruct.teachSmooth[0] = value;
}

void MainWindow::on_teachSmooth2_valueChanged(int value)
{
    adsWriteStruct.teachSmooth[1] = value;
}

void MainWindow::on_teachSmooth3_valueChanged(int value)
{
    adsWriteStruct.teachSmooth[2] = value;
}

void MainWindow::on_teachSmooth4_valueChanged(int value)
{
    adsWriteStruct.teachSmooth[3] = value;
}

void MainWindow::on_teachSmooth5_valueChanged(int value)
{
    adsWriteStruct.teachSmooth[4] = value;
}

void MainWindow::on_teachSmooth6_valueChanged(int value)
{
    adsWriteStruct.teachSmooth[5] = value;
}

void MainWindow::on_teachSmooth7_valueChanged(int value)
{
    adsWriteStruct.teachSmooth[6] = value;
}

void MainWindow::on_collisionSensitive_valueChanged(int value)
{
    if(value == 1){ ui->collisionSensitiveLabel->setText("碰撞检测灵敏度: 高"); adsWriteStruct.collisionSensitive = 1;}
    if(value == 2){ ui->collisionSensitiveLabel->setText("碰撞检测灵敏度: 中"); adsWriteStruct.collisionSensitive = 2;}
    if(value == 3){ ui->collisionSensitiveLabel->setText("碰撞检测灵敏度: 低"); adsWriteStruct.collisionSensitive = 3;}

    //set the "force control" activation
    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "COLLISIONAUTO"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){

        if(adsWriteStruct.collisionDetectActive)
            text.replace(replaceTarget, "COLLISIONAUTO	:	on  :" + QString::number(value) + "\n");
        else
            text.replace(replaceTarget, "COLLISIONAUTO	:	off\n");

    }else{
        if(adsWriteStruct.collisionDetectActive)
            text.append("\nCOLLISIONAUTO	:	on  :" + QString::number(value) + "\n");
        else
            text.append("\nCOLLISIONAUTO	:	off\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_dynamicThreshActive_clicked()
{
    if(adsWriteStruct.dynamicThresh){
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->dynamicThreshActive->setIcon(ButtonIcon);
        ui->dynamicThreshActive->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->dynamicThreshActive->setText("动态阈值 关");
        }else if(systemLanguage == "en"){
            ui->dynamicThreshActive->setText("Dynamic Thresh Off");
        }

        ui->dynamicThreshStackedWidget->setCurrentIndex(0);

    }else{
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->dynamicThreshActive->setIcon(ButtonIcon);
        ui->dynamicThreshActive->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->dynamicThreshActive->setText("动态阈值 开");
        }else if(systemLanguage == "en"){
            ui->dynamicThreshActive->setText("Dynamic Thresh On");
        }

        ui->dynamicThreshStackedWidget->setCurrentIndex(1);
    }

    adsWriteStruct.dynamicThresh = !adsWriteStruct.dynamicThresh;

    if(adsWriteStruct.dynamicThresh){
        //set the "dynamic thresh" activation
        //read the init file
        bool find = false;
        QString replaceTarget;
        QFile file(fileSavePath + "\\config.init");;
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //set full screen
            if(keywordPieces.value(0).trimmed() == "DYNAMICTHRESH"){

                replaceTarget = initCom;
                find = true;
            }
        }
        file.close();

        QByteArray fileData;
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        fileData = file.readAll();
        QString text(fileData);

        if(find){

            if(adsWriteStruct.dynamicThresh)
                text.replace(replaceTarget, "DYNAMICTHRESH	:	on : " + ui->dynamicThread_1->text() + ":"+ \
                             ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                             ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");
            else
                text.replace(replaceTarget, "DYNAMICTHRESH	:	off : " + ui->dynamicThread_1->text() + ":"+ \
                             ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                             ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");

        }else{
            if(adsWriteStruct.dynamicThresh)
                text.append("\nDYNAMICTHRESH	:	on : " + ui->dynamicThread_1->text() + ":"+ \
                            ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                            ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");
            else
                text.append("\nDYNAMICTHRESH	:	off : " + ui->dynamicThread_1->text() + ":"+ \
                            ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                            ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");

        }

        file.seek(0); // go to the beginning of the file
        file.resize(0); //clear all the old text
        file.write(text.toUtf8()); // write the new text back to the file
        file.close(); // close the file handle.
    }
}

void MainWindow::on_setInstallOrient_clicked()
{
    if(ui->anywayInstall->isChecked()){
        xmlWriteStruct.theta_G = ui->installRx->text().toDouble();
        xmlWriteStruct.alpha_G = ui->installRy->text().toDouble();
    }

    QDomDocument document;
    QDomElement root = document.createElement("Install_Position");
    document.appendChild(root);


    QDomElement node = document.createElement("value");
    node.setAttribute("rx", xmlWriteStruct.theta_G); //ry
    root.appendChild(node);

    node = document.createElement("value");
    node.setAttribute("ry", xmlWriteStruct.alpha_G); //rz
    root.appendChild(node);




    QFile file ("C:\\SiasunRobot\\xml\\INSTALL.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream stream(&file);
    stream << document.toString();
    file.close();

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("安装方向设置完成\n重启机械臂生效");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Install orient set success");
    }
    WarningWindow::getInstance()->exec();
}

void MainWindow::on_setDynamicThread_clicked()
{
    adsWriteStruct.dynamicThreshJoint[0] = ui->dynamicThread_1->text().toDouble();
    adsWriteStruct.dynamicThreshJoint[1] = ui->dynamicThread_2->text().toDouble();
    adsWriteStruct.dynamicThreshJoint[2] = ui->dynamicThread_3->text().toDouble();
    adsWriteStruct.dynamicThreshJoint[3] = ui->dynamicThread_4->text().toDouble();
    adsWriteStruct.dynamicThreshJoint[4] = ui->dynamicThread_5->text().toDouble();
    adsWriteStruct.dynamicThreshJoint[5] = ui->dynamicThread_6->text().toDouble();
    adsWriteStruct.dynamicThreshJoint[6] = ui->dynamicThread_7->text().toDouble();

    //set the "dynamic thresh" activation
    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "DYNAMICTHRESH"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){

        if(adsWriteStruct.dynamicThresh)
            text.replace(replaceTarget, "DYNAMICTHRESH	:	on : " + ui->dynamicThread_1->text() + ":"+ \
                         ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                         ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");

        else
            text.replace(replaceTarget, "DYNAMICTHRESH	:	off : " + ui->dynamicThread_1->text() + ":"+ \
                         ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                         ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");

    }else{
        if(adsWriteStruct.dynamicThresh)
            text.append( "\nDYNAMICTHRESH	:	on : " + ui->dynamicThread_1->text() + ":"+ \
                         ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                         ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");

        else
            text.append( "\nDYNAMICTHRESH	:	off : " + ui->dynamicThread_1->text() + ":"+ \
                         ui->dynamicThread_2->text() +":"+ ui->dynamicThread_3->text() +":"+ ui->dynamicThread_4->text() +":"+\
                         ui->dynamicThread_5->text() +":"+ui->dynamicThread_6->text() +":"+ui->dynamicThread_7->text() + "\n");

    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("已设定阈值参数");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Set dynamic thresh manually");
    }
    WarningWindow::getInstance()->exec();
}

void MainWindow::on_setDynamicThread_2_clicked()
{
    //read XML
    //char xmlWritePath [] = "GVL.xml_write";
    //ADS::getAdsInstance()->adsRead(xmlWritePath, sizeof(xmlWritePath), sizeof(xmlReadStruct), &xmlReadStruct); //read from PLC

    startReadXML = true;
    QThread::msleep(1000);
    ui->dynamicThreadCurrent1->setText(QString::number(xmlReadStruct.currentDynamicThresh[0]));
    ui->dynamicThreadCurrent2->setText(QString::number(xmlReadStruct.currentDynamicThresh[1]));
    ui->dynamicThreadCurrent3->setText(QString::number(xmlReadStruct.currentDynamicThresh[2]));
    ui->dynamicThreadCurrent4->setText(QString::number(xmlReadStruct.currentDynamicThresh[3]));
    ui->dynamicThreadCurrent5->setText(QString::number(xmlReadStruct.currentDynamicThresh[4]));
    ui->dynamicThreadCurrent6->setText(QString::number(xmlReadStruct.currentDynamicThresh[5]));
    ui->dynamicThreadCurrent7->setText(QString::number(xmlReadStruct.currentDynamicThresh[6]));

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("阈值已刷新");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Threshold refreshed");
    }
    WarningWindow::getInstance()->exec();
    startReadXML = false;
}

void MainWindow::on_toolOutput1_clicked()
{
    tool_ioOut[0] = !tool_ioOut[0];

    if(tool_ioOut[0]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput1->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput1->setIcon(ButtonIcon);
    }
}

void MainWindow::on_toolOutput2_clicked()
{
    tool_ioOut[1] = !tool_ioOut[1];

    if(tool_ioOut[1]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput2->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput2->setIcon(ButtonIcon);
    }
}

void MainWindow::on_toolOutput3_clicked()
{
    tool_ioOut[2] = !tool_ioOut[2];

    if(tool_ioOut[2]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput3->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput3->setIcon(ButtonIcon);
    }
}

void MainWindow::on_toolOutput4_clicked()
{
    tool_ioOut[3] = !tool_ioOut[3];

    if(tool_ioOut[3]){
        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput4->setIcon(ButtonIcon);
    }else{
        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->toolOutput4->setIcon(ButtonIcon);
    }
}

void MainWindow::on_toolLoadRecognize_clicked()
{
    ui->settingStack->setCurrentIndex(7);
}

void MainWindow::on_toolLoad_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入末端工具质量(kg)");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter tool weight(kg)");
    numberBoard->editLineNum = 14; //for tool weight
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolLoadPositionX_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入质心相对法兰面偏移X(m)");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter tool weight point bias X(m)");
    numberBoard->editLineNum = 15; //for tool weight
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolLoadPositionY_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入质心相对法兰面偏移Y(m)");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter tool weight point bias Y(m)");
    numberBoard->editLineNum = 16; //for tool weight
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolLoadPositionZ_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("输入质心相对法兰面偏移Z(m)");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Enter tool weight point bias Z(m)");
    numberBoard->editLineNum = 17; //for tool weight
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolLoadAutoRecognizeReset_clicked()
{
    adsWriteStruct.tcpIdStart = false;
    adsWriteStruct.tcpIdPause = false;
    adsWriteStruct.tcpIdReset = true;
    ui->toolLoadPositionX->setText("0");
    ui->toolLoadPositionY->setText("0");
    ui->toolLoadPositionZ->setText("0");

    QThread::msleep(200);

    adsWriteStruct.tcpIdReset = false;
}

void MainWindow::on_toolLoadAutoRecognize_clicked()
{
    if(ui->toolLoad->text().toDouble() <= 0 || ui->toolLoad->text().toDouble() > 5){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("末端负载质量不合理");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Invalid tool load");
        }
        WarningWindow::getInstance()->exec();
    }else{
        adsWriteStruct.tcpLoad = ui->toolLoad->text().toDouble();
        adsWriteStruct.tcpIdStart = true;
        adsWriteStruct.tcpIdPause = false;
        adsWriteStruct.tcpIdReset = false;
    }
}

void MainWindow::on_toolLoadAutoRecognizePause_clicked()
{
    adsWriteStruct.tcpIdStart = false;
    adsWriteStruct.tcpIdPause = true;
    adsWriteStruct.tcpIdReset = false;
}

void MainWindow::on_setToolWeightPosition_clicked()
{
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("确定使用此重心位置?");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Be sure to use this tool position?");
    }

    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);


    if (WarningWindow::getInstance()->yesOrNo){
        //set the "move to home position" position
        //read the init file
        bool find = false;
        QString replaceTarget;
        QFile file(fileSavePath + "\\config.init");;
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        while (!file.atEnd()){
            QString initCom = file.readLine();
            QStringList keywordPieces = initCom.split(':');

            //set full screen
            if(keywordPieces.value(0).trimmed() == "TOOLWPOS"){

                replaceTarget = initCom;
                find = true;
            }
        }
        file.close();

        QByteArray fileData;
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        fileData = file.readAll();
        QString text(fileData);

        if(find){

            text.replace(replaceTarget, "TOOLWPOS	:	" + ui->toolLoad->text() + " : " + \
                         ui->toolLoadPositionX->text() + " : " +ui->toolLoadPositionY->text() + " : " +ui->toolLoadPositionZ->text() + "\n");

        }else{
            text.append( "\nTOOLWPOS	:	" + ui->toolLoad->text() + " : " + \
                         ui->toolLoadPositionX->text() + " : " +ui->toolLoadPositionY->text() + " : " +ui->toolLoadPositionZ->text() + "\n");

        }

        file.seek(0); // go to the beginning of the file
        file.resize(0); //clear all the old text
        file.write(text.toUtf8()); // write the new text back to the file
        file.close(); // close the file handle.

        adsWriteStruct.tcpLoad = ui->toolLoad->text().toDouble();
        adsWriteStruct.tcpLoadPos[0] = ui->toolLoadPositionX->text().toDouble();
        adsWriteStruct.tcpLoadPos[1] = ui->toolLoadPositionY->text().toDouble();
        adsWriteStruct.tcpLoadPos[2] = ui->toolLoadPositionZ->text().toDouble();

        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("设置成功");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("Set success");
        }

        WarningWindow::getInstance()->exec();
    }
}

void MainWindow::on_readToolWeightPosition_clicked()
{
    ui->toolLoadPositionX->setText(QString::number(adsReadStruct.Pcload[0]));
    ui->toolLoadPositionY->setText(QString::number(adsReadStruct.Pcload[1]));
    ui->toolLoadPositionZ->setText(QString::number(adsReadStruct.Pcload[2]));
}

extern "C" int calib_finish;          //read from camera ex: int calib_finish();
extern "C" int start_calib;           //read from camera ex: int calib_start();
void MainWindow::on_stopCalibration_clicked()
{
    calib_finish = 1;
    start_calib = 0;
    while(cal->isRunning())
    {
        QCoreApplication::processEvents();
    }
    ui->commandLinkButton_manualTrigger->setEnabled(true);
    ui->commandLinkButton_calibration->setEnabled(true);
}


void MainWindow::on_writeDynamicParameters_clicked()
{
    ////read XML
    //char xmlWritePath [] = "GVL.xml_write";
    //
    ////read from PLC
    //ADS::getAdsInstance()->adsRead(xmlWritePath, sizeof(xmlWritePath), sizeof(xmlReadStruct), &xmlReadStruct);
    startReadXML = true;
    QThread::msleep(1000);

    QDomDocument document;
    QDomElement root = document.createElement("Dynamic");
    document.appendChild(root);




    //Kt
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("Kt" + QString::number(i+1), QString::number(xmlReadStruct.Kt[i]));
        root.appendChild(node);
    }


    //FC
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("FC" + QString::number(i+1), QString::number(xmlReadStruct.FC[i]));
        root.appendChild(node);
    }


    //FV
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("FV" + QString::number(i+1), QString::number(xmlReadStruct.FV[i]));
        root.appendChild(node);
    }


    //FVV
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("FVV" + QString::number(i+1), QString::number(xmlReadStruct.FVV[i]));
        root.appendChild(node);
    }


    //FVVV
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("FVVV" + QString::number(i+1), QString::number(xmlReadStruct.FVVV[i]));
        root.appendChild(node);
    }


    //TFv
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("TFv" + QString::number(i+1), QString::number(xmlReadStruct.TFv[i]));
        root.appendChild(node);
    }


    //TFc
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("TFc" + QString::number(i+1), QString::number(xmlReadStruct.TFc[i]));
        root.appendChild(node);
    }


    //Th
    for(int i = 0; i < 7; i++){
        QDomElement node = document.createElement("Value");
        node.setAttribute("Th" + QString::number(i+1), QString::number(xmlReadStruct.Th[i]));
        root.appendChild(node);
    }



    QFile file ("C:\\SiasunRobot\\xml\\DYNAMIC.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream stream(&file);
    stream << document.toString();
    file.close();

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("力控参数写入完成");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Axis bias set success");
    }
    WarningWindow::getInstance()->exec();

    startReadXML = false;
}

void MainWindow::on_logList_itemClicked(QListWidgetItem *item)
{
    QFile file(fileSavePath + "\\log\\" + item->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    //show program on the list widget
    ui->debugPrint->clear();

    while (!file.atEnd()) {
        //read each line
        QString line = file.readLine();
        ui->debugPrint->addItem(line);
    }
}

void MainWindow::on_callOsk_2_clicked()
{
    on_callOsk_clicked();
}

void MainWindow::on_callOsk_3_clicked()
{
    on_callOsk_clicked();
}

void MainWindow::on_teachReplayMode_clicked()
{
    if(toolButtonSaveNode){
        toolButtonSaveNode = false;

        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->teachReplayMode->setIcon(ButtonIcon);
        ui->teachReplayMode->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->teachReplayMode->setText("末端记录按钮 关");
        }else if(systemLanguage == "en"){
            ui->teachReplayMode->setText("Tool button save node: Disable");
        }
    }else{
        toolButtonSaveNode = true;

        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->teachReplayMode->setIcon(ButtonIcon);
        ui->teachReplayMode->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->teachReplayMode->setText("末端记录按钮 开");
        }else if(systemLanguage == "en"){
            ui->teachReplayMode->setText("Tool button save node: Enable");
        }

        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("在编程页面中\n牵引时按下末端记录按钮来记录过程点");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("In program page\nPress save node button\nto save important points on the path");
        }
        WarningWindow::getInstance()->exec();
    }

}

void MainWindow::on_tcpCalibration_clicked()
{
    ui->settingStack->setCurrentIndex(8);
}

void MainWindow::on_tcpCalibration4p_clicked()
{
    ui->settingStack->setCurrentIndex(15);
    adsWriteStruct.tcpCalibrationStart = true;
    adsWriteStruct.tcp4pCalibration = true;
}

void MainWindow::on_tcpCalibration6p_clicked()
{
    ui->settingStack->setCurrentIndex(9);
    adsWriteStruct.tcpCalibrationStart = true;
    adsWriteStruct.tcp4pCalibration = false;
}

void MainWindow::on_tcpCalibrationP1Next_clicked()
{
    adsWriteStruct.tcpCalibrationSave = true;
    QThread::msleep(300);
    adsWriteStruct.tcpCalibrationSave = false;
    ui->settingStack->setCurrentIndex(10);
}

void MainWindow::on_tcpCalibrationP2Next_clicked()
{
    adsWriteStruct.tcpCalibrationSave = true;
    QThread::msleep(300);
    adsWriteStruct.tcpCalibrationSave = false;
    ui->settingStack->setCurrentIndex(11);
}

void MainWindow::on_tcpCalibrationP3Next_clicked()
{
    adsWriteStruct.tcpCalibrationSave = true;
    QThread::msleep(300);
    adsWriteStruct.tcpCalibrationSave = false;
    ui->settingStack->setCurrentIndex(12);
}

void MainWindow::on_tcpCalibrationP4Next_clicked()
{
    adsWriteStruct.tcpCalibrationSave = true;
    QThread::msleep(300);
    adsWriteStruct.tcpCalibrationSave = false;
    QThread::msleep(300);

    if(adsWriteStruct.tcp4pCalibration){
        if(systemLanguage == "cn"){
            WarningWindow::getInstance()->setWarningText("TCP标定完成\n" + QString::number(adsReadStruct.tcpCalibration[0]) + " " +\
                    QString::number(adsReadStruct.tcpCalibration[1]) + " " +QString::number(adsReadStruct.tcpCalibration[2]) + " " +\
                    QString::number(adsReadStruct.tcpCalibration[3]) + " " +QString::number(adsReadStruct.tcpCalibration[4]) + " " +\
                    QString::number(adsReadStruct.tcpCalibration[5])+"\n是否加载此坐标系？");
        }else if(systemLanguage == "en"){
            WarningWindow::getInstance()->setWarningText("TCP calibration finish\n" + QString::number(adsReadStruct.tcpCalibration[0]) + " " +\
                    QString::number(adsReadStruct.tcpCalibration[1]) + " " +QString::number(adsReadStruct.tcpCalibration[2]) + " " +\
                    QString::number(adsReadStruct.tcpCalibration[3]) + " " +QString::number(adsReadStruct.tcpCalibration[4]) + " " +\
                    QString::number(adsReadStruct.tcpCalibration[5])+"\nLoad this TCP?");
        }
        WarningWindow::getInstance()->showCancelButton(true);
        WarningWindow::getInstance()->exec();
        WarningWindow::getInstance()->showCancelButton(false);

        if(WarningWindow::getInstance()->yesOrNo){
            ui->toolCoordinateLineX->setText(QString::number(adsReadStruct.tcpCalibration[0]));
            ui->toolCoordinateLineY->setText(QString::number(adsReadStruct.tcpCalibration[1]));
            ui->toolCoordinateLineZ->setText(QString::number(adsReadStruct.tcpCalibration[2]));
            ui->toolCoordinateLineRX->setText(QString::number(adsReadStruct.tcpCalibration[3]));
            ui->toolCoordinateLineRY->setText(QString::number(adsReadStruct.tcpCalibration[4]));
            ui->toolCoordinateLineRZ->setText(QString::number(adsReadStruct.tcpCalibration[5]));
        }

        adsWriteStruct.tcpCalibrationStart = false;

        ui->settingStack->setCurrentIndex(2);
    }else{
        ui->settingStack->setCurrentIndex(13);
    }
}

void MainWindow::on_tcpCalibrationP5Next_clicked()
{
    adsWriteStruct.tcpCalibrationSave = true;
    QThread::msleep(300);
    adsWriteStruct.tcpCalibrationSave = false;
    ui->settingStack->setCurrentIndex(14);
}

void MainWindow::on_tcpCalibrationP6Next_clicked()
{
    adsWriteStruct.tcpCalibrationSave = true;
    QThread::msleep(300);
    adsWriteStruct.tcpCalibrationSave = false;
    QThread::msleep(300);

    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("TCP标定完成\n" + QString::number(adsReadStruct.tcpCalibration[0]) + " " +\
                QString::number(adsReadStruct.tcpCalibration[1]) + " " +QString::number(adsReadStruct.tcpCalibration[2]) + " " +\
                QString::number(adsReadStruct.tcpCalibration[3]) + " " +QString::number(adsReadStruct.tcpCalibration[4]) + " " +\
                QString::number(adsReadStruct.tcpCalibration[5])+"\n是否加载此坐标系？");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("TCP calibration finish\n" + QString::number(adsReadStruct.tcpCalibration[0]) + " " +\
                QString::number(adsReadStruct.tcpCalibration[1]) + " " +QString::number(adsReadStruct.tcpCalibration[2]) + " " +\
                QString::number(adsReadStruct.tcpCalibration[3]) + " " +QString::number(adsReadStruct.tcpCalibration[4]) + " " +\
                QString::number(adsReadStruct.tcpCalibration[5])+"\nLoad this TCP?");
    }
    WarningWindow::getInstance()->showCancelButton(true);
    WarningWindow::getInstance()->exec();
    WarningWindow::getInstance()->showCancelButton(false);

    if(WarningWindow::getInstance()->yesOrNo){
        ui->toolCoordinateLineX->setText(QString::number(adsReadStruct.tcpCalibration[0]));
        ui->toolCoordinateLineY->setText(QString::number(adsReadStruct.tcpCalibration[1]));
        ui->toolCoordinateLineZ->setText(QString::number(adsReadStruct.tcpCalibration[2]));
        ui->toolCoordinateLineRX->setText(QString::number(adsReadStruct.tcpCalibration[3]));
        ui->toolCoordinateLineRY->setText(QString::number(adsReadStruct.tcpCalibration[4]));
        ui->toolCoordinateLineRZ->setText(QString::number(adsReadStruct.tcpCalibration[5]));
    }

    adsWriteStruct.tcpCalibrationStart = false;

    ui->settingStack->setCurrentIndex(2);
}

void MainWindow::on_toolCoordinateLineRX_2_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具姿态RX");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate RX");
    numberBoard->editLineNum = 18; //for tool coordinate RX
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolCoordinateLineRY_2_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具姿态RY");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate RY");
    numberBoard->editLineNum = 19; //for tool coordinate RX
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_toolCoordinateLineRZ_2_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("工具姿态RZ");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Tool coordinate RZ");
    numberBoard->editLineNum = 20; //for tool coordinate RX
    //numberBoard->windowIsOpened = true;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_tcpCalibrationRxyzNext_clicked()
{
    adsWriteStruct.toolCoordinateR[0] = ui->toolCoordinateLineRX_2->text().toDouble();
    adsWriteStruct.toolCoordinateR[1] = ui->toolCoordinateLineRY_2->text().toDouble();
    adsWriteStruct.toolCoordinateR[2] = ui->toolCoordinateLineRZ_2->text().toDouble();
    ui->settingStack->setCurrentIndex(9);
}


void MainWindow::on_autoLogin_clicked()
{
    if(autoLoginActive){
        autoLoginActive = false;

        QPixmap pixmap(":/images/node_warning.png");
        QIcon ButtonIcon(pixmap);
        ui->autoLogin->setIcon(ButtonIcon);
        ui->autoLogin->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->autoLogin->setText("自动登录 关");
        }else if(systemLanguage == "en"){
            ui->autoLogin->setText("Auto login: Disable");
        }
    }else{
        autoLoginActive = true;

        QPixmap pixmap(":/images/node_ok.png");
        QIcon ButtonIcon(pixmap);
        ui->autoLogin->setIcon(ButtonIcon);
        ui->autoLogin->setIconSize(pixmap.rect().size());
        if(systemLanguage == "cn"){
            ui->autoLogin->setText("自动登录 开");
        }else if(systemLanguage == "en"){
            ui->autoLogin->setText("Auto login: Enable");
        }

    }

    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "AUTOLOGIN"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){

        if(autoLoginActive)
            text.replace(replaceTarget, "AUTOLOGIN	:	on\n");

        else
            text.replace(replaceTarget, "AUTOLOGIN	:	off\n");


    }else{
        if(autoLoginActive)
            text.append( "\nAUTOLOGIN	:	on\n");

        else
            text.append( "\nAUTOLOGIN	:	off\n");


    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_virtualWall_clicked()
{
    ui->settingStack->setCurrentIndex(16);
}

void MainWindow::on_setVirtualWall_clicked()
{
    ui->move3DWidget->rob->buildWalls(false, 0, 0, 0); //clear old walls

    QString wallSave = "VIRTUALWALL : ";

    //virtual wall 1
    if(ui->virtualWallCheckbox1->isChecked()){
        wallSave = wallSave + "on:" + ui->virtualWall1Length->text()+ ":";
        ui->move3DWidget->rob->buildWalls(true, ui->virtualWall1Length->text().toDouble(), 0, 1);
        virtualWallSetted = true;
    }else{
        wallSave = wallSave + "off:0:";
    }

    //virtual wall 2
    if(ui->virtualWallCheckbox2->isChecked()){
        wallSave = wallSave + "on:" + ui->virtualWall2Length->text()+ ":";
        ui->move3DWidget->rob->buildWalls(true, ui->virtualWall2Length->text().toDouble(), 0, 2);
        virtualWallSetted = true;
    }else{
        wallSave = wallSave + "off:0:";
    }

    //virtual wall 3
    if(ui->virtualWallCheckbox3->isChecked()){
        wallSave = wallSave + "on:" + ui->virtualWall3Length->text() + ":" + ui->virtualWall3Rot->text() + ":";
        ui->move3DWidget->rob->buildWalls(true, ui->virtualWall3Length->text().toDouble(), ui->virtualWall3Rot->text().toDouble(), 3);
        virtualWallSetted = true;
    }else{
        wallSave = wallSave + "off:0:0:";
    }

    //virtual wall 4
    if(ui->virtualWallCheckbox4->isChecked()){
        wallSave = wallSave + "on:" + ui->virtualWall4Length->text() + ":" + ui->virtualWall4Rot->text() + ":";
        ui->move3DWidget->rob->buildWalls(true, ui->virtualWall4Length->text().toDouble(), ui->virtualWall4Rot->text().toDouble(), 4);
        virtualWallSetted = true;
    }else{
        wallSave = wallSave + "off:0:0:";
    }

    //virtual wall 5
    if(ui->virtualWallCheckbox5->isChecked()){
        wallSave = wallSave + "on:" + ui->virtualWall5Length->text() + ":" + ui->virtualWall5Rot->text() + ":";
        ui->move3DWidget->rob->buildWalls(true, ui->virtualWall5Length->text().toDouble(), ui->virtualWall5Rot->text().toDouble(), 5);
        virtualWallSetted = true;
    }else{
        wallSave = wallSave + "off:0:0:";
    }

    //virtual wall 6
    if(ui->virtualWallCheckbox6->isChecked()){
        wallSave = wallSave + "on:" + ui->virtualWall6Length->text() + ":" + ui->virtualWall6Rot->text() + "\n";
        ui->move3DWidget->rob->buildWalls(true, ui->virtualWall6Length->text().toDouble(), ui->virtualWall6Rot->text().toDouble(), 6);
        virtualWallSetted = true;
    }else{
        wallSave = wallSave + "off:0:0\n";
    }

    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        //set full screen
        if(keywordPieces.value(0).trimmed() == "VIRTUALWALL"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        text.replace(replaceTarget, wallSave);
    }else{
        text.append("\n" + wallSave);
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_cancelVirtualWall_clicked()
{
    ui->move3DWidget->rob->buildWalls(false, 0, 0, 0);
    virtualWallSetted = false;

    on_virtualWallCheckbox1_toggled(false);
    on_virtualWallCheckbox2_toggled(false);
    on_virtualWallCheckbox3_toggled(false);
    on_virtualWallCheckbox4_toggled(false);
    on_virtualWallCheckbox5_toggled(false);
    on_virtualWallCheckbox6_toggled(false);

    ui->virtualWallCheckbox1->setChecked(false);
    ui->virtualWallCheckbox2->setChecked(false);
    ui->virtualWallCheckbox3->setChecked(false);
    ui->virtualWallCheckbox4->setChecked(false);
    ui->virtualWallCheckbox5->setChecked(false);
    ui->virtualWallCheckbox6->setChecked(false);

    //read the init file
    bool find = false;
    QString replaceTarget;
    QFile file(fileSavePath + "\\config.init");;
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()){
        QString initCom = file.readLine();
        QStringList keywordPieces = initCom.split(':');

        if(keywordPieces.value(0).trimmed() == "VIRTUALWALL"){

            replaceTarget = initCom;
            find = true;
        }
    }
    file.close();

    QByteArray fileData;
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    fileData = file.readAll();
    QString text(fileData);

    if(find){
        text.replace(replaceTarget, "VIRTUALWALL : off:0:off:0:off:0:0:off:0:0:off:0:0:off:0:0\n");
    }else{
        text.append("\nVIRTUALWALL : off:0:off:0:off:0:0:off:0:0:off:0:0:off:0:0\n");
    }

    file.seek(0); // go to the beginning of the file
    file.resize(0); //clear all the old text
    file.write(text.toUtf8()); // write the new text back to the file
    file.close(); // close the file handle.
}

void MainWindow::on_virtualWall1Length_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("水平边界1与XY平面的距离 mm");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Length virtual wall to plane XY");
    numberBoard->editLineNum = 21;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall2Length_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("水平边界2与XY平面的距离 mm");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Length virtual wall to plane XY");
    numberBoard->editLineNum = 22;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall3Length_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界1与Z轴距离 mm");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Length virtual wall to axis Z");
    numberBoard->editLineNum = 23;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall3Rot_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界1绕Z轴旋转 °");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Rot virtual wall on axis Z");
    numberBoard->editLineNum = 27;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall4Length_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界2与Z轴距离 mm");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Length virtual wall to axis Z");
    numberBoard->editLineNum = 24;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall4Rot_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界2绕Z轴旋转 °");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Rot virtual wall on axis Z");
    numberBoard->editLineNum = 28;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall5Length_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界3与Z轴距离 mm");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Length virtual wall to axis Z");
    numberBoard->editLineNum = 25;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall5Rot_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界3绕Z轴旋转 °");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Rot virtual wall on axis Z");
    numberBoard->editLineNum = 29;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall6Length_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界4与Z轴距离 mm");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Length virtual wall to axis Z");
    numberBoard->editLineNum = 26;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWall6Rot_clicked()
{
    //show number board
    numberBoard->setModal(true);
    numberBoard->cleanLineEdit();
    numberBoard->axisGroupVisible = false;
    numberBoard->positionGroupVisible = false;
    numberBoard->changeWindowSize();
    if(systemLanguage == "cn")
        numberBoard->setLabelText("垂直边界4绕Z轴旋转 °");
    else if(systemLanguage == "en")
        numberBoard->setLabelText("Rot virtual wall on axis Z");
    numberBoard->editLineNum = 30;
    numberBoard->changeLanguage();
    numberBoard->exec();
}

void MainWindow::on_virtualWallCheckbox1_toggled(bool checked)
{
    if(checked){
        ui->virtualWall1Length->setEnabled(true);
    }else{
        ui->virtualWall1Length->setEnabled(false);
    }
}

void MainWindow::on_virtualWallCheckbox2_toggled(bool checked)
{
    if(checked){
        ui->virtualWall2Length->setEnabled(true);
    }else{
        ui->virtualWall2Length->setEnabled(false);
    }
}

void MainWindow::on_virtualWallCheckbox3_toggled(bool checked)
{
    if(checked){
        ui->virtualWall3Length->setEnabled(true);
        ui->virtualWall3Rot->setEnabled(true);

    }else{
        ui->virtualWall3Length->setEnabled(false);
        ui->virtualWall3Rot->setEnabled(false);

    }
}

void MainWindow::on_virtualWallCheckbox4_toggled(bool checked)
{
    if(checked){
        ui->virtualWall4Length->setEnabled(true);
        ui->virtualWall4Rot->setEnabled(true);
    }else{
        ui->virtualWall4Length->setEnabled(false);
        ui->virtualWall4Rot->setEnabled(false);
    }
}

void MainWindow::on_virtualWallCheckbox5_toggled(bool checked)
{
    if(checked){
        ui->virtualWall5Length->setEnabled(true);
        ui->virtualWall5Rot->setEnabled(true);
    }else{
        ui->virtualWall5Length->setEnabled(false);
        ui->virtualWall5Rot->setEnabled(false);
    }
}

void MainWindow::on_virtualWallCheckbox6_toggled(bool checked)
{
    if(checked){
        ui->virtualWall6Length->setEnabled(true);
        ui->virtualWall6Rot->setEnabled(true);
    }else{
        ui->virtualWall6Length->setEnabled(false);
        ui->virtualWall6Rot->setEnabled(false);
    }
}

void MainWindow::on_warningOK_clicked()
{
    QPixmap bkgnd(":/images/background2.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    ui->mainStackWindow->setCurrentIndex(2);
}

void MainWindow::on_loadToolCoordinate_clicked()
{
    if(ui->selectToolCoordinate1->text() != "1") ui->settingStack->setCurrentIndex(17);
}

void MainWindow::on_selectToolCoordinateBack_clicked()
{
    ui->settingStack->setCurrentIndex(2);
}

void MainWindow::on_selectToolCoordinate1_clicked()
{
    selectToolCoordinate(0);
}

void MainWindow::on_selectToolCoordinate2_clicked()
{
    selectToolCoordinate(1);
}

void MainWindow::on_selectToolCoordinate3_clicked()
{
    selectToolCoordinate(2);
}

void MainWindow::on_selectToolCoordinate4_clicked()
{
    selectToolCoordinate(3);
}

void MainWindow::on_selectToolCoordinate5_clicked()
{
    selectToolCoordinate(4);
}

void MainWindow::selectToolCoordinate(int index)
{
    ui->toolCoordinateLineX->setText(QString::number(toolCoordinateSave[index][0]));
    ui->toolCoordinateLineY->setText(QString::number(toolCoordinateSave[index][1]));
    ui->toolCoordinateLineZ->setText(QString::number(toolCoordinateSave[index][2]));
    ui->toolCoordinateLineRX->setText(QString::number(toolCoordinateSave[index][3]));
    ui->toolCoordinateLineRY->setText(QString::number(toolCoordinateSave[index][4]));
    ui->toolCoordinateLineRZ->setText(QString::number(toolCoordinateSave[index][5]));
    ui->settingStack->setCurrentIndex(2);
}
