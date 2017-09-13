#ifndef ABSYN_H
#define ABSYN_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/21
// Description: Abstract Syntax M
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "sysutil.h"	
#include "symbol.h"

typedef struct A_stmList_ *A_stmList;
typedef struct A_stm_ *A_stm;                   //Statement语句
typedef struct A_while_	*A_while;
typedef struct A_funDec_ *A_funDec;
typedef struct A_typeFields_ *A_typeFields;
typedef struct A_field_ *A_field;
typedef struct A_fieldl_ *A_fieldl;
typedef struct A_fieldd_ *A_fieldd;
typedef struct A_fielddl_ *A_fielddl;
typedef struct A_threadDec_	*A_threadDec;
typedef struct A_if_ *A_if;
typedef struct A_elifExp_ *A_elifExp;
typedef struct A_elifList_ *A_elifList;
typedef struct A_exp_ *A_exp;
typedef struct A_op_ *A_op;
typedef struct A_funcall_ *A_funcall;
typedef struct A_args_ *A_args;
typedef struct A_variate_ *A_variate;
typedef struct A_assign_ *A_assign;
typedef struct A_cnum_ *A_cnum;
typedef struct A_cnumlist_ *A_cnumlist;
typedef struct A_arraydef_ *A_arraydef;
typedef struct A_arraytool_ *A_arraytool;
typedef struct A_string_ *A_string;
typedef int A_pos;


void DelStmList(A_stmList *stml);
void DelExp(A_exp *exp);

typedef enum{
	A_plusOp, A_minusOp, A_timesOp, A_divideOp,
	A_eqOp, A_neqOp, A_ltOp, A_leOp, A_gtOp, A_geOp,
	A_andOp, A_orOp, A_xorOp
}A_oper;

///////////////////////////////////////////////////////////////////////////////////////////
//statement 语句列表
struct A_stmList_{
	A_stm stm;
	A_stmList tail;
};

//创建语句列表
A_stmList A_StmList(A_stm stm, A_stmList tail);

///////////////////////////////////////////////////////////////////////////////////////////
//statement 语句结构
struct A_stm_{
    A_pos pos;  //int
	enum {
		A_expStm, A_whileStm, A_breakStm,A_fundecStm, A_threaddecStm, 
		A_returnStm, A_ifStm, A_arraydefStm, A_globalStm
    }kind;
	union{
		A_exp expStm;										// A_expstm
		A_while whileStm;									// A_whileStm
		A_funDec fundStm;									// A_fundecStm
		A_threadDec threaddStm;								// A_threaddecStm
		A_exp reStm;										// A_returnStm
		A_if ifStm;											// A_ifStm
		A_arraydef arraydef;								// A_arraydefStm
		S_symbol gid;										// A_globalStm
	}u;
};

//创建语句函数
A_stm A_ExpStm(A_pos pos, A_exp exp);
A_stm A_WhileStm(A_pos pos, A_while wl);
A_stm A_BreakStm(A_pos pos);
A_stm A_FunDecStm(A_pos pos, A_funDec fundec);
A_stm A_ThreadDecStm(A_pos pos, A_threadDec threaddec);
A_stm A_ReturnStm(A_pos pos, A_exp exp);
A_stm A_IfStm(A_pos pos, A_if ifstm);
A_stm A_ArraydefStm(A_pos pos, A_arraydef arraydef);
A_stm A_GlobalStm(A_pos pos, S_symbol gid);

///////////////////////////////////////////////////////////////////////////////////////////

struct A_while_{
	A_exp cond;
	A_stmList stml;
};
A_while A_While(A_exp cond, A_stmList stml);

///////////////////////////////////////////////////////////////////////////////////////////

struct A_funDec_{
	S_symbol name;
	A_typeFields params;
	A_stmList body;
};

struct A_typeFields_{
	A_fieldl fl;
	A_fielddl fdl;
};

struct A_field_{
	S_symbol typ;
	S_symbol name;
};

struct A_fieldl_{
	A_field head;
	A_fieldl tail;
};

struct A_fieldd_{
	S_symbol typ;
	S_symbol name;
	A_exp exp;
};

struct A_fielddl_{
	A_fieldd head;
	A_fielddl tail;
};

A_funDec A_FunDec(S_symbol name, A_typeFields params, A_stmList body);
A_typeFields A_TypeFields(A_fieldl fl, A_fielddl fdl);
A_field A_Field(S_symbol typ, S_symbol name);
A_fieldl A_Fieldl(A_field head, A_fieldl tail);
A_fieldd A_Fieldd(S_symbol typ, S_symbol name, A_exp exp);
A_fielddl A_Fielddl(A_fieldd head, A_fielddl tail);

///////////////////////////////////////////////////////////////////////////////////////////

