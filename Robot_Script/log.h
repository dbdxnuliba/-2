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
// D: ��ǰ��׷����־
// D: ֻ����Log�ļ���ʱ����Ч�� ���Զ������ļ�
void LogA(char *msg, ...);

// N: Log_ Append
// D: ͬLogAһ��
void Log_A(char *msg, va_list ap);

void Log_end();

#endif