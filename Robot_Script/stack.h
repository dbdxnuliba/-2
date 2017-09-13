#ifndef STACK_H
#define STACK_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/23
// Description: Abstract Stack M
//				a. �����������еĹ����У����߳�����²���һ����վ��һ����ջ������
//				b. ջĬ�Ϸ���Ĵ洢�ռ���1024B �� �� STACKSIZE stack.c ����
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
	S_symbol name;							// ������
	datatype type;							// �������� datatype
	void *addr;								// ָ������洢�ռ��ָ��
	int stanum;								// ��¼�������ڵ�ջ��Ӧ�Ĳ��
	sbinder next;
};

typedef struct stack_ *stack;
struct stack_{
	sbinder tab[VALCOUNT];
	void *memoryspace;						// ָ��ջ�ʹ洢�ռ䣬 ��������ջ�ĵ�ַ
	void *top;								// ��Զָ����һ��Ҫ��ŵ�λ��
	int size;								// ��ǰջ�д洢���ݵĴ�С
};
stack Stack(void);

sbinder Stack_getSbinder(stack s, S_symbol name, int stackdeep);			//û���ҵ�����NULL

int Stack_setSbinderIntVal(sbinder sb, int val);
int Stack_setSbinderDouVal(sbinder sb, double val);
int Stack_setSbinderCharVal(sbinder sb, char val);

// N: Stack_setSbinderCStringVal
// D: ������ַ�������len�����ǡ�\0��
// D: ֻ֧�ֳ�����ȵ��ַ�����������
int Stack_setSbinderCStringVal(sbinder sb, char *val, int len);
int Stack_setSbinderSbinderVal(sbinder des, sbinder src);
int Stack_setSbinderCNumListVal(sbinder sb, A_cnumlist cnl);				//��������ģ������ԣ���Ҫ��д
int Stack_setSbinderPoseZero(sbinder sb);

int Stack_setArraySbinderIntVal(sbinder sb, int val, int offset);
int Stack_setArraySbinderDouVal(sbinder sb, double val, int offset);
int Stack_setArraySbinderCharVal(sbinder sb, char val, int offset);

void Stack_pushIntVal(stack s, S_symbol name, int val, int stanum);
void Stack_pushDouVal(stack s, S_symbol name, double val, int stanum);
void Stack_pushCharVal(stack s, S_symbol name, char val, int stanum);		//ϵͳ�п���ȥ��Bool����

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
// D: ��ӡ��������Ϣ
//    str: ��Ϣ��ŵ�ַ
//    len: str����
int Stack_printfSbinder(char *str, sbinder sb, int len);

void Stack_popOneLeval(stack s, int stanum);								// ֻ����������һ���ջ

void D_Stack(stack *stk);
void C_Stack(stack stk);
#endif