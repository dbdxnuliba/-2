#include "7dof_camera.h"
#include "7dof_socket.h"

#include "7dof_socket.h"
#include "../errmsg.h"
#include <WinSock2.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define CAM_CAPERR_ -1
#define CAM_PORT 60003

#define CAM_SOCK_OPEN_ERR			(-255)
#define CAM_PARAM_ERR				(-254)
#define CAM_SEND_ERR				(-253)
#define CAM_RECV_DATA_FORMAT_ERR	(-252)

#define CAM_MAX_DATA_LEN		6100							    // 协议包最大长度
#define CAM_MIN_DATA_LEN		8									// 协议包最小长度
#define CAM_DATA_START_POS		7									// 协议数据段起始位置， 编号从0开始

#define CAM_DATA_KIND_D			0
#define CAM_DATA_KIND_B			1
#define CAM_DATA_KIND_U			2
#define CAM_DATA_KIND_ERR		(-251)


extern double end_pos[6];
static char * camer_send_msg = NULL;// malloc(CAM_MAX_DATA_LEN);

static SOCKET camera_sockClient = INVALID_SOCKET;
static char camer_recv_msg[CAM_MAX_DATA_LEN];

char * get_sendmsg(const int actionnum)
{
	static char send_msg[5] = "[go0]";
	send_msg[3] = '0' + actionnum;
	return send_msg;
}

int get_sendmsg_len()
{
	return 5;
}



static int _get_camdata_kind()
{
#define CAM_DATA_KIND_POS 5
	if (camer_recv_msg[CAM_DATA_KIND_POS] == 'd'){
		return CAM_DATA_KIND_D;
	} else if (camer_recv_msg[CAM_DATA_KIND_POS] == 'b'){
		return CAM_DATA_KIND_B;
	} else if (camer_recv_msg[CAM_DATA_KIND_POS] == 'u'){
		return CAM_DATA_KIND_U;
	}

	return CAM_DATA_KIND_ERR;
}

static int _get_camdata_count()
{
#define CAM_COUNT_POS 4
	return camer_recv_msg[4] - '0';
}

static double _get_camdata_pos_val_d(int offset, int num)
{
#define CAM_DATA_D_LEN 8

    char temp; double result;
    int pos = offset + CAM_DATA_START_POS + (CAM_DATA_D_LEN + 1) * num;
    temp = camer_recv_msg[pos + CAM_DATA_D_LEN];
    camer_recv_msg[pos + CAM_DATA_D_LEN] = '\0';
    result = atof(&camer_recv_msg[pos]);
    camer_recv_msg[pos + CAM_DATA_D_LEN] = temp;

    return result;
}

//
// num: 编号从0开始
//
static double _get_camdata_pos_val_b(int offset, int num)
{
#define CAM_DATA_B_LEN 1
	char temp; double result;
    int pos = offset + CAM_DATA_START_POS + (CAM_DATA_B_LEN + 1) * num ;
	temp = camer_recv_msg[pos + CAM_DATA_B_LEN];
	camer_recv_msg[pos + CAM_DATA_B_LEN] = '\0';
	result = atoi(&camer_recv_msg[pos]);
	camer_recv_msg[pos + CAM_DATA_B_LEN] = temp;
	return result;
}

static void _set_camdata_kind_func(double(**fun)(int))
{
#define CAM_DATA_KIND_POS 5
	if (camer_recv_msg[CAM_DATA_KIND_POS] == 'd'){
		(*fun) = _get_camdata_pos_val_d;
	} else if (camer_recv_msg[CAM_DATA_KIND_POS] == 'b'){
		(*fun) = _get_camdata_pos_val_b;
	}
}

static double _get_camdata_errnum()
{
#define CAM_DATA_U_LEN 2
	char temp ; double result;
	int pos = CAM_DATA_START_POS;
	temp = camer_recv_msg[pos + CAM_DATA_U_LEN];
	camer_recv_msg[pos + CAM_DATA_U_LEN] = '\0';
	result = -atoi(&camer_recv_msg[pos]);
	camer_recv_msg[pos + CAM_DATA_U_LEN] = temp;
	return result;
}
/*
double rob_atof(const char *str, int length)
{
	double s = 0.0;
	double d = 10.0;
	int flag = 0;		//

	while (*(str++) == ' ')
		; 

	if (*(str++) == '-') flag = 1;

	while (*str >= '0'&&*str <= '9'&&*str != '.') 
	{
		s = s*10.0 + *str - '0';
		str++;
	}

}*/

