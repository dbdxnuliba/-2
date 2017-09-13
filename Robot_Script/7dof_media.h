#ifndef DOF_MEDIA_H
#define DOF_MEDIA_H


// NOACT ����ǰ��䲻��Ҫ�κ���λ������
// ERROR , ִ�г���
// END , ����ɹ�ִ�н����� û���κ���������
struct rsi_media{
	enum{
		M_MOVEJ, M_MOVEL, M_MOVEC, 
		M_BD, M_MOVEPSI,
		M_NOACT, M_ERROR, M_END,
        M_RUNPROG,M_SETTCP,M_MOVETCP,
        GRIPPER,M_MOVEJ_POSE,CALIB_POSE,
        REPLAY, M_CLEANTCP
	}kind;

	union{
		struct{ double pos[7]; double speed; double acc; double rad; } movej;
		struct{ double pos[6]; double speed; double acc; double rad; double axis[7]; double psi; } movel;
		struct{ double center[6]; double end[6]; double speed; double acc; double rad;  double axis[7]; double psi; } movec;
		struct{ double pos[6]; double speed; double acc; double rad; } bd;
		struct{ double pos[7]; double speed; double acc; double rad; } movepsi;
		struct{ char *name; int size; } runprog;
        struct{ double pos[6]; }settcp;
        struct{ double pos[3]; double speed;}tcpmove;
        struct{ int num;}gripper;
        struct{ int num; int speed;}replay;
	}u;

	int linenum;
	int counter;
};

#endif
