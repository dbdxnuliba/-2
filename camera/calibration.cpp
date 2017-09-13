#include "calibration.h"

const QString cameraData = "C:/SiasunRobot/opencv/OutCameraData";
//const QString robotPose = "C:/SiasunRobot/opencv/robot_pose.dat";
const QString inParameters = "C:/SiasunRobot/opencv/in_parameters.txt";

Mat Hcg(4, 4, CV_64FC1);
Mat Hcal(4, 4, CV_64FC1);
Mat Hc1(4, 4, CV_64FC1);
Mat Camera_Matrix(3, 3, CV_64FC1);//内参矩阵
Mat distCoeffs(5, 1, CV_64FC1);//畸变参数矩阵

//从控制器获取变量(写进calibration.h里了)

//机器人位置和位姿
//extern double X;
//extern double Y;
//extern double Z;
//extern double Rx;
//extern double Ry;
//extern double Rz;
//

//extern bool robot_move_finished;//机器人移动完成（单条移动指令）信号
//向控制器发送的数据(写进calibration.h里了)
//int error_type = 0;//错误信息
//bool start_calib = false;//手眼标定触发信号
//bool robot_move = false;//机器人移动信号（单条移动指令）
//bool calib_finish = false;//手眼标定结束信号

calibration::calibration(QObject *parent):QThread(parent)
{
    //mstop = false;
}

calibration::~calibration()
{
    //stop();
}

void calibration::run()
{
    //    if(!mstop)
    //    {
    //        HandEye_Calib(calPlate, Camera_IP);
    //    }
    //    mstop = false;
    calibration::str = "";
    calibration::error_type = 0;
    if(camPosNum == 0)
    {
        if(HandEye_Calib(calPlate, Camera_IP))
            calibSucceeded = true;
        else
            calibSucceeded = false;
    }
    else if(camPosNum >0)
    {
        if(CalculateExtrPar(boardSize.height, boardSize.width, squareSize))
            calibSucceeded = true;
        else
            calibSucceeded = false;
    }
}

//void calibration::stop()
//{
//    QMutexLocker locker(&m_lock);
//    mstop = true;
//}

bool calibration::HandEye_Calib(int calPlateNum, QString IP)
{
    calPlate = calPlateNum;




    //变量声明
    //quint16 port = 60002;
    vector<Mat> Hgij;
    vector<Mat> Hcij;

    //Mat Hcg(4, 4, CV_64FC1);
    vector<vector<double>> extr_par;//外参
    double extrinsic_parameters[150][6];
    vector<vector<double>> pos;//机器人位姿
    double pose[150][6];

    //Mat Hc1(4, 4, CV_64FC1);
    Mat Hg1(4, 4, CV_64FC1);
    //Mat Hcal(4, 4, CV_64FC1);

    int label = 0;//读取的位姿序号
    //
    str = "";
    str += "Calib plate type: ";
    str += QString::number(calPlate, 10);
    str += '\n';

    //init
    //    InsightDataRead = true;
    //    calibrated = false;
    //    calibration::num = 14;
    //    calibration::points_num = 56;
    //

    switch (calPlateNum) {
    case 1:
        //init
        boardSize.height = 8;
        boardSize.width = 7;
        calibration::squareSize = 12.5;
        //
        break;
    case 2:
        //init
        boardSize.height = 8;
        boardSize.width = 7;
        calibration::squareSize = 6;
        break;
    case 3:
        //customized init
        break;
    default:
        break;
    }

    points_num = boardSize.height * boardSize.width;

    if(!InsightDataRead)
    {
        //GetInsightDataToFile(IP, 60002);
        if(AutoMoveGetInsightAndRobotDataOnline(IP, 60002))
        {
            calib_points = calibration::ReadCameraPointFromFile();

            if(!calibrated)
            {
                calibrated = CameraCalibrate();
            }
            //异常处理
            if(!calibrated)
            {
                str += "CameraCalibrate Error!";
                error_type = 4;
                qDebug() << "Error!";
                return 0;
            }
            //

            //pos = Calib::ReadRobotPose();
            for(int i = 0; i < num; i++)
            {
                //qDebug() << i + 1 << " :";
                for(int j = 0; j < 6; j++)
                {
                    //pose[i][j] = pos[i][j];
                    pose[i][j] = robot_pos[i][j];
                    //qDebug() << pose[i][j];
                }
            }



            extr_par = calibration::ReadCameraData();
            for(int i = 0; i < num; i++)
            {
                //qDebug() << i + 1 << " :";
                for(int j = 0; j < 6; j++)
                {
                    extrinsic_parameters[i][j] = extr_par[i][j];
                    //            cout << extr_par[i][j] << endl;
                    //            qDebug() << extrinsic_parameters[i][j];
                }
            }






            Hcij = calibration::CalculateHcij(extr_par);
            //Hgij = calibration::CalculateHgij( pos );
            Hgij = calibration::CalculateHgij( robot_pos );

            qDebug() << "data successfully read";

            //            for(int m = 0; m < num - 1; m++)
            //            {
            //                qDebug() << m << ":";
            //                for(int i=0;i<4;i++)
            //                {
            //                    for(int j=0;j<4;j++)
            //                    {
            //                        qDebug() << Hcij[m].at<double>(i,j);

            //                    }
            //                }
            //            }

            Hcg = Tsai_HandEye(Hgij,  Hcij);

            qDebug() << "Tsai_handeye success!";

            //str += "Hand-eye calibration finished.\n";


            if(calibrated)
            {
                if(!SaveHcgToFile())
                {
                    return 0;
                }

                Hc1 = Camera(extrinsic_parameters[label]);
                Hg1 = Gripper(pose[label]);

                Hcal = Hg1 * Hcg * Hc1;

                if(!SaveHc1ToFile())
                {
                    return 0;
                }
                if(!SaveHcalToFile())
                {
                    return 0;
                }
                qDebug() <<"Hcal " << ":";
                for(int i=0;i<4;i++)
                {
                    for(int j=0;j<4;j++)
                    {
                        qDebug() << Hcal.at<double>(i,j);
                    }
                }
                str += "Calib plate in robot coordinate: ";
                str += '\n';
                str += "X = ";
                str += QString::number(Hcal.at<double>(0, 3));
                str += "\n";
                str += "Y = ";
                str += QString::number(Hcal.at<double>(1, 3));
                str += "\n";
                str += "Z = ";
                str += QString::number(Hcal.at<double>(2, 3));
                str += "\n";

            }
            //        qDebug() << "Hcg:";
            //    str += "Hand-eye calibration finished.\n Hcg = ";
            //    str += '\n';
            //    for(int i=0;i<4;i++)
            //    {
            //        for(int j=0;j<4;j++)
            //        {
            //            qDebug() << Hcg.at<double>(i,j);
            //            //            str += QString::number(Hcg.at<double>(i,j), 'f', 2);
            //            //            str += " ";
            //        }
            //        //        str += ";";
            //        //        str += '\n';
            //    }
            return 1;
        }
        else
        {
            str += '\n';
            str += "Auto calibration failed.\n";
            return 0;
        }

    }
    else
    {
        str += '\n';
        str += "Already got camera data.";
        return 0;
    }






}


vector<vector<double>> calibration::ReadCameraData()
{
    vector<vector<double>> extr_par;
    vector<double> temp_par;
    QFile file(outputFileName);


    double extrinsic_parameters[150][6];//读取外参临时储存
    bool read_next_line = true;


    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream( &file );
        QString buffer;


        //file reading
        while(read_next_line)
        {
            int j = 0;
            int k = 0;
            QString temp;
            buffer = stream.readLine(); // 不包括“\n”的一行文本

            temp = "";
            while(buffer[j] != ' '&& buffer[j] != '\0')
            {
                temp += buffer[j];
                j++;
            }

            j = 0;
            k = 0;
            if(temp == "<Extrinsic_Parameters>")//读取外参
            {

                for(int m = 0 ; m < num ; m++)
                {

                    buffer = stream.readLine(); // 不包括“\n”的一行文本
                    j = 2;
                    k = 0;
                    while( k < 6 )
                    {

                        temp = "";
                        while(buffer[j] != ','&& buffer[j] != ';'&&buffer[j] != ']')
                        {
                            temp += buffer[j];
                            j++;
                        }
                        j++;
                        j++;
                        //qDebug() << buffer;
                        //qDebug() << temp;

                        extrinsic_parameters[m][k] = temp.toDouble();
                        //qDebug() << "m" << m;
                        //qDebug("%.8lf", extrinsic_parameters[m][k]);
                        k++;


                    }

                }
                read_next_line = false;
            }


        }
    }
    else
    {
        error_type = 5;
        str += "Output file error.\n";
        return extr_par;
    }
    file.close();
    // end of file reading

    //            for(int i=0;i<14;i++)
    //            {
    //                for(int j=0;j<6;j++)
    //                {
    //                    qDebug() << extrinsic_parameters[i][j];

    //                }
    //            }

    //
    temp_par.clear();
    for(int i = 0; i < num; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            temp_par.push_back( extrinsic_parameters[i][j] );
        }
        extr_par.push_back(temp_par);
        temp_par.clear();
    }

    //        for(int i = 0; i < num; i++)
    //        {
    //            for(int j = 0; j < 6; j++)
    //            {
    //                cout << extr_par[i][j] << endl;
    //            }
    //        }
    return extr_par;

}


vector<Mat> calibration::CalculateHcij(vector<vector<double>> extr_par)
{
    vector<Mat> Hcij;
    Mat Hci(4, 4, CV_64FC1);
    Mat Hcj(4, 4, CV_64FC1);
    Mat Hci_inv(4, 4, CV_64FC1);
    Mat temp_Hcij(4, 4, CV_64FC1);
    double extrinsic_parameters[6];

    for(int i = 0; i < 6; i++)
        extrinsic_parameters[i] = extr_par[0][i];
    Hci = Camera(extrinsic_parameters);
    for(int p = 0; p < num; p++)
    {



        for(int i = 0; i < 6; i++)
            extrinsic_parameters[i] = extr_par[p][i];
        Hcj = Camera(extrinsic_parameters);

        //                            for(int i=0;i<4;i++)
        //                            {
        //                                for(int j=0;j<4;j++)
        //                                {
        //                                    qDebug() << Hcj.at<double>(i,j);

        //                                }
        //                            }
        if(p > 0)
        {
            invert(Hci, Hci_inv, DECOMP_SVD);
            temp_Hcij = Hcj * Hci_inv;
            Mat temp = temp_Hcij.clone();

            Hcij.push_back(temp);

        }

    }
    //    for(int m = 0; m < num - 1; m++)
    //    {
    //        qDebug() << m << ":";
    //        for(int i=0;i<4;i++)
    //        {
    //            for(int j=0;j<4;j++)
    //            {
    //                qDebug() << Hcij[m].at<double>(i,j);

    //            }
    //        }
    //    }
    return Hcij;
}

