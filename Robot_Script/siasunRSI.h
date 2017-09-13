#ifndef SIASUN_RSI
#define SIASUN_RSI

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/20
// Description: SIASUN Robot Script Interpreter
// Environment: windows(vs)
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "sysutil.h"

#define FILEINPUT_MODE 0x0001
#define SHELL_MODE     0x0002

int RSI_initEnv();
int RSI_start(int mode, ...);
void *RSI_execNext();
int RSI_end();

int RSI_getErrSize();
char *RSI_getErrMsg();

int RSI_exexProg();

#endif