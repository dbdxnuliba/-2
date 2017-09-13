#include "addnodewindow.h"
#include "ui_addnodewindow.h"
#include <QDebug>
#include <QtCore>
#include "warningwindow.h"
extern AdsMessage_Write adsWriteStruct;
extern QString systemLanguage;
extern QString fileSavePath;

AddNodeWindow::AddNodeWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNodeWindow)
{
    ui->setupUi(this);

    //keyboard window
    keyboard4Function = new Keyboard(this);

    //slot for keyboard
    connect(keyboard4Function,SIGNAL(sendText(int, QString)), this, SLOT(createFunction(int, QString)));

    QPixmap bkgnd(":/images/background_number.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //disable windows headers
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    //switch to the first stack
    on_moveTab_released();
}

AddNodeWindow::~AddNodeWindow()
{
    delete ui;
}

void AddNodeWindow::functionsForThread(bool status)
{
    ui->stop_programButton->setDisabled(!status);
    ui->nodeMoveJButton->setDisabled(status);
    ui->nodeMoveLButton->setDisabled(status);
    ui->nodeMoveCButton->setDisabled(status);
    ui->threadButton->setDisabled(status);
}

void AddNodeWindow::functionWindowExpertMode(bool status)
{
    ui->threadButton->setVisible(status);
    ui->stop_programButton->setVisible(status);
    ui->run_programButton->setVisible(status);
    //ui->save_statusButton->setVisible(status);

    //ui->nodeMoveLButton->setVisible(status);
    //ui->nodeMoveCButton->setVisible(status);
    ui->nodeMovePsiButton->setVisible(status);
    ui->gripper->setVisible(status);
    ui->cameraGo->setVisible(status);
    ui->biaodingButton->setVisible(status);

    ui->threadTab->setVisible(status);
    ui->socketTab->setVisible(status);
    //ui->teachButton->setVisible(status);
    ui->save_statusButton->setVisible(status);
    ui->label_12->setVisible(status);
}

void AddNodeWindow::changeLanguage()
{
    if(systemLanguage == "cn"){
        ui->movejLabel->setText("MoveJ:关节运动");
        ui->movelLabel->setText("MoveL:末端直线运动");
        ui->movecLabel->setText("MoveC:末端圆弧运动");
        ui->scriptLanguage->setText("脚本语言");
        ui->sleepLabel->setText("等待");
        ui->ioOutLabel->setText("IO输出设置");
        ui->ioInLabel->setText("IO输入");
        ui->commentButton->setText("添加注释");
        ui->socketOpenLabel->setText("打开socket连接");
        ui->socketSendLabel->setText("发送信息");
        ui->socketRcvLabel->setText("接收信息");
        ui->socketCloseLabel->setText("关闭socket连接");
        ui->teachButton->setText("牵引复现");
        ui->teachReplayLabel->setText("选择需要记录的轨迹序号，点击确定后拖拽机械臂来记录轨迹\n蓝点表示该轨迹未被占用\n红点表示该轨迹已被占用，重新示教会被覆盖");

    }else if(systemLanguage == "en"){
        ui->movejLabel->setText("MoveJ:joint move");
        ui->movelLabel->setText("MoveL:robot end line move");
        ui->movecLabel->setText("MoveC:robot end arc move");
        ui->scriptLanguage->setText("Script language");
        ui->sleepLabel->setText("Wait");
        ui->ioOutLabel->setText("IO out set");
        ui->ioInLabel->setText("IO in read");
        ui->commentButton->setText("Add comments");
        ui->socketOpenLabel->setText("Open socket");
        ui->socketSendLabel->setText("Send message");
        ui->socketRcvLabel->setText("Receive message");
        ui->socketCloseLabel->setText("Close socket");
        ui->teachButton->setText("Teach Replay");
        ui->teachReplayLabel->setText("Select a path number to save teach path,\nClick OK and drag the robot to save the path\nBlue point: Path number not used\nRed point: Path number already used\nOld path will be covered if use red path");

    }
}

void AddNodeWindow::checkReplayNodeAvailable()
{
    QPixmap pixmapWarning(":/images/node_warning.png");
    QIcon ButtonIconWarning(pixmapWarning);
    QPixmap pixmapOK(":/images/node_ok.png");
    QIcon ButtonIconOK(pixmapOK);


    QFile checkFile(fileSavePath + "\\traj\\01.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_1->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_1->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_1->setIcon(ButtonIconOK);
        ui->pathStatusLabel_1->setIconSize(pixmapOK.rect().size());
    }

    checkFile.setFileName(fileSavePath + "\\traj\\02.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_2->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_2->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_2->setIcon(ButtonIconOK);
        ui->pathStatusLabel_2->setIconSize(pixmapOK.rect().size());
    }

    checkFile.setFileName(fileSavePath + "\\traj\\03.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_3->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_3->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_3->setIcon(ButtonIconOK);
        ui->pathStatusLabel_3->setIconSize(pixmapOK.rect().size());
    }

    checkFile.setFileName(fileSavePath + "\\traj\\04.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_4->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_4->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_4->setIcon(ButtonIconOK);
        ui->pathStatusLabel_4->setIconSize(pixmapOK.rect().size());
    }

    checkFile.setFileName(fileSavePath + "\\traj\\05.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_5->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_5->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_5->setIcon(ButtonIconOK);
        ui->pathStatusLabel_5->setIconSize(pixmapOK.rect().size());
    }

    checkFile.setFileName(fileSavePath + "\\traj\\06.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_6->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_6->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_6->setIcon(ButtonIconOK);
        ui->pathStatusLabel_6->setIconSize(pixmapOK.rect().size());
    }

    checkFile.setFileName(fileSavePath + "\\traj\\07.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_7->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_7->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_7->setIcon(ButtonIconOK);
        ui->pathStatusLabel_7->setIconSize(pixmapOK.rect().size());
    }

    checkFile.setFileName(fileSavePath + "\\traj\\08.txt");
    if(!checkFile.exists()){
        ui->pathStatusLabel_8->setIcon(ButtonIconWarning);
        ui->pathStatusLabel_8->setIconSize(pixmapWarning.rect().size());
    }else{
        ui->pathStatusLabel_8->setIcon(ButtonIconOK);
        ui->pathStatusLabel_8->setIconSize(pixmapOK.rect().size());
    }
}

void AddNodeWindow::on_nodeMoveJButton_clicked()
{
    emit moveJSelected();
}

void AddNodeWindow::on_nodeMoveLButton_clicked()
{
    emit moveLSelected(false);
}

void AddNodeWindow::on_nodeMoveL_PsiButton_clicked()
{
    emit moveLSelected(true);
}

void AddNodeWindow::on_nodeMoveCButton_clicked()
{
    emit moveCSelected();
}

void AddNodeWindow::on_nodeMovePsiButton_clicked()
{
    emit movePsiSelected();
}

void AddNodeWindow::on_nodeSleepButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("Sleep(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_socket_openButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("socket_open(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_socket_sendButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("socket_send(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_socket_rcvButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("socket_rcv(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_socket_closeButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("socket_close()");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->on_enter_clicked_external();
}

void AddNodeWindow::createFunction(int keyboardType, QString text)
{
    keyboardType;//this parameter is not used for now
    emit functionCreated(text);
    keyboard4Function->close();
}


void AddNodeWindow::on_threadButton_clicked()
{
    emit threadSelected();
}


void AddNodeWindow::on_stop_programButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("stop_program()");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->on_enter_clicked_external();
}

void AddNodeWindow::on_save_statusButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("save_status()");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->on_enter_clicked_external();
}

void AddNodeWindow::on_run_programButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("run_program(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}


void AddNodeWindow::on_set_tcpButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("set_tcp(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}


void AddNodeWindow::on_IO_outButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("io_out(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}


void AddNodeWindow::on_nodeMoveCenterButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("moveCenter(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_cameraGo_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("camera()");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_close_clicked()
{
    this->close(); //close the window
}


void AddNodeWindow::on_moveTab_released()
{
    QPixmap pixmap(":/images/icon-move_2.png");
    QIcon ButtonIcon(pixmap);
    ui->moveTab->setIcon(ButtonIcon);
    ui->moveTab->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-functions.png");
    QIcon ButtonIcon2(pixmap2);
    ui->functionsTab->setIcon(ButtonIcon2);
    ui->functionsTab->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-socket.png");
    QIcon ButtonIcon3(pixmap3);
    ui->socketTab->setIcon(ButtonIcon3);
    ui->socketTab->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-thread.png");
    QIcon ButtonIcon4(pixmap4);
    ui->threadTab->setIcon(ButtonIcon4);
    ui->threadTab->setIconSize(pixmap4.rect().size());

    ui->nodeStacksWidget->setCurrentIndex(0);
}


void AddNodeWindow::on_functionsTab_released()
{
    QPixmap pixmap(":/images/icon-move.png");
    QIcon ButtonIcon(pixmap);
    ui->moveTab->setIcon(ButtonIcon);
    ui->moveTab->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-functions_2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->functionsTab->setIcon(ButtonIcon2);
    ui->functionsTab->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-socket.png");
    QIcon ButtonIcon3(pixmap3);
    ui->socketTab->setIcon(ButtonIcon3);
    ui->socketTab->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-thread.png");
    QIcon ButtonIcon4(pixmap4);
    ui->threadTab->setIcon(ButtonIcon4);
    ui->threadTab->setIconSize(pixmap4.rect().size());

    ui->nodeStacksWidget->setCurrentIndex(1);
}


void AddNodeWindow::on_socketTab_released()
{
    QPixmap pixmap(":/images/icon-move.png");
    QIcon ButtonIcon(pixmap);
    ui->moveTab->setIcon(ButtonIcon);
    ui->moveTab->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-functions.png");
    QIcon ButtonIcon2(pixmap2);
    ui->functionsTab->setIcon(ButtonIcon2);
    ui->functionsTab->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-socket_2.png");
    QIcon ButtonIcon3(pixmap3);
    ui->socketTab->setIcon(ButtonIcon3);
    ui->socketTab->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-thread.png");
    QIcon ButtonIcon4(pixmap4);
    ui->threadTab->setIcon(ButtonIcon4);
    ui->threadTab->setIconSize(pixmap4.rect().size());

    ui->nodeStacksWidget->setCurrentIndex(2);
}

void AddNodeWindow::on_threadTab_released()
{
    QPixmap pixmap(":/images/icon-move.png");
    QIcon ButtonIcon(pixmap);
    ui->moveTab->setIcon(ButtonIcon);
    ui->moveTab->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-functions.png");
    QIcon ButtonIcon2(pixmap2);
    ui->functionsTab->setIcon(ButtonIcon2);
    ui->functionsTab->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-socket.png");
    QIcon ButtonIcon3(pixmap3);
    ui->socketTab->setIcon(ButtonIcon3);
    ui->socketTab->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-thread_2.png");
    QIcon ButtonIcon4(pixmap4);
    ui->threadTab->setIcon(ButtonIcon4);
    ui->threadTab->setIconSize(pixmap4.rect().size());

    ui->nodeStacksWidget->setCurrentIndex(3);
}

void AddNodeWindow::on_teachButton_clicked()
{
    if(!QDir(fileSavePath + "\\traj").exists()) QDir().mkdir(fileSavePath + "\\traj");
    checkReplayNodeAvailable();
    ui->nodeStacksWidget->setCurrentIndex(4);
}


void AddNodeWindow::on_beginTeach_clicked()
{
    adsWriteStruct.beginTeach = true;




    /******************************************************/
    //set traction teach FLAG to true
    //adsWriteStruct.tractionTeach = true;

    /******************************************************/





    //emit startTeach();
    if(systemLanguage == "cn"){
        WarningWindow::getInstance()->setWarningText("牵引模式\n按下末端按钮并拖拽,记录轨迹");
    }else if(systemLanguage == "en"){
        WarningWindow::getInstance()->setWarningText("Teach: Drag the robot");
    }
    WarningWindow::getInstance()->exec();


    adsWriteStruct.beginTeach = false;

    //add replay node
    emit addReplayNode();


    QThread::msleep(1000);
    checkReplayNodeAvailable();

    /******************************************************/
    //set traction teach FLAG to false
    //adsWriteStruct.tractionTeach = false;

    /******************************************************/
}


void AddNodeWindow::on_gripper_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("gripper(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_biaodingButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("biaoding()");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->on_enter_clicked_external();
}

void AddNodeWindow::on_locus1_clicked()
{
    QPixmap pixmap(":/images/icon-1_2.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 1;
}

void AddNodeWindow::on_locus2_clicked()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2_2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 2;
}

void AddNodeWindow::on_locus3_clicked()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3_2.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 3;
}

void AddNodeWindow::on_locus4_clicked()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4_2.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 4;
}

void AddNodeWindow::on_locus5_clicked()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5_2.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 5;
}

void AddNodeWindow::on_locus6_clicked()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6_2.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 6;
}

void AddNodeWindow::on_locus7_clicked()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7_2.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 7;
}

void AddNodeWindow::on_locus8_clicked()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->locus1->setIcon(ButtonIcon);
    ui->locus1->setIconSize(pixmap.rect().size());
    QPixmap pixmap2(":/images/icon-2.png");
    QIcon ButtonIcon2(pixmap2);
    ui->locus2->setIcon(ButtonIcon2);
    ui->locus2->setIconSize(pixmap2.rect().size());
    QPixmap pixmap3(":/images/icon-3.png");
    QIcon ButtonIcon3(pixmap3);
    ui->locus3->setIcon(ButtonIcon3);
    ui->locus3->setIconSize(pixmap3.rect().size());
    QPixmap pixmap4(":/images/icon-4.png");
    QIcon ButtonIcon4(pixmap4);
    ui->locus4->setIcon(ButtonIcon4);
    ui->locus4->setIconSize(pixmap4.rect().size());
    QPixmap pixmap5(":/images/icon-5.png");
    QIcon ButtonIcon5(pixmap5);
    ui->locus5->setIcon(ButtonIcon5);
    ui->locus5->setIconSize(pixmap5.rect().size());
    QPixmap pixmap6(":/images/icon-6.png");
    QIcon ButtonIcon6(pixmap6);
    ui->locus6->setIcon(ButtonIcon6);
    ui->locus6->setIconSize(pixmap6.rect().size());
    QPixmap pixmap7(":/images/icon-7.png");
    QIcon ButtonIcon7(pixmap7);
    ui->locus7->setIcon(ButtonIcon7);
    ui->locus7->setIconSize(pixmap7.rect().size());
    QPixmap pixmap8(":/images/icon-8_2.png");
    QIcon ButtonIcon8(pixmap8);
    ui->locus8->setIcon(ButtonIcon8);
    ui->locus8->setIconSize(pixmap8.rect().size());

    adsWriteStruct.pathNum = 8;
}

void AddNodeWindow::on_commentButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("#");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}



void AddNodeWindow::on_scriptLanguage_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("");
    keyboard4Function->keyboardType = 3; //add script language
    keyboard4Function->exec();
}

void AddNodeWindow::on_ifButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("if(  ):");
    keyboard4Function->keyboardType = 3; //add function type
    keyboard4Function->exec();

    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("end");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->on_enter_clicked_external();
}

void AddNodeWindow::on_elifButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("elif(  ):");
    keyboard4Function->keyboardType = 3; //add function type
    keyboard4Function->exec();
}

void AddNodeWindow::on_elseButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("else:");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->on_enter_clicked_external();
}

void AddNodeWindow::on_whileButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("while(  ):");
    keyboard4Function->keyboardType = 3; //add function type
    keyboard4Function->exec();

    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("end");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->on_enter_clicked_external();
}

void AddNodeWindow::on_IO_inButton_clicked()
{
    //show keyboard to enter the function
    keyboard4Function->setModal(true);
    keyboard4Function->expertModeSwitch(expertMode);
    keyboard4Function->setText("io_in(");
    keyboard4Function->keyboardType = 2; //add function type
    keyboard4Function->exec();
}