vector<Mat> calibration::CalculateHgij(vector<vector<double>> pos)
{
    vector<Mat> Hgij;
    Mat temp_Hgm(4, 4, CV_64FC1);
    Mat temp_Hgn(4, 4, CV_64FC1);
    Mat temp_Hgmn(4, 4, CV_64FC1);
    Mat temp_Hgn_inv(4, 4, CV_64FC1);

    double pose[150][6];

    for(int i = 0; i < num; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            pose[i][j] = pos[i][j];
        }
    }

    temp_Hgm = Gripper(pose[0]);
    for(int i = 0; i < num; i++)
    {

        temp_Hgn = Gripper(pose[i]);

        if(i > 0)
        {
            invert(temp_Hgn, temp_Hgn_inv, DECOMP_SVD);

            temp_Hgmn = temp_Hgn_inv * temp_Hgm;

            Mat temp = temp_Hgmn.clone();
            Hgij.push_back(temp);

            //            qDebug() << i << ":";
            //            for(int m=0;m<4;m++)
            //            {
            //                for(int j=0;j<4;j++)
            //                {
            //                    qDebug() << Hgij[i-1].at<double>(m,j);

            //                }
            //            }
        }
    }
    //        for(int m = 0; m < num - 1; m++)
    //        {
    //            qDebug() << m << ":";
    //            for(int i=0;i<4;i++)
    //            {
    //                for(int j=0;j<4;j++)
    //                {
    //                    qDebug() << Hgij[m].at<double>(i,j);

    //                }
    //            }
    //        }
    return Hgij;
}

vector< vector<double> > calibration::ReadRobotPose()
{
    vector<vector<double>> pos;
    vector<double> temp_pos;
    QFile file("robot_pose.dat");

    double pose[150][6];

    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream( &file );
        QString buffer;
        for(int i = 0; i < num; i++)
        {
            buffer = stream.readLine(); // 不包括“\n”的一行文本
            //cout << buffer;
            int j = 0;
            int k = 0;
            QString temp;
            while(buffer[j] != '\0' && k < 6)
            {
                while(buffer[j] != ' '&& buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                j++;
                //qDebug() << temp;
                pose[i][k] = temp.toDouble();
                k++;
                temp = "";
            }
            //                for(int m=0;m<6;m++)
            //                {
            //                    qDebug() << pose[i][m];
            //                }
        }
    }
    file.close();

    temp_pos.clear();
    for(int i = 0; i < num; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            temp_pos.push_back( pose[i][j] );
        }
        pos.push_back( temp_pos );
        temp_pos.clear();
    }

    return pos;

}


Mat calibration::Camera(double extr_par[6])
{

    Mat temp_Hextr(4, 4, CV_64FC1);//外参4*4矩阵
    Mat R(3, 3, CV_64FC1);//外参旋转矩阵
    Mat Rv(1, 3, CV_64FC1);

    //R
    Rv.at<double>(0,0) = extr_par[0];
    Rv.at<double>(0,1) = extr_par[1];
    Rv.at<double>(0,2) = extr_par[2];
    Rodrigues(Rv, R);
    //    for(int i=0;i<3;i++)
    //    {
    //        for(int j=0;j<3;j++)
    //        {
    //            qDebug() << R.at<double>(i,j);

    //        }
    //    }

    R.copyTo(temp_Hextr(Rect(0, 0, 3, 3)));
    //T
    temp_Hextr.at<double>(0, 3) = extr_par[3];
    temp_Hextr.at<double>(1, 3) = extr_par[4];
    temp_Hextr.at<double>(2, 3) = extr_par[5];

    //        temp_Hextr.at<double>(0, 3) = extr_par[3] * 1000;
    //        temp_Hextr.at<double>(1, 3) = extr_par[4] * 1000;
    //        temp_Hextr.at<double>(2, 3) = extr_par[5] * 1000;

    //        temp_Hextr.at<double>(0, 2) = 0.0;
    //        temp_Hextr.at<double>(1, 2) = 0.0;
    //        temp_Hextr.at<double>(2, 2) = 0.0;



    temp_Hextr.at<double>(3, 0) = 0.0;
    temp_Hextr.at<double>(3, 1) = 0.0;
    temp_Hextr.at<double>(3, 2) = 0.0;
    temp_Hextr.at<double>(3, 3) = 1.0;


    return temp_Hextr;
}





Mat calibration::Gripper(double pose[6])
{
    Mat temp_Hgi(4, 4, CV_64FC1);

    Mat Rx(3, 3, CV_64FC1);
    Mat Ry(3, 3, CV_64FC1);
    Mat Rz(3, 3, CV_64FC1);
    Mat Rot(3, 3, CV_64FC1);


    Rx = (Mat_<double>(3,3)<< 1, 0, 0, 0, cos(pose[3]/180*pi), -sin(pose[3]/180*pi), 0, sin(pose[3]/180*pi), cos(pose[3]/180*pi));
    Ry = (Mat_<double>(3,3)<< cos(pose[4]/180*pi), 0, sin(pose[4]/180*pi), 0, 1, 0, -sin(pose[4]/180*pi), 0, cos(pose[4]/180*pi));
    Rz = (Mat_<double>(3,3)<< cos(pose[5]/180*pi), -sin(pose[5]/180*pi), 0, sin(pose[5]/180*pi), cos(pose[5]/180*pi), 0, 0, 0, 1);
    Rot = Rx * Ry * Rz;

    Rot.copyTo(temp_Hgi(Rect(0, 0, 3, 3)));

    temp_Hgi.at<double>(0, 3) = pose[0];
    temp_Hgi.at<double>(1, 3) = pose[1];
    temp_Hgi.at<double>(2, 3) = pose[2];


    temp_Hgi.at<double>(3, 0) = 0.0;
    temp_Hgi.at<double>(3, 1) = 0.0;
    temp_Hgi.at<double>(3, 2) = 0.0;
    temp_Hgi.at<double>(3, 3) = 1.0;


    //                for(int i=0;i<4;i++)
    //                {
    //                    for(int j=0;j<4;j++)
    //                    {
    //                        qDebug() << temp_Hgi.at<double>(i,j);

    //                    }
    //                }
    return temp_Hgi;
}



Mat calibration::Tsai_HandEye(vector<Mat> Hgij, vector<Mat> Hcij)
{
    Mat Hcg(4, 4, CV_64FC1);
    CV_Assert(Hgij.size() == Hcij.size());
    int nStatus = Hgij.size();

    qDebug() << "Data amount" << nStatus;

    Mat Rgij(3, 3, CV_64FC1);
    Mat Rcij(3, 3, CV_64FC1);

    Mat rgij(3, 1, CV_64FC1);
    Mat rcij(3, 1, CV_64FC1);

    double theta_gij;
    double theta_cij;

    Mat rngij(3, 1, CV_64FC1);
    Mat rncij(3, 1, CV_64FC1);

    Mat Pgij(3, 1, CV_64FC1);
    Mat Pcij(3, 1, CV_64FC1);

    Mat tempA(3, 3, CV_64FC1);
    Mat tempb(3, 1, CV_64FC1);

    Mat A;
    Mat b;
    Mat pinA;


    Mat Pcg_prime(3, 1, CV_64FC1);
    Mat Pcg(3, 1, CV_64FC1);
    Mat PcgTrs(1, 3, CV_64FC1);

    Mat Rcg(3, 3, CV_64FC1);
    Mat eyeM = Mat::eye(3, 3, CV_64FC1);

    Mat Tgij(3, 1, CV_64FC1);
    Mat Tcij(3, 1, CV_64FC1);

    Mat tempAA(3, 3, CV_64FC1);
    Mat tempbb(3, 1, CV_64FC1);

    Mat AA;
    Mat bb;
    Mat pinAA;

    Mat Tcg(3, 1, CV_64FC1);



    for (int i = 0; i < nStatus; i++)
    {
        Hgij[i](Rect(0, 0, 3, 3)).copyTo(Rgij);
        Hcij[i](Rect(0, 0, 3, 3)).copyTo(Rcij);

        Rodrigues(Rgij, rgij);
        Rodrigues(Rcij, rcij);

        theta_gij = norm(rgij);
        theta_cij = norm(rcij);

        //                    for(int m=0;m<3;m++)
        //                    {
        //                        for(int j=0;j<3;j++)
        //                        {
        //                            qDebug() << i << ":" << Hgij[i].at<double>(m,j);

        //                        }
        //                    }
        //qDebug() << i << ":" << theta_cij;

        rngij = rgij / theta_gij;
        rncij = rcij / theta_cij;

        Pgij = 2 * sin(theta_gij / 2)*rngij;
        Pcij = 2 * sin(theta_cij / 2)*rncij;


        tempA = skew(Pgij + Pcij);
        tempb = Pcij - Pgij;

        //            for(int i=0;i<3;i++)
        //            {
        //                for(int j=0;j<3;j++)
        //                {
        //                    qDebug() << Rgij.at<double>(i,j);

        //                }
        //            }

        A.push_back(tempA); //add element at the end
        b.push_back(tempb);
    }


    //Compute rotation
    invert(A, pinA, DECOMP_SVD);


    Pcg_prime = pinA * b;
    Pcg = 2 * Pcg_prime / sqrt(1 + norm(Pcg_prime) * norm(Pcg_prime));
    PcgTrs = Pcg.t();
    Rcg = (1 - norm(Pcg) * norm(Pcg) / 2) * eyeM + 0.5 * (Pcg * PcgTrs + sqrt(4 - norm(Pcg)*norm(Pcg))*skew(Pcg));

    //Computer Translation
    for (int i = 0; i < nStatus; i++)
    {
        Hgij[i](Rect(0, 0, 3, 3)).copyTo(Rgij);
        Hcij[i](Rect(0, 0, 3, 3)).copyTo(Rcij);
        Hgij[i](Rect(3, 0, 1, 3)).copyTo(Tgij);
        Hcij[i](Rect(3, 0, 1, 3)).copyTo(Tcij);


        tempAA = Rgij - eyeM;
        tempbb = Rcg * Tcij - Tgij;

        AA.push_back(tempAA);
        bb.push_back(tempbb);
    }

    invert(AA, pinAA, DECOMP_SVD);
    Tcg = pinAA * bb;

    Rcg.copyTo(Hcg(Rect(0, 0, 3, 3)));
    Tcg.copyTo(Hcg(Rect(3, 0, 1, 3)));
    Hcg.at<double>(3, 0) = 0.0;
    Hcg.at<double>(3, 1) = 0.0;
    Hcg.at<double>(3, 2) = 0.0;
    Hcg.at<double>(3, 3) = 1.0;


    return Hcg;

}



