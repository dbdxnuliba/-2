#include "numberboard.h"
#include "ui_numberboard.h"
#include "warningwindow.h"
#include <QDebug>

extern AdsMessage_Write adsWriteStruct;
extern AdsMessage_Read  adsReadStruct;
extern QString systemLanguage;
extern bool programIsRunning;
extern double programAxisMoveTo[7];

NumberBoard::NumberBoard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NumberBoard),
    axisGroupVisible(false),        //enter axis position value group box visible
    positionGroupVisible(false),    //enter end position value group box visible
    activeAxisNum(0),               //current selected axis block on the window
    activePositionNum(0),           //current selected end position block on the window
    windowIsOpened(false),          //an enter axis/end position value window is opened
    editLineNum(0)                  //1:speed, 2:acceleration, 3:tool coordinate X,
                                    //4:tool coordinate Y, 5:tool coordinate Z
                                    //6:transition semidiameter
{
    ui->setupUi(this);
    moveToThread = new MoveToThread();

    //disable windows headers
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

NumberBoard::~NumberBoard()
{
    delete moveToThread;
    delete ui;
}

void NumberBoard::cleanLineEdit()
{
    on_numberBoardClear_clicked();
}

void NumberBoard::changeWindowSize()
{
    //set axis group visible or not
    ui->axisGroupBox->setVisible(axisGroupVisible);
    ui->positionGroupBox->setVisible(positionGroupVisible);

    QPixmap bkgnd(":/images/background_number.png");
    QPalette palette;
    //change window size
    if(axisGroupVisible){
        resize(500, 273);
        setGeometry(310, 160, 500, 273);
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);


        ui->axisLineEdit1->setText(QString::number(adsReadStruct.axisCurrentAngle[0], 'f', 2 ));
        ui->axisLineEdit2->setText(QString::number(adsReadStruct.axisCurrentAngle[1], 'f', 2 ));
        ui->axisLineEdit3->setText(QString::number(adsReadStruct.axisCurrentAngle[2], 'f', 2 ));
        ui->axisLineEdit4->setText(QString::number(adsReadStruct.axisCurrentAngle[3], 'f', 2 ));
        ui->axisLineEdit5->setText(QString::number(adsReadStruct.axisCurrentAngle[4], 'f', 2 ));
        ui->axisLineEdit6->setText(QString::number(adsReadStruct.axisCurrentAngle[5], 'f', 2 ));
        ui->axisLineEdit7->setText(QString::number(adsReadStruct.axisCurrentAngle[6], 'f', 2 ));

        ui->pressToMove->setVisible(true);
        ui->numberBoardOK->setVisible(false);

        //disable the number buttons first
        enableDisableButtons(false);
    }else if(positionGroupVisible){
        resize(500, 273);
        setGeometry(310, 160, 500, 273);
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);

        if(adsWriteStruct.statusUseToolCoordinate){
            ui->xLineEdit->setText(QString::number(adsReadStruct.currentFlangePositionTool[0], 'f', 2 ));
            ui->yLineEdit->setText(QString::number(adsReadStruct.currentFlangePositionTool[1], 'f', 2 ));
            ui->zLineEdit->setText(QString::number(adsReadStruct.currentFlangePositionTool[2], 'f', 2 ));
            ui->rxLineEdit->setText(QString::number(adsReadStruct.currentFlangePositionTool[3], 'f', 2 ));
            ui->ryLineEdit->setText(QString::number(adsReadStruct.currentFlangePositionTool[4], 'f', 2 ));
            ui->rzLineEdit->setText(QString::number(adsReadStruct.currentFlangePositionTool[5], 'f', 2 ));
        }else{
            ui->xLineEdit->setText(QString::number(adsReadStruct.currentFlangePosition[0], 'f', 2 ));
            ui->yLineEdit->setText(QString::number(adsReadStruct.currentFlangePosition[1], 'f', 2 ));
            ui->zLineEdit->setText(QString::number(adsReadStruct.currentFlangePosition[2], 'f', 2 ));
            ui->rxLineEdit->setText(QString::number(adsReadStruct.currentFlangePosition[3], 'f', 2 ));
            ui->ryLineEdit->setText(QString::number(adsReadStruct.currentFlangePosition[4], 'f', 2 ));
            ui->rzLineEdit->setText(QString::number(adsReadStruct.currentFlangePosition[5], 'f', 2 ));
        }

        ui->pressToMove->setVisible(true);
        ui->numberBoardOK->setVisible(false);

        //disable the number buttons first
        enableDisableButtons(false);
    }
    else {
        //enable the buttons
        ui->pressToMove->setVisible(false);
        ui->numberBoardOK->setVisible(true);
        enableDisableButtons(true);
        resize(290, 273);
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);
    }
}