//
// ip			相机ip地址
// actionnum	动作编号
// array		存放返回数据 
// arrsize		array大小
// recvsize		期待收取数据的大小
// timeout		最大收取延时
// 返回值		是否收取成功, 如果<0则收取失败， >=0 返回实际收到的个数
// 
int rob_camera_capture_i(const char *ip, const int actionnum, int *array_i, int arrsize, int recvsize, int timeout)
{
	double(*fun)(int ) = NULL;
	const int ClOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
	DWORD timeout_t = 100; //millisec
	clock_t t1, t2;
	int iResult, readc = 0, a_i = 0, i = 0, count = 0, re = 0;
	int cam_port = CAM_PORT;

	if ((actionnum > 9 || actionnum < 0) || recvsize > arrsize)
		return CAM_PARAM_ERR;
	if ((camera_sockClient = socket_open(ip, &cam_port)) == -1)
		return CAM_SOCK_OPEN_ERR;
	
	if (socket_send(camera_sockClient, get_sendmsg(actionnum), get_sendmsg_len(), 0, 500) == 0) {
		socket_close(camera_sockClient);
		camera_sockClient = INVALID_SOCKET;
		return CAM_SEND_ERR;
	}


	setsockopt(camera_sockClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_t, sizeof(timeout_t));

	t1 = clock() / ClOCKS_PER_MS;
	while (1){
		iResult = recv(camera_sockClient, camer_recv_msg, CAM_MAX_DATA_LEN, 0);
		if (iResult < 0)
			;
		else{
			if (iResult < CAM_MIN_DATA_LEN || _get_camdata_kind() == CAM_DATA_KIND_ERR){
				re = readc > 0 ? readc : CAM_RECV_DATA_FORMAT_ERR;
				break;
			}

			if (_get_camdata_kind() == CAM_DATA_KIND_U){
				re = readc > 0 ? readc : (int)_get_camdata_errnum();
				break;
			}  

			_set_camdata_kind_func(&fun);
            //count = _get_camdata_count();
            //for (i = 0; i < count && readc < arrsize; i++){
            //	array_i[readc++] = fun(i);
            //}

            blockArrayCount=0;
            int sizeCount = 0;
            //printf("****receive=%d\n",iResult);

            while(sizeCount + 2 <= iResult - 2){
                blockArray[blockArrayCount].numCount = camer_recv_msg[sizeCount+4] - '0';
                blockArray[blockArrayCount].size = 5 + (int)(camer_recv_msg[sizeCount+1] - '0')*10 + (int)(camer_recv_msg[sizeCount+2] - '0');
                sizeCount += blockArray[blockArrayCount].size;
                blockArrayCount++;
            }

            int j;
            sizeCount = 0;
            for (i = 0; i < blockArrayCount && readc < arrsize; i++){
                //printf("**************block %d, size %d, num %d\n", i, blockArray[i].size, blockArray[i].numCount);

                for(j = 0; j < blockArray[i].numCount; j++){
                    array_i[readc++] = fun(sizeCount, j);
                }

                sizeCount += blockArray[i].size;
            }

            for (i = 0; i < readc ; i++){
                printf("**************get %d\n",array_i[i]);
            }
		}

		if (readc+1 >= recvsize)
			break;

		t2 = clock() / ClOCKS_PER_MS;
		if ((t2 - t1) < timeout)
			continue;
		else
			break;


	}


	if (camer_send_msg == NULL) camer_send_msg = malloc(CAM_MAX_DATA_LEN);
	if (sprintf(camer_send_msg, "(56,6d,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f)", end_pos[0], end_pos[1], end_pos[2], end_pos[3], end_pos[4], end_pos[5]) == CAM_MAX_DATA_LEN){
		if (socket_send(camera_sockClient, camer_send_msg, CAM_MAX_DATA_LEN, 0, 500) == 0) {
			socket_close(camera_sockClient);
			camera_sockClient = INVALID_SOCKET;
			return CAM_SEND_ERR;
		}
	}


	socket_close(camera_sockClient);
	camera_sockClient = INVALID_SOCKET;
	return readc > 0? readc: re;
}