Mat calibration::skew(Mat in)
{
    Mat out;
    double vx = in.at<double>(0,0);
    double vy = in.at<double>(1,0);
    double vz = in.at<double>(2,0);
    out = (Mat_<double>(3,3)<<0,-vz,vy,vz,0,-vx,-vy,vx,0);

    return out;
}

bool calibration::AutoMoveGetInsightAndRobotDataOnline(QString IP, quint16 port)
{
    //参数初始化
    calib_finish = 0;
    start_calib = 0;
    robot_move = 0;
    robot_move_finished = 0;
    robot_pos.clear();
    //

    vector<double> pos;
    //    QTcpSocket *client;
    //    QByteArray buffer_rec;
    QFile outfile(outputFileName);

    if ( outfile.open(  QIODevice::WriteOnly ) )
    {
        QTextStream stream( &outfile );
        stream << "<Calib_Points>" << "\r\n";


        //发送标定板种类信号
        QTcpSocket *client_calplate;
        client_calplate = new QTcpSocket;
        client_calplate->connectToHost(QHostAddress(IP),60004);
        client_calplate->waitForConnected();
        int connectState = client_calplate->state();
        //qDebug() << connectState;
        //qDebug() << client_calplate->waitForConnected();
        if(connectState == 3)
        {
            qDebug() << "Connect to camera succeed.\n";
            char cal_plate[1];
            itoa(calPlate, cal_plate, 10);
            //qDebug() << cal_plate;
            client_calplate->write(cal_plate);
            client_calplate->waitForBytesWritten(30000);
        }
        else
        {
            str = "Connect to camera failed.\n";
            qDebug() << "Connect to camera failed.\n";
            error_type = 1;
            client_calplate->close();
            delete client_calplate;
            return 0;
        }
        //

        //手眼标定触发信号
        start_calib = 1;

        for(int i = 0; i < num; i++)
        {
            //robot_move = 0;
            //qDebug() << "a";
            //状态更新
            while(!robot_move_finished&&!calib_finish)
            {
                QCoreApplication::processEvents();
            }
            //qDebug() << "b";
            if(calib_finish)
            {
                str += "Calibration manually terminated.\n";
                qDebug() << "Calibration manually terminated.";
                return 0;
            }

            //robot_move = 0;
            //
            //机器人位置和位姿读取
            pos.clear();
            pos.push_back(adsReadStruct.currentFlangePosition[0]);
            pos.push_back(adsReadStruct.currentFlangePosition[1]);
            pos.push_back(adsReadStruct.currentFlangePosition[2]);
            pos.push_back(adsReadStruct.currentFlangePosition[3]);
            pos.push_back(adsReadStruct.currentFlangePosition[4]);
            pos.push_back(adsReadStruct.currentFlangePosition[5]);
            robot_pos.push_back(pos);
            //            for(int j = 0; j < 6; j++)
            //                cout << robot_pos[i][j] << endl;
            //

            //拍照，Insight数据解析，存入文件
            vector<Point2f> pointBuf;

            char Do_work[] = "calib";
            double point[14][8];

            pointBuf.clear();
            QTcpSocket *client;
            QByteArray buffer_rec;

            client = new QTcpSocket;
            client->connectToHost(QHostAddress(IP),port);
            client->waitForConnected();
            int connectState = client->state();
            //qDebug() << connectState;
            if(connectState == 3)
            {
                qDebug() << "Connect to camera succeed.\n";
                client->write(Do_work);

                int time_counter = 0;

                int buf_num = floor(boardSize.height * boardSize.width * 2 / 8);
                int last_buf_num = boardSize.height * boardSize.width * 2 - buf_num * 8;


                //client->waitForBytesWritten(3000);
                //while((client->bytesAvailable() < 859) && (time_counter++ < 80))
                while((client->bytesAvailable() < ( 66 * (buf_num - 1) + 66 * (int)(bool)last_buf_num + 1 )) && (time_counter++ < 180))
                {
                    //qDebug() << "time_counter:" << time_counter;
                    client->waitForReadyRead(100);
                    //qDebug() << "client_available:" << client->bytesAvailable();
                }
                qDebug() << "msg size:" << client->bytesAvailable()<< "time_counter:" << time_counter ;
                if(time_counter > 180)
                {
                    qDebug() << "error: no message receive----------------------";
                    str += "Error: no message receive.\n";
                    error_type = 2;
                    return 0;
                }
                buffer_rec = client->readAll();
                qDebug() << "client_rvc:";
                qDebug() << buffer_rec.data();


                QString msg = (QString)buffer_rec;
                QStringList parameters = msg.split(',');



                if(parameters.size() == ( boardSize.height * boardSize.width * 2 + 1 ))//标定板特征点解析
                {
                    for(int i = 0; i < buf_num; i++)
                    {
                        for(int j = 0; j < 4; j++)
                        {
                            point[i][ j * 2 ] = parameters.value( i * 8 + j ).toDouble();
                        }
                        for(int j = 4; j < 8; j++)
                        {
                            point[i][ ( j - 4) * 2 + 1] = parameters.value( i * 8 + j ).toDouble();
                        }
                    }
                    for(int i = 0; i < buf_num; i++)
                    {
                        for(int j = 0; j < 4; j++)
                        {
                            pointBuf.push_back( Point2f(point[i][j * 2],point[i][j * 2 + 1]) );
                        }
                    }
                    for(int i = 0; i < last_buf_num / 2; i++)
                    {
                        pointBuf.push_back( Point2f(parameters.value(buf_num * 8 + i).toDouble(), parameters.value(buf_num * 8 + i  + last_buf_num / 2).toDouble()));
                    }
                    //cout << pointBuf;

                }
                else if(parameters.size() == 1 && msg != "")
                {
                    qDebug() << "Error" << msg;
                    str += "Read message error.\n";
                    error_type = 2;
                    return 0;

                }
                else{
                    qDebug() << "read message error";
                    str += "Read message error.\n Not enough points found.\n";
                    error_type = 2;
                    return 0;
                }
            }
            else
            {
                qDebug() << "Connect to camera failed.";
                str = "Connect to camera failed.\n";
                error_type = 1;
                client->close();
                delete client;
                return 0;
            }




            client->close();
            delete client;

            for(int i = 0; i < points_num; i++)
            {
                if(i == 0)
                    stream << " [";
                else
                    stream << "  ";
                stream << pointBuf[i].x << ", ";
                stream << pointBuf[i].y ;
                if(i == points_num - 1)
                    stream << "]" << "\r\n";
                else
                    stream << ";" << "\r\n";
            }
            //

            //状态更新
            robot_move_finished = 0;
            robot_move = 1;
            QThread::msleep(1500);
            qDebug() << "move finished";
            //



        }

        stream << "</Calib_Points>" << "\r\n";

        //写出机器人位置、位姿
        stream << "<robot_pose>" << "\r\n";
        for(int i = 0; i < num; i++)
        {
            for(int j = 0; j < 6; j++)
                stream << robot_pos[i][j] << " ";
            stream << "\r\n";
        }
        stream << "</robot_pose>" << "\r\n";
        //

        //状态更新
        calib_finish = 1;
        //
        client_calplate->close();
        delete client_calplate;

        outfile.close();
        return 1;
    }
    else
    {
        str += "Write file error.\n";
        return 0;
    }

}


void calibration::GetInsightDataToFile(QString IP, quint16 port)
{
    QTcpSocket *client;
    QByteArray buffer_rec;
    QFile outfile(outputFileName);

    if ( outfile.open(  QIODevice::WriteOnly ) )
    {
        QTextStream stream( &outfile );
        stream << "<Calib_Points>" << "\r\n";


        //发送标定板种类信号
        QTcpSocket *client_calplate;
        client_calplate = new QTcpSocket;
        client_calplate->connectToHost(QHostAddress(IP),60004);
        char cal_plate[1];
        itoa(calPlate, cal_plate, 10);
        qDebug() << cal_plate;
        client_calplate->write(cal_plate);
        client_calplate->waitForBytesWritten(30000);
        //


        for(int i = 0; i < num; i++)
        {
            int time_counter = 0;
            char sig[] = "waiting";
            client = new QTcpSocket;
            client->connectToHost(QHostAddress(IP),port);

            qDebug() << "Image: " << i+1;
            client->write(sig);
            while((client->bytesAvailable() < 3) && (time_counter++ < 600))
            {
                //qDebug() << "time_counter:" << time_counter;
                client->waitForReadyRead(100);
                //qDebug() << "client_available:" << client->bytesAvailable();
            }
            qDebug() << "time_counter:" << time_counter ;
            if(time_counter > 600)
            {
                qDebug() << "error: no signal received----------------------";
            }
            buffer_rec = client->readAll();
            QString msg = (QString)buffer_rec;
            //qDebug() << msg;
            if(msg == "next\r\n")
            {

                //calibration::ReadCalibDataFromInsightAndSaveToFile(IP, port);
                vector<Point2f> pointBuf;

                char Do_work[] = "calib";

                double point[14][8];

                pointBuf.clear();
                QTcpSocket *client;
                QByteArray buffer_rec;

                client = new QTcpSocket;
                client->connectToHost(QHostAddress(IP),port);

                QString str;

                client->write(Do_work);

                int time_counter = 0;

                int buf_num = floor(boardSize.height * boardSize.width * 2 / 8);
                int last_buf_num = boardSize.height * boardSize.width * 2 - buf_num * 8;


                //client->waitForBytesWritten(3000);
                //while((client->bytesAvailable() < 859) && (time_counter++ < 80))
                while((client->bytesAvailable() < ( 66 * (buf_num - 1) + 66 * (int)(bool)last_buf_num + 1 )) && (time_counter++ < 80))
                {
                    //qDebug() << "time_counter:" << time_counter;
                    client->waitForReadyRead(100);
                    //qDebug() << "client_available:" << client->bytesAvailable();
                }
                qDebug() << "msg size:" << client->bytesAvailable()<< "time_counter:" << time_counter ;
                if(time_counter > 80)
                {
                    qDebug() << "error: no message recive----------------------";

                }
                buffer_rec = client->readAll();
                qDebug() << "client_rvc:";
                qDebug() << buffer_rec.data();


                QString msg = (QString)buffer_rec;
                QStringList parameters = msg.split(',');


                if(parameters.size() == ( boardSize.height * boardSize.width * 2 + 1 ))//标定板特征点解析
                {
                    for(int i = 0; i < buf_num; i++)
                    {
                        for(int j = 0; j < 4; j++)
                        {
                            point[i][ j * 2 ] = parameters.value( i * 8 + j ).toDouble();
                        }
                        for(int j = 4; j < 8; j++)
                        {
                            point[i][ ( j - 4) * 2 + 1] = parameters.value( i * 8 + j ).toDouble();
                        }
                    }
                    for(int i = 0; i < buf_num; i++)
                    {
                        for(int j = 0; j < 4; j++)
                        {
                            pointBuf.push_back( Point2f(point[i][j * 2],point[i][j * 2 + 1]) );
                        }
                    }
                    for(int i = 0; i < last_buf_num / 2; i++)
                    {
                        pointBuf.push_back( Point2f(parameters.value(buf_num * 8 + i).toDouble(), parameters.value(buf_num * 8 + i  + last_buf_num / 2).toDouble()));
                    }
                    cout << pointBuf;

                }
                else if(parameters.size() == 1 && msg != "")
                {
                    qDebug() << "Error" << msg;

                }
                else{
                    qDebug() << "read message error";

                }


                client->close();
                delete client;

                for(int i = 0; i < points_num; i++)
                {
                    if(i == 0)
                        stream << " [";
                    else
                        stream << "  ";
                    stream << pointBuf[i].x << ", ";
                    stream << pointBuf[i].y ;
                    if(i == points_num - 1)
                        stream << "]" << "\r\n";
                    else
                        stream << ";" << "\r\n";
                }

            }
            client->close();

        }

        stream << "</Calib_Points>" << "\r\n";
        client_calplate->close();
        delete client_calplate;
    }
    delete client;
    outfile.close();
}

