#ifndef DOF_ROBOT_H
#define DOF_ROBOT_H

void rob_movej(const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7, 
				const double *speed, const double *acc, const double *rad);

//void rob_movej_new(const double *a, const double *speed, const double *acc, const double *rad);
void rob_movej_new_i(const int *a, const double *speed, const double *acc, const double *rad);
void rob_movej_new_d(const double *a, const double *speed, const double *acc, const double *rad);
void rob_movej_pose(const double *p, const double *speed, const double *acc);
void rob_start(const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7,
	const double *speed, const double *acc, const double *rad);
void rob_start_new(const double *a, const double *speed, const double *acc, const double *rad);
void rob_start_new_i(const int *a, const double *speed, const double *acc, const double *rad);
void rob_start_new_d(const double *a, const double *speed, const double *acc, const double *rad);

/*void rob_movel(const double *p1, const double *p2, const double *p3, const double *p4, const double *p5, const double *p6,
	const double *speed, const double *acc, const double *rad);*/
void rob_movel(const double *p1, const double *p2, const double *p3, const double *p4, const double *p5, const double *p6,
	const double *speed, const double *acc,
	const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7,
	const double *rad);
void rob_movel_new(const double *p, const double *speed, const double *acc, const double *rad, const double *psi);

void rob_movec(const double *c1, const double *c2, const double *c3, const double *c4, const double *c5, const double *c6,
	const double *e1, const double *e2, const double *e3, const double *e4, const double *e5, const double *e6, const double *speed, const double *acc, const double *rad);
void rob_movec_new(const double *p1, const double *p2, const double *speed, const double *acc, const double *rad, const double *psi);

void rob_movec1(const double *c1, const double *c2, const double *c3, const double * c4, const double *c5, const double *c6, 
	const double *speed, const double *acc, const double *rad);
void rob_movec2(const double *e1, const double *e2, const double *e3, const double *e4, const double *e5, const double *e6, 
	const double *speed, const double *acc, 
	const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7,
	const double *rad);


void rob_movepsi(const double *a1, const double *a2, const double *a3, const double *a4, const double *a5, const double *a6, const double *a7, 
	const double *speed, const double *acc, const double *rad);
//void rob_movepsi_new(const double *a, const double *speed, const double *acc, const double *rad);
void rob_movepsi_new_i(const int *a, const double *speed, const double *acc, const double *rad);
void rob_movepsi_new_d(const double *a, const double *speed, const double *acc, const double *rad);
void rob_biaoding(const double *p1, const double *p2, const double *p3, const double *p4, const double *p5, const double *p6, 
				const double *speed, const double *acc, const double *rad);

void rob_set_tcp(const double *p1, const double *p2, const double *p3, const double *p4, const double *p5, const double *p6);
void move_tcp(const double *p1, const double *p2, const double *p3, const double *p4);
void rob_get_flange_pos(double *p);
void rob_get_joint_pos_d(double *p);
void rob_get_joint_pos_i(int *p);

void rob_io_out(const int *num, const int *io);
int rob_io_in(const int* num);
void tool_io_out(const int *num, const int *io);
int tool_io_in(const int* num);

void rob_sleep(const int *time);
void rob_gripper(const int *num);
//void rob_runprog(const char *name, int psize);
void moveCalib(const int *num);

int rob_start_calib();
int rob_calib_finish();
void rob_move_finished(const int *a);
void rob_move_write(const int *a);
int rob_move();
void calibAdjustPose();
void replay(const int *num, const int *speed);

#endif
