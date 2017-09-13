#include "log.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include "errmsg.h"

#define LOG_LOCATION "C:\\SiasunRobot\\rsi\\r_log\\RSIL_%m%d%H%M.log"
#define FILENAME_SIZE 128
#define HEADER_SIZE 32

static char filename[FILENAME_SIZE];
static FILE *pfile = NULL;
static char logheader[HEADER_SIZE];

static void _log_init(char *filename){ }

void Log(char *msg, ...)
{
	va_list ap;
	time_t t;
	struct tm * newtime = NULL;

	t = time(NULL);
	newtime = localtime(&t);

	if (pfile == NULL){
		strftime(filename, FILENAME_SIZE, LOG_LOCATION, newtime);
        if ((pfile = fopen(filename, "a+")) == NULL){
			EM_errorE(0, "Log file %s create failed\n", filename);
		}
	} else{
		fprintf(pfile, "\n");
	}

	strftime(logheader, 32, "%m-%d %H:%M:%S  ", newtime);
	fprintf(pfile, logheader);
	va_start(ap, msg);
	vfprintf(pfile, msg, ap);
	va_end(ap);
}

void LogA(char *msg, ...)
{
	va_list ap;
	if (pfile == NULL)	return;

	va_start(ap, msg);
	vfprintf(pfile, msg, ap);
	va_end(ap);
}

void Log_(char *msg, va_list ap)
{
	time_t t;
	struct tm * newtime = NULL;

	t = time(NULL);
	newtime = localtime(&t);

	if (pfile == NULL){
        strftime(filename, FILENAME_SIZE, LOG_LOCATION, newtime);
		if ((pfile = fopen(filename, "a+")) == NULL){
			EM_errorE(0, "Log file %s create failed\n", filename);
		}
	} else{
		fprintf(pfile, "\n");
	}
	strftime(logheader, 32, "%m-%d %H:%M:%S  ", newtime);
	fprintf(pfile, logheader);
	vfprintf(pfile, msg, ap);
}

void Log_A(char *msg, va_list ap)
{
	if (pfile == NULL)	return;
	vfprintf(pfile, msg, ap);
}

void Log_end()
{
	if (pfile != NULL){
		fclose(pfile);
		pfile = NULL;
	} 
}
