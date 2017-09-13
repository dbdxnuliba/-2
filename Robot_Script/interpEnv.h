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

#define REG_COUNT 20		// 系统中模拟寄存器数量, 寄存器的大小直接决定解释器在性能最差的情况下支持的运算数的数量

enum Istate { I_MAIN, I_FUNCCALL, I_THREADCALL, I_FUNCDEF, I_THREADDEF };

typedef struct interpEnv_ *interpEnv;
struct interpEnv_{
	//
    // 抽象栈（变量）
	//
	stack global_sta;							// main_stack(static_stack)
    stack local_sta;							// local_stack
	
	//
	// 抽象寄存器组
	//
    E_val etx;									// temp register 语句运算结果
    E_val regs[REG_COUNT];						// a group of register 运算过程中辅助寄存器组
	int p;										// 指向一个空的寄存器
	int regc;									// 当前已经使用的寄存器个数
	
	//
	// 抽象系统表信息
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
	int stackdeep;								// 栈的调用深度， 默认情况下调用深度为 0 ，在主栈调用， 最大调用深度为10
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
