#ifndef ENVENTRY_H
#define ENVENTRY_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/24
// Description: environment entry (System Table) M
//				命名方式：E：M前缀 / C：Clear / Delete
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include"absyn.h"
#include"datatype.h"

//
// struct E_param_
// D: name		形参名字
//    defaval	形参的默认值, 默认值的类型([POSE]/算数类型常量/CList/conststring)
//
typedef struct E_param_ *E_param;
struct E_param_{
	datatype ty;
	S_symbol name;
	A_exp defval;
	E_param next;
};
E_param E_Param(S_symbol name, datatype ty, A_exp defval, E_param next);
E_param E_Param_REG(S_symbol name, S_symbol tyid, A_exp defval, E_param next);

//
// struct E_funEnventry_
// D: params	形参列表
//    funcbody	函数体, 如果是内核函数，则没有函数体
//
typedef struct E_funEnventry_ * E_funEnventry;
struct E_funEnventry_{
	E_param params;
	A_stmList funcbody;
};
E_funEnventry E_FunEnventry(E_param params, A_stmList funcbody);

S_table E_TyEnv(void);
// N: E_D_TyEnv
// D: 删除系统类型表，务必在系统结束时候调用，其他情况不能调用
void E_D_TyEnv(void);

S_table E_SysFuncEnv(void);
void E_D_SysFuncEnv(void);
void E_enter_SysFE_REG(S_symbol name, E_param params);

S_table E_SysThreadEnv(void);
void E_D_SysThreadEnv(void);
void E_enter_SysTE_REG(S_symbol name);

// N: User FunEnventry Env
S_table E_UFEnv(void);
void E_D_UFEnv(S_table *tab);
void E_C_UFEnv(S_table tab);

// N: Output to Screen(shell) (function env)
void E_OutputTS_FuncEnv(S_table table);

#endif
