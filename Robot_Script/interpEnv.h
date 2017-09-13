#ifndef INTERPENV_H
#define INTERPENV_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/25
// Description: Interpreter Environment (System Info) M
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "stack.h"
#include "enventry.h"
#include "expVal.h"
#include "symbol.h"
#include "sysutil.h"

#define REG_COUNT 20		// ϵͳ��ģ��Ĵ�������, �Ĵ����Ĵ�Сֱ�Ӿ������������������������֧�ֵ�������������

enum Istate { I_MAIN, I_FUNCCALL, I_THREADCALL, I_FUNCDEF, I_THREADDEF };

typedef struct interpEnv_ *interpEnv;
struct interpEnv_{
	//
    // ����ջ��������
	//
	stack global_sta;							// main_stack(static_stack)
    stack local_sta;							// local_stack
	
	//
	// ����Ĵ�����
	//
    E_val etx;									// temp register ���������
    E_val regs[REG_COUNT];						// a group of register ��������и����Ĵ�����
	int p;										// ָ��һ���յļĴ���
	int regc;									// ��ǰ�Ѿ�ʹ�õļĴ�������
	
	//
	// ����ϵͳ����Ϣ
	//
	S_table ty_tab;								// typetable
	S_table sfunc_tab;							// system function table
	S_table sthr_tab;							// system thread table
	S_table ufunc_tab;							// user func table
	S_table uthr_tab;							// user thread table

	//
	// Ohter Info
	//
	int interpstate;							// I_MAIN, I_FUNCCALL, I_THREADCALL, I_FUNCDEF, I_THREADDEF, I_ERROR 
	int stackdeep;								// ջ�ĵ�����ȣ� Ĭ������µ������Ϊ 0 ������ջ���ã� ���������Ϊ10
};

interpEnv IE_getInterpEnv(void);				// New
// N: IE_getInterpThreadEnv
// D: support mutex Thread
interpEnv IE_getInterpThreadEnv(interpEnv env);

void IE_D_InterpEnv(interpEnv *sysenv);			// New
// N: IE_F_InterpEnv
// D: support mutex Thread
void IE_F_InterpEnv(interpEnv *sysenv);			// New
void IE_C_InterpEnv(interpEnv sysenv);			// New

E_val IE_getNullReg(interpEnv sysenv);			// New
void IE_freeReg(interpEnv sysenv, E_val reg);	// New

#endif
