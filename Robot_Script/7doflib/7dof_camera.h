#ifndef DOF_CAMERA_H
#define DOF_CAMERA_H

typedef struct block{
    int size;       //整个block的size,带括号
    int numCount;   //block中有几个数据
}block;

block blockArray[1000];
int blockArrayCount;

int rob_camera_capture_i(const char *ip, const int actionnum, int *array_i, int arrsize, int recvsize, int timeout);

int rob_camera_capture_d(const char *ip, const int actionnum, double *array_d, int arrsize, int recvsize, int timeout);


int rob_cam_CalPos_d(const double *x, const double *y, double *arr, int *num);

int rob_cam_CalPos_i(const double *x, const double *y, int *arr, int *num);
#endif
