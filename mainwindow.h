#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include "dataupdate.h"
#include "program/runprogram.h"
#include "UI/addnodewindow.h"
#include "UI/numberboard.h"
#include "UI/keyboard.h"
#include "TCP/tcpserver.h"
#include "threads/movetothread.h"
#include "threads/loadingthread.h"
#include "threads/autologin.h"
#include "UI/warningwindow.h"
#include <QProcess>
#include <QWindow>
#include <windows.h>
#include <GL/gl.h>
#include <QLibrary>
#include <QSerialPort>
#include "camera/calibration.h"



//for Chinese character
#pragma execution_character_set("utf-8")

namespace Ui {class MainWindow;}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startInit(); //init parameters when starting this software

private:
    QString programName;                            //the name of the program file which will be modified
    QString runProgramName;                         //the name of the program which will be run

    Ui::MainWindow *ui;                             //use this value to reach all widgets in UI
    DataUpdate *updateData;                         //a thread to update real-time data
    RunProgram *runProgram;                         //explain the script program to action commands
    AddNodeWindow *nodeWindow;                      //add node window in program
    NumberBoard *numberBoard;                       //numberboard
    Keyboard *keyboard;                             //keyboard
    TcpServer *Server;                              //a tcp server listening to other PC
    TcpServer *StatusFeedback;                      //a tcp server sending status feedback
    QTcpSocket *fun_socket;                         //this socket work in robot program
    MoveToThread *moveToThread;                     //let the moveto function run in a thread
    QSerialPort *serialPort;                        //serial port control for teach pendant
    LoadingThread *loadingThread;                   //a thread to load the page when robot Start
    QProcess *myProcess;                            //a process run camera program
    QWindow *m_window;
    QWidget *m_widget;
    calibration *cal;                               //camera calibration
    AutoLogin   *autoLogin;                         //program auto login, enable all axis

    void startJogAxis(int num, int direction);      //jog axis button pressed
    void stopJogAxis();                             //jog axis button released
    void stopJogLine();                             //jog end button released
    void robotWindowChangePosition(int type);       //change the 3D robot view to different windows
    int selectProgramHandler(QString name = "");    //open a program to run
    void openProgramHandler(QString name);          //open a program to modify
    void saveProgramHandler(QString programName);   //save all the nodes in the program tree into program
    void saveTheNewProgramTree();                   //save the ancient modify to the program(used for undo/redo)
    void setSystemLanguage(QString language);       //change system language
    QTreeWidgetItem* programAddRoot(QString name);  //add a root node to the program tree
    QTreeWidgetItem* programAddChild(QTreeWidgetItem *currentItm, QString name); //add a child to the root node