vector< vector<Point2f> > calibration::ReadCameraPointFromFile()
{
    vector< vector<Point2f> > calib_point;
    vector<Point2f> pointBuf;
    QFile file(outputFileName);

    QString temp;
    double x;
    double y;
    bool read_next_line = true;

    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream( &file );
        QString buffer;
        //file reading
        while(read_next_line && !stream.atEnd())
        {
            int j = 0;
            int k = 0;
            QString temp;
            buffer = stream.readLine();

            temp = "";
            while(buffer[j] != ' '&& buffer[j] != '\0')
            {
                temp += buffer[j];
                j++;
            }

            j = 0;
            k = 0;

            if(temp == "<Calib_Points>" && !stream.atEnd())//读取特征点
            {


                for(int m = 0; m < num; m++)
                {
                    for(int n = 0; n < points_num; n++)
                    {
                        buffer = stream.readLine();
                        if(buffer.length() > 5)
                        {
                            int j = 2;
                            temp = "";
                            while(buffer[j] != ';'&&buffer[j] != ']')
                            {
                                while(buffer[j] != ','&&buffer[j] != ';'&&buffer[j] != ']')
                                {
                                    temp += buffer[j];
                                    j++;
                                }
                                if(buffer[j] == ',')
                                {
                                    j++;
                                    j++;
                                    y = temp.toDouble();
                                    temp = "";
                                }
                                //            qDebug() << temp;
                                if(buffer[j] == ';'||buffer[j] == ']')
                                {
                                    x = temp.toDouble();
                                    pointBuf.push_back( Point2f(x, y));
                                }
                            }
                        }
                        else
                        {
                            error_type = 5;
                            str += "Output file error.\n";
                            return calib_point;
                        }
                    }
                    //cout << m+1 << ": " << endl << pointBuf;
                    calib_point.push_back(pointBuf);
                    pointBuf.clear();
                }
                read_next_line = false;
            }
        }
    }
    else
    {
        error_type = 5;
        str += "Output file error.\n";
        return calib_point;
    }
    //    for(int m = 0; m < num; m++)
    //    {
    //        cout << calib_point[m] <<endl;
    //    }
    file.close();
    return calib_point;
}
bool calibration::CameraCalibrate()
{

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    Mat distCoeffs = Mat::zeros(8, 1, CV_64F);
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    vector<vector<Point2f> > imagePoints;
    //vector<float> reprojErrs;
    //double totalAvgErr;

    rvecs.clear();
    tvecs.clear();

    imagePoints = calibration::ReadCameraPointFromFile();

    vector<vector<Point3f> > objectPoints(1);
    objectPoints.clear();
    vector<Point3f> corners;
    corners.clear();

    //    switch (calPlate) {
    //    case 1:
    //        for( int i = 0; i < boardSize.height; ++i )
    //            for( int j = 0; j < boardSize.width; ++j )
    //                corners.push_back(Point3f(float( j*squareSize - ((boardSize.width - 1) / 2) * squareSize ), float( i*squareSize - ((boardSize.height - 1) / 2) * squareSize ), 0));
    //        break;
    //    case 2:

    //        break;
    //    case 3:

    //        break;
    //    default:
    //        break;
    //    }

    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            corners.push_back(Point3f(float( j*squareSize - ((double)(boardSize.width - 1) / 2) * squareSize ), float( i*squareSize - ((double)(boardSize.height - 1) / 2) * squareSize ), 0));

    objectPoints.push_back(corners);





    //Find intrinsic and extrinsic camera parameters
    if(imagePoints.size() != 0)
    {
        objectPoints.resize(imagePoints.size(),objectPoints[0]);

        //    for(int m = 0; m < num; m++)
        //    {
        //        cout << objectPoints[m] <<endl;
        //    }
        double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                                     distCoeffs, rvecs, tvecs);
        cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;

        str += "Reprojection error: ";
        str += QString::number(rms,'f',5);
        str += '\n';

        bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

        if( ok )
        {
            if(!saveCameraParams( cameraMatrix, distCoeffs, rvecs ,tvecs, rms ))
            {
                return false;
            }
        }


        return ok;
    }
    else
    {
        str += "Error image points received!\n";
        qDebug() << "Error image points received!";
        error_type = 3;
        return false;
    }


}
bool calibration::saveCameraParams( Mat cameraMatrix, Mat distCoeffs, vector<Mat> rvecs, vector<Mat> tvecs, double rms )
{

    QFile outfile(outputFileName);
    if ( outfile.open(  QIODevice::WriteOnly | QIODevice::Append ) )
    {
        QTextStream stream( &outfile );
        //cameraMatrix
        stream << "<Camera_Matrix>" << "\r\n";
        for(int i = 0; i < 3; i++)
        {
            if(i == 0)
                stream << " [";
            else
                stream << "  ";
            for(int j = 0; j <3; j++)
            {
                if(j == 2)
                    stream << cameraMatrix.at<double>(i, j) ;
                else
                    stream << cameraMatrix.at<double>(i, j) << ", ";
            }
            if(i == 2)
                stream << "]" << "\r\n";
            else
                stream << ";" << "\r\n";
        }
        stream << "</Camera_Matrix>" << "\r\n";
        //

        //distCoeffs
        stream << "<Distortion_Coefficients>" << "\r\n";
        for(int i = 0; i < 5; i++)
        {
            if(i == 0)
                stream << " [";
            else
                stream << "  ";
            int j = 0;
            stream << distCoeffs.at<double>(i, j) ;
            if(i == 4)
                stream << "]" << "\r\n";
            else
                stream << ";" << "\r\n";
        }
        stream << "</Distortion_Coefficients>" << "\r\n";
        //

        //rvecs/tvecs
        if(!rvecs.empty() && !tvecs.empty() && rvecs.size() == tvecs.size())
        {
            Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
            for(int i = 0; i < (int)rvecs.size(); i++)
            {
                Mat r = bigmat(Range(i, i + 1), Range(0, 3));
                Mat t = bigmat(Range(i, i + 1), Range(3, 6));

                CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
                CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
                //*.t() is MatExpr (not Mat) so we can use assignment operator
                r = rvecs[i].t();
                t = tvecs[i].t();
            }
            stream << "<!-- a set of 6-tuples (rotation vector + translation vector) for each view -->" <<"\r\n";
            stream << "<Extrinsic_Parameters>" << "\r\n";
            for(int i = 0; i < num; i++)
            {
                if(i == 0)
                    stream << " [";
                else
                    stream << "  ";
                for(int j = 0; j < 6; j++)
                {
                    if(j == 5)
                        stream << bigmat.at<double>(i, j) ;
                    else
                        stream << bigmat.at<double>(i, j) << ", ";
                }
                if(i == num - 1)
                    stream << "]" << "\r\n";
                else
                    stream << ";" << "\r\n";
            }
            stream << "</Extrinsic_Parameters>" << "\r\n";
        }
        else
        {
            return 0;
        }
        //
        stream << "<Avg_Reprojection_Error>" << "\r\n";
        stream << " " << rms << "\r\n";
        stream << "<Avg_Reprojection_Error>" << "\r\n";
        //

        outfile.close();
        return 1;
    }
    else
    {
        error_type = 5;
        str += "Output file error.\n";
        return 0;
    }

}
bool calibration::SaveHcgToFile()
{
    QFile outfile(outputFileName);
    if ( outfile.open( QIODevice::WriteOnly | QIODevice::Append ) )
    {
        QTextStream stream( &outfile );
        stream << "<Hcg>" << "\r\n";
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                stream << Hcg.at<double>(i,j) << " ";
            }
            stream << "\r\n";

        }
        stream << "</Hcg>" << "\r\n";

        outfile.close();
        return 1;
    }
    else
    {
        error_type = 5;
        str += "Output file error.\n";
        return 0;
    }

}

bool calibration::SaveHcalToFile()
{
    QFile outfile(outputFileName);
    if ( outfile.open( QIODevice::WriteOnly | QIODevice::Append ) )
    {
        QTextStream stream( &outfile );
        stream << "<Hcal>" << "\r\n";
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                stream << Hcal.at<double>(i,j) << " ";
            }
            stream << "\r\n";

        }
        stream << "</Hcal>" << "\r\n";

        outfile.close();
        return 1;
    }
    else{
        error_type = 5;
        str += "Output file error.\n";
        return 0;
    }

}

bool calibration::SaveHc1ToFile()
{
    QFile outfile(outputFileName);
    if ( outfile.open( QIODevice::WriteOnly | QIODevice::Append ) )
    {
        QTextStream stream( &outfile );
        stream << "<Hc1>" << "\r\n";
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                stream << Hc1.at<double>(i,j) << " ";
            }
            stream << "\r\n";

        }
        stream << "</Hc1>" << "\r\n";

        outfile.close();
        return 1;
    }
    else{
        error_type = 5;
        str += "Output file error.\n";
        return 0;
    }

}

