#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QFile>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QString>
#include "dataupdate.h"

#include <QThread>
//#include <QMutex>

#include <QtNetwork>
using namespace std;
using namespace cv;
extern AdsMessage_Read  adsReadStruct;

extern "C" int start_calib;           //read from camera ex: int start_calib();
extern "C" int calib_finish;          //read from camera ex: int calib_finish();
extern "C" int robot_move_finished;   //write to camera  ex: robot_move_finished(int a);
extern "C" int robot_move;            //read from camera ex: int robot_move();


#define pi 3.141592653589793

class calibration: public QThread
{
    Q_OBJECT
public:
    explicit calibration(QObject *parent = 0);
    ~calibration();
    //void stop();

    bool InsightDataRead = false;
    bool calibrated = false;
    Size imageSize;
    QString str; //状态显示栏输出

    int calPlate = 1;//标定板种类编号
    Size boardSize;//标定板特征点数
    float squareSize = 15;//标定板特征点间距
    int num = 14;//照片张数
    QString outputFileName = "C:/SiasunRobot/opencv/OutCameraData.txt";//定义输出文件名

    bool HandEye_Calib(int calPlateNum, QString IP);//手眼标定主函数
    bool SaveHcgToFile();
    bool SaveHcalToFile();
    bool SaveHc1ToFile();
    void GetInsightDataToFile(QString IP, quint16 port);//从指定IP和端口号的Cognex相机中完成所有图像圆点数据的读取,存入文件
    bool AutoMoveGetInsightAndRobotDataOnline(QString IP, quint16 port);//在线轨迹移动，自动采集机器人位置位姿数据并从指定IP和端口号的Cognex相机中完成所有图像角点数据的读取,存入文件
    vector< vector<Point2f> > ReadCameraPointFromFile();//从文件中读取标定板特征点坐标数据
    //void ReadCalibDataFromInsightAndSaveToFile(char IP[], quint16 port);//从指定IP和端口号的Cognex相机中读取单幅图像圆点数据,存入文件
    bool CameraCalibrate();//自己写
    bool saveCameraParams( Mat cameraMatrix, Mat distCoeffs, vector<Mat> rvecs, vector<Mat> tvecs, double rms );//保存相机标定参数
    vector<vector<double>> ReadCameraData();//读取文件中相机信息,返回6元外参
    vector<vector<double>> ReadRobotPose();//读取文件中机器人末端位姿
    vector<Mat> CalculateHgij(vector<vector<double>> pos);//将从文件中读取的位姿，利用Gripper函数转换成相对基坐标的变换矩阵,计算gripper变换矩阵Hgij
    vector<Mat> CalculateHcij(vector<vector<double>> extr_par);//计算Hci、Hcj的变换矩阵Hcij
    Mat Gripper(double pose[6]);//将机器人末端位姿pose[6]转换为gripper相对于基坐标的变换矩阵temp_Hgi
    Mat Camera(double extr_par[6]);//将相机内外参转换为标定板相对于相机的变换矩阵temp_Hci
    Mat skew(Mat in);//向量转换为反对称矩阵
    Mat Tsai_HandEye(vector<Mat> Hgij, vector<Mat> Hcij);//Tsai的手眼标定算法，Hcg为求得的camera-gripper变换矩阵，Hgij为两次拍摄gripper的变换矩阵，Hcij为两次拍摄camera的变换矩阵

    void ReadInParametersFromFile();
    void ReadOutCameraDataFromFile(int cameraPositionNum = 1);
    Mat CalculatePosition(double x, double y, int num);//从图像像素坐标（x，y）解算出机器人坐标（返回的Mat（6*1）矩阵）,num is the number of camera position
    Mat pose2H(double x, double y, double z, double Rx, double Ry, double Rz);
    Mat H2pose(Mat R);//rotation matrix  3*3 to 3*1
    vector<Mat> CalculateRobotMovePosition(double x, double y, double z, double Rx, double Ry, double Rz, int h_index);//从机器人初始位置计算一键标定后续移动位置,h_index is the index of the distance between camera and working plane

    bool CalculateExtrPar(int boardHeightNum, int boardWidthNum, double cornerDist);//从用户指定拍照位置获取相机外参和机器人位置并存入文件
    bool saveHandEyeData(Mat cameraMatrix, Mat distCoeffs, Mat Hcg);//写入内参M、畸变disCoeff及手眼标定矩阵Hcg
    vector<Point2f> GetInsightDataOnline(QString IP, quint16 port);//从指定IP和端口号的Cognex相机中完成所有图像角点数据的读取

//    //对外接口交互数据
//    //接收(extern)

    //bool robot_move_finished;//机器人移动完成（单条移动指令）信号

    //发送
    int error_type = 0;//错误信息
    //bool start_calib = false;//手眼标定触发信号
    //bool robot_move = false;//机器人移动信号（单条移动指令）
    //bool calib_finish = false;//手眼标定结束信号



    vector< vector<Point2f> > calib_points;//insight中提取的标定角点
    vector< vector<double> > robot_pos;//机器人位置和位姿
    int points_num = 56;//单幅图圆点个数
    //Mat Hcg;
    //Mat Hcal;
    //Mat Hc1;

    QString Camera_IP;
    int ImageSize_Width;
    int ImageSize_Height;
    int BoardSize_Width;
    int BoardSize_Height;
    float Square_Size;
    int Calibrate_NrOfFrameToUse;
    QString Write_outputFileName;
    int camPosNum = 0;


    bool calibSucceeded = false;
protected:
    void run();
//private:
    //volatile bool mstop;
    //QMutex m_lock;

};

#endif // CALIBRATION_H
