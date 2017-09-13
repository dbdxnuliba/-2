#include "7dof_robot.h"
#include "../7dof_media.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#	include <Windows.h>
#else
#	include <unistd>
#endif

int robot_ioIn[8];
double end_pos[6] = { 1, 2, 3, 4, 5, 6 };
double joint_pos[7] = { 1, 2, 3, 4, 5, 6, 7 };
int robot_ioOut[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

int tool_ioIn[4];
int tool_ioOut[4] = { 0, 0, 0, 0 };

int start_calib;											// read value from camera ex: int start_calib();           
int calib_finish;											// read value from camera ex: int calib_finish();          
int robot_move_finished;									// write value to camera  ex: robot_move_finished(int a);  
int robot_move;												// read value from camera ex: int robot_move(); 

double movePosition[13][6];                                 //robot calibration move points

extern struct rsi_media sm_media;

#define mid sm_media

void rob_movej(const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7,
	const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEJ;
	mid.u.movej.pos[0] = *a1;
	mid.u.movej.pos[1] = *a2;
	mid.u.movej.pos[2] = *a3;
	mid.u.movej.pos[3] = *a4;
	mid.u.movej.pos[4] = *a5;
	mid.u.movej.pos[5] = *a6;
	mid.u.movej.pos[6] = *a7;

	mid.u.movej.speed = *speed;
	mid.u.movej.acc = *acc;
	mid.u.movej.rad = *rad;
	//fprintf(stdout, "MoveJ(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", a1, a2, a3, a4, a5, a6, a7, speed, acc);
}

void rob_movej_new(const double *a, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEJ;
	mid.u.movej.pos[0] = a[0];
	mid.u.movej.pos[1] = a[1];
	mid.u.movej.pos[2] = a[2];
	mid.u.movej.pos[3] = a[3];
	mid.u.movej.pos[4] = a[4];
	mid.u.movej.pos[5] = a[5];
	mid.u.movej.pos[6] = a[6];

	mid.u.movej.speed = *speed;
	mid.u.movej.acc = *acc;
	mid.u.movej.rad = *rad;
	//fprintf(stdout, "MoveJ(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", a1, a2, a3, a4, a5, a6, a7, speed, acc);
}

void rob_movej_new_d(const double *a, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEJ;
	mid.u.movej.pos[0] = a[0];
	mid.u.movej.pos[1] = a[1];
	mid.u.movej.pos[2] = a[2];
	mid.u.movej.pos[3] = a[3];
	mid.u.movej.pos[4] = a[4];
	mid.u.movej.pos[5] = a[5];
	mid.u.movej.pos[6] = a[6];

	mid.u.movej.speed = *speed;
	mid.u.movej.acc = *acc;
	mid.u.movej.rad = *rad;
	//fprintf(stdout, "MoveJ(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", a1, a2, a3, a4, a5, a6, a7, speed, acc);
}

void rob_movej_new_i(const int *a, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEJ;
	mid.u.movej.pos[0] = a[0];
	mid.u.movej.pos[1] = a[1];
	mid.u.movej.pos[2] = a[2];
	mid.u.movej.pos[3] = a[3];
	mid.u.movej.pos[4] = a[4];
	mid.u.movej.pos[5] = a[5];
	mid.u.movej.pos[6] = a[6];

	mid.u.movej.speed = *speed;
	mid.u.movej.acc = *acc;
	mid.u.movej.rad = *rad;
	//fprintf(stdout, "MoveJ(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", a1, a2, a3, a4, a5, a6, a7, speed, acc);
}

void rob_start(const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7,
	const double *speed, const double *acc, const double *rad)
{
	rob_movej(a1, a2, a3, a4, a5, a6, a7, speed, acc, rad);
	//fprintf("Start(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", a1, a2, a3, a4, a5, a6, a7, speed, acc);
}

// void rob_start_new(const double *a, const double *speed, const double *acc, const double *rad)
// {
//		rob_movej_new(a, speed, acc, rad);
// }

void rob_start_new_i(const int *a, const double *speed, const double *acc, const double *rad)
{
	rob_movej_new_i(a, speed, acc, rad);
}

void rob_start_new_d(const double *a, const double *speed, const double *acc, const double *rad)
{
	rob_movej_new_d(a, speed, acc, rad);
}

void rob_movel(const double *p1, const double *p2, const double *p3, const double *p4, const double *p5, const double *p6,
	const double *speed, const double *acc,
	const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7,
	const double *rad)
{
	mid.kind = M_MOVEL;
	mid.u.movel.pos[0] = *p1;
	mid.u.movel.pos[1] = *p2;
	mid.u.movel.pos[2] = *p3;
	mid.u.movel.pos[3] = *p4;
	mid.u.movel.pos[4] = *p5;
	mid.u.movel.pos[5] = *p6;

	mid.u.movel.axis[0] = *a1;
	mid.u.movel.axis[1] = *a2;
	mid.u.movel.axis[2] = *a3;
	mid.u.movel.axis[3] = *a4;
	mid.u.movel.axis[4] = *a5;
	mid.u.movel.axis[5] = *a6;
	mid.u.movel.axis[6] = *a7;

	mid.u.movel.speed = *speed;
	mid.u.movel.acc = *acc;
	mid.u.movel.rad = *rad;
	//fprintf(stdout, "MoveL(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", p1, p2, p3, p4, p5, p6, speed, acc, rad);
}

void rob_movel_new(const double *p, const double *speed, const double *acc, const double *rad, const double *psi)
 {
 	mid.kind = M_MOVEL;
 	mid.u.movel.pos[0] = p[0];
 	mid.u.movel.pos[1] = p[1];
 	mid.u.movel.pos[2] = p[2];
 	mid.u.movel.pos[3] = p[3];
 	mid.u.movel.pos[4] = p[4];
 	mid.u.movel.pos[5] = p[5];
	
	mid.u.movel.psi = *psi;
 	mid.u.movel.speed = *speed;
 	mid.u.movel.acc = *acc;
 	mid.u.movel.rad = *rad;
 }

void rob_movec(const double *c1, const double *c2, const double *c3, const double *c4, const double *c5, const double *c6,
	const double *e1, const double *e2, const double *e3, const double *e4, const double *e5, const double *e6, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEC;
	mid.u.movec.center[0] = *c1;
	mid.u.movec.center[1] = *c2;
	mid.u.movec.center[2] = *c3;
	mid.u.movec.center[3] = *c4;
	mid.u.movec.center[4] = *c5;
	mid.u.movec.center[5] = *c6;

	mid.u.movec.end[0] = *e1;
	mid.u.movec.end[1] = *e2;	
	mid.u.movec.end[2] = *e3;
	mid.u.movec.end[3] = *e4;
	mid.u.movec.end[4] = *e5;
	mid.u.movec.end[5] = *e6;

	mid.u.movec.speed = *speed;
	mid.u.movec.acc = *acc;
	mid.u.movec.rad = *rad;
	//fprintf(stdout, "MoveC(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n",
	//	c1, c2, c3, c4, c5, c6, e1, e2, e3, e4, e5, e6, speed, acc, rad);
}

void rob_movec_new(const double *p1, const double *p2, const double *speed, const double *acc, const double *rad, const double *psi)
{
	mid.kind = M_MOVEC;
	mid.u.movec.center[0] = p1[0];
	mid.u.movec.center[1] = p1[1];
	mid.u.movec.center[2] = p1[2];
	mid.u.movec.center[3] = p1[3];
	mid.u.movec.center[4] = p1[4];
	mid.u.movec.center[5] = p1[5];

	mid.u.movec.end[0] = p2[0];
	mid.u.movec.end[1] = p2[1];
	mid.u.movec.end[2] = p2[2];
	mid.u.movec.end[3] = p2[3];
	mid.u.movec.end[4] = p2[4];
	mid.u.movec.end[5] = p2[5];

	mid.u.movec.psi = *psi;
	mid.u.movec.speed = *speed;
	mid.u.movec.acc = *acc;
	mid.u.movec.rad = *rad;
}

void rob_movec1(const double *c1, const double *c2, const double *c3, const double * c4, const double *c5, const double *c6, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_NOACT;
	mid.u.movec.center[0] = *c1;
	mid.u.movec.center[1] = *c2;
	mid.u.movec.center[2] = *c3;
	mid.u.movec.center[3] = *c4;
	mid.u.movec.center[4] = *c5;
	mid.u.movec.center[5] = *c6;

	mid.u.movec.speed = *speed;
	mid.u.movec.acc = *acc;
	mid.u.movec.rad = *rad;
	//fprintf(stdout, "MoveC_1(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", c1, c2, c3, c4, c5, c6, speed, acc, rad);
}

void rob_movec2(const double *e1, const double *e2, const double *e3, const double *e4, const double *e5, const double *e6, 
	const double *speed, const double *acc, 
	const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7,
	const double *rad)
{
	mid.kind = M_MOVEC;

	mid.u.movec.end[0] = *e1;
	mid.u.movec.end[1] = *e2;
	mid.u.movec.end[2] = *e3;
	mid.u.movec.end[3] = *e4;
	mid.u.movec.end[4] = *e5;
	mid.u.movec.end[5] = *e6;

	mid.u.movec.axis[0] = *a1;
	mid.u.movec.axis[1] = *a2;
	mid.u.movec.axis[2] = *a3;
	mid.u.movec.axis[3] = *a4;
	mid.u.movec.axis[4] = *a5;
	mid.u.movec.axis[5] = *a6;
	mid.u.movec.axis[6] = *a7;

	mid.u.movec.speed = *speed;
	mid.u.movec.acc = *acc;
	mid.u.movec.rad = *rad;
	//fprintf(stdout, "MoveC_1(%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf)\n", e1, e2, e3, e4, e5, e6, speed, acc, rad);
}

void rob_movepsi(const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEPSI;
	mid.u.movepsi.pos[0] = *a1;
	mid.u.movepsi.pos[1] = *a2;
	mid.u.movepsi.pos[2] = *a3;
	mid.u.movepsi.pos[3] = *a4;
	mid.u.movepsi.pos[4] = *a5;
	mid.u.movepsi.pos[5] = *a6;
	mid.u.movepsi.pos[6] = *a7;

 	mid.u.movepsi.speed = *speed;
	mid.u.movepsi.acc = *acc;
	mid.u.movepsi.rad = *rad;
}

// //
// // N: rob_movepsi_new
// // D: 需要考虑参数传递时候的问题
// void rob_movepsi_new(const double *a, const double *speed, const double *acc, const double *rad)
// {
//	mid.kind = M_MOVEPSI;
// 	mid.u.movepsi.pos[0] = a[0];
// 	mid.u.movepsi.pos[1] = a[1];
// 	mid.u.movepsi.pos[2] = a[2];
// 	mid.u.movepsi.pos[3] = a[3];
// 	mid.u.movepsi.pos[4] = a[4];
// 	mid.u.movepsi.pos[5] = a[5];
// 	mid.u.movepsi.pos[6] = a[6];
// 
// 	mid.u.movepsi.speed = *speed; 
// 	mid.u.movepsi.acc = *acc;
// 	mid.u.movepsi.rad = *rad;
// }
   
void rob_movepsi_new_d(const double *a, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEPSI;
	mid.u.movepsi.pos[0] = a[0];
	mid.u.movepsi.pos[1] = a[1];
	mid.u.movepsi.pos[2] = a[2];
	mid.u.movepsi.pos[3] = a[3];
	mid.u.movepsi.pos[4] = a[4];
	mid.u.movepsi.pos[5] = a[5];
	mid.u.movepsi.pos[6] = a[6];

	mid.u.movepsi.speed = *speed;
	mid.u.movepsi.acc = *acc;
	mid.u.movepsi.rad = *rad;
}

void rob_movepsi_new_i(const int *a, const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_MOVEPSI;
	mid.u.movepsi.pos[0] = a[0];
	mid.u.movepsi.pos[1] = a[1];
	mid.u.movepsi.pos[2] = a[2];
	mid.u.movepsi.pos[3] = a[3];
	mid.u.movepsi.pos[4] = a[4];
	mid.u.movepsi.pos[5] = a[5];
	mid.u.movepsi.pos[6] = a[6];

	mid.u.movepsi.speed = *speed;
	mid.u.movepsi.acc = *acc;
	mid.u.movepsi.rad = *rad;
}

void rob_biaoding(const double *p1, const double *p2, const double *p3, const double *p4, const double *p5, const double *p6,
	const double *speed, const double *acc, const double *rad)
{
	mid.kind = M_BD;
	mid.u.bd.pos[0] = *p1;
	mid.u.bd.pos[1] = *p2;
	mid.u.bd.pos[2] = *p3;
	mid.u.bd.pos[3] = *p4;
	mid.u.bd.pos[4] = *p5;
	mid.u.bd.pos[5] = *p6;

	mid.u.bd.speed = *speed;
	mid.u.bd.acc = *acc;
	mid.u.bd.rad = *rad;
}

void calibAdjustPose(){
    mid.kind = CALIB_POSE;
}

void replay(const int *num, const int *speed){
    mid.kind = REPLAY;
    mid.u.replay.num = *num;
    mid.u.replay.speed = *speed;
}

void rob_io_out(const int *num, const int *io)
{
	mid.kind = M_NOACT;
	robot_ioOut[*num] = (*io) > 0 ? 1 : 0;
}

int rob_io_in(const int* num)
{
	mid.kind = M_NOACT;
	return robot_ioIn[*num] > 0 ? 1 : 0;
}

void rob_sleep(const int *time)
{
	int t =  (*time) > 10000 ? 10000 : (*time);
#ifdef _MSC_VER
	Sleep(t);
#else
#	sleep(t);
#endif
}

void rob_gripper(const int *num)
{
    mid.kind = GRIPPER;
    mid.u.gripper.num = *num;
}
/*
void rob_runprog(const char *name, int psize)
{
	mid.kind = M_RUNPROG;
	mid.u.runprog.name = name;
	mid.u.runprog.size = psize;
}
*/

void rob_set_tcp(const double *p1, const double *p2, const double *p3, const double *p4, const double *p5, const double *p6)
{
	mid.kind = M_SETTCP;
	mid.u.settcp.pos[0] = *p1;
	mid.u.settcp.pos[1] = *p2;
	mid.u.settcp.pos[2] = *p3;
	mid.u.settcp.pos[3] = *p4;
	mid.u.settcp.pos[4] = *p5;
	mid.u.settcp.pos[5] = *p6;

}

void rob_clean_tcp(){
    mid.kind = M_CLEANTCP;
}

void rob_get_flange_pos(double *p)
{
	mid.kind = M_NOACT;
	p[0] = end_pos[0];
	p[1] = end_pos[1];
	p[2] = end_pos[2];
	p[3] = end_pos[3];
	p[4] = end_pos[4];
	p[5] = end_pos[5];
}

void rob_get_joint_pos_d(double *p)
{
	mid.kind = M_NOACT;
	p[0] = joint_pos[0];
	p[1] = joint_pos[1];
	p[2] = joint_pos[2];
	p[3] = joint_pos[3];
	p[4] = joint_pos[4];
	p[5] = joint_pos[5];
	p[6] = joint_pos[6];
}

void rob_get_joint_pos_i(int *p)
{
	mid.kind = M_NOACT;
	p[0] = (int)joint_pos[0];
	p[1] = (int)joint_pos[1];
	p[2] = (int)joint_pos[2];
	p[3] = (int)joint_pos[3];
	p[4] = (int)joint_pos[4];
	p[5] = (int)joint_pos[5];
	p[6] = (int)joint_pos[6];
}

int rob_start_calib() 
{ 
	mid.kind = M_NOACT;
	return start_calib; 
}

int rob_calib_finish() 
{ 
	mid.kind = M_NOACT;
	return calib_finish;  
}

void rob_move_finished(const int *a)
{
	mid.kind = M_NOACT;
	robot_move_finished = *a;
}


int rob_move()
{
	mid.kind = M_NOACT;
	return  robot_move;
}

void tool_io_out(const int *num, const int *io)
{
	//num:1-4
	mid.kind = M_NOACT;
	if (*num <= 0 || *num >= 5) return;
	tool_ioOut[*num - 1] = (*io) > 0 ? 1 : 0;
}

int tool_io_in(const int *num)
{
	//num:1-4
	mid.kind = M_NOACT;
    if (*num <= 0 || *num >= 5) return -1;
	return tool_ioIn[*num - 1] > 0 ? 1 : 0;
}

void moveCalib(const int *num)
{
    mid.kind = M_MOVEL;
    mid.u.movel.pos[0] = movePosition[*num][0];
    mid.u.movel.pos[1] = movePosition[*num][1];
    mid.u.movel.pos[2] = movePosition[*num][2];
    mid.u.movel.pos[3] = movePosition[*num][3];
    mid.u.movel.pos[4] = movePosition[*num][4];
    mid.u.movel.pos[5] = movePosition[*num][5];

    mid.u.movel.psi = 999;
    mid.u.movel.speed = 300;
    mid.u.movel.acc = 1000;
    mid.u.movel.rad = -1;
}

void move_tcp(const double *p1, const double *p2, const double *p3, const double *p4)
{
    mid.kind = M_MOVETCP;
    mid.u.tcpmove.pos[0] = *p1;
    mid.u.tcpmove.pos[1] = *p2;
    mid.u.tcpmove.pos[2] = *p3;
    mid.u.tcpmove.speed = *p4;
}

void rob_movej_pose(const double *p, const double *speed, const double *acc)
{
    mid.kind = M_MOVEJ_POSE;
    mid.u.movej.pos[0] = p[0];
    mid.u.movej.pos[1] = p[1];
    mid.u.movej.pos[2] = p[2];
    mid.u.movej.pos[3] = p[3];
    mid.u.movej.pos[4] = p[4];
    mid.u.movej.pos[5] = p[5];

    mid.u.movej.speed = *speed;
    mid.u.movej.acc = *acc;
}

void rob_move_write(const int *a)
{
    mid.kind = M_NOACT;
    robot_move = *a;
}