void calibration::ReadInParametersFromFile()
{
    QFile file(inParameters);

    bool read_next_line = true;

    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream( &file );
        QString buffer;
        //file reading
        while(read_next_line)
        {

            int j = 0;
            QString temp;
            buffer = stream.readLine();

            //输入文件错误的处理
            if(buffer[0] == '\0')
            {
                error_type = 6;
                //ui->label_statusText->setText("Input file error.\n");
                qDebug() << "Input file error.\n";
                return;
            }
            //

            temp = "";
            while(buffer[j] != '\0')
            {
                temp += buffer[j];
                j++;
            }
            j = 0;

            if(temp == "<Camera_IP>")        //读取相机IP
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                Camera_IP = temp;
            }
            if(temp == "<ImageSize_Width>")        //读取图像宽
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                ImageSize_Width = temp.toInt();
            }
            if(temp == "<ImageSize_Height>")        //读取图像高
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                ImageSize_Height = temp.toInt();
            }
            if(temp == "<BoardSize_Width>")        //读取标定板宽
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                BoardSize_Width = temp.toInt();
            }
            if(temp == "<BoardSize_Height>")        //读取标定板高
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                BoardSize_Height = temp.toInt();
            }
            if(temp == "<Square_Size>")        //读取标定板特征点间距
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                Square_Size = temp.toFloat();
            }
            if(temp == "<Calibrate_NrOfFrameToUse>")        //读取标定板图片数量
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                Calibrate_NrOfFrameToUse = temp.toInt();
            }
            if(temp == "<Write_outputFileName>")        //读取输出文件名
            {
                buffer = stream.readLine();
                temp = "";
                while(buffer[j] != '\0')
                {
                    temp += buffer[j];
                    j++;
                }
                //qDebug() << temp;
                Write_outputFileName = "C:/SiasunRobot/opencv/";
                Write_outputFileName += temp;
                read_next_line = false;
            }

        }
    }
    else{
        error_type = 6;
        //ui->label_statusText->setText("Input file error.\n");
        qDebug() << "Input file error.\n";
        return;
    }

    file.close();
    // end of file reading
}

void calibration::ReadOutCameraDataFromFile(int cameraPositionNum)
{
    QString filename;
    //qDebug() << cameraPositionNum;
    filename = cameraData + QString::number(cameraPositionNum,10) + ".txt";
    qDebug() << filename;
    QFile file(filename);

    double camera_matrix[9];//读取内参临时储存
    double dist_coeff[5];//读取畸变参数临时储存
    bool read_next_line = true;
    bool camera_matrix_read = false;
    bool dist_coeff_read = false;
    bool Hc1_read = false;
    bool Hcal_read = false;
    bool Hcg_read = false;

    if(file.open( QIODevice::ReadOnly ))
    {
        QTextStream stream( &file );
        QString buffer;
        //file reading
        while(read_next_line)
        {

            int j = 0;
            int k = 0;
            QString temp;
            buffer = stream.readLine();

            //输出文件错误的处理
            if(buffer[0] == '\0' && ((camera_matrix_read && dist_coeff_read && Hc1_read && Hcal_read && Hcg_read) == false))
            {
                error_type = 5;
                //ui->label_statusText->setText("Output file error.\n");
                return;
            }
            //

            temp = "";
            while(buffer[j] != ' '&& buffer[j] != '\0')
            {
                temp += buffer[j];
                j++;
            }
            j = 0;
            k = 0;
            if(camera_matrix_read == false && temp == "<Camera_Matrix>")        //读取内参
            {
                buffer = stream.readLine();
                j = 2;
                while(k < 3)
                {

                    temp = "";
                    while(buffer[j] != ','&& buffer[j] != ';'&&buffer[j] != ']')
                    {
                        temp += buffer[j];
                        j++;
                    }
                    j++;
                    j++;
                    //qDebug() << buffer;
                    //qDebug() << temp;
                    camera_matrix[k] = temp.toDouble();
                    //qDebug("%.8lf", camera_matrix[k]);
                    k++;
                }

                buffer = stream.readLine();
                j = 2;
                while( k < 6 )
                {
                    temp = "";
                    while(buffer[j] != ','&& buffer[j] != ';'&&buffer[j] != ']')
                    {
                        temp += buffer[j];
                        j++;
                    }
                    j++;
                    j++;
                    //qDebug() << buffer;
                    //qDebug() << temp;
                    camera_matrix[k] = temp.toDouble();
                    //qDebug("%.8lf", camera_matrix[k]);
                    k++;
                }

                buffer = stream.readLine();
                j = 2;
                while( k < 9 )
                {
                    temp = "";
                    while(buffer[j] != ','&& buffer[j] != ';'&&buffer[j] != ']')
                    {
                        temp += buffer[j];
                        j++;
                    }
                    j++;
                    j++;
                    //qDebug() << buffer;
                    //qDebug() << temp;
                    camera_matrix[k] = temp.toDouble();
                    //qDebug("%.8lf", camera_matrix[k]);
                    k++;
                }
                Camera_Matrix = (Mat_<double>(3,3) << camera_matrix[0],camera_matrix[1],camera_matrix[2],camera_matrix[3],camera_matrix[4],camera_matrix[5],camera_matrix[6],camera_matrix[7],camera_matrix[8]);
                camera_matrix_read = true;
            }

            if(dist_coeff_read == false && temp == "<Distortion_Coefficients>")        //读取畸变参数
            {


                while(k < 5)
                {
                    buffer = stream.readLine();
                    j = 2;
                    temp = "";
                    while(buffer[j] != ','&& buffer[j] != ';'&&buffer[j] != ']')
                    {
                        temp += buffer[j];
                        j++;
                    }
                    //qDebug() << buffer;
                    //qDebug() << temp;
                    dist_coeff[k] = temp.toDouble();
                    //qDebug("%.8lf", dist_coeff[k]);
                    k++;
                }

                distCoeffs = (Mat_<double>(5,1) << dist_coeff[0], dist_coeff[1], dist_coeff[2], dist_coeff[3], dist_coeff[4]);
                dist_coeff_read = true;
            }

            if(Hcg_read == false && temp == "<Hcg>")        //读取Hcg
            {
                for(int i = 0; i < 4; i++)
                {
                    buffer = stream.readLine();
                    j = 0;
                    for(int k = 0; k < 4; k++)
                    {
                        temp = "";
                        while(buffer[j] != ' ')
                        {
                            temp += buffer[j];
                            j++;
                        }
                        j++;
                        //qDebug() << buffer;
                        //qDebug() << temp;
                        Hcg.at<double>(i, k) = temp.toDouble();
                    }
                }
                Hcg_read = true;
            }

            if(Hc1_read == false && temp == "<Hc1>")        //读取Hc1
            {
                for(int i = 0; i < 4; i++)
                {
                    buffer = stream.readLine();
                    j = 0;
                    for(int k = 0; k < 4; k++)
                    {
                        temp = "";
                        while(buffer[j] != ' ')
                        {
                            temp += buffer[j];
                            j++;
                        }
                        j++;
                        //qDebug() << buffer;
                        //qDebug() << temp;
                        Hc1.at<double>(i, k) = temp.toDouble();
                    }
                }
                Hc1_read = true;
            }

            if(Hcal_read == false && temp == "<Hcal>")        //读取Hcal
            {
                for(int i = 0; i < 4; i++)
                {
                    buffer = stream.readLine();
                    j = 0;
                    for(int k = 0; k < 4; k++)
                    {
                        temp = "";
                        while(buffer[j] != ' ')
                        {
                            temp += buffer[j];
                            j++;
                        }
                        j++;
                        //qDebug() << buffer;
                        //qDebug() << temp;
                        Hcal.at<double>(i, k) = temp.toDouble();
                    }
                }
                Hcal_read = true;
            }
            if(camera_matrix_read && Hc1_read && Hcal_read && Hcg_read )
                read_next_line = false;
        }
    }
    else{
        error_type = 5;
        //ui->label_statusText->setText("Output file error.\n");
        qDebug() << "Input file error.\n";
        return;
    }

    file.close();
    // end of file reading
}

Mat calibration::CalculatePosition(double x, double y, int num)
{
    //qDebug() << "**" << num;
    ReadOutCameraDataFromFile(num);


    //    for(int i = 0; i < 4; i++)
    //    {
    //        for(int j = 0; j < 4; j++)
    //        {
    //            qDebug() << cal.Hcal.at<double>(i,j);
    //        }
    //    }

    //    for(int i = 0; i < 3; i++)
    //    {
    //        for(int j = 0; j < 3; j++)
    //        {
    //            qDebug() << Camera_Matrix.at<double>(i,j);
    //        }
    //    }
    //        for(int i = 0; i < 4; i++)
    //        {
    //            for(int j = 0; j < 4; j++)
    //            {
    //                qDebug() << Hc1.at<double>(i,j);
    //            }
    //        }
    Mat M_inv(3, 3, CV_64FC1);//内参矩阵的逆
    Mat Hc(3, 3, CV_64FC1);//3*3外参矩阵
    Mat pixels(3, 1, CV_64FC1);//图像像素坐标（u,v,1）
    Mat Cc(3, 1, CV_64FC1);//标定板坐标系下（x,y,1）
    Mat Cc0(3, 1, CV_64FC1);//标定板坐标系下（x,y,s）
    //Mat ObjInCal0(4, 1, CV_64FC1);//标定板坐标系下（x,y,0,1）
    Mat ObjInCal(4, 4, CV_64FC1);//标定板坐标系下4*4
    Mat ObjInBase(4, 4, CV_64FC1);//机器人坐标系下（x,y,z,1）
    //Mat Objpos(3, 1, CV_64FC1);//机器人坐标系下（x,y,z)
    Mat Objpos(6, 1, CV_64FC1);//机器人坐标系下（x,y,z)
    Mat ObjposRot(3, 3, CV_64FC1);//rotation matrix

    //输出文件错误的处理
    if(error_type == 5)
    {
        Objpos.at<double>(0,0) = 0;
        Objpos.at<double>(1,0) = 0;
        Objpos.at<double>(2,0) = 0;
        return Objpos;
    }

    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            Hc.at<double>(j, i) = Hc1.at<double>(j,i);
        }
    }
    for(int i = 3; i < 4; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            Hc.at<double>(j, i - 1) = Hc1.at<double>(j,i);
        }
    }
    invert(Camera_Matrix * Hc, M_inv, DECOMP_SVD);
    pixels.at<double>(0,0) = x;
    pixels.at<double>(1,0) = y;
    pixels.at<double>(2,0) = 1;
    Cc0 = M_inv * pixels;
    Cc = Cc0 / Cc0.at<double>(2,0);
    //        ObjInCal0 = Cc;
    //        ObjInCal0.at<double>(2,0) = 0;
    //        ObjInCal0.at<double>(3,0) = 1;
    ObjInCal = (Mat_<double>(4,4) << 1, 0, 0, Cc.at<double>(0,0), 0, 1, 0, Cc.at<double>(1,0), 0, 0, 1, 0, 0, 0, 0, 1);
    ObjInBase = Hcal * ObjInCal;

    ObjInBase(Rect(0, 0, 3, 3)).copyTo(ObjposRot);
    Mat ObjposRot0(3, 1, CV_64FC1);//rotation matrix 3*1
    ObjposRot0 = H2pose(ObjposRot);

    //ObjInBase(Rect(3, 0, 1, 3)).copyTo(Objpos);





    Objpos.at<double>(0, 0) = ObjInBase.at<double>(0, 3);
    Objpos.at<double>(1, 0) = ObjInBase.at<double>(1, 3);
    Objpos.at<double>(2, 0) = ObjInBase.at<double>(2, 3);
    Objpos.at<double>(3, 0) = ObjposRot0.at<double>(0, 0);
    Objpos.at<double>(4, 0) = ObjposRot0.at<double>(1, 0);
    Objpos.at<double>(5, 0) = ObjposRot0.at<double>(2, 0);

    qDebug() << "Target pose:";
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 1; j++)
        {
            qDebug() << Objpos.at<double>(i,j);
        }
    }




    return Objpos;
}