void NumberBoard::updateValueOnAxisButtons()
{
    if(ui->axisGroupBox->isVisible()){
        switch(activeAxisNum){
            case 1:
                ui->axisLineEdit1->setText(ui->numberBoardLineEdit->text());
            break;

            case 2:
                ui->axisLineEdit2->setText(ui->numberBoardLineEdit->text());
            break;

            case 3:
                ui->axisLineEdit3->setText(ui->numberBoardLineEdit->text());
            break;

            case 4:
                ui->axisLineEdit4->setText(ui->numberBoardLineEdit->text());
            break;

            case 5:
                ui->axisLineEdit5->setText(ui->numberBoardLineEdit->text());
            break;

            case 6:
                ui->axisLineEdit6->setText(ui->numberBoardLineEdit->text());
            break;

            case 7:
                ui->axisLineEdit7->setText(ui->numberBoardLineEdit->text());
            break;
        }
    }
}

void NumberBoard::updateValueOnPositionButtons()
{
    if(ui->positionGroupBox->isVisible()){
        switch(activePositionNum){
            case 1:
                ui->xLineEdit->setText(ui->numberBoardLineEdit->text());
            break;

            case 2:
                ui->yLineEdit->setText(ui->numberBoardLineEdit->text());
            break;

            case 3:
                ui->zLineEdit->setText(ui->numberBoardLineEdit->text());
            break;

            case 4:
                ui->rxLineEdit->setText(ui->numberBoardLineEdit->text());
            break;

            case 5:
                ui->ryLineEdit->setText(ui->numberBoardLineEdit->text());
            break;

            case 6:
                ui->rzLineEdit->setText(ui->numberBoardLineEdit->text());
            break;
        }
    }
}

void NumberBoard::on_numberBoardOK_clicked()
{
    //enter value axis move
    if(ui->axisGroupBox->isVisible()){
        //joint move

        //check if all the values are valid
        if(ui->axisLineEdit1->text().toDouble()<-179 || ui->axisLineEdit1->text().toDouble() > 179 ||
           ui->axisLineEdit2->text().toDouble()<-105  || ui->axisLineEdit2->text().toDouble() > 105  ||
           ui->axisLineEdit3->text().toDouble()<-179 || ui->axisLineEdit3->text().toDouble() > 179 ||
           ui->axisLineEdit4->text().toDouble()<-120  || ui->axisLineEdit4->text().toDouble() > 120  ||
           ui->axisLineEdit5->text().toDouble()<-179 || ui->axisLineEdit5->text().toDouble() > 179 ||
           ui->axisLineEdit6->text().toDouble()<-110  || ui->axisLineEdit6->text().toDouble() > 110  ||
           ui->axisLineEdit7->text().toDouble()<-179 || ui->axisLineEdit7->text().toDouble() > 179){
            //value not valid
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("警告: 关节值超限");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Warning: Joint values overrun");
            }
            WarningWindow::getInstance()->exec();

            emit sendNumber(0, ""); //just make the window close
            return;
        }

        adsWriteStruct.progStop = false;
        adsWriteStruct.enteredAxisValue[0] = ui->axisLineEdit1->text().toDouble();
        adsWriteStruct.enteredAxisValue[1] = ui->axisLineEdit2->text().toDouble();
        adsWriteStruct.enteredAxisValue[2] = ui->axisLineEdit3->text().toDouble();
        adsWriteStruct.enteredAxisValue[3] = ui->axisLineEdit4->text().toDouble();
        adsWriteStruct.enteredAxisValue[4] = ui->axisLineEdit5->text().toDouble();
        adsWriteStruct.enteredAxisValue[5] = ui->axisLineEdit6->text().toDouble();
        adsWriteStruct.enteredAxisValue[6] = ui->axisLineEdit7->text().toDouble();
        adsWriteStruct.progRunSpeed = 70;//default moveto speed
        adsWriteStruct.jogMode = 4;

        QThread::msleep(100);//wait for sync
        adsWriteStruct.jogMode = 0; //reset

        emit sendNumber(0, ""); //just close the window
    }else if(ui->positionGroupBox->isVisible()){
        //check if all the values are valid
        if(ui->xLineEdit->text().toDouble()<-999 || ui->xLineEdit->text().toDouble() > 999 ||
           ui->yLineEdit->text().toDouble()<-999  || ui->yLineEdit->text().toDouble() > 999  ||
           ui->zLineEdit->text().toDouble()<-211 || ui->zLineEdit->text().toDouble() > 1372 ||
           ui->rxLineEdit->text().toDouble()<-360  || ui->rxLineEdit->text().toDouble() > 360  ||
           ui->ryLineEdit->text().toDouble()<-360 || ui->ryLineEdit->text().toDouble() > 360 ||
           ui->rzLineEdit->text().toDouble()<-360  || ui->rzLineEdit->text().toDouble() > 360  ){
            //value not valid
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("警告: 位置超限");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Warning: End position overrun");
            }
            WarningWindow::getInstance()->exec();
            emit sendNumber(0, ""); //just make the window close
            return;
        }

        //enter value position move
        adsWriteStruct.progStop=false;
        adsWriteStruct.enteredXyzValue[0] = ui->xLineEdit->text().toDouble();
        adsWriteStruct.enteredXyzValue[1] = ui->yLineEdit->text().toDouble();
        adsWriteStruct.enteredXyzValue[2] = ui->zLineEdit->text().toDouble();
        adsWriteStruct.enteredXyzValue[3] = ui->rxLineEdit->text().toDouble();
        adsWriteStruct.enteredXyzValue[4] = ui->ryLineEdit->text().toDouble();
        adsWriteStruct.enteredXyzValue[5] = ui->rzLineEdit->text().toDouble();
        adsWriteStruct.progRunSpeed = 70;//default moveto speed
        adsWriteStruct.jogMode = 5;

        QThread::msleep(100);//wait for sync
        adsWriteStruct.jogMode = 0; //reset

        emit sendNumber(0, ""); //just close the window

    }else{
        //normal send value
        emit sendNumber(editLineNum, ui->numberBoardLineEdit->text());
    }
}

