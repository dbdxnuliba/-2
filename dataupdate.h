#ifndef DATAUPDATE_H
#define DATAUPDATE_H
#include <QThread>
#include <QElapsedTimer>
#include "ADS/ads.h"
#include "Robot_Script/7dof_media.h"

//for Chinese character
#pragma execution_character_set("utf-8")


//Tcp server default port
const int PORT_RECEIVE = 2000;   //port connect to other PC
const int PORT_STATUS = 2001;    //port send status feedback


//osg meshes path
const std::string osgMeshes0 = "C:/SiasunRobot/meshes3/base_link.STL";
const std::string osgMeshes1 = "C:/SiasunRobot/meshes3/link_1.STL";
const std::string osgMeshes2 = "C:/SiasunRobot/meshes3/link_2.STL";
const std::string osgMeshes3 = "C:/SiasunRobot/meshes3/link_3.STL";
const std::string osgMeshes4 = "C:/SiasunRobot/meshes3/link_4.STL";
const std::string osgMeshes5 = "C:/SiasunRobot/meshes3/link_5.STL";
const std::string osgMeshes6 = "C:/SiasunRobot/meshes3/link_6.STL";
const std::string osgMeshes7 = "C:/SiasunRobot/meshes3/link_7.STL";

const std::string osgMeshes3Kg0 = "C:/SiasunRobot/meshes3/3/base_link.STL";
const std::string osgMeshes3Kg1 = "C:/SiasunRobot/meshes3/3/link_1.STL";
const std::string osgMeshes3Kg2 = "C:/SiasunRobot/meshes3/3/link_2.STL";
const std::string osgMeshes3Kg3 = "C:/SiasunRobot/meshes3/3/link_3.STL";
const std::string osgMeshes3Kg4 = "C:/SiasunRobot/meshes3/3/link_4.STL";
const std::string osgMeshes3Kg5 = "C:/SiasunRobot/meshes3/3/link_5.STL";
const std::string osgMeshes3Kg6 = "C:/SiasunRobot/meshes3/3/link_6.STL";
const std::string osgMeshes3Kg7 = "C:/SiasunRobot/meshes3/3/link_7.STL";

const QString logFileName = "\\siasun_log_";
const QString appVersion = "2.6.16";

//default speed
const QString defaultMoveJSpeed = "30";     //percent %
const QString defaultMoveLSpeed = "100";    //mm/s