Mat calibration::pose2H(double x, double y, double z, double Rx, double Ry, double Rz)
{
    Mat H(4, 4, CV_64FC1);
    Mat Rotx(3, 3, CV_64FC1);
    Mat Roty(3, 3, CV_64FC1);
    Mat Rotz(3, 3, CV_64FC1);
    Mat R(3, 3, CV_64FC1);

    Rotx = (Mat_<double>(3,3)<< 1, 0, 0, 0, cos(Rx/180*pi), -sin(Rx/180*pi), 0, sin(Rx/180*pi), cos(Rx/180*pi));
    Roty = (Mat_<double>(3,3)<< cos(Ry/180*pi), 0, sin(Ry/180*pi), 0, 1, 0, -sin(Ry/180*pi), 0, cos(Ry/180*pi));
    Rotz = (Mat_<double>(3,3)<< cos(Rz/180*pi), -sin(Rz/180*pi), 0, sin(Rz/180*pi), cos(Rz/180*pi), 0, 0, 0, 1);

    R = Rotx * Roty * Rotz;

    R.copyTo(H(Rect(0, 0, 3, 3)));

    H.at<double>(0, 3) = x;
    H.at<double>(1, 3) = y;
    H.at<double>(2, 3) = z;


    H.at<double>(3, 0) = 0.0;
    H.at<double>(3, 1) = 0.0;
    H.at<double>(3, 2) = 0.0;
    H.at<double>(3, 3) = 1.0;
    return H;
}