void NumberBoard::on_numberBoard0_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "0");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoardPoint_clicked()
{
    //check if there is already a point
    if(!ui->numberBoardLineEdit->text().contains("."))
        ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + ".");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard1_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "1");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard2_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "2");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard3_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "3");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard4_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "4");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard5_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "5");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard6_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "6");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard7_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "7");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard8_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "8");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoard9_clicked()
{
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text() + "9");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoardBack_clicked()
{
    int length = ui->numberBoardLineEdit->text().length();
    ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text().remove(length - 1, 1));
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoardClear_clicked()
{
    ui->numberBoardLineEdit->setText("");
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::on_numberBoardMinus_clicked()
{
    //check if there is already a minus
    if(ui->numberBoardLineEdit->text().at(0) == '-')
        ui->numberBoardLineEdit->setText(ui->numberBoardLineEdit->text().split('-').value(1));
    else
        ui->numberBoardLineEdit->setText('-' + ui->numberBoardLineEdit->text());
    updateValueOnAxisButtons();
    updateValueOnPositionButtons();
}

void NumberBoard::enableDisableButtons(bool status){
    ui->numberBoardPoint->setEnabled(status);
    ui->numberBoardMinus->setEnabled(status);
    ui->numberBoard0->setEnabled(status);
    ui->numberBoard3->setEnabled(status);
    ui->numberBoard2->setEnabled(status);
    ui->numberBoard1->setEnabled(status);
    ui->numberBoard6->setEnabled(status);
    ui->numberBoard5->setEnabled(status);
    ui->numberBoard4->setEnabled(status);
    ui->numberBoard9->setEnabled(status);
    ui->numberBoard8->setEnabled(status);
    ui->numberBoard7->setEnabled(status);
}

void NumberBoard::setLabelText(QString text)
{
    ui->numberBoardLabel->setText(text);
}

void NumberBoard::changeLanguage()
{
    if(systemLanguage == "cn"){
        ui->pressToMove->setText("按住移动");
        QPixmap pixmap(":/images/num_cancel.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardCancel->setIcon(ButtonIcon);
        ui->numberBoardCancel->setIconSize(pixmap.rect().size());
        pixmap.load(":/images/numOK.png");
        ButtonIcon.addPixmap(pixmap);
        ui->numberBoardOK->setIcon(ButtonIcon);
        ui->numberBoardOK->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        ui->pressToMove->setText("Press Move");
        QPixmap pixmap(":/img_en/img_en/num_cancel-en.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardCancel->setIcon(ButtonIcon);
        ui->numberBoardCancel->setIconSize(pixmap.rect().size());
        pixmap.load(":/img_en/img_en/numOK.png");
        ButtonIcon.addPixmap(pixmap);
        ui->numberBoardOK->setIcon(ButtonIcon);
        ui->numberBoardOK->setIconSize(pixmap.rect().size());

    }
}

void NumberBoard::on_axisLineEdit1_clicked(){
    activeAxisNum = 1;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->axisLineEdit1->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->axisLineEdit2->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit3->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit4->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit5->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit6->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit7->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");}
void NumberBoard::on_axisLineEdit2_clicked(){
    activeAxisNum = 2;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->axisLineEdit2->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->axisLineEdit1->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit3->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit4->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit5->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit6->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit7->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");}
void NumberBoard::on_axisLineEdit3_clicked(){
    activeAxisNum = 3;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->axisLineEdit3->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->axisLineEdit1->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit2->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit4->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit5->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit6->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit7->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");}
void NumberBoard::on_axisLineEdit4_clicked(){
    activeAxisNum = 4;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->axisLineEdit4->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->axisLineEdit1->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit2->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit3->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit5->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit6->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit7->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");}
void NumberBoard::on_axisLineEdit5_clicked(){
    activeAxisNum = 5;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->axisLineEdit5->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->axisLineEdit1->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit2->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit3->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit4->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit6->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit7->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");}
void NumberBoard::on_axisLineEdit6_clicked(){
    activeAxisNum = 6;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->axisLineEdit6->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->axisLineEdit1->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit2->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit3->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit4->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit5->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit7->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");}
void NumberBoard::on_axisLineEdit7_clicked(){
    activeAxisNum = 7;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->axisLineEdit7->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->axisLineEdit1->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit2->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit3->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit4->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit5->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->axisLineEdit6->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");}

void NumberBoard::on_xLineEdit_clicked()
{
    activePositionNum = 1;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->xLineEdit->setStyleSheet(" color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->yLineEdit->setStyleSheet(" color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->zLineEdit->setStyleSheet(" color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->rxLineEdit->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->ryLineEdit->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->rzLineEdit->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
}

void NumberBoard::on_yLineEdit_clicked()
{
    activePositionNum = 2;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->yLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(0, 255, 255, 40);");
    ui->xLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->zLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->rxLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->ryLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->rzLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
}

void NumberBoard::on_zLineEdit_clicked()
{
    activePositionNum = 3;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->zLineEdit->setStyleSheet(" color:rgb(179, 181, 180); background-color: rgba(0, 255, 255, 40);");
    ui->yLineEdit->setStyleSheet(" color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->xLineEdit->setStyleSheet(" color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->rxLineEdit->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->ryLineEdit->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
    ui->rzLineEdit->setStyleSheet("color:rgb(179, 181, 180); background-color: rgba(255, 255, 255, 40);");
}

void NumberBoard::on_rxLineEdit_clicked()
{
    activePositionNum = 4;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->rxLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(0, 255, 255, 40);");
    ui->yLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->zLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->xLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->ryLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->rzLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
}

void NumberBoard::on_ryLineEdit_clicked()
{
    activePositionNum = 5;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->ryLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(0, 255, 255, 40);");
    ui->yLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->zLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->rxLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->xLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->rzLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
}

void NumberBoard::on_rzLineEdit_clicked()
{
    activePositionNum = 6;
    ui->numberBoardLineEdit->clear();
    enableDisableButtons(true);
    ui->rzLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(0, 255, 255, 40);");
    ui->yLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->zLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->rxLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->ryLineEdit->setStyleSheet("color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
    ui->xLineEdit->setStyleSheet(" color:rgb(179, 181, 180);background-color: rgba(255, 255, 255, 40);");
}

void NumberBoard::on_numberBoardCancel_clicked()
{
    emit sendNumber(0, ""); //just close the window
}

void NumberBoard::on_numberBoardBack_pressed()
{
    QPixmap pixmap(":/images/numCancel_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardBack->setIcon(ButtonIcon);
    ui->numberBoardBack->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoardBack_released()
{
    QPixmap pixmap(":/images/numCancel.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardBack->setIcon(ButtonIcon);
    ui->numberBoardBack->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoardClear_pressed()
{
    QPixmap pixmap(":/images/numClear_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardClear->setIcon(ButtonIcon);
    ui->numberBoardClear->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoardClear_released()
{
    QPixmap pixmap(":/images/numClear.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardClear->setIcon(ButtonIcon);
    ui->numberBoardClear->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoardCancel_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/num_cancel_2.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardCancel->setIcon(ButtonIcon);
        ui->numberBoardCancel->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/num_cancel_2-en.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardCancel->setIcon(ButtonIcon);
        ui->numberBoardCancel->setIconSize(pixmap.rect().size());
    }
}

void NumberBoard::on_numberBoardCancel_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/num_cancel.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardCancel->setIcon(ButtonIcon);
        ui->numberBoardCancel->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/num_cancel-en.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardCancel->setIcon(ButtonIcon);
        ui->numberBoardCancel->setIconSize(pixmap.rect().size());
    }
}

void NumberBoard::on_numberBoardOK_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardOK->setIcon(ButtonIcon);
        ui->numberBoardOK->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardOK->setIcon(ButtonIcon);
        ui->numberBoardOK->setIconSize(pixmap.rect().size());
    }
}

void NumberBoard::on_numberBoardOK_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardOK->setIcon(ButtonIcon);
        ui->numberBoardOK->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->numberBoardOK->setIcon(ButtonIcon);
        ui->numberBoardOK->setIconSize(pixmap.rect().size());
    }
}

void NumberBoard::on_numberBoardPoint_pressed()
{
    QPixmap pixmap(":/images/numDot_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardPoint->setIcon(ButtonIcon);
    ui->numberBoardPoint->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoardPoint_released()
{
    QPixmap pixmap(":/images/numDot.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardPoint->setIcon(ButtonIcon);
    ui->numberBoardPoint->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoardMinus_pressed()
{
    QPixmap pixmap(":/images/numMinus_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardMinus->setIcon(ButtonIcon);
    ui->numberBoardMinus->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoardMinus_released()
{
    QPixmap pixmap(":/images/numMinus.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoardMinus->setIcon(ButtonIcon);
    ui->numberBoardMinus->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard0_pressed()
{
    QPixmap pixmap(":/images/num0_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard0->setIcon(ButtonIcon);
    ui->numberBoard0->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard0_released()
{
    QPixmap pixmap(":/images/num0.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard0->setIcon(ButtonIcon);
    ui->numberBoard0->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard1_pressed()
{
    QPixmap pixmap(":/images/num1_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard1->setIcon(ButtonIcon);
    ui->numberBoard1->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard1_released()
{
    QPixmap pixmap(":/images/num1.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard1->setIcon(ButtonIcon);
    ui->numberBoard1->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard2_pressed()
{
    QPixmap pixmap(":/images/num2_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard2->setIcon(ButtonIcon);
    ui->numberBoard2->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard2_released()
{
    QPixmap pixmap(":/images/num2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard2->setIcon(ButtonIcon);
    ui->numberBoard2->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard3_pressed()
{
    QPixmap pixmap(":/images/num3_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard3->setIcon(ButtonIcon);
    ui->numberBoard3->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard3_released()
{
    QPixmap pixmap(":/images/num3.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard3->setIcon(ButtonIcon);
    ui->numberBoard3->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard4_pressed()
{
    QPixmap pixmap(":/images/num4_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard4->setIcon(ButtonIcon);
    ui->numberBoard4->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard4_released()
{
    QPixmap pixmap(":/images/num4.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard4->setIcon(ButtonIcon);
    ui->numberBoard4->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard5_pressed()
{
    QPixmap pixmap(":/images/num5_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard5->setIcon(ButtonIcon);
    ui->numberBoard5->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard5_released()
{
    QPixmap pixmap(":/images/num5.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard5->setIcon(ButtonIcon);
    ui->numberBoard5->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard6_pressed()
{
    QPixmap pixmap(":/images/num6_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard6->setIcon(ButtonIcon);
    ui->numberBoard6->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard6_released()
{
    QPixmap pixmap(":/images/num6.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard6->setIcon(ButtonIcon);
    ui->numberBoard6->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard7_pressed()
{
    QPixmap pixmap(":/images/num7_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard7->setIcon(ButtonIcon);
    ui->numberBoard7->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard7_released()
{
    QPixmap pixmap(":/images/num7.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard7->setIcon(ButtonIcon);
    ui->numberBoard7->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard8_pressed()
{
    QPixmap pixmap(":/images/num8_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard8->setIcon(ButtonIcon);
    ui->numberBoard8->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard8_released()
{
    QPixmap pixmap(":/images/num8.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard8->setIcon(ButtonIcon);
    ui->numberBoard8->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard9_pressed()
{
    QPixmap pixmap(":/images/num9_2.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard9->setIcon(ButtonIcon);
    ui->numberBoard9->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_numberBoard9_released()
{
    QPixmap pixmap(":/images/num9.png");
    QIcon ButtonIcon(pixmap);
    ui->numberBoard9->setIcon(ButtonIcon);
    ui->numberBoard9->setIconSize(pixmap.rect().size());
}

void NumberBoard::on_pressToMove_pressed()
{
    emit startEnterValueMove();

    //enter value axis move
    if(ui->axisGroupBox->isVisible()){
        //joint move

        //check if all the values are valid
        if(ui->axisLineEdit1->text().toDouble()<-179 || ui->axisLineEdit1->text().toDouble() > 179 ||
           ui->axisLineEdit2->text().toDouble()<-105  || ui->axisLineEdit2->text().toDouble() > 105  ||
           ui->axisLineEdit3->text().toDouble()<-179 || ui->axisLineEdit3->text().toDouble() > 179 ||
           ui->axisLineEdit4->text().toDouble()<-120  || ui->axisLineEdit4->text().toDouble() > 120  ||
           ui->axisLineEdit5->text().toDouble()<-179 || ui->axisLineEdit5->text().toDouble() > 179 ||
           ui->axisLineEdit6->text().toDouble()<-110  || ui->axisLineEdit6->text().toDouble() > 110  ||
           ui->axisLineEdit7->text().toDouble()<-179 || ui->axisLineEdit7->text().toDouble() > 179){
            //value not valid
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("警告: 关节值超限");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Warning: Joint values overrun");
            }
            WarningWindow::getInstance()->exec();

            return;
        }

        //detect if a program is running
        if(!programIsRunning){
            //move to original point
            adsWriteStruct.progStop = false;

            programAxisMoveTo[0] = ui->axisLineEdit1->text().toDouble();
            programAxisMoveTo[1] = ui->axisLineEdit2->text().toDouble();
            programAxisMoveTo[2] = ui->axisLineEdit3->text().toDouble();
            programAxisMoveTo[3] = ui->axisLineEdit4->text().toDouble();
            programAxisMoveTo[4] = ui->axisLineEdit5->text().toDouble();
            programAxisMoveTo[5] = ui->axisLineEdit6->text().toDouble();
            programAxisMoveTo[6] = ui->axisLineEdit7->text().toDouble();


            //Start moveto thread
            moveToThread->start();
        }

    }else if(ui->positionGroupBox->isVisible()){
        //check if all the values are valid
        if(ui->xLineEdit->text().toDouble()<-999 || ui->xLineEdit->text().toDouble() > 999 ||
           ui->yLineEdit->text().toDouble()<-999  || ui->yLineEdit->text().toDouble() > 999  ||
           ui->zLineEdit->text().toDouble()<-211 || ui->zLineEdit->text().toDouble() > 1372 ||
           ui->rxLineEdit->text().toDouble()<-360  || ui->rxLineEdit->text().toDouble() > 360  ||
           ui->ryLineEdit->text().toDouble()<-360 || ui->ryLineEdit->text().toDouble() > 360 ||
           ui->rzLineEdit->text().toDouble()<-360  || ui->rzLineEdit->text().toDouble() > 360  ){
            //value not valid
            if(systemLanguage == "cn"){
                WarningWindow::getInstance()->setWarningText("警告: 位置超限");
            }else if(systemLanguage == "en"){
                WarningWindow::getInstance()->setWarningText("Warning: End position overrun");
            }
            WarningWindow::getInstance()->exec();

            return;
        }

        //detect if a program is running
        if(!programIsRunning){
            //move to original point
            adsWriteStruct.progStop = false;

            //enter value position move
            adsWriteStruct.enteredXyzValue[0] = ui->xLineEdit->text().toDouble();
            adsWriteStruct.enteredXyzValue[1] = ui->yLineEdit->text().toDouble();
            adsWriteStruct.enteredXyzValue[2] = ui->zLineEdit->text().toDouble();
            adsWriteStruct.enteredXyzValue[3] = ui->rxLineEdit->text().toDouble();
            adsWriteStruct.enteredXyzValue[4] = ui->ryLineEdit->text().toDouble();
            adsWriteStruct.enteredXyzValue[5] = ui->rzLineEdit->text().toDouble();

            //Start moveto thread
            moveToThread->endPositionMove = true;
            moveToThread->start();
        }
    }
}

void NumberBoard::on_pressToMove_released()
{
    emit stopEnterValueMove();
}