#pragma pack (8) //64bit
//a struct with all the values write to the PLC using ADS
typedef struct AdsMessage_Write
{
    //system
    bool power = false;                     //power the robot and let the UI connect to the robot
    bool shutDown = false;                  //shutdown the whole system
    bool reset = false;                     //reset UI and TwinCAT system

    //jog
    char jogMode = -1;                      //1:end move, 2:joint move, 3:end-fix move, 4:joint value entered, 5:end value entered
    char jogSpeed = 50;                     //jog speed, moveto speed in percentage%
    char axisAngleMoveDirection[7];         //axis move direction
    char flangeMoveDirection[6];            //flange move direction
    double enteredAxisValue[7];             //in jog mode, entered axis value
    double enteredXyzValue[6];              //in jog mode, entered X/Y/Z/rx/ry/rz value
    bool toolCoordinateJog = false;         //in jog move, use tool coordinate to move

    bool tractionTeach = false;             //begin traction teach
    bool moveTo = false;                    //move to original position/program node position/entered axis/end position

    //run program
    bool progRun = false;                   //Start program
    bool progPause = false;                 //pause program
    bool progStop = false;                  //stop program
    char blockType = 0;                     //program command type: 1:moveJ, 2:moveL, 3:moveC, 4:finish, 5:movePsi, 6:moveL blend, 7:replay
    int progRunSpeed = 100;                 //program speed
    double progMoveJAxis[7];                //target joint position
    double progMoveJAxisBlend[7];           //target joint position for blend
    double progMoveLAxis[6];                //target end position
    double progMoveLAxisMoveC[6];           //second target end position for moveC
    double semidiameter = -1;               //transition semidiameter 交融半径
    char blendStep = 0;                     //blend step, 0&1:START, 2:RUN, 3:END
    char dataFinish = 0;                    //when a new program command send, dataFinish++
    bool moveLPsi = false;                  //moveL change psi

    char gripperMoveLength;                 //external hand control, gripper move distance

    double toolCoordinate[3];               //tool coordinate: x y z
    double toolCoordinateR[3];              //tool coordinate: rx ry rz
    bool statusUseToolCoordinate = false;   //for status show, not for jog move
    bool toolCoordinateProg = false;        //program parameters use tool coordinate

    bool configTwinCAT = false;             //make TwinCAT into config mode

    //save traction teach path
    char pathNum = 1;                       //which path to save
    bool beginTeach = false;                //begin save path or not

    bool io_output_num[6];                  //for I/O output control, 6 io output port

    bool tool_io_output_num[4];             //for tool I/O output control, 4 io output port

    bool collisionDetectActive = true;      //collision detect active

    bool joint0ValueRead = false;           //load joint 0 value from XML file

    bool enableAllAxis = false;             //enable all axis

    double targetPsi;                       //target moveL psi

    char moveToType;                        //joint moveto or end moveto

    bool blendMoveStopClick;                //in blend move, click stop

    char teachSmoothAll = 50;               //set all joints traction teach smooth
    char teachSmooth[7];


    char collisionSensitive = 2;            //set collision sensitive (1 2 3)
    bool dynamicThresh = true;              //set dynamic collision threshold
    double dynamicThreshJoint[7];           //set dynamic threshold manually

    bool movejBlend;                        //movej use blend or not
    bool movejBlendValueSend;               //guarantee a value is send

    bool tcpIdStart;                        //start TCP tool weight position identify
    bool tcpIdPause;                        //pause TCP tool weight position identify
    bool tcpIdReset;                        //reset TCP tool weight position identify
    double tcpLoad = 0;                     //TCP tool weight
    double tcpLoadPos[3];                   //tcp tool weight point position

    bool clearWarning = false;              //clear the warning message

    double tcpMove[3];                      //move an increase value following the tcp coordinate

    bool tcpMoveActive = false;             //tcp move enable

    bool reserve[10];                       //for reserve use 0:gripper, 1:M_MOVEJ_POSE, 2:CALIB_POSE, 3:useTP, 4:PC shutdown

    bool tcp4pCalibration;                  //tcp 4p calibration or 6p calibration

    bool tcpCalibrationSave;                //save tcp calibration point

    bool tcpCalibrationStart;               //start/stop tcp calibration
}AdsMessage_Write;

//a struct with all the values read from the PLC using ADS
typedef struct AdsMessage_Read
{
    //real-time robot position
    double currentFlangePosition[6];        //current position of flange
    double currentFlangePositionTool[6];    //current position of flange with tool coordinate
    double axisCurrentAngle[7];             //current angle of axis

    bool jogEnterValueMoveFinished;         //for jog mode enter value move finished

    //run program
    bool oneCommandFinish;//bFinish         //a command finished
    bool stopClicked;     //bStopped        //stop button clicked and robot stopped
    bool commandReceived;                   //feedback the new command was received
    bool RTS_Blends;                        //blend move require to send next point
    bool pauseThenStopped;                  //paused and then stopped done

    bool collisionDetected;                 //collision detection

    bool exceedJointLimit;                  //exceed joint limit
    bool exceedWorkingLimit;                //exceed working limit (joint 4 nearly 0)

    bool externalIORead[6];                 //for external IO input

    bool externalToolIORead[4];             //for external tool IO input

    bool sysInitDone = false;               //system init finish

    bool emcStopSignal;                     //emergency stop signal

    bool moveDoneSignal;                    //moveto finish signal

    bool gripperDone;                       //gripper move done 移动机械手，爪子

    bool endButton;                         //末端按钮按下

    bool endButton2;                        //2号末端按钮按下

    bool saveStatus;                        //标定用

    bool resetSystem;                       //error, reset system

    bool powerOn;                           //check if the robot has power on

    bool brakeDone;                         //brake release done

    bool axis_error;                        //axis has any error

    bool pauseDone;                         //in run program, robot paused done

    bool slave_ready[7];                    //axis controller ready

    bool moveToDone;                        //moveto finished

    char collisionID;                       //which joint has collision

    unsigned long int errorID[11];          //error ID

    char warningID[7];                      //warning ID

    char stateMachine;                      //PLC state machine

    char motionMachine;                     //PLC motion machine

    bool movejRTS;                          //movej blend require to send

    double Pcload[3];                       //tcp tool weight point position

    bool moveLCBlendFinish;                 //movel movec blend block finished, nstate is 0 now

    double tcpCalibration[6];               //tool coordinate calibration result

    bool reserve[10];                       //for reserve use 0:blend finish
}AdsMessage_Read;