struct A_threadDec_{
	S_symbol name;
	A_stmList body;
};
A_threadDec A_ThreadDec(S_symbol name, A_stmList body);

///////////////////////////////////////////////////////////////////////////////////////////

struct A_elifExp_{
	A_exp test;
	A_stmList body;
};

struct A_elifList_{
	A_elifExp head;
	A_elifList tail;
};

struct A_if_{
	enum{ A_noelseIfStm, A_elseIfStm } kind;
	union{
		struct{ A_exp exp; A_stmList stmlist; A_elifList eliflist; } noelse;
		struct{ A_exp exp; A_stmList stmlist; A_elifList eliflist; A_stmList elsestmlist; } elsee;
	}u;
};
A_elifExp A_ElifExp(A_exp test, A_stmList body);
A_elifList A_ElifList(A_elifExp head, A_elifList tail);
A_if A_NoelseIfstm(A_exp exp, A_stmList stmlist, A_elifList eliflist);
A_if A_ElseIfstm(A_exp exp, A_stmList stmlist, A_elifList eliflist, A_stmList elsestmlist);

///////////////////////////////////////////////////////////////////////////////////////////

struct A_arraydef_{
	A_variate var;
	A_arraytool array;
};
A_arraydef A_Arraydef(A_variate var, A_arraytool array);

struct A_arraytool_{
	enum{
		A_iarray, A_farray, A_carray, A_arraycopy
	}kind;
	union{
		struct{ int size; A_cnumlist list; } array;
		int cnum;
		S_symbol id;
	}u;
};
A_arraytool A_Iarray(int size, A_cnumlist list);
A_arraytool A_Farray(int size, A_cnumlist list);
A_arraytool A_Carray(int size);
A_arraytool A_Arraycopy(S_symbol id);

///////////////////////////////////////////////////////////////////////////////////////////

//表达式 语句结构
struct A_exp_{
    A_pos pos; //int
	void *type;
	enum{
		A_opExp, A_notExp, A_funcallExp, A_threadcallExp, A_parentExp, A_variateExp, A_assignExp, 
		A_cnumExp, A_clistExp, A_stringExp, A_pose
	}kind;
	union{
		A_op ope;
		A_exp notexp;
		A_funcall funcall;
		S_symbol threadcall;
		A_exp parexp;
		A_variate var;
		A_assign assign;
		A_cnum cnum;
		A_cnumlist clist;
		//////////////////
		A_string str;
		//////////////////
		//string str;
	}u;
};

A_exp A_OpExp(A_pos pos, A_op ope);
A_exp A_NotExp(A_pos pos, A_exp exp);
A_exp A_FuncallExp(A_pos pos, A_funcall funcall);
A_exp A_ThreadcallExp(A_pos pos, S_symbol name);
A_exp A_ParentExp(A_pos pos, A_exp exp);				// 系统中去掉了ParentExp这一项，在absyn.h模块没有删除相关代码对系统没有影响
A_exp A_VariateExp(A_pos pos, A_variate var);
A_exp A_AssignExp(A_pos pos, A_assign assign);
A_exp A_CnumExp(A_pos pos, A_cnum cnum);
A_exp A_ClistExp(A_pos pos, A_cnumlist clist);
A_exp A_StringExp(A_pos pos, string s);
A_exp A_Pose(A_pos pos);


struct A_op_{
	A_oper oper;
	A_exp left;
	A_exp right;
};
A_op A_Op(A_oper oper, A_exp left, A_exp right);

struct A_funcall_{
	S_symbol func; 
	A_args args;
};
A_funcall A_Funcall(S_symbol func, A_args args);

struct A_args_{
    A_exp head; 
	A_args tail;	
};
A_args A_Args(A_exp head, A_args tail);

struct A_variate_{
	enum{A_simpleVar, A_subscriptVar} kind;
	union{
        S_symbol simple;
        struct{S_symbol var; A_exp e;} subscript;
    }u;
};
A_variate A_SimpleVar(S_symbol simple);
A_variate A_SubscriptVar(S_symbol var, A_exp e);

struct A_assign_{
	A_variate var;
	A_exp exp;
};
A_assign A_Assign(A_variate var, A_exp exp);

struct A_cnum_{
	enum {A_int, A_double, A_char} kind;
    double d;
};
A_cnum A_Cint(int i);
A_cnum A_Cdouble(double d);
A_cnum A_Cchar(char b);			// 暂时没有使用，用作单个字符类型的扩展

struct A_cnumlist_{
	A_cnum num;
	A_cnumlist tail;
};
A_cnumlist A_Cnumlist(A_cnum num, A_cnumlist tail);

///////////////////////////////////////////////////////////////////////////////////////////

struct A_string_{
	string str;
	unsigned int len;			// 保留字符串实际长度， 包含‘\0’的长度
};
A_string A_String(string str , unsigned int len);

#endif
