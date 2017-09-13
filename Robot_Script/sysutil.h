#ifndef UTIL_H
#define UTIL_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/20
// Description: System Utility M
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>

typedef char *string;
typedef char bool;

#define TRUE		1
#define FALSE		0
#define POSECOUNT	6

//
// 系统类型定义， 也许有用， 也许没用
//
#define RSI_INTSIZE		sizeof(int)						
#define RSI_CHARSIZE	sizeof(char)
#define RSI_DOUBLESIZE	sizeof(double)
#define POSESIZE		(POSECOUNT * sizeof(double))	// 定义, 未使用


#define COMMANDPROMPT ">>> "

#define MAX(x, y) ((x)>(y) ? (x):(y))
#define MIN(x, y) MAX(y, x)

void* checked_malloc(int);
string String(char *);

typedef struct U_boolList_* U_boolList;
typedef struct U_intList_* U_intList;

//
// Bool链表的构造与销毁
//
struct U_boolList_ {
	bool head;
	U_boolList tail;
};
U_boolList U_BoolList(bool head, U_boolList tail);
void U_D_BoolList(U_boolList *bl);

//
// 整型链表的构造与销毁
//
struct U_intList_{
	int i;
	U_intList rest;
};
U_intList U_IntList(int i, U_intList il);
void U_D_IntList(U_intList *il);

// N: U_ToolGetMSize
// D: 得到当前动态分配的存储空间数量 ，单位:Byte
unsigned long U_ToolGetMSize();

#endif