//8byte对齐
//status struct
typedef struct
{
    double currentFlangePosition[6];        //current flange position #8byte
    double currentFlangePositionTool[6];    //current flange position with tool coordinate #8byte
    double axisCurrentAngle[7];             //current axis angle #8byte
    bool   externalIORead[8];               //external IO #1byte
    char   robotCurrentState;               //robot current status: 0:stand-by, 1:running, 2:error #1byte
}Status_Struct;

//xml struct
typedef struct XML_Struct
{
    double axisBias[7];                 //axis init bias
    double DH[4];

    //dynamic parameters
    double Kt[7];                       //电机电流与输出力矩比值
    double FC[7];                       //库仑摩擦
    double FV[7];                       //粘滞摩擦
    double FVV[7];                      //非线性摩擦力(速度二次项)
    double FVVV[7];                     //非线性摩擦力(速度三次项)
    double TFv[7];                      //关节负载对粘滞摩擦的影响
    double TFc[7];                      //关节负载对库仑摩擦的影响
    double Th[7];                       //碰撞检测阈值

    double theta_G = 90;                //绕y转动角度,单位°
    double alpha_G = 0;                 //绕z转动角度,单位°

    double currentDynamicThresh[7];     //set dynamic threshold manually

    double limits_velo[7];              //axis speed limit
    double limits_acc[7];               //axis acceleration limit
    double limits_Pos[7];               //axis position limit
}XML_Struct;
#pragma pack ()


class DataUpdate : public QThread
{
    Q_OBJECT

public:
    DataUpdate();
    ~DataUpdate();
    bool stop = false;

private: 
    double axisOldAngle[7];             //old axis angle
    double oldFlangePosition[6];        //old flange position
    short oldMoveSpeed = 0;
    long adsErrorWrite;
    long adsErrorRead;
    QElapsedTimer timer,timeTest;
    bool showRunTime = false;

    void run();
    void update();

private slots:
    void startTimeCount();              //start time count
    void stopTimeCount();               //stop time count

signals:
    void axisAngleValueChanged(double,int);
    void flangePositionChanged(double,int);
    void axisChangedUpdate3DView(double*);
    void adsErrorMessage(long);
    void tcpErrorMessage(QString);
    void showCommandLine(int);
    void lockJogScreen(bool);
    void exceedJointLimit();
    void exceedJointLimitClear();
    void switchJogSpiButton(bool);
    void externalIOReadUpdate(bool*);
    void externalIOWriteUpdate(bool*);
    void externalToolIOReadUpdate(bool*);
    void externalToolIOWriteUpdate(bool*);
    void endToolButtonStatus(bool);             //end tool button pressed or released
    void resetSystem();                         //reset system
    void robotPowerOff();                       //robot is power off
    void robotBreakState(bool);
    void axisError();
    void selfCollisionDetect();
    void wallCollisionDetect();
    void showProgramRunTime(int);
    void updateSlaveStatus(bool*);
    void axisWarning();
    void teachLockScreen(bool);
    void pressButtonAddNode();
    void emcBtnStatusChange(bool);
};

#endif // DATAUPDATE_H
