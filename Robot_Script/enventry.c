#include "enventry.h"

#include "absyn.h"
#include "sysutil.h"
#include "datatype.h"

static void E_D_Param(E_param *params);					// �����û��Զ��庯���β��б�
static void E_D_Param_(E_param *params);				// �����û�ϵͳ�ں˺����β��б�

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
// D: ��ɾ��Ĭ���βε���ʽ����������û��Զ��庯���б�ʱʹ��
// D: �û��Զ���ĺ�����Ĭ�ϲ���ָ���﷨���ϵĽ�㣬ɾ���﷨��ʱһͬɾ��
static void E_D_Param(E_param *params)
{
	E_param p = *params, tp = NULL;
	while (p){
		tp = p;
		p = p->next;
		if (tp->ty != NULL) D_Ty(&(tp->ty));
		// name�ڷ��ű���
		// defval��Ĭ���βα��ʽ
		free(tp);
	}
	(*params) = NULL;
}

// N: E_D_Param_
// D: ɾ��Ĭ���βε���ʽ���������ϵͳ�ں˺����б�ʱʹ��
static void E_D_Param_(E_param *params)
{
	E_param p = *params, tp = NULL;
	while (p){
		tp = p;
		p = p->next;
		if (tp->ty != NULL) D_Ty(&(tp->ty));
		// name�ڷ��ű���
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
	// funcbodyָ���﷨���ϵĽ�㣬�˴���ɾ��
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
	// TODO: ϵͳĬ���̶߳��� - ���߳�
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
			// ����ɾ���û������Ϣ
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
