#include "errmsg.h"
#include "log.h"
#include <string.h>

bool EM_anyErrors = FALSE;
int EM_tokPos = 0;

static string fileName = NULL;
static int lineNum = 1;
static U_intList linePos = NULL;

///////////////////////////////////////////////////////////////////////////////////////////

static __inline void EM_putErrMsg(char *message, ...);
static void EM_initErrMsg();

#ifdef _MSC_VER
#	include <Windows.h>
#	include <excpt.h>
#	include <crtdbg.h>

void myInvalidParameterHandler(const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved)
{
	EM_initErrMsg();
	EM_putErrMsg("Invalid parameter detected in function %s", function);
	EM_putErrMsg("Expression: %s\n", expression);
}

#endif

#define _EM_MSGINFO_SIZE 512
static char errmsg[_EM_MSGINFO_SIZE];
static char *top = errmsg;
static int left = _EM_MSGINFO_SIZE - 1;

char* EM_getErrMsg()
{
	return errmsg;
}

int EM_getErrMsgSize()
{
	return _EM_MSGINFO_SIZE - left;
}

static void EM_initErrMsg()
{
	errmsg[0] = '\0';
	left = _EM_MSGINFO_SIZE - 1;
	top = errmsg;
}

static __inline void EM_putErrMsg(char *message, ...)
{
	va_list ap;
	int len;

#ifdef _MSC_VER
	_invalid_parameter_handler oldHandler, newHandler;
	newHandler = myInvalidParameterHandler;
	oldHandler = _set_invalid_parameter_handler(newHandler);
#endif
		va_start(ap, message);
		if ((len = vsprintf_s(top, left, message, ap)) == -1){
			EM_initErrMsg();
			// Linux 环境下会直接退出
		} else{
			left -= len;
			top += len;
		}
		va_end(ap);
#ifdef _MSC_VER
	oldHandler = _set_invalid_parameter_handler(oldHandler);
#endif

}
static __inline void EM_putErrMsg_(char *message, va_list ap)
{
	int len;

#ifdef _MSC_VER
	_invalid_parameter_handler oldHandler, newHandler;
	newHandler = myInvalidParameterHandler;
	oldHandler = _set_invalid_parameter_handler(newHandler);
#endif

	if ((len = vsprintf_s(top, left, message, ap)) == -1){
		EM_initErrMsg();
		// Linux 环境下会直接退出
	} else{
		left -= len;
		top += len;
	}
#ifdef _MSC_VER
	oldHandler = _set_invalid_parameter_handler(oldHandler);
#endif

}
///////////////////////////////////////////////////////////////////////////////////////////

void EM_newline(void)
{
	lineNum++;
	linePos = U_IntList(EM_tokPos, linePos);
}

void EM_error(int pos, char *message,...)
{
	va_list ap;
	U_intList lines = linePos;
	int num = lineNum;
	int temp = 0;
	int logSign = FALSE;

	EM_anyErrors = TRUE;
	while(lines && (lines->i > pos)){
		lines = lines->rest;
		num--;
	}

	// Log函数没有在EM_putErrMsg中调用主要考虑到，在Log函数中是自动输出‘\n’
	if (fileName) {
		Log("%s: ", fileName);
		EM_putErrMsg("%s: ", fileName); 
		logSign = TRUE;
	}
	if (lines){
		logSign ? LogA("%d.%d  ", num, pos - lines->i) :
			Log("%d.%d  ", num, pos - lines->i), logSign = TRUE;	
		EM_putErrMsg("%d.%d  ", num, pos - lines->i);
	} 

	va_start(ap, message);
	logSign ? Log_A(message, ap): Log_(message, ap), logSign = TRUE;
	EM_putErrMsg_(message, ap);
	va_end(ap);
	
	EM_putErrMsg("\n");
}

void EM_reset(string fname)
{
	EM_anyErrors = FALSE;
	fileName = fname;

	// reset行列表
	lineNum = 1;
	U_D_IntList(&linePos);
	linePos = U_IntList(0, NULL);

	EM_initErrMsg();
}

int EM_getLineNum(int pos)
{
	U_intList lines = linePos;
	int num = lineNum;
	while (lines && lines->i > pos){
		lines = lines->rest;
		num--;
	}
	return num;
}

///////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#	include <windows.h>
#	include <excpt.h>
#endif
void EM_errorE(unsigned int exceptionCode, char *message, ...)
{
	va_list ap;
#ifdef _MSC_VER		// windows system error handle
	va_start(ap, message);
	Log_(message, ap);
	EM_putErrMsg_(message, ap);
	va_end(ap);
	EM_putErrMsg("\n");
	RaiseException(RI_ERROR_SYSERR | exceptionCode, EXCEPTION_NONCONTINUABLE, 0, NULL);
#else				// linux system error handle
	va_start(ap, message);
	Log(message, ap);
	vfprintf(stderr, message, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(0);
#endif
}
