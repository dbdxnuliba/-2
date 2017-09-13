#ifndef SYMBOL_H
#define SYMBOL_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/20
// Description: Symbol Table M
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "sysutil.h"
#include "table.h"

/*
 * struct S_symbol_{
    string name;
    S_symbol next;
};                  //defined in symbol.c
*/

typedef struct S_symbol_ *S_symbol;
typedef struct TAB_table_ * S_table;

S_symbol S_Symbol(string);				// 创建符号表construct function
string S_name(S_symbol);				// get the name from the S_symbol struct
void S_D_Symbols(void);					// 销毁符号表系统，在系统结束时调用

//
// these three function encapsulate the functions defined in table module 
//
S_table S_empty(void);
void S_enter(S_table t, S_symbol sym, void *value);
void *S_look(S_table t, S_symbol sym); // return value

// void *S_look_s(S_table t, S_symbol sym);

//
// these two function can help the complier to sign the scope in the code
//
// void S_beginScope(S_table t);
// void S_endScope(S_table t);

#endif