private slots:
    //functions below are slots connected to the signals from other classes
    void updateAxisPosition(double value, int axisNum);         //refresh axis position on the screen
    void updateFlangePosition(double value, int positionNum);   //refresh end position on the screen
    void update3DView(double* newAngle);                        //update robot 3D view
    void updateDataAdsError(long adsError);                     //use ads connect to TwinCAT error
    void updateDataTcpError(QString tcpAdd);                    //use tcp connect to media program error
    void addMoveLNode(bool psi);                                //in robot program, add a MoveL node
    void addMoveJNode();                                        //in robot program, add a MoveJ node
    void addMoveCNode();                                        //in robot program, add a MoveC node
    void addMovePsiNode();                                      //in robot program, add a MovePsi node
    void addThreadNode();                                       //in robot program, add a thread node
    void addProgramFunction(QString functionCmd);               //in robot program, add a function node
    void addReplayNode();                                       //in robot program, add teach replay node
    void runCommand(int lineNum);                               //indicate current running command line
    void stopRunProgram();                                      //stop the program when program finished or has error
    void moveToSelectedNode();                                  //call moveToThread move to the target position
    void moveToOriginalPoint();                                 //call moveToThread move to the original position
    void runTargetProgram(QString programPath);                 //run the target program
    void setRunProgramSpeed2UI(int speedPercentage);            //change run program speed percentage
    void collisionDetected(int collisionID);                    //this function will be called when collision detected
    void lockJogScreen(bool lock);                              //disable jog window to prevent miscontrol
    void teachLockScreen(bool lock);
    void jogAxisMoveButtonStatus(bool status, bool status2);    //enable/disable joint/end jog buttons
    void exceedJointLimit();                                    //function will be called when robot reached joint limits
    void exceedJointLimitClear();                               //clear the status from exceed joint limit
    void getSerialPortMsg();                                    //teach pendant serial port input
    //void autoRunProgram();                                      //automatically run program when Start
    void emcButtonCheckThreadHandler(bool status);              //emergency button pushed down/released up
    void changeRunButtonStatus(bool status);                    //toggle the run button status when stop a program
    void startUpLoadDone();                                     //Start up gif image load done
    void externalIOReadUpdate(bool* ioStatus);                  //update IO input status
    void externalIOWriteUpdate(bool *ioStatus);                 //update IO output status
    void externalToolIOReadUpdate(bool* ioStatus);              //update tool IO input status
    void externalToolIOWriteUpdate(bool *ioStatus);             //update tool IO output status
    void changeToolCoordinate(QStringList toolCoordinate);      //change tool coordinate value on UI
    void switchExpertMode(bool status);                         //switch to expert mode or switch back
    bool functionValidCheck(QString func);                      //check if all commands in program are valid
    void switchJogSpiButton(bool status);                       //set end fix jog buttons enable/disable
    void endToolButtonStatus(bool status);                      //button on the tool end pressed or released
    void startTeach();                                          //switch to traction teach mode
    void resetSystem();                                         //system reset
    void startEnterValueMove();                                 //in jog mode, start enter value move
    void stopEnterValueMove();                                  //in jog mode, stop enter value move
    void robotPowerOff();                                       //called when robot suddenly power off
    void robotBreakState(bool state);                           //when robot all axis break released, enable buttons
    void axisError();                                           //when robot axis error detected
    void axisWarning();                                         //when robot axis warning detected
    void pressButtonAddNode();                                  //press tool button2 to save node
    void selfCollisionDetect();                                 //when robot collide itself
    void wallCollisionDetect();
    void programError(QString msg);                             //program syntax error

    void fun_socket_open(QString address, int port, int timeout);   //create a tcp client, connect to server called by function in program
    void fun_socket_send(QString message);                      //send tcp message to server
    void fun_socket_rcv(int timeout);                           //receive tcp message from the server
    void fun_socket_close();                                    //close the tcp client

    void hideRunButtons(bool stop, bool run, bool step, bool pause);    //set run/stop/pause buttons visible or not
    void updateProgramNumber(int whichOne, QString value);      //called by numberboard, modify speed/blend radius in program or set tool coordinate
    void updateKeyboardText(int keyboardType, QString text);    //called by soft keyboard, create program or modify functions in program

    void addNode2Simulator(QString func);                       //add program node to the 3D robot simulator
    void showSystemIP();
    void checkRepeatProgram();                                  //check if need to repeat program
    void showProgramRunTime(int time);                          //show program run time
    void stopScript();                                          //stop script program
    void pauseScript();                                         //pause script program
    void resumeScript();                                        //resume script program
    void readXMLFile();                                         //read XML file to init axis bias
    void stopRepeat();
    void updateSlaveStatus(bool* status);                       //update slave status
    void tcpConnected(bool port2000, bool port2001);            //tcp port connect status

    //functions below are slots connected to the UI widgets
    //axis angle
    void on_axis1MoveBackward_pressed();
    void on_axis1MoveBackward_released();
    void on_axis1MoveForward_pressed();
    void on_axis1MoveForward_released();
    void on_axis2MoveBackward_pressed();
    void on_axis2MoveBackward_released();
    void on_axis2MoveForward_pressed();
    void on_axis2MoveForward_released();
    void on_axis3MoveBackward_pressed();
    void on_axis3MoveBackward_released();
    void on_axis3MoveForward_pressed();
    void on_axis3MoveForward_released();
    void on_axis4MoveBackward_pressed();
    void on_axis4MoveBackward_released();
    void on_axis4MoveForward_pressed();
    void on_axis4MoveForward_released();
    void on_axis5MoveBackward_pressed();
    void on_axis5MoveBackward_released();
    void on_axis5MoveForward_pressed();
    void on_axis5MoveForward_released();
    void on_axis6MoveBackward_pressed();
    void on_axis6MoveBackward_released();
    void on_axis6MoveForward_pressed();
    void on_axis6MoveForward_released();
    void on_axis7MoveBackward_pressed();
    void on_axis7MoveBackward_released();
    void on_axis7MoveForward_pressed();
    void on_axis7MoveForward_released();

    //flange position
    void on_lineMoveUp_pressed();
    void on_lineMoveUp_released();
    void on_lineMoveDown_pressed();
    void on_lineMoveDown_released();
    void on_lineMoveForward_pressed();
    void on_lineMoveForward_released();
    void on_lineMoveBackward_pressed();
    void on_lineMoveBackward_released();
    void on_lineMoveLeft_pressed();
    void on_lineMoveLeft_released();
    void on_lineMoveRight_pressed();
    void on_lineMoveRight_released();
    void on_angleMoveUp_pressed();
    void on_angleMoveUp_released();
    void on_angleMoveDown_pressed();
    void on_angleMoveDown_released();
    void on_angleMoveForward_pressed();
    void on_angleMoveForward_released();
    void on_angleMoveBackward_pressed();
    void on_angleMoveBackward_released();
    void on_angleMoveLeft_pressed();
    void on_angleMoveLeft_released();
    void on_angleMoveRight_pressed();
    void on_angleMoveRight_released();

    void on_selectProgramButton_clicked();
    void on_runProgramButton_run_clicked();
    void on_pauseProgramButton_run_clicked();
    void on_startStopButton_clicked();
    void on_jogMoveButton_clicked();
    void on_programButton_clicked();
    void on_runButton_clicked();
    void on_statusButton_clicked();
    void on_settingButton_clicked();
    void on_aboutButton_clicked();
    void on_shutDownButton_clicked();
    void on_openButton_clicked();
    void on_addNodeButton_clicked();
    void on_deleteButton_clicked();
    void on_saveButton_clicked();
    void on_moveButton_clicked();
    void on_saveNodeButton_clicked();
    void on_resetButton_clicked();

    void on_createButton_clicked();
    void on_stopProgramButton_run_clicked();
    void on_stepProgramButton_run_clicked();

    void on_jogSpeedSlider_valueChanged(int value);

    void on_setSpeedButton_clicked();
    void on_setAccelerationButton_clicked();
    void on_deleteProgramButton_clicked();
    void on_runSpeedSlider_valueChanged(int value);

    //simulation move
    void on_zoomIn_Run_pressed();
    void on_zoomIn_Run_released();
    void on_zoomOut_Run_pressed();
    void on_zoomOut_Run_released();
    void on_reset_Run_clicked();
    void on_up3D_pressed();
    void on_up3D_released();
    void on_down3D_pressed();
    void on_down3D_released();
    void on_left3D_pressed();
    void on_left3D_released();
    void on_right3D_pressed();
    void on_right3D_released();

    void on_pushButtonMin_clicked();

    void on_tractionTeachButton_clicked();

    void on_fixPositionMoveBackward_pressed();

    void on_fixPositionMoveBackward_released();

    void on_fixPositionMoveForward_pressed();

    void on_fixPositionMoveForward_released();

    void on_refreshProgram_clicked();

    void on_programBackward_clicked();

    void on_programForward_clicked();

    void on_editButton_clicked();

    void on_suppressButton_clicked();

    void on_AdvancedSetting_clicked();

    void on_toolCoordinate_clicked();

    void on_setToolCoordinateButton_clicked();

    void on_switchToEndMove_clicked();

    void on_switchToJointMove_clicked();

    void on_jointAngleLcdNumberAxis1_clicked();

    void on_jointAngleLcdNumberAxis2_clicked();

    void on_jointAngleLcdNumberAxis3_clicked();

    void on_jointAngleLcdNumberAxis4_clicked();

    void on_jointAngleLcdNumberAxis5_clicked();

    void on_jointAngleLcdNumberAxis6_clicked();

    void on_jointAngleLcdNumberAxis7_clicked();

    void on_positionLcdNumberX_clicked();

    void on_positionLcdNumberY_clicked();

    void on_positionLcdNumberZ_clicked();

    void on_toolCoordinateLineX_clicked();

    void on_toolCoordinateLineY_clicked();

    void on_toolCoordinateLineZ_clicked();

    void on_stopJogMove1_clicked();

    void on_toolCoordinateLineRX_clicked();

    void on_toolCoordinateLineRY_clicked();

    void on_toolCoordinateLineRZ_clicked();

    void on_resetToolCoordinateButton_clicked();

    void on_programTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_expertMode_clicked();

    void on_positionLcdNumberRX_clicked();

    void on_positionLcdNumberRY_clicked();

    void on_positionLcdNumberRZ_clicked();

    void on_copySpeedButton_clicked();

    void on_transitionButton_clicked();

    void on_speedSettingButton_clicked();

    void on_teachButton_clicked();

    void on_threeDViewSwitch_clicked();

    void on_coordinateSwitch_clicked();

    void on_disconnect_clicked();

    void on_disconnectAction_clicked();

    void on_programBackBtn_clicked();

    void on_programNameList_itemClicked(QListWidgetItem *item);

    void on_programChooseOK_clicked();

    void on_programChooseCancel_clicked();

    void on_programNameList_itemDoubleClicked(QListWidgetItem *item);

    void on_programChooseDelete_clicked();

    void on_baseCoordinateBtn_clicked();

    void on_toolCoordinateBtn_clicked();

    void on_pushButtonMoveToOrigin_pressed();

    void on_pushButtonMoveToOrigin_released();

    void on_saveNodeButton_pressed();

    void on_saveNodeButton_released();

    void on_stopJogMove1_pressed();

    void on_stopJogMove1_released();

    void on_openButton_pressed();

    void on_openButton_released();

    void on_createButton_pressed();

    void on_createButton_released();

    void on_programChooseCancel_pressed();

    void on_programChooseCancel_released();

    void on_programChooseOK_pressed();

    void on_programChooseOK_released();

    void on_programBackBtn_pressed();

    void on_programBackBtn_released();

    void on_saveButton_pressed();

    void on_saveButton_released();

    void on_saveSuccessOK_pressed();

    void on_saveSuccessOK_released();

    void on_deleteProgramButton_pressed();

    void on_deleteProgramButton_released();

    void on_editButton_pressed();

    void on_editButton_released();

    void on_moveButton_pressed();

    void on_moveButton_released();

    void on_moveToButton_pressed();

    void on_moveToButton_released();

    void on_transitionButton_pressed();

    void on_transitionButton_released();

    void on_addNodeButton_pressed();

    void on_addNodeButton_released();

    void on_suppressButton_pressed();

    void on_suppressButton_released();

    void on_deleteButton_pressed();

    void on_deleteButton_released();

    void on_programBackward_pressed();

    void on_programBackward_released();

    void on_programForward_pressed();

    void on_programForward_released();

    void on_selectProgramButton_pressed();

    void on_selectProgramButton_released();

    void on_refreshProgram_pressed();

    void on_refreshProgram_released();

    void on_pushButton_clicked();

    void on_languageSwitch_clicked();

    void on_toolCoordinate_pressed();

    void on_toolCoordinate_released();

    void on_resetButton_pressed();

    void on_resetButton_released();

    void on_startStopButton_pressed();

    void on_startStopButton_released();

    void on_expertMode_pressed();

    void on_expertMode_released();

    void on_setInitPosition_clicked();

    void on_blendRadiusValue_clicked();

    void on_pushButton_2_clicked();

    void on_clearInfo_clicked();


    void on_ioOutput3_clicked();

    void on_ioOutput4_clicked();

    void on_ioOutput5_clicked();

    void on_ioOutput6_clicked();

    void on_ioOutput7_clicked();

    void on_ioOutput8_clicked();

    void on_programTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_moveToStartPoint_pressed();

    void on_moveToStartPoint_released();

    void on_back2Program_clicked();

    void on_importProgram_clicked();
    void on_setJoint0Points_clicked();
    void on_digitalIO_clicked();
    void on_ipConfig_clicked();
    void on_ipAddressLineEdit_selectionChanged();
    void on_dhcpIP_clicked();
    void on_staticIP_clicked();
    void on_netmaskLineEdit_selectionChanged();
    void on_gatewayLineEdit_selectionChanged();
    void on_ipSettingOK_clicked();
    void on_defaultProgram_clicked();
    void on_loadDefaultProgram_clicked();
    void on_saveDefaultProgram_clicked();
    void on_cancelDefaultProgram_clicked();
    void on_cameraSetting_clicked();
    void on_pushButton_exit_clicked();
    void on_callOsk_clicked();
    void on_commandLinkButton_manualTrigger_clicked();
    void on_commandLinkButton_calibration_clicked();
    void on_jointEndValue_clicked();
    void on_log_clicked();
    void on_analogIO_clicked();
    void on_toolIO_clicked();
    void on_teachSmooth_valueChanged(int value);
    void on_safeSetting_clicked();
    void on_installSetting_clicked();
    void on_setJointValue_clicked();
    void on_setEndValue_clicked();
    void on_collisionDetectActive_2_clicked();
    void on_robotActive_clicked();
    void on_return2Start_clicked();
    void on_copyNode_clicked();
    void on_pasteNode_clicked();

    void on_acrossInstall_toggled(bool checked);

    void on_verticalInstall_toggled(bool checked);

    void on_anywayInstall_toggled(bool checked);

    void on_copyNode_3_clicked();

    void on_copyNode_2_clicked();

    void on_teachSmooth1_valueChanged(int value);

    void on_teachSmooth2_valueChanged(int value);

    void on_teachSmooth3_valueChanged(int value);

    void on_teachSmooth4_valueChanged(int value);

    void on_teachSmooth5_valueChanged(int value);

    void on_teachSmooth6_valueChanged(int value);

    void on_teachSmooth7_valueChanged(int value);

    void on_collisionSensitive_valueChanged(int value);

    void on_dynamicThreshActive_clicked();

    void on_setInstallOrient_clicked();

    void on_setDynamicThread_clicked();

    void on_setDynamicThread_2_clicked();

    void on_toolOutput1_clicked();

    void on_toolOutput2_clicked();

    void on_toolOutput3_clicked();

    void on_toolOutput4_clicked();

    void on_toolLoadRecognize_clicked();

    void on_toolLoad_clicked();

    void on_toolLoadPositionX_clicked();

    void on_toolLoadPositionY_clicked();

    void on_toolLoadPositionZ_clicked();

    void on_toolLoadAutoRecognizeReset_clicked();

    void on_toolLoadAutoRecognize_clicked();

    void on_toolLoadAutoRecognizePause_clicked();

    void on_setToolWeightPosition_clicked();

    void on_readToolWeightPosition_clicked();

    void on_stopCalibration_clicked();

    void on_writeDynamicParameters_clicked();

    void on_logList_itemClicked(QListWidgetItem *item);

    void on_callOsk_2_clicked();

    void on_callOsk_3_clicked();

    void on_teachReplayMode_clicked();

    void on_tcpCalibration_clicked();

    void on_tcpCalibration4p_clicked();

    void on_tcpCalibration6p_clicked();

    void on_tcpCalibrationP1Next_clicked();

    void on_tcpCalibrationP2Next_clicked();

    void on_tcpCalibrationP3Next_clicked();

    void on_tcpCalibrationP4Next_clicked();

    void on_tcpCalibrationP5Next_clicked();

    void on_tcpCalibrationP6Next_clicked();

    void on_toolCoordinateLineRX_2_clicked();

    void on_toolCoordinateLineRY_2_clicked();

    void on_toolCoordinateLineRZ_2_clicked();

    void on_tcpCalibrationRxyzNext_clicked();

    void on_autoLogin_clicked();

    void on_virtualWall_clicked();

    void on_setVirtualWall_clicked();

    void on_cancelVirtualWall_clicked();

    void on_virtualWall1Length_clicked();

    void on_virtualWall2Length_clicked();

    void on_virtualWall3Length_clicked();

    void on_virtualWall3Rot_clicked();

    void on_virtualWall4Length_clicked();

    void on_virtualWall4Rot_clicked();

    void on_virtualWall5Length_clicked();

    void on_virtualWall5Rot_clicked();

    void on_virtualWall6Length_clicked();

    void on_virtualWall6Rot_clicked();

    void on_virtualWallCheckbox1_toggled(bool checked);

    void on_virtualWallCheckbox2_toggled(bool checked);

    void on_virtualWallCheckbox3_toggled(bool checked);

    void on_virtualWallCheckbox4_toggled(bool checked);

    void on_virtualWallCheckbox5_toggled(bool checked);

    void on_virtualWallCheckbox6_toggled(bool checked);

    void on_warningOK_clicked();

    void on_loadToolCoordinate_clicked();

    void on_selectToolCoordinateBack_clicked();

    void on_selectToolCoordinate1_clicked();

    void on_selectToolCoordinate2_clicked();

    void on_selectToolCoordinate3_clicked();

    void on_selectToolCoordinate4_clicked();

    void on_selectToolCoordinate5_clicked();

    void selectToolCoordinate(int index);

signals:
    void startTimeCount();                      //start count run program time
    void stopTimeCount();                       //stop count run program time
    void sendErrorMsg2Client(QString);          //send error message to TCP client
};

#endif // MAINWINDOW_H
