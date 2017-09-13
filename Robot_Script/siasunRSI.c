#include "siasunRSI.h"

#include "absyn.h"
#include "errmsg.h"
#include "log.h"
#include "enventry.h"
#include "interpEnv.h"
#include "interpProg.h"
#include "7dof_media.h"

#ifdef _MSC_VER
#	include <Windows.h>
#	include <excpt.h>
#endif

#define SYSFUNC_REG_POS "C:/SiasunRobot/rsi/regtable"

#define MAXFILESIZE 1310700
char myinput[MAXFILESIZE];
char *myinputlim = NULL;
char *myinputptr = NULL;
int inputsize = 0;

interpEnv sysenv = NULL;

// lec.yy.c 7dof.lex
extern int charPos;
extern int yylex();

// 7dof.tab.c
extern int yyparse();           //语法分析入口
extern A_stmList absyn_root;    //语法树根节点

extern char *regetable;

static int RSI_clear();
static int _rsi_registSysFune();
static int robot_action(struct rsi_media *pm);

static void _RSI_reset()
{
	DelStmList(&absyn_root);									// 1. 清空语法树

	myinputptr = myinput;										// 2. reset flex 输入
	myinputlim = myinputptr + inputsize;
	charPos = 1;

	EM_reset(NULL);												// 3. reset errmsg M
}

static int _rsi_registSysFune_new()
{
    int err = 0;
    FILE *file = NULL;

    inputsize = strlen(regetable);
    strcpy(myinput, regetable);
    myinput[inputsize] = '\0';

    __try{
        _RSI_reset();
        yyparse();
    }
    __except (EXCEPTION_EXECUTE_HANDLER){
        err = 1;
    }

    if (EM_anyErrors == TRUE)  err = 1;
    return err;
}

static int _rsi_registSysFune()
{
	int err = 0;
	FILE *file = NULL;
	__try{
		if (fopen_s(&file, SYSFUNC_REG_POS, "r") == 0){
			inputsize = fread_s(myinput, MAXFILESIZE, sizeof(char), MAXFILESIZE, file);
			myinput[inputsize] = '\0';
		} else err = 1;
	}__finally{	if(file) fclose(file); }
	if (err) return err;

	__try{
		_RSI_reset();
		yyparse();
	}__except (EXCEPTION_EXECUTE_HANDLER){
		err = 1;
	}

	if (EM_anyErrors == TRUE)  err =  1;
	return err;
}

// N: RSI_initEnv
// D: 进入系统时候执行，只调用一次
// D: 返回值，成功 1， 失败 0
int RSI_initEnv()
{
	int err = 0;
    if (err = _rsi_registSysFune_new()/*_rsi_registSysFune()*/)	{							// 注册系统内核
		return err;
	}
	// 打印内核函数表
	// printf("system function table:\n"); E_OutputTS_FuncEnv(E_SysFuncEnv()); printf("\n"); 

	__try{
		sysenv = IE_getInterpEnv();								// 解释器系统初始化
	}__except (EXCEPTION_EXECUTE_HANDLER){
		IE_D_InterpEnv(&sysenv);								// 初始化失败， 销毁初始环境
		err = 1;
	}
	return err;
}

int RSI_start(int mode, ...)
{
	char *filename = NULL, *desstr = NULL;
	FILE *file = NULL;
	va_list ap;
	int fileOpenSuccess = TRUE;

	__try{
		if (mode == FILEINPUT_MODE){
			va_start(ap, mode);
			filename = va_arg(ap, char *);
			va_end(ap);
			if (fopen_s(&file, filename, "r") == 0){
				inputsize = fread_s(myinput, MAXFILESIZE, sizeof(char), MAXFILESIZE, file);

                if (inputsize == MAXFILESIZE){
					EM_error(0, RSIERR_PROGF_LARGE_INFO, filename);
					fileOpenSuccess = FALSE;
					__leave;
				} else{
					myinput[inputsize] = '\0';
				}
			} else{
				fileOpenSuccess = FALSE;
				EM_error(0, RSIERR_PROGF_OPENFAILED_INFO, filename);
				__leave;
			}
        } //else if (mode == STRINPUT_MODE){
			/*va_start(ap, mode);
			desstr = va_arg(ap, char *);
			va_end(ap);
			inputsize = sprintf_s();*/
        //}
	}__finally{
		if (file) fclose(file);
	}
	if (fileOpenSuccess == FALSE) return 1;						// 文件打开失败，返回

	RSI_clear();
	_RSI_reset();

	__try{
        yyparse();                                              //构建语法树
	}__except (EXCEPTION_EXECUTE_HANDLER){
		DelStmList(&absyn_root);					
		return 1;												// 构造异常， 删除语法树， 返回
	}

	if (EM_anyErrors == TRUE){									// 有语法错误
		DelStmList(&absyn_root);
		return 1;
	}

	__try{
		I_init(sysenv, absyn_root);								// interpreter M 初始化 
	}__except (EXCEPTION_EXECUTE_HANDLER){
		I_end();
		return 1;
	}
	return 0;													// Start Success
}

