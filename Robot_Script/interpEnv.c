#include "interpEnv.h"

#include "expval.h"
#include "errmsg.h"

//创建编译环境
interpEnv IE_getInterpEnv(void)
{
    interpEnv ie = NULL;
	int i = 0;
	ie = (interpEnv)checked_malloc(sizeof(*ie));

	ie->global_sta = Stack();
	ie->local_sta = Stack();

	ie->etx = E_Val();
	ie->p = 0;
	ie->regc = 0;
	for (; i < REG_COUNT; i++){
		ie->regs[i] = E_Val();
	}

    //
    // 抽象系统表信息
    //
    ie->ty_tab = E_TyEnv();             // typetable
    ie->sfunc_tab = E_SysFuncEnv();     // system function table
    ie->sthr_tab = E_SysThreadEnv();    // system thread table
    ie->ufunc_tab = E_UFEnv();          // user func table
    ie->uthr_tab = E_UFEnv();           // user thread table

	ie->interpstate = I_MAIN;
	ie->stackdeep = 0;

	return ie;
}

interpEnv IE_getInterpThreadEnv(interpEnv env)
{
	interpEnv ie = NULL;
	int i = 0;
	ie = (interpEnv)checked_malloc(sizeof(*ie));

	ie->global_sta = env->global_sta;
	ie->local_sta = Stack();

	ie->etx = E_Val();
	ie->p = 0;
	ie->regc = 0;
	for (; i < REG_COUNT; i++){
		ie->regs[i] = E_Val();
	}

	ie->ty_tab = env->ty_tab;
	ie->sfunc_tab = env->sfunc_tab;
	ie->sthr_tab = env->sthr_tab;
	ie->ufunc_tab = env->ufunc_tab;
	ie->uthr_tab = env->uthr_tab;

	ie->interpstate = I_MAIN; // no use
	ie->stackdeep = 0;

	return ie;
}

void IE_D_InterpEnv(interpEnv *sysenv)
{
	interpEnv env = NULL;
	int i = 0;
	if (*sysenv == NULL) return;

	env = *sysenv;

	D_Stack(&(env->global_sta));
	D_Stack(&(env->local_sta));

	E_D_Val(&(env->etx));
	for (;i < REG_COUNT; i++){
		E_D_Val(&((env->regs)[i]));
	}

	E_D_TyEnv();
	E_D_SysFuncEnv();
	E_D_SysThreadEnv();
	E_D_UFEnv(&(env->ufunc_tab));
	E_D_UFEnv(&(env->uthr_tab));

	free(env);
	*sysenv = NULL;
}

void IE_F_InterpEnv(interpEnv *sysenv)
{
	interpEnv env = NULL;
	int i = 0;
	if (*sysenv == NULL) return;
	env = *sysenv;

	D_Stack(&(env->local_sta));
	
	E_D_Val(&(env->etx));
	for (; i < REG_COUNT; i++){
		E_D_Val(&((env->regs)[i]));
	}

	free(env);
	*sysenv = NULL;
}

void IE_C_InterpEnv(interpEnv sysenv)
{
	int i = 0;
	if (sysenv == NULL) return;

	C_Stack(sysenv->global_sta);
	C_Stack(sysenv->local_sta);

	E_C_Val(sysenv->etx);
	for (i = 0; i < REG_COUNT; i++){
		E_C_Val(sysenv->regs[i]);
	}
	sysenv->p = 0;
	sysenv->regc = 0;

	E_C_UFEnv(sysenv->ufunc_tab);
	E_C_UFEnv(sysenv->uthr_tab);
	
	sysenv->interpstate = I_MAIN;
	sysenv->stackdeep = 0;
}

E_val IE_getNullReg(interpEnv sysenv)
{
	int t, i;

	if (sysenv->regc++ < REG_COUNT){
		t = sysenv->p;
		for (i = (t + 1) % REG_COUNT; i != t; i = (i + 1) % REG_COUNT) {
			if (sysenv->regs[i]->kind == E_NULL){
				sysenv->p = i;
				break;
			}
		}
	} else{
		EM_errorE(0, RSIERR_REG_REG_LIMITED_INFO);
	}
	return sysenv->regs[t];
}

void IE_freeReg(interpEnv sysenv, E_val reg)
{
	sysenv->regc--;
	reg->kind = E_NULL;
}
