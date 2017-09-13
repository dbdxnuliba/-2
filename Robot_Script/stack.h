#ifndef STACK_H
#define STACK_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/23
// Description: Abstract Stack M
//				a. 解释器在运行的过程中，单线程情况下采用一个主站和一个从栈来运行
//				b. 栈默认分配的存储空间是1024B ， 宏 STACKSIZE stack.c 定义
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "sysutil.h"
#include "symbol.h"
#include "datatype.h"
#include "absyn.h"

#define VALCOUNT 128

typedef struct sbinder_ *sbinder;
struct sbinder_{
	S_symbol name;							// 变量名
	datatype type;							// 变量类型 datatype
	void *addr;								// 指向变量存储空间的指针
	int stanum;								// 记录变量所在的栈对应的层次
	sbinder next;
};

typedef struct stack_ *stack;
struct stack_{
	sbinder tab[VALCOUNT];
	void *memoryspace;						// 指向栈低存储空间， 保存整个栈的地址
	void *top;								// 永远指向下一个要存放的位置
	int size;								// 当前栈中存储内容的大小
};
stack Stack(void);

sbinder Stack_getSbinder(stack s, S_symbol name, int stackdeep);			//没有找到返回NULL

int Stack_setSbinderIntVal(sbinder sb, int val);
int Stack_setSbinderDouVal(sbinder sb, double val);
int Stack_setSbinderCharVal(sbinder sb, char val);

// N: Stack_setSbinderCStringVal
// D: 传入的字符串长度len不涵盖‘\0’
// D: 只支持长度相等的字符串进行设置
int Stack_setSbinderCStringVal(sbinder sb, char *val, int len);
int Stack_setSbinderSbinderVal(sbinder des, sbinder src);
int Stack_setSbinderCNumListVal(sbinder sb, A_cnumlist cnl);				//函数具有模块耦合性，需要重写
int Stack_setSbinderPoseZero(sbinder sb);

int Stack_setArraySbinderIntVal(sbinder sb, int val, int offset);
int Stack_setArraySbinderDouVal(sbinder sb, double val, int offset);
int Stack_setArraySbinderCharVal(sbinder sb, char val, int offset);

void Stack_pushIntVal(stack s, S_symbol name, int val, int stanum);
void Stack_pushDouVal(stack s, S_symbol name, double val, int stanum);
void Stack_pushCharVal(stack s, S_symbol name, char val, int stanum);		//系统中考虑去掉Bool类型

void Stack_pushPoseZeroVal(stack s, S_symbol name, int stanum);
void Stack_pushStringVal(stack s, S_symbol name, char *str, int stanum);
void Stack_pushIntArrayVal(stack s, S_symbol name, int *value, int count, int stanum);
void Stack_pushDouArrayVal(stack s, S_symbol name, double *value, int count, int stanum);
void Stack_pushStrArrayVal(stack s, S_symbol name, char * value, int count, int stanum);

void Stack_pushPoseCNListVal(stack s, S_symbol name, A_cnumlist list, int stanum);
void Stack_pushIntArrCNListVal(stack s, S_symbol name, A_cnumlist list, int count, int stanum);
void Stack_pushDouArrCNListVal(stack s, S_symbol name, A_cnumlist list, int count, int stanum);

void Stack_pushSbinderVal(stack s, S_symbol name, sbinder sb, int stanum);
void Stack_pushValR(stack s, S_symbol name, sbinder sb, int stanum);
void Stack_pushPoseR(stack s, S_symbol name, void *addr, int stanum);
void Stack_pushDouArrayR(stack s, S_symbol name, void *addr, int count, int stanum);
void Stack_pushIntArrayR(stack s, S_symbol name, void *addr, int count, int stanum);

// N: Stack_printfSbinder
// D: 打印出变量信息
//    str: 信息存放地址
//    len: str长度
int Stack_printfSbinder(char *str, sbinder sb, int len);

void Stack_popOneLeval(stack s, int stanum);								// 只能是最上面一层出栈

void D_Stack(stack *stk);
void C_Stack(stack stk);
#endif