int rob_camera_capture_d(const char *ip, const int actionnum, double *array_d, int arrsize, int recvsize, int timeout)
{
	double(*fun)(int) = NULL;
	const int ClOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
	DWORD timeout_t = 100; //millisec
	clock_t t1, t2;
	int iResult, readc = 0, a_i = 0, i = 0, count = 0, re = 0;
	int cam_port = CAM_PORT;

	if ((actionnum > 9 || actionnum < 0) || recvsize > arrsize)
		return CAM_PARAM_ERR;
	if ((camera_sockClient = socket_open(ip, &cam_port)) == -1)
		return CAM_SOCK_OPEN_ERR;

	if (socket_send(camera_sockClient, get_sendmsg(actionnum), get_sendmsg_len(), 0, 500) == 0) {
		socket_close(camera_sockClient);
		camera_sockClient = INVALID_SOCKET;
		return CAM_SEND_ERR;
	}


	setsockopt(camera_sockClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_t, sizeof(timeout_t));

	t1 = clock() / ClOCKS_PER_MS;
	while (1){
        blockArrayCount = 0;
		iResult = recv(camera_sockClient, camer_recv_msg, CAM_MAX_DATA_LEN, 0);
        int i;
        //for(i = 0; i<iResult;i++) printf("@@@@@@@@@@@@@@@@@@@@@@@%c\n",camer_recv_msg[i]);
		if (iResult < 0)
			;
		else{
			if (iResult < CAM_MIN_DATA_LEN || _get_camdata_kind() == CAM_DATA_KIND_ERR){
				re = readc > 0 ? readc : CAM_RECV_DATA_FORMAT_ERR;
				break;
			}

			if (_get_camdata_kind() == CAM_DATA_KIND_U){
				re = readc > 0 ? readc : _get_camdata_errnum();
				break;
			}

			_set_camdata_kind_func(&fun);
            //count = _get_camdata_count();

            blockArrayCount=0;
            int sizeCount = 0;
            //printf("****receive=%d\n",iResult);

            while(sizeCount + 2 <= iResult - 2){
                blockArray[blockArrayCount].numCount = camer_recv_msg[sizeCount+4] - '0';
                blockArray[blockArrayCount].size = 5 + (int)(camer_recv_msg[sizeCount+1] - '0')*10 + (int)(camer_recv_msg[sizeCount+2] - '0');
                sizeCount += blockArray[blockArrayCount].size;
                blockArrayCount++;
            }

            int j;
            sizeCount = 0;
            for (i = 0; i < blockArrayCount && readc < arrsize; i++){
                //printf("**************block %d, size %d, num %d\n", i, blockArray[i].size, blockArray[i].numCount);

                for(j = 0; j < blockArray[i].numCount; j++){
                    array_d[readc++] = fun(sizeCount, j);
                }

                sizeCount += blockArray[i].size;
            }


            for (i = 0; i < readc ; i++){
                printf("**************get %f\n",array_d[i]);
            }
		}

		if (readc >= recvsize)
			break;

		t2 = clock() / ClOCKS_PER_MS;
		if ((t2 - t1) < timeout)
			continue;
		else
			break;

	}


	if (camer_send_msg == NULL) camer_send_msg = malloc(CAM_MAX_DATA_LEN);
	if (sprintf(camer_send_msg, "(56,6d,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f)", end_pos[0], end_pos[1], end_pos[2], end_pos[3], end_pos[4], end_pos[5]) == CAM_MAX_DATA_LEN){
		if (socket_send(camera_sockClient, camer_send_msg, CAM_MAX_DATA_LEN, 0, 500) == 0) {
			socket_close(camera_sockClient);
			camera_sockClient = INVALID_SOCKET;
			return CAM_SEND_ERR;
		}
	}

	socket_close(camera_sockClient);
	camera_sockClient = INVALID_SOCKET;
	return readc > 0 ? readc : re;
}

extern void rob_CalculatePosition_d(const double *x, const double *y, double *arr, int *num);
extern void rob_CalculatePosition_i(const double *x, const double *y, int *arr, int *num);

int rob_cam_CalPos_d(const double *x, const double *y, double *arr, int *num)
{
    rob_CalculatePosition_d(x , y, arr, num);
    if( !(arr[0] || arr[1] || arr[2]))
        EM_errorE(0, RSIERR_FUNCALL_CALPOS_ERR_INFO);
    return 1;
}

int rob_cam_CalPos_i(const double *x, const double *y, int *arr, int *num)
{
    rob_CalculatePosition_i(x , y, arr, num);
    if( !(arr[0] || arr[1] || arr[2]))
        EM_errorE(0, RSIERR_FUNCALL_CALPOS_ERR_INFO);
    return 1;
}