void *RSI_execNext()
{
	static void* p = NULL;
	__try{
		p = I_interpNext(sysenv);
	}__except (EXCEPTION_EXECUTE_HANDLER){
		RSI_clear();
		if (p != NULL)
			((struct rsi_media *)p)->kind = M_ERROR;
		// p = NULL;
	}
	return p;
}

int RSI_exexProg()
{
	while (robot_action(RSI_execNext())) 
		;
	return 0;
}


static int RSI_clear()
{
	Log_end();
	I_end();
	IE_C_InterpEnv(sysenv);
	DelStmList(&absyn_root);
	return 0;
}

int RSI_end()
{
	IE_D_InterpEnv(&sysenv);
	DelStmList(&absyn_root);
	I_end();
	return 0;
}

char *RSI_getErrMsg()
{
	return EM_getErrMsg();
}

int RSI_getErrSize()
{
	return EM_getErrMsgSize();
}

// N: robot_action
// D: 系统测试函数
// D: 执行机器人动作
static int robot_action(struct rsi_media *pm)
{
	int i;
	if (pm == NULL){
		printf("errmsg <size:%d> : %s", RSI_getErrSize(), RSI_getErrMsg());
		return 0;
	} 
	fprintf(stdout, "%2d. %2d ... ", pm->counter, pm->linenum);
	switch (pm->kind){
	case M_MOVEJ:
		fprintf(stdout, "MoveJ(%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf)\n",
			pm->u.movej.pos[0], pm->u.movej.pos[1], pm->u.movej.pos[2], pm->u.movej.pos[3],
			pm->u.movej.pos[4], pm->u.movej.pos[5], pm->u.movej.pos[6], pm->u.movej.speed, pm->u.movej.acc, pm->u.movej.rad);  break;
	case M_MOVEL:
		fprintf(stdout, "MoveL(%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf)\n",
			pm->u.movel.pos[0], pm->u.movel.pos[1], pm->u.movel.pos[2], pm->u.movel.pos[3],
			pm->u.movel.pos[4], pm->u.movel.pos[5], pm->u.movel.speed, pm->u.movel.acc, pm->u.movel.rad); break;
	case M_MOVEC: fprintf(stdout, "MoveC_2\n"); break;
	/*case M_MOVE
	C_1: fprintf(stdout, "MoveC_1\n"); break;
	case M_MOVEC_2:
		fprintf(stdout, "MoveC_2(%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, \n  \
								%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, \n  \
								%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.21\n  \
								%.2lf, %.2lf)\n",
			pm->u.movec.center[0], pm->u.movec.center[1], pm->u.movec.center[2], pm->u.movec.center[3], pm->u.movec.center[4], pm->u.movec.center[5],
			pm->u.movec.end[0], pm->u.movec.end[1], pm->u.movec.end[2], pm->u.movec.end[3], pm->u.movec.end[4], pm->u.movec.end[5],
			pm->u.movec.axis[0], pm->u.movec.axis[1], pm->u.movec.axis[2], pm->u.movec.axis[3], pm->u.movec.axis[4], pm->u.movec.axis[5], pm->u.movec.axis[6],
			pm->u.movec.speed, pm->u.movec.acc); break;*/
	case M_BD: fprintf(stdout, "biaoding()\n"); break;
	case M_NOACT: fprintf(stdout, "NOACTION\n"); break;
	case M_ERROR: printf("errmsg <size:%d> : %s  \n", RSI_getErrSize(), RSI_getErrMsg()); return 0; break;
	case M_END: fprintf(stdout, "program execute over!\n");  return 0;  break;
	case M_MOVEPSI: fprintf(stdout, "MovePsi()\n"); break;
	case M_RUNPROG: fprintf(stdout, "run_prog("); i = 0;
		while (i < pm->u.runprog.size){
			if ((pm->u.runprog.name)[i] != '\0')
				fputc((pm->u.runprog.name)[i], stdout);
			else break;
			i++;
		}
		fprintf(stdout, " , %d)\n", pm->u.runprog.size); break;
	case M_SETTCP: fprintf(stdout, "set_tcp(%.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf)\n",
		pm->u.settcp.pos[0], pm->u.settcp.pos[1], pm->u.settcp.pos[2], pm->u.settcp.pos[3],
		pm->u.settcp.pos[4], pm->u.settcp.pos[5]); break;
	default: fprintf(stdout, "unknow state\n"); break;
	}
	return 1;
}
