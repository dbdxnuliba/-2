#include "enventry.h"

#include "absyn.h"
#include "sysutil.h"
#include "datatype.h"

static void E_D_Param(E_param *params);					// 消除用户自定义函数形参列表
static void E_D_Param_(E_param *params);				// 消除用户系统内核函数形参列表

static void E_D_FunEnventry(E_funEnventry * funenv);
static void E_D_FunEnventry_(E_funEnventry * funenv);

E_param E_Param(S_symbol name, datatype ty, A_exp defval, E_param next) 
{
	E_param p = (E_param)checked_malloc(sizeof(*p));
	p->name = name;
	p->defval = defval;
	p->ty = ty;
	p->next = next;
	return p;
}

E_param E_Param_REG(S_symbol name, S_symbol tyid, A_exp defval, E_param next)
{
	E_param p = (E_param)checked_malloc(sizeof(*p));
	S_table tabty = E_TyEnv();
	datatype ty = S_look(tabty, tyid);
	p->name = name;
	p->defval = defval;
	p->ty = ty;
	p->next = next;
	return p;
}

// N: E_D_Param
// D: 不删除默认形参的形式，用于清空用户自定义函数列表时使用
// D: 用户自定义的函数的默认参数指向语法树上的结点，删除语法树时一同删除
static void E_D_Param(E_param *params)
{
	E_param p = *params, tp = NULL;
	while (p){
		tp = p;
		p = p->next;
		if (tp->ty != NULL) D_Ty(&(tp->ty));
		// name在符号表中
		// defval是默认形参表达式
		free(tp);
	}
	(*params) = NULL;
}

// N: E_D_Param_
// D: 删除默认形参的形式，用于清空系统内核函数列表时使用
static void E_D_Param_(E_param *params)
{
	E_param p = *params, tp = NULL;
	while (p){
		tp = p;
		p = p->next;
		if (tp->ty != NULL) D_Ty(&(tp->ty));
		// name在符号表中
//		if ((*params)->defval != NULL)
			DelExp(&(tp->defval));
		free(tp);
	}
	(*params) = NULL;
}

E_funEnventry E_FunEnventry(E_param params, A_stmList funcbody)
{
	E_funEnventry fe = (E_funEnventry)checked_malloc(sizeof(*fe));

	fe->funcbody = funcbody;
	fe->params = params;
	return fe;
}

static void E_D_FunEnventry(E_funEnventry * funenv)
{
	if (*funenv == NULL) return;
	if ((*funenv)->params != NULL)
		E_D_Param(&((*funenv)->params));
	// funcbody指向语法树上的结点，此处不删除
}

static void E_D_FunEnventry_(E_funEnventry * funenv)
{
	if (*funenv == NULL) return;
	if ((*funenv)->params != NULL)
		E_D_Param_(&((*funenv)->params));
}

S_table E_TyEnv(void)
{
    static S_table tytab = NULL;
    if (tytab) return tytab;

    tytab = S_empty();
    S_enter(tytab, S_Symbol("void"), VoidTy());
    S_enter(tytab, S_Symbol("char"), CharTy());
    S_enter(tytab, S_Symbol("int"), IntTy());
    S_enter(tytab, S_Symbol("float"), DouTy());
    S_enter(tytab, S_Symbol("pose"), PPoseTy());
    S_enter(tytab, S_Symbol("array"), PArrayTy());
    S_enter(tytab, S_Symbol("string"), PStrTy());

    return tytab;
}

void E_D_TyEnv()
{
	int i = 0;
	S_table tab = E_TyEnv();
	binder b = NULL, tb = NULL;

	if (tab == NULL) return;
	for (i = 0; i < _TABSIZE; i++){
		b = tab->table[i];
		while (b){
			tb = b;
			b = b->next;
			D_Ty(&((datatype)(tb->value)));
			free(tb);
		}
		tab->table[i] = NULL;
	}
	tab->top = NULL;

	free(tab);
}

S_table E_SysFuncEnv()
{
	static S_table tyfenv = NULL;
	if (tyfenv) return tyfenv;
	tyfenv = S_empty();
	return tyfenv;
}


void E_D_SysFuncEnv()
{
	int i = 0;
	binder b = NULL, tb = NULL;
	S_table tab = E_SysFuncEnv();
	if (tab == NULL) return;

	for (i = 0; i < _TABSIZE; i++){
		b = tab->table[i];
		while (b){
			tb = b;
			b = b->next;
			E_D_FunEnventry_(&((E_funEnventry)(tb->value)));
			free(tb);
		}
		tab->table[i] = NULL;
	}
	tab->top = NULL;

	free(tab);
}


// N: enter system function env regtool
void E_enter_SysFE_REG(S_symbol name, E_param params)
{
	S_table tyfenv = E_SysFuncEnv();
	S_enter(tyfenv, name, E_FunEnventry(params, NULL));
}

S_table E_SysThreadEnv(void)
{
	// TODO: 系统默认线程定义 - 多线程
	static S_table ty = NULL;
	if (ty) return ty;
	ty = S_empty();
	return ty;
}

void E_D_SysThreadEnv(void)
{
	int i = 0;
	binder b = NULL, tb = NULL;
	S_table tab = E_SysThreadEnv();
	if (tab == NULL) return;

	for (i = 0; i < _TABSIZE; i++){
		b = tab->table[i];
		while (b){
			tb = b;
			b = b->next;
			E_D_FunEnventry_(&((E_funEnventry)(tb->value)));
			free(tb);
		}
		tab->table[i] = NULL;
	}
	tab->top = NULL;

	free(tab);
}

void E_enter_SysTE_REG(S_symbol name)
{
	S_table tyfenv = E_SysThreadEnv();
	S_enter(tyfenv, name, tyfenv);
}

S_table E_UFEnv(void)
{
	return S_empty();
}

void E_D_UFEnv(S_table *tab)
{
	if (*tab == NULL) return;
	E_C_UFEnv(*tab);
	free(*tab);
	*tab = NULL;
}

void E_C_UFEnv(S_table tab)
{
	int i;
	binder b = NULL, tb = NULL;
	E_funEnventry fune = NULL;
	if (tab == NULL) return;

	for (i = 0; i < _TABSIZE; i++){
		b = tab->table[i];
		while (b){
			tb = b;
			b = b->next;
			// 用来删除用户表的信息
			E_D_FunEnventry(&((E_funEnventry)(tb->value)));
			free(tb);
		}
		tab->table[i] = NULL;
	}
	tab->top = NULL;
}

void E_OutputTS_FuncEnv(S_table table)
{
	S_symbol name;
	E_funEnventry fune;
	E_param params;
	binder b;
	int i = 0;
	if (table == NULL) return;

	for (;i < _TABSIZE; i++){
		b = table->table[i];
		while (b){
			name = ((binder)(b))->key;
			fune = ((binder)(b))->value;
			params = fune->params;
			printf("%s <", S_name(name));
			while (params){
				printf("%s:", S_name(params->name) );
				OutputTS_Ty(params->ty);
				params = params->next;
			}
			printf(">\n");
			b = b->next;
		}
	}

}
