#ifndef ADDNODEWINDOW_H
#define ADDNODEWINDOW_H

#include <QDialog>
#include "keyboard.h"
#include "dataupdate.h"
#include <QFile>

//for Chinese character
#pragma execution_character_set("utf-8")

namespace Ui {
class AddNodeWindow;
}

class AddNodeWindow : public QDialog
{
    Q_OBJECT

private:
    Ui::AddNodeWindow *ui;
    Keyboard *keyboard4Function;                    //keyboard to edit functions

signals:
    void moveJSelected();                           //moveJ button ckicked
    void moveLSelected(bool psi);                   //moveL button ckicked
    void moveCSelected();                           //moveC button ckicked
    void movePsiSelected();                         //movePsi button ckicked
    void functionCreated(QString);                  //create a function
    void threadSelected();                          //thread button clicked
    void startTeach();                              //Start traction teach
    void addReplayNode();                           //add replay() function

public:
    explicit AddNodeWindow(QWidget *parent = 0);
    ~AddNodeWindow();
    void functionsForThread(bool status);           //in thread function, disable some nodes
    void functionWindowExpertMode(bool status);     //in expert mode, show all functions
    void changeLanguage();                          //change language
    void checkReplayNodeAvailable();                //check which number of replay node is used

private slots:
    void on_nodeMoveJButton_clicked();

    void on_nodeMoveLButton_clicked();

    void on_nodeMoveCButton_clicked();

    void on_nodeSleepButton_clicked();

    void on_socket_openButton_clicked();

    void on_socket_sendButton_clicked();

    void on_socket_rcvButton_clicked();

    void createFunction(int keyboardType, QString text);

    void on_socket_closeButton_clicked();

    void on_threadButton_clicked();

    void on_ifButton_clicked();

    void on_stop_programButton_clicked();

    void on_save_statusButton_clicked();

    void on_run_programButton_clicked();

    void on_set_tcpButton_clicked();

    void on_nodeMovePsiButton_clicked();

    void on_nodeMoveCenterButton_clicked();

    void on_cameraGo_clicked();

    void on_close_clicked();

    void on_moveTab_released();

    void on_functionsTab_released();

    void on_socketTab_released();

    void on_threadTab_released();

    void on_teachButton_clicked();

    void on_locus1_clicked();

    void on_locus2_clicked();

    void on_beginTeach_clicked();

    void on_IO_outButton_clicked();

    void on_nodeMoveL_PsiButton_clicked();

    void on_gripper_clicked();

    void on_biaodingButton_clicked();

    void on_locus3_clicked();

    void on_locus4_clicked();

    void on_locus5_clicked();

    void on_locus6_clicked();

    void on_locus7_clicked();

    void on_locus8_clicked();

    void on_commentButton_clicked();
    void on_scriptLanguage_clicked();
    void on_elifButton_clicked();
    void on_elseButton_clicked();
    void on_whileButton_clicked();
    void on_IO_inButton_clicked();
};

#endif // ADDNODEWINDOW_H