vector<Mat> calibration::CalculateRobotMovePosition(double x, double y, double z, double Rx, double Ry, double Rz,int h_index)
{

    double x1 = x;
    double y1 = y;
    double z1 = z;
    double Rx1 = Rx;
    double Ry1 = Ry;
    double Rz1 = Rz;
    double x2;
    double y2;
    double z2;
    double Rx2;
    double Ry2;
    double Rz2;
    Mat H1(4, 4, CV_64FC1);
    Mat H2(4, 4, CV_64FC1);
    Mat R2(3, 3, CV_64FC1);
    Mat r2(3, 1, CV_64FC1);

    int num = 14;
    vector<Mat> H;
    vector<Mat> Pos;
    Mat H_temp(4, 4, CV_64FC1);
    Mat Pos_temp(6, 1, CV_64FC1);
    H.clear();
    Pos.clear();

    //old edition

    //    double T[150][3] = {
    //        3.56604, 86.6045, -1.13687e-13,
    //        60.3466, -2.18413, 0.68896,
    //        20.9627, -90.3684, 26.4123,
    //        -95.5223, -36.4806, 15.5265,
    //        117.738, 68.7972, 47.2202,
    //        -43.0321, 41.8741, -7.34599,
    //        -15.7059, -3.83467, -17.4653,
    //        -67.5067, -24.0719, -28.5323,
    //        -23.5395, -143.893, 22.0467,
    //        -12.3323, -9.63469, -27.7956,
    //        -64.8365, 164.031, 40.6353,
    //        0.050998, 2.30099, 5.56267,
    //        -50.0068, -76.7845, 10.7631
    //    };


    //    double R[150][9] = {
    //        1, -7.90123e-05, -0.000491916, -7.90123e-05, 0.949699, -0.313163, 0.000491916, 0.313163, 0.949699,
    //        0.983572, 2.58058e-05, -0.180519, 2.58058e-05, 1, 0.000283559, 0.180519, -0.000283559, 0.983571,
    //        0.995661, -0.0793434, -0.0486212, 0.0925526, 0.89862, 0.428854, 0.0096652, -0.431494, 0.902064,
    //        0.965019, -0.0171188, 0.261619, -0.00420216, 0.996728, 0.0807205, -0.262144, -0.0789962, 0.96179,
    //        0.943335, -0.0170093, -0.331406, -0.0922676, 0.945861, -0.311182, 0.318757, 0.324127, 0.890694,
    //        0.988825, 0.0232704, 0.147254, -0.0113347, 0.996619, -0.0813804, -0.14865, 0.0788019, 0.985745,
    //        0.967818, -0.250017, 0.028625, 0.249998, 0.968237, 0.00429183, -0.0287888, 0.00300248, 0.999581,
    //        0.933208, -0.343496, 0.105514, 0.308251, 0.916154, 0.25621, -0.184674, -0.206572, 0.960845,
    //        0.711322, 0.701866, 0.0374875, -0.698938, 0.700708, 0.143155, 0.0742081, -0.128031, 0.98899,
    //        0.946539, 0.321006, 0.031917, -0.318803, 0.945954, -0.0594557, -0.0492777, 0.0461019, 0.997721,
    //        0.986313, 0.108467, 0.124183, -0.0389179, 0.885024, -0.463916, -0.160224, 0.452734, 0.877132,
    //        0.538623, -0.841416, -0.0436381, 0.830814, 0.521797, 0.193587, -0.140117, -0.140526, 0.980112,
    //        0.98437, 0.120051, 0.128858, -0.151585, 0.95003, 0.272881, -0.0896594, -0.288149, 0.953379
    //    };

    //
    //h=190mm
    double T190[14][3] = {
        2.59, 78.84, 0.03,
        45.31, -2.98197, 0.984035,
        13.7178, -105.132, 33.8272,
        -64.2803, -15.1072, 11.8906,
        73.3086, 68.8301, 40.0299,
        -33.5966, 14.2076, -0.440821,
        -11.5978, 23.9336, -25.9686,
        -31.6865, -52.9785, -13.0601,
        -17.8639, -90.2428, 16.2406,
        -12.098, -9.5903, -16.2677,
        -41.8135, 145.09, 36.4513,
        -24.8325, -3.16978, -10.9657,
        -35.6935, -77.5036, 19.5295
    };


    double R190[14][9] = {
        1, -4.00322e-17, -1.45365e-16, 8.37135e-18, 0.946593, -0.322431, -1.82558e-16, 0.322431, 0.946593,
        0.982255, 4.57226e-17, -0.187553, -2.77556e-17, 1, -5.55112e-17, 0.187553, 5.55112e-17, 0.982255,
        0.995502, -0.0837735, -0.0442559, 0.0946964, 0.894701, 0.436513, 0.00302755, -0.43874, 0.898609,
        0.964095, -0.0188094, 0.26489, -0.00490613, 0.996057, 0.0885847, -0.265511, -0.0867037, 0.960201,
        0.942969, -0.0186046, -0.33236, -0.0951593, 0.941706, -0.3227, 0.318989, 0.335923, 0.886229,
        0.988412, 0.0237661, 0.149925, -0.0113309, 0.996464, -0.0832582, -0.151373, 0.0805946, 0.985186,
        0.966296, -0.255975, 0.027374, 0.255524, 0.966618, 0.0189124, -0.0313013, -0.0112803, 0.999446,
        0.973408, -0.204123, 0.103973, 0.173342, 0.953065, 0.248231, -0.149762, -0.223607, 0.963105,
        0.804195, 0.594167, 0.0153252, -0.589578, 0.794188, 0.147184, 0.0752809, -0.1274, 0.98899,
        0.940991, 0.337058, 0.0304499, -0.335265, 0.940687, -0.0520191, -0.0461773, 0.0387407, 0.998182,
        0.965133, -0.225345, 0.133188, 0.261659, 0.844792, -0.466757, -0.00733454, 0.485332, 0.874299,
        0.776413, -0.62991, 0.0199019, 0.613383, 0.762539, 0.205662, -0.144724, -0.147471, 0.978421,
        0.983676, 0.127802, 0.126682, -0.155831, 0.957041, 0.244517, -0.0899904, -0.260267, 0.961334
    };
    //
    //
    //h=300mm
    double T300[14][3] = {
        1.13687e-13, 106.43, 8.64,
        66.21, 0.713781, -9.31778,
        20.599, -146.117, 46.672,
        -89.0075, -33.3773, 23.6114,
        110.661, 114.456, 44.4467,
        -52.1606, 25.95, 3.70981,
        -15.0928, 10.4981, -24.4339,
        -38.5358, -76.7015, -1.13244,
        -18.5651, -102.927, 9.68473,
        -13.3743, -8.96848, -16.173,
        -58.5802, 203.451, 45.7605,
        -34.4379, -30.8768, -0.446547,
        -44.0147, -107.187, 25.716

    };


    double R300[14][9] = {
        1, -1.08034e-17, -3.80343e-17, -5.06092e-17, 0.946593, -0.322431, 3.72553e-16, 0.322431, 0.946593,
        0.982255, -4.5257e-18, -0.187553, 6.245e-17, 1, 0, 0.187553, 0, 0.982255,
        0.995502, -0.0837735, -0.0442559, 0.0946964, 0.894701, 0.436513, 0.00302755, -0.43874, 0.898609,
        0.964095, -0.0188094, 0.26489, -0.00490613, 0.996057, 0.0885847, -0.265511, -0.0867037, 0.960201,
        0.942969, -0.0186046, -0.33236, -0.0951593, 0.941706, -0.3227, 0.318989, 0.335923, 0.886229,
        0.988412, 0.0237661, 0.149925, -0.0113309, 0.996464, -0.0832582, -0.151373, 0.0805946, 0.985186,
        0.966296, -0.255975, 0.027374, 0.255524, 0.966618, 0.0189124, -0.0313013, -0.0112803, 0.999446,
        0.973408, -0.204123, 0.103973, 0.173342, 0.953065, 0.248231, -0.149762, -0.223607, 0.963105,
        0.804195, 0.594167, 0.0153252, -0.589578, 0.794188, 0.147184, 0.0752809, -0.1274, 0.98899,
        0.940991, 0.337058, 0.0304499, -0.335265, 0.940687, -0.0520191, -0.0461773, 0.0387407, 0.998182,
        0.965133, -0.225345, 0.133188, 0.261659, 0.844792, -0.466757, -0.00733454, 0.485332, 0.874299,
        0.776413, -0.62991, 0.0199019, 0.613383, 0.762539, 0.205662, -0.144724, -0.147471, 0.978421,
        0.983676, 0.127802, 0.126682, -0.155831, 0.957041, 0.244517, -0.0899904, -0.260267, 0.961334

    };
    //
    //
    //h=420mm
    double T420[14][3] = {
        1.13687e-13, 155.64, -1.94,
        102.57, 3.03593, 0.0117504,
        17.6756, -216.731, 72.4167,
        -121.529, -31.6972, 20.3463,
        149.261, 150.129, 54.7389,
        -67.5755, 35.7781, 7.01403,
        -21.5441, 16.1664, -24.7478,
        -60.0042, -116.905, 8.3414,
        -9.92406, -121.842, 7.46759,
        -11.5724, -5.24489, -4.26743,
        -84.2416, 264.49, 55.1799,
        -36.0121, -62.3591, 12.1998,
        -57.7167, -133.939, 35.4929

    };


    double R420[14][9] = {
        1, 1.65226e-16, 3.71151e-16, -2.03699e-18, 0.946593, -0.322431, -7.15359e-17, 0.322431, 0.946593,
        0.982255, 3.61988e-17, -0.187553, 3.46945e-17, 1, 5.55112e-17, 0.187553, -1.66533e-16, 0.982255,
        0.995502, -0.0837735, -0.0442559, 0.0946964, 0.894701, 0.436513, 0.00302755, -0.43874, 0.898609,
        0.964095, -0.0188094, 0.26489, -0.00490613, 0.996057, 0.0885847, -0.265511, -0.0867037, 0.960201,
        0.942969, -0.0186046, -0.33236, -0.0951593, 0.941706, -0.3227, 0.318989, 0.335923, 0.886229,
        0.988412, 0.0237661, 0.149925, -0.0113309, 0.996464, -0.0832582, -0.151373, 0.0805946, 0.985186,
        0.966296, -0.255975, 0.027374, 0.255524, 0.966618, 0.0189124, -0.0313013, -0.0112803, 0.999446,
        0.973408, -0.204123, 0.103973, 0.173342, 0.953065, 0.248231, -0.149762, -0.223607, 0.963105,
        0.804195, 0.594167, 0.0153252, -0.589578, 0.794188, 0.147184, 0.0752809, -0.1274, 0.98899,
        0.940991, 0.337058, 0.0304499, -0.335265, 0.940687, -0.0520191, -0.0461773, 0.0387407, 0.998182,
        0.965133, -0.225345, 0.133188, 0.261659, 0.844792, -0.466757, -0.00733454, 0.485332, 0.874299,
        0.776413, -0.62991, 0.0199019, 0.613383, 0.762539, 0.205662, -0.144724, -0.147471, 0.978421,
        0.983676, 0.127802, 0.126682, -0.155831, 0.957041, 0.244517, -0.0899904, -0.260267, 0.961334
    };
    //
    double T[14][3];
    double R[14][9];
    if(h_index == 0)
    {
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 3; j++)
            {
                T[i][j] = T190[i][j];
            }
        }
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 9; j++)
            {
                R[i][j] = R190[i][j];
            }
        }

    }
    else if(h_index == 1)
    {
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 3; j++)
            {
                T[i][j] = T300[i][j];
            }
        }
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 9; j++)
            {
                R[i][j] = R300[i][j];
            }
        }
    }
    else if(h_index == 2)
    {
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 3; j++)
            {
                T[i][j] = T420[i][j];
            }
        }
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 9; j++)
            {
                R[i][j] = R420[i][j];
            }
        }
    }
    else
    {
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 3; j++)
            {
                T[i][j] = T300[i][j];
            }
        }
        for(int i = 0;i < 14;i++)
        {
            for(int j = 0;j < 9; j++)
            {
                R[i][j] = R300[i][j];
            }
        }
    }










    for(int i = 0; i < num - 1; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            H_temp.at<double>(j,3) = T[i][j] ;
            H_temp.at<double>(3,j) = 0.0;
        }
        H_temp.at<double>(3,3) = 1.0;
        for(int j = 0; j < 3; j++)
        {
            H_temp.at<double>(0,j) = R[i][j] ;
        }
        for(int j = 3; j < 6; j++)
        {
            H_temp.at<double>(1,j-3) = R[i][j] ;
        }
        for(int j = 6; j < 9; j++)
        {
            H_temp.at<double>(2,j-6) = R[i][j] ;
        }
        Mat temp = H_temp.clone();
        H.push_back(temp);
    }
    //    for(int i = 0; i < num - 1; i++)
    //    {
    //        for(int j = 0; j < 4; j++)
    //        {
    //            for(int m = 0; m < 4; m++)
    //            qDebug() << H[i].at<double>(j,m);
    //        }
    //    }

    //    Pos_temp.at<double>(0,0) = x1;
    //    Pos_temp.at<double>(1,0) = y1;
    //    Pos_temp.at<double>(2,0) = z1;
    //    Pos_temp.at<double>(3,0) = Rx1;
    //    Pos_temp.at<double>(4,0) = Ry1;
    //    Pos_temp.at<double>(5,0) = Rz1;
    //    Mat temp =Pos_temp.clone();
    //    Pos.push_back(temp);
    //qDebug() << x1 << " " << y1 << " " << z1 << " " << Rx1 << " " << Ry1 << " " << Rz1;
    H1 = pose2H(x1, y1, z1, Rx1, Ry1, Rz1);
    H2 = H1 * H[0];
    H2(Rect(0, 0, 3, 3)).copyTo(R2);

    //Rodrigues(R2, r2);
    //    Rx2 = r2.at<double>(0,0);
    //    Ry2 = r2.at<double>(1,0);
    //    Rz2 = r2.at<double>(2,0);
    if(abs(R2.at<double>(2,2)) < 1e-8 && abs(R2.at<double>(1,2)) < 1e-8)
    {
        Rx2 = 0;
        Ry2 = atan2(R2.at<double>(0,2),R2.at<double>(2,2));
        Rz2 = atan2(R2.at<double>(1,0),R2.at<double>(1,1));
    }
    else
    {
        Rx2 = atan2(-R2.at<double>(1,2),R2.at<double>(2,2));
        double sr = sin(Rx2);
        double cr = cos(Rx2);
        Ry2 = atan2(R2.at<double>(0,2),cr * R2.at<double>(2,2) - sr * R2.at<double>(1,2));
        Rz2 = atan2(-R2.at<double>(0,1),R2.at<double>(0,0));
    }
    x2 = H2.at<double>(0,3);
    y2 = H2.at<double>(1,3);
    z2 = H2.at<double>(2,3);
    Rx2 = Rx2/pi*180;
    Ry2 = Ry2/pi*180;
    Rz2 = Rz2/pi*180;

    Pos_temp.at<double>(0,0) = x2;
    Pos_temp.at<double>(1,0) = y2;
    Pos_temp.at<double>(2,0) = z2;
    Pos_temp.at<double>(3,0) = Rx2;
    Pos_temp.at<double>(4,0) = Ry2;
    Pos_temp.at<double>(5,0) = Rz2;


    Mat temp =Pos_temp.clone();
    Pos.push_back(temp);
    //qDebug() << x2 << " " << y2 << " " << z2 << " " << Rx2 << " " << Ry2 << " " << Rz2;

    for(int i = 0; i < num - 2;i++)
    {
        x1 = x2;
        y1 = y2;
        z1 = z2;
        Rx1 = Rx2;
        Ry1 = Ry2;
        Rz1 = Rz2;
        H1 = pose2H(x1, y1, z1, Rx1, Ry1, Rz1);
        H2 = H1 * H[i+1];
        H2(Rect(0, 0, 3, 3)).copyTo(R2);
        //Rodrigues(R2, r2);
        //    Rx2 = r2.at<double>(0,0);
        //    Ry2 = r2.at<double>(1,0);
        //    Rz2 = r2.at<double>(2,0);
        if(abs(R2.at<double>(2,2)) < 1e-8 && abs(R2.at<double>(1,2)) < 1e-8)
        {
            Rx2 = 0;
            Ry2 = atan2(R2.at<double>(0,2),R2.at<double>(2,2));
            Rz2 = atan2(R2.at<double>(1,0),R2.at<double>(1,1));
        }
        else
        {
            Rx2 = atan2(-R2.at<double>(1,2),R2.at<double>(2,2));
            double sr = sin(Rx2);
            double cr = cos(Rx2);
            Ry2 = atan2(R2.at<double>(0,2),cr * R2.at<double>(2,2) - sr * R2.at<double>(1,2));
            Rz2 = atan2(-R2.at<double>(0,1),R2.at<double>(0,0));
        }
        x2 = H2.at<double>(0,3);
        y2 = H2.at<double>(1,3);
        z2 = H2.at<double>(2,3);
        Rx2 = Rx2/pi*180;
        Ry2 = Ry2/pi*180;
        Rz2 = Rz2/pi*180;

        Pos_temp.at<double>(0,0) = x2;
        Pos_temp.at<double>(1,0) = y2;
        Pos_temp.at<double>(2,0) = z2;
        Pos_temp.at<double>(3,0) = Rx2;
        Pos_temp.at<double>(4,0) = Ry2;
        Pos_temp.at<double>(5,0) = Rz2;
        Mat temp =Pos_temp.clone();
        Pos.push_back(temp);
        //qDebug() << x2 << " " << y2 << " " << z2 << " " << Rx2 << " " << Ry2 << " " << Rz2;
    }

    return Pos;
}

extern "C" void rob_CalculatePosition_d(const double *x, const double *y, double *arr, int *num)
{
    calibration cal;            // = new calibration();
    qDebug() << "****" << *num;
    Mat objpos = cal.CalculatePosition(*x, *y, *num);
    arr[0] = objpos.at<double>(0, 0);
    arr[1] = objpos.at<double>(1, 0);
    arr[2] = objpos.at<double>(2, 0);
    arr[3] = objpos.at<double>(3, 0);
    arr[4] = objpos.at<double>(4, 0);
    arr[5] = objpos.at<double>(5, 0);
}

extern "C" void rob_CalculatePosition_i(const double *x, const double *y, int *arr, int *num)
{
    calibration cal;            // = new calibration();
    Mat objpos = cal.CalculatePosition(*x, *y, *num);
    arr[0] = objpos.at<double>(0, 0);
    arr[1] = objpos.at<double>(1, 0);
    arr[2] = objpos.at<double>(2, 0);
    arr[3] = objpos.at<double>(3, 0);
    arr[4] = objpos.at<double>(4, 0);
    arr[5] = objpos.at<double>(5, 0);
}

