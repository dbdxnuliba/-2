#ifndef EXPVAL_H
#define EXPVAL_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/22
// Description: Abstract Register M
//				a. 暂存解释过程中产生的临时变量和结果
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////
#include "sysutil.h"

//
// struct E_val_
// D: E_NULL 标识当前寄存器处于空闲状态
//	  E_CHAR, E_INT, E_DOU定义类型时的顺序不可改变， 决定了解释阶段类型转化算法
//    E_ARRAYP(sbinder-int, dou, str, rint, rdou, rstr), E_CNUMLISTP(A_stmlist)
//    E_PSTRING(A_string-const string), E_CPOSE([POSE])
//    E_ARRAYP, E_CNUMLISTP, E_PSTRING的值都保存在void*p中
//
typedef struct E_val_ *E_val;
struct E_val_{
	enum{
		E_CHAR, E_INT, E_DOU,
		E_ARRAYP, E_CNUMLISTP, E_PSTRING, E_CPOSE,
		E_NULL, E_NIL
	}kind; 
	union{
		char c;
		int i;
		double d;
		void *p;									
	}u;
};
E_val E_Val(void);

void E_setIntVal(E_val e, int i);
void E_setDouVal(E_val e, double d);
void E_setCharVal(E_val e, char b);
void E_setCPoseVal(E_val e);
void E_setCNumList(E_val e, void *p);
void E_setCStringVal(E_val e, void *p);
void E_setArraypVal(E_val e, void *p);
void E_setNilVal(E_val e);

double E_getDouVal(E_val e);
int E_getIntVal(E_val e);
char E_getCharVal(E_val e);
void* E_getPointVal(E_val e);
// N: E_reverseVal
// D: 在原始数值上取反(!)
void E_reverseVal(E_val e);

void E_ValCopy(E_val dst, E_val src);

void E_D_Val(E_val *ev);

void E_C_Val(E_val rv);

int E_TF_Val(E_val rv);

#endif