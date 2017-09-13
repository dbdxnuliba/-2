#ifndef LOG_H
#define LOG_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/21
// Description: Log M
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>

void Log(char *msg, ...);

void Log_(char *msg, va_list ap);

// N: Log Append
// D: 向当前行追加日志
// D: 只有在Log文件打开时候有效， 不自动创建文件
void LogA(char *msg, ...);

// N: Log_ Append
// D: 同LogA一致
void Log_A(char *msg, va_list ap);

void Log_end();

#endif