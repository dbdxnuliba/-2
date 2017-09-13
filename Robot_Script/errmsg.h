#ifndef ERRMSG_H
#define ERRMSG_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/20
// Description: Error Message M
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include"sysutil.h"
#include<stdarg.h>

extern bool EM_anyErrors;
extern int EM_tokPos;

// User Interface
int EM_getErrMsgSize(void);
char* EM_getErrMsg(void);


void EM_newline(void);
// N: EM_reset
// D: EM_reset在每次需要重新解释脚本时候调用
void EM_reset(string filename);
int  EM_getLineNum(int pos);

// N: EM_error
// D: Only used for Lexical & Syntax analyze
void EM_error(int, string, ...);

//
// SEH -- ERROR NUM only for windows
//
#define RI_ERROR_SYSERR		0xE0000001
#define RI_WARNING			0xA0000001

// N: EM_errorE
// D: 用于报告需要程序结束的错误类型
// D: exceptionCode 自定义的错误编号, 目前全部填入0
void EM_errorE(unsigned int exceptionCode, char *message, ...);


//
// 错误类型及编号， 暂时未用（简单记录）
//
#define RSIERR_PROGF_LARGE
#define RSIERR_PROGF_LARGE_INFO "illegal file: %s file too large"
#define RSIERR_PROGF_OPENFAILED 
#define RSIERR_PROGF_OPENFAILED_INFO "file err: %s open failed"
#define RSIERR_STA_OVERFLOW	
#define RSIERR_STA_OVERFLOW_INFO "runtime error: \"stack err_\" stack overflow"
#define RSIERR_STA_ALLOC_FAILUER 
#define RSIERR_STA_ALLOC_FAILUER_INFO "runtime error: \"stack err_\" can't alloc space for this kind of val"
#define RSIERR_STA_PUSHREF_FAILURE
#define RSIERR_STA_PUSHREF_FAILURE_INFO "runtime error: \"stack err_\" push reference failure"
#define RSIERR_STA_STACK_DISORDERED
#define RSIERR_STA_STACK_DISORDERED_INFO "runtime error: \"stack err_\" stack disorder"
#define RSIERR_REG_REG_LIMITED
#define RSIERR_REG_REG_LIMITED_INFO "runtime error: register is not enough"

#define RSIERR_PRO_TYPE_MISMATCH
#define RSIERR_PRO_TYPE_MISMATCH_INFO "Prog error: type mismatched"
#define RSIERR_PRO_TYPE_OPNOTSUPPORT 
#define RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO "Prog error: operation not support this type"
#define RSIERR_PRO_DIVZERO
#define RSIERR_PRO_DIVZERO_INFO "Prog error: div zero"
#define RSIERR_PRO_VAR_UNDEF
#define RSIERR_PRO_VAR_UNDEF_INFO "Prog error: variate not defined"
#define RSIERR_PRO_ASSIGN_FAILED 
#define RSIERR_PRO_ASSIGN_FAILED_INFO "Prog error: variate assign failed"
#define RSIERR_PRO_SUB_TYPEERROR 
#define RSIERR_PRO_SUB_TYPEERROR_INFO "Prog error: subscript type error"
#define RSIERR_PRO_VAR_DEFERR
#define RSIERR_PRO_VAR_DEFERR_INFO "Prog error: var define error"
#define RSIERR_PRO_SUB_OVERFLOW
#define RSIERR_PRO_SUB_OVERFLOW_INFO "Prog error: subscript overflow"
#define RSIERR_PRO_FUNDEF_NESTED 
#define RSIERR_PRO_FUNDEF_NESTED_INFO "Prog error: \"function define failure\" function define nested"
#define RSIERR_PRO_FUN_REDEFINED
#define RSIERR_PRO_FUN_REDEFINED_INFO "Prog error: \"function define failure\" function redefinition"
#define RSIERR_PRO_FUN_CONFLICT_SYSF 
#define RSIERR_PRO_FUN_CONFLICT_SYSF_INFO "Prog error: \"function define failure\" function name conflict with system function"
#define RSIERR_PRO_UNKNOWN_TY 
#define RSIERR_PRO_UNKNOWN_TY_INFO "Prog error: unknown system type"
#define RSIERR_PRO_PA_REDEFINE
#define RSIERR_PRO_PA_REDEFINE_INFO "Prog err: \"function define failure\" param has already defined"
#define RSIERR_PRO_PA_DEFAULTPA_ERR 
#define RSIERR_PRO_PA_DEFAULTPA_ERR_INFO "Prog err: \"function define failure\" default param value kind error"
#define RSIERR_PRO_PA_TY_ERR
#define RSIERR_PRO_PA_TY_ERR_INFO "Prog err: \"function define failure\" param Type error"
#define RSIERR_PRO_ARRAYDEF_ERR 
#define RSIERR_PRO_ARRAYDEF_ERR_INFO "Prog err: array define error"
#define RSIERR_PRO_ARRAY_RED
#define RSIERR_PRO_ARRAY_RED_INFO "Prog err: array redefined"
#define RSIERR_FUN_NOTDEF 
#define RSIERR_FUNCALL_NOTDEF_INFO "Prog error: function not exist"
#define RSIERR_FUN_LACKPARAM
#define RSIERR_FUN_LACKPARAM_INFO "Prog error: function call lack of param"
#define RSIERR_FUNCALL_ILLEGALP
#define RSIERR_FUNCALL_ILLEGALP_INFO "Prog error: illegal params"
#define RSIERR_FUNCALL_DLOREF_ERR
#define RSIERR_FUNCALL_DLOREF_ERR_INFO "Prog error: gloabal reference not exist"
#define RSIERR_FUNCALL_CALPOS_ERR
#define RSIERR_FUNCALL_CALPOS_ERR_INFO "Prog error: Cal_pos func called err"

#define RSIERR_UNKNOWN_EXCEPTION 
#define RSIERR_UNKNOWN_EXCEPTION_INFO "runtime error: system unknown exception"


#define RSIERR_PRO_OP_XOR_NOTSUPPORT
#define RSIERR_PRO_OP_XOR_NOTSUPPORT_INFO "Prog error: xor operation not support"

#define RSIERR_SOCK_WSA_START_FAILED
#define RSIERR_SOCK_WSA_START_FAILED_INFO "WSAStartup failed with error: %d\n"
#define RSIERR_SOCK_WINSOCK_VERSION_ERR
#define RSIERR_SOCK_WINSOCK_VERSION_ERR_INFO "Could not find a usable version of Winsock.dll"


#endif
