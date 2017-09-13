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
    QString str; //״̬��ʾ�����

    int calPlate = 1;//�궨��������
    Size boardSize;//�궨����������
    float squareSize = 15;//�궨����������
    int num = 14;//��Ƭ����
    QString outputFileName = "C:/SiasunRobot/opencv/OutCameraData.txt";//��������ļ���

    bool HandEye_Calib(int calPlateNum, QString IP);//���۱궨������
    bool SaveHcgToFile();
    bool SaveHcalToFile();
    bool SaveHc1ToFile();
    void GetInsightDataToFile(QString IP, quint16 port);//��ָ��IP�Ͷ˿ںŵ�Cognex������������ͼ��Բ�����ݵĶ�ȡ,�����ļ�
    bool AutoMoveGetInsightAndRobotDataOnline(QString IP, quint16 port);//���߹켣�ƶ����Զ��ɼ�������λ��λ�����ݲ���ָ��IP�Ͷ˿ںŵ�Cognex������������ͼ��ǵ����ݵĶ�ȡ,�����ļ�
    vector< vector<Point2f> > ReadCameraPointFromFile();//���ļ��ж�ȡ�궨����������������
    //void ReadCalibDataFromInsightAndSaveToFile(char IP[], quint16 port);//��ָ��IP�Ͷ˿ںŵ�Cognex����ж�ȡ����ͼ��Բ������,�����ļ�
    bool CameraCalibrate();//�Լ�д
    bool saveCameraParams( Mat cameraMatrix, Mat distCoeffs, vector<Mat> rvecs, vector<Mat> tvecs, double rms );//��������궨����
    vector<vector<double>> ReadCameraData();//��ȡ�ļ��������Ϣ,����6Ԫ���
    vector<vector<double>> ReadRobotPose();//��ȡ�ļ��л�����ĩ��λ��
    vector<Mat> CalculateHgij(vector<vector<double>> pos);//�����ļ��ж�ȡ��λ�ˣ�����Gripper����ת������Ի�����ı任����,����gripper�任����Hgij
    vector<Mat> CalculateHcij(vector<vector<double>> extr_par);//����Hci��Hcj�ı任����Hcij
    Mat Gripper(double pose[6]);//��������ĩ��λ��pose[6]ת��Ϊgripper����ڻ�����ı任����temp_Hgi
    Mat Camera(double extr_par[6]);//����������ת��Ϊ�궨�����������ı任����temp_Hci
    Mat skew(Mat in);//����ת��Ϊ���Գƾ���
    Mat Tsai_HandEye(vector<Mat> Hgij, vector<Mat> Hcij);//Tsai�����۱궨�㷨��HcgΪ��õ�camera-gripper�任����HgijΪ��������gripper�ı任����HcijΪ��������camera�ı任����

    void ReadInParametersFromFile();
    void ReadOutCameraDataFromFile(int cameraPositionNum = 1);
    Mat CalculatePosition(double x, double y, int num);//��ͼ���������꣨x��y����������������꣨���ص�Mat��6*1������,num is the number of camera position
    Mat pose2H(double x, double y, double z, double Rx, double Ry, double Rz);
    Mat H2pose(Mat R);//rotation matrix  3*3 to 3*1
    vector<Mat> CalculateRobotMovePosition(double x, double y, double z, double Rx, double Ry, double Rz, int h_index);//�ӻ����˳�ʼλ�ü���һ���궨�����ƶ�λ��,h_index is the index of the distance between camera and working plane

    bool CalculateExtrPar(int boardHeightNum, int boardWidthNum, double cornerDist);//���û�ָ������λ�û�ȡ�����κͻ�����λ�ò������ļ�
    bool saveHandEyeData(Mat cameraMatrix, Mat distCoeffs, Mat Hcg);//д���ڲ�M������disCoeff�����۱궨����Hcg
    vector<Point2f> GetInsightDataOnline(QString IP, quint16 port);//��ָ��IP�Ͷ˿ںŵ�Cognex������������ͼ��ǵ����ݵĶ�ȡ

//    //����ӿڽ�������
//    //����(extern)

    //bool robot_move_finished;//�������ƶ���ɣ������ƶ�ָ��ź�

    //����
    int error_type = 0;//������Ϣ
    //bool start_calib = false;//���۱궨�����ź�
    //bool robot_move = false;//�������ƶ��źţ������ƶ�ָ�
    //bool calib_finish = false;//���۱궨�����ź�



    vector< vector<Point2f> > calib_points;//insight����ȡ�ı궨�ǵ�
    vector< vector<double> > robot_pos;//������λ�ú�λ��
    int points_num = 56;//����ͼԲ�����
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