bool calibration::CalculateExtrPar(int boardHeightNum, int boardWidthNum, double cornerDist)
{
    Mat rvec;
    Mat tvec;
    vector<Point3f> objectPoints;
    objectPoints.clear();
    vector<Point3f> corners;
    corners.clear();
    //vector<vector<Point2f> > imagePoints0;
    vector<Point2f> imagePoints;

    double extr_par[6];//外参
    double pose[6];//pose of robot
    Mat Hg1(4, 4, CV_64FC1);

    boardSize.height = boardHeightNum;
    boardSize.width = boardWidthNum;
    squareSize = cornerDist;

    //get imagePoints
    //imagePoints0 = calibration::ReadCameraPointFromFile();
    //imagePoints = imagePoints0[0];
    imagePoints = GetInsightDataOnline(Camera_IP, 60002);
    cout << imagePoints;
    //
    if(imagePoints.size() != 0)
    {
        //get robot Pos
        //pose[0] = adsReadStruct.currentFlangePosition[0];
        //pose[1] = adsReadStruct.currentFlangePosition[1];
        //pose[2] = adsReadStruct.currentFlangePosition[2];
        //pose[3] = adsReadStruct.currentFlangePosition[3];
        //pose[4] = adsReadStruct.currentFlangePosition[4];
        //pose[5] = adsReadStruct.currentFlangePosition[5];
        pose[0] = 522.73;
        pose[1] = 24.49;
        pose[2] = 588.72;
        pose[3] = 180;
        pose[4] = 0;
        pose[5] = -179.91;
        //

        //get camera_Matrix, distCoeffs, and Hcg from file
        ReadOutCameraDataFromFile(0);
        if(error_type == 5)
        {
            str += "Hand-eye calibration result error!";
            return 0;
        }
        //Mat distCoeffs = (Mat_<double>(5,1) << -0.045732 , 0.627201 , -0.00071495 , 0.00128281 , 4.82866);
        //

        //build objectPoints
        for( int i = 0; i < boardSize.height; ++i )
            for( int j = 0; j < boardSize.width; ++j )
                corners.push_back(Point3f(float( j*squareSize - ((double)(boardSize.width - 1) / 2) * squareSize ), float( i*squareSize - ((double)(boardSize.height - 1) / 2) * squareSize ), 0));
        objectPoints = corners;
        //

        //solvePnP

        if(solvePnP(objectPoints,imagePoints, Camera_Matrix, distCoeffs, rvec, tvec))
        {
            cout << rvec << endl;
            cout << tvec << endl;

            //write camera_Matrix, distCoeffs, and Hcg to file
            if(saveHandEyeData(Camera_Matrix,distCoeffs, Hcg))
            {
                for(int i = 0; i < 3; i++)
                {
                    extr_par[i] = rvec.at<double>(i, 0);
                    extr_par[i+3] = tvec.at<double>(i, 0);
                }
                //            for(int i = 0; i < 6; i++)
                //            {
                //                qDebug() << extr_par[i];
                //            }
                Hc1 = Camera(extr_par);
                //for(int i=0;i<4;i++)
                //{
                //    for(int j=0;j<4;j++)
                //    {
                //        qDebug() << Hc1.at<double>(i,j);
                //    }
                //}
                Hg1 = Gripper(pose);
                Hcal = Hg1 * Hcg * Hc1;

                if(!SaveHc1ToFile())
                {
                    return 0;
                }
                if(!SaveHcalToFile())
                {
                    return 0;
                }
                qDebug() <<"Hcal " << ":";
                for(int i=0;i<4;i++)
                {
                    for(int j=0;j<4;j++)
                    {
                        qDebug() << Hcal.at<double>(i,j);
                    }
                }
                str += "Calib plate in robot coordinate: ";
                str += '\n';
                str += "X = ";
                str += QString::number(Hcal.at<double>(0, 3));
                str += "\n";
                str += "Y = ";
                str += QString::number(Hcal.at<double>(1, 3));
                str += "\n";
                str += "Z = ";
                str += QString::number(Hcal.at<double>(2, 3));
                str += "\n";
                return 1;
            }
            else
                return 0;
        }
        else
            return 0;
    }
    else
        return 0;
}
bool calibration::saveHandEyeData(Mat cameraMatrix, Mat distCoeffs, Mat Hcg)
{
    QFile outfile(outputFileName);
    if ( outfile.open(  QIODevice::WriteOnly ) )
    {
        QTextStream stream( &outfile );
        //cameraMatrix
        stream << "<Camera_Matrix>" << "\r\n";
        for(int i = 0; i < 3; i++)
        {
            if(i == 0)
                stream << " [";
            else
                stream << "  ";
            for(int j = 0; j <3; j++)
            {
                if(j == 2)
                    stream << cameraMatrix.at<double>(i, j) ;
                else
                    stream << cameraMatrix.at<double>(i, j) << ", ";
            }
            if(i == 2)
                stream << "]" << "\r\n";
            else
                stream << ";" << "\r\n";
        }
        stream << "</Camera_Matrix>" << "\r\n";
        //

        //distCoeffs
        stream << "<Distortion_Coefficients>" << "\r\n";
        for(int i = 0; i < 5; i++)
        {
            if(i == 0)
                stream << " [";
            else
                stream << "  ";
            int j = 0;
            stream << distCoeffs.at<double>(i, j) ;
            if(i == 4)
                stream << "]" << "\r\n";
            else
                stream << ";" << "\r\n";
        }
        stream << "</Distortion_Coefficients>" << "\r\n";
        //

        //Hcg
        stream << "<Hcg>" << "\r\n";
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                stream << Hcg.at<double>(i,j) << " ";
            }
            stream << "\r\n";

        }
        stream << "</Hcg>" << "\r\n";
        //

        outfile.close();
        return 1;
    }
    else
    {
        error_type = 5;
        str += "Output file error.\n";
        return 0;
    }
}

vector<Point2f> calibration::GetInsightDataOnline(QString IP, quint16 port)
{
    vector<Point2f> imagePoints;
    imagePoints.clear();

    //发送标定板种类信号
    QTcpSocket *client_calplate;
    client_calplate = new QTcpSocket;
    client_calplate->connectToHost(QHostAddress(IP),60004);
    client_calplate->waitForConnected();
    int connectState = client_calplate->state();
    //qDebug() << connectState;
    //qDebug() << client_calplate->waitForConnected();
    if(connectState == 3)
    {
        qDebug() << "Connect to camera succeed.\n";
        char cal_plate[1];
        itoa(calPlate, cal_plate, 10);
        //qDebug() << cal_plate;
        client_calplate->write(cal_plate);
        client_calplate->waitForBytesWritten(30000);
    }
    else
    {
        str = "Connect to camera failed.\n";
        qDebug() << "Connect to camera failed.\n";
        error_type = 1;
        client_calplate->close();
        delete client_calplate;
        return imagePoints;
    }
    //

    //拍照，Insight数据解析，存入文件

    char Do_work[] = "calib";
    double point[14][8];

    QTcpSocket *client;
    QByteArray buffer_rec;

    client = new QTcpSocket;
    client->connectToHost(QHostAddress(IP),port);
    client->waitForConnected();
    int connectState2 = client->state();
    //qDebug() << connectState2;
    if(connectState2 == 3)
    {
        qDebug() << "Connect to camera succeed.\n";
        client->write(Do_work);

        int time_counter = 0;

        int buf_num = floor(boardSize.height * boardSize.width * 2 / 8);
        int last_buf_num = boardSize.height * boardSize.width * 2 - buf_num * 8;


        //client->waitForBytesWritten(3000);
        //while((client->bytesAvailable() < 859) && (time_counter++ < 80))
        while((client->bytesAvailable() < ( 66 * (buf_num - 1) + 66 * (int)(bool)last_buf_num + 1 )) && (time_counter++ < 180))
        {
            //qDebug() << "time_counter:" << time_counter;
            client->waitForReadyRead(100);
            //qDebug() << "client_available:" << client->bytesAvailable();
        }
        qDebug() << "msg size:" << client->bytesAvailable()<< "time_counter:" << time_counter ;
        if(time_counter > 180)
        {
            qDebug() << "error: no message receive----------------------";
            str += "Error: no message receive.\n";
            error_type = 2;
            return imagePoints;
        }
        buffer_rec = client->readAll();
        qDebug() << "client_rvc:";
        qDebug() << buffer_rec.data();


        QString msg = (QString)buffer_rec;
        QStringList parameters = msg.split(',');



        if(parameters.size() == ( boardSize.height * boardSize.width * 2 + 1 ))//标定板特征点解析
        {
            for(int i = 0; i < buf_num; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    point[i][ j * 2 ] = parameters.value( i * 8 + j ).toDouble();
                }
                for(int j = 4; j < 8; j++)
                {
                    point[i][ ( j - 4) * 2 + 1] = parameters.value( i * 8 + j ).toDouble();
                }
            }
            for(int i = 0; i < buf_num; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    imagePoints.push_back( Point2f(point[i][j * 2 + 1], point[i][j * 2]) );
                }
            }
            for(int i = 0; i < last_buf_num / 2; i++)
            {
                imagePoints.push_back( Point2f(parameters.value(buf_num * 8 + i  + last_buf_num / 2).toDouble(), parameters.value(buf_num * 8 + i).toDouble()));
            }
            //cout << imagePoints;

        }
        else if(parameters.size() == 1 && msg != "")
        {
            qDebug() << "Error" << msg;
            str += "Read message error.\n";
            error_type = 2;
            return imagePoints;

        }
        else{
            qDebug() << "read message error";
            str += "Read message error.\n Not enough points found.\n";
            error_type = 2;
            return imagePoints;
        }
    }
    else
    {
        qDebug() << "Connect to camera failed.";
        str = "Connect to camera failed.\n";
        error_type = 1;
        client->close();
        delete client;
        return imagePoints;
    }

    client->close();
    delete client;

    client_calplate->close();
    delete client_calplate;

    return imagePoints;
}

Mat calibration::H2pose(Mat R)
{
    double Rx;
    double Ry;
    double Rz;
    Mat Pos_temp(3, 1, CV_64FC1);
    if(abs(R.at<double>(2,2)) < 1e-8 && abs(R.at<double>(1,2)) < 1e-8)
    {
        Rx = 0;
        Ry = atan2(R.at<double>(0,2),R.at<double>(2,2));
        Rz = atan2(R.at<double>(1,0),R.at<double>(1,1));
    }
    else
    {
        Rx = atan2(-R.at<double>(1,2),R.at<double>(2,2));
        double sr = sin(Rx);
        double cr = cos(Rx);
        Ry = atan2(R.at<double>(0,2),cr * R.at<double>(2,2) - sr * R.at<double>(1,2));
        Rz = atan2(-R.at<double>(0,1),R.at<double>(0,0));
    }
    Rx = Rx/pi*180;
    Ry = Ry/pi*180;
    Rz = Rz/pi*180;

    Pos_temp.at<double>(0,0) = Rx;
    Pos_temp.at<double>(1,0) = Ry;
    Pos_temp.at<double>(2,0) = Rz;
    return Pos_temp;
}
