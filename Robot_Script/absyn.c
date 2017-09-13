#include "absyn.h"
#include "symbol.h"

static __inline void freeZ(void **p);
void DelStmList(A_stmList *stml);
void DelExp(A_exp *exp);
static void DelStm(A_stm *stm);
static void DelWhile(A_while *w);
static void DelFunDec(A_funDec *fund);
static void DelTypeFields(A_typeFields *typef);
static void DelField(A_field *field);
static void DelFieldl(A_fieldl *fieldl);
static void DelFieldd(A_fieldd *fieldd);
static void DelFielddl(A_fielddl *fielddl);
static void DelThreadDec(A_threadDec *thrdec);
static void DelElifExp(A_elifExp *elife);
static void DelElifList(A_elifList *elifl);
static void DelIfstm(A_if *ifs);
static void DelArraydef(A_arraydef *arraydef);
static void DelArrayTool(A_arraytool *arraytool);
static void DelOp(A_op *op);
static void DelFuncall(A_funcall *func);
static void DelArgs(A_args *args);
static void DelVar(A_variate *var);
static void DelAssign(A_assign *assign);
static void DelCnum(A_cnum *cnum);
static void DelCnumlist(A_cnumlist *cnuml);
static void DelString(A_string *str);

static __inline void freeZ(void **p)
{
	free(*p);
	*p = NULL;
}

//创建新的语句列表，并初始化
A_stmList A_StmList(A_stm stm, A_stmList tail)
{
	A_stmList psl = (A_stmList)checked_malloc(sizeof(*psl));
	psl->stm = stm;
	psl->tail = tail;
	return psl;
}

//删除语句列表，并删除列表内所有语句
void DelStmList(A_stmList *stml)
{
	A_stmList stl = *stml, tstl = NULL;
	while (stl){
		tstl = stl;
		stl = stl->tail;
		DelStm(&(tstl->stm));
		free(tstl);
	}
	(*stml) = NULL;
}

//////////////////////////////////////////////////////////////////////////////////

A_stm A_ExpStm(A_pos pos, A_exp exp)
{
    A_stm s = (A_stm)checked_malloc(sizeof(*s));
    s->pos = pos;
	s->kind = A_expStm;
    s->u.expStm = exp;
    return s;
}

A_stm A_WhileStm(A_pos pos, A_while wl)
{
	A_stm s = (A_stm)checked_malloc(sizeof(*s));
	s->pos = pos;
	s->kind = A_whileStm;
    s->u.whileStm = wl;
	return s;
}

A_stm A_BreakStm(A_pos pos)
{
	A_stm s = (A_stm)checked_malloc(sizeof(*s));
	s->pos = pos;
	s->kind = A_breakStm;
	return s;
}

A_stm A_FunDecStm(A_pos pos, A_funDec fundec)
{
	A_stm s = (A_stm)checked_malloc(sizeof(*s));
	s->pos = pos;
	s->kind = A_fundecStm;
	s->u.fundStm = fundec;
	return s;
}

A_stm A_ThreadDecStm(A_pos pos, A_threadDec threaddec)
{
	A_stm s = (A_stm)checked_malloc(sizeof(*s));
	s->pos = pos;
	s->kind = A_threaddecStm;
	s->u.threaddStm = threaddec;
	return s;
}

A_stm A_ReturnStm(A_pos pos, A_exp exp)
{
	A_stm s = (A_stm)checked_malloc(sizeof(*s));
	s->pos = pos;
	s->kind = A_returnStm;
	s->u.reStm = exp;
	return s;
}

A_stm A_IfStm(A_pos pos, A_if ifstm)
{
	A_stm s = (A_stm)checked_malloc(sizeof(*s));
	s->pos = pos;
	s->kind = A_ifStm;
	s->u.ifStm = ifstm;
	return s;
}

A_stm A_ArraydefStm(A_pos pos, A_arraydef arraydef)
{
	A_stm p = (A_stm)checked_malloc(sizeof(*p));
	p->pos = pos;

	p->kind = A_arraydefStm;
	p->u.arraydef = arraydef;
	return p;
}

A_stm A_GlobalStm(A_pos pos, S_symbol gid)
{
	A_stm p = (A_stm)checked_malloc(sizeof(*p));
	p->pos = pos;

	p->kind = A_globalStm;
	p->u.gid = gid;
	return p;
}

//
static void DelStm(A_stm *stm)
{	
	A_stm st = *stm;
	if (st == NULL) return;

	switch (st->kind){
	case A_expStm: 
		DelExp(&(st->u.expStm)); break;
	case A_whileStm: 
		DelWhile(&(st->u.whileStm)); break;
	case A_breakStm: 
		break;
	case A_fundecStm: 
		DelFunDec(&(st->u.fundStm)); break;
	case A_threaddecStm: 
		DelThreadDec(&(st->u.threaddStm)); break;
	case A_returnStm: 
		DelExp(&(st->u.reStm)); break;
	case A_ifStm: 
		DelIfstm(&(st->u.ifStm)); break;
	case A_arraydefStm: 
		 DelArraydef(&(st->u.arraydef)); break;
	default: return;  break;
	}
	freeZ(stm);
}

///////////////////////////////////////////////////////////////////////////////////////

A_while A_While(A_exp cond, A_stmList stml)
{
	A_while pw = (A_while)checked_malloc(sizeof(*pw));
	pw->cond = cond;
	pw->stml = stml;
	return pw;
}

static void DelWhile(A_while *w)
{
	if (*w == NULL) return;
	DelExp(&((*w)->cond));
	DelStmList(&((*w)->stml));

	freeZ(w);
}

///////////////////////////////////////////////////////////////////////////////////////

A_funDec A_FunDec(S_symbol name, A_typeFields params, A_stmList body)
{
	A_funDec pf = (A_funDec)checked_malloc(sizeof(*pf));
//	A_fieldl fl = params->fl;
	A_fielddl fdl = params->fdl;
//	A_typeFields ps = params;
	pf->name = name;
	pf->params = params;
	/*
	while (fl){
		printf("%s : %s ", S_name(fl->head->typ), S_name(fl->head->name));
		fl = fl->tail;
	}
	while (fdl){
		printf("%s : %s \n", S_name(fdl->head->typ), S_name(fdl->head->name));
		fdl = fdl->tail;
	}*/
	pf->body = body;
	return pf;
}

static void DelFunDec(A_funDec *fund)
{
	if ((*fund) == NULL) return;
	DelTypeFields(&((*fund)->params));
	DelStmList(&((*fund)->body));

	freeZ(fund);
}

///////////////////////////////////////////////////////////////////////////////////////

A_typeFields A_TypeFields(A_fieldl fl, A_fielddl fdl)
{
	A_typeFields p = (A_typeFields)checked_malloc(sizeof(*p));
	p->fl = fl;
	p->fdl = fdl;
	return p;
}

static void DelTypeFields(A_typeFields *typef)
{
	if (*typef == NULL) return;

	DelFielddl(&((*typef)->fdl));
	DelFieldl(&((*typef)->fl));

	freeZ(typef);
}

A_field A_Field(S_symbol typ, S_symbol name)
{
	A_field p = (A_field)checked_malloc(sizeof(*p));
	p->typ = typ;
	p->name = name;
	return p;
}

static void DelField(A_field *field)
{
	if ((*field) == NULL) return;
	freeZ(field);
}

A_fieldl A_Fieldl(A_field head, A_fieldl tail)
{
	A_fieldl p = (A_fieldl)checked_malloc(sizeof(*p));
	p->head = head;
	p->tail = tail;
	return p;
}

static void DelFieldl(A_fieldl *fieldl)
{
	A_fieldl fdl = *fieldl, tfdl = NULL;
	while (fdl){
		tfdl = fdl;
		fdl = fdl->tail;
		DelField(&(tfdl->head));
		free(tfdl);
	}
	*fieldl = NULL;
}

A_fieldd A_Fieldd(S_symbol typ, S_symbol name, A_exp exp)
{
	A_fieldd p = (A_fieldd)checked_malloc(sizeof(*p));
	p->typ = typ;
	p->name = name;
	p->exp = exp;
	return p;
}

static void DelFieldd(A_fieldd *fieldd)
{
	if (*fieldd == NULL) return;

	DelExp(&((*fieldd)->exp));
	freeZ(fieldd);
}

A_fielddl A_Fielddl(A_fieldd head, A_fielddl tail)
{
	A_fielddl p = (A_fielddl)checked_malloc(sizeof(*p));
	p->head = head;
	p->tail = tail;
	return p;
}

static void DelFielddl(A_fielddl *fielddl)
{
	A_fielddl fddl = *fielddl, tfddl = NULL;
	if (fddl == NULL) return;
	while (fddl){
		tfddl = fddl;
		fddl = fddl->tail;
		DelFieldd(&(tfddl->head));
		free(tfddl);
	}
	*fielddl = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////

A_threadDec A_ThreadDec(S_symbol name, A_stmList body)
{
	A_threadDec p = (A_threadDec)checked_malloc(sizeof(*p));
	p->name = name;
	p->body = body;
	return p;
}

static void DelThreadDec(A_threadDec *thrdec)
{
	if (*thrdec == NULL) return;
	DelStmList(&((*thrdec)->body));

	freeZ(thrdec);
}

///////////////////////////////////////////////////////////////////////////

A_elifExp A_ElifExp(A_exp test, A_stmList body)
{
	A_elifExp p = (A_elifExp)checked_malloc(sizeof(*p));
	p->test = test;
	p->body = body;
	return p;
}

static void DelElifExp(A_elifExp *elife)
{
	if (*elife == NULL) return;
	DelExp(&((*elife)->test));
	DelStmList(&((*elife)->body));

	freeZ(elife);
}

A_elifList A_ElifList(A_elifExp head, A_elifList tail)
{
	A_elifList p = (A_elifList)checked_malloc(sizeof(*p));
	p->head = head;
	p->tail = tail;
	return p;
}

static void DelElifList(A_elifList *elifl)
{
	A_elifList el = *elifl, tel = NULL;
	while (el){
		tel = el;
		el = el->tail;
		DelElifExp(&(tel->head));
		free(tel);
	}
	*elifl  = NULL;
}

A_if A_NoelseIfstm(A_exp exp, A_stmList stmlist, A_elifList eliflist)
{
	A_if p = (A_if)checked_malloc(sizeof(*p));
	p->kind = A_noelseIfStm;
	p->u.noelse.exp = exp;
	p->u.noelse.stmlist = stmlist;
	p->u.noelse.eliflist = eliflist;
	return p;
}

A_if A_ElseIfstm(A_exp exp, A_stmList stmlist, A_elifList eliflist, A_stmList elsestmlist)
{
	A_if p = (A_if)checked_malloc(sizeof(*p));
	p->kind = A_elseIfStm;
	p->u.elsee.exp = exp;
	p->u.elsee.stmlist = stmlist;
	p->u.elsee.eliflist = eliflist;
	p->u.elsee.elsestmlist = elsestmlist;
	return p;
}

static void DelIfstm(A_if *ifs)
{
	A_if aif = *ifs;

	if (aif == NULL ) return;

	switch ((aif)->kind){
	case A_noelseIfStm: 
		DelExp(&(aif->u.noelse.exp));
		DelStmList(&(aif->u.noelse.stmlist));
		DelElifList(&(aif->u.noelse.eliflist));
		break;
	case A_elseIfStm: 
		DelExp(&(aif->u.elsee.exp));
		DelStmList(&(aif->u.elsee.stmlist));
		DelElifList(&(aif->u.elsee.eliflist));
		DelStmList(&(aif->u.elsee.elsestmlist));
		break;
	default: return;  break;
	}
	freeZ(ifs);
}

///////////////////////////////////////////////////////////////////////////

A_arraydef A_Arraydef(A_variate var, A_arraytool array)
{
	A_arraydef p = (A_arraydef)checked_malloc(sizeof(*p));
	p->var = var;
	p->array = array;
	return p;
}

static void DelArraydef(A_arraydef *arraydef)
{
	A_arraydef ad = *arraydef;
	DelVar(&(ad->var));
	DelArrayTool(&(ad->array));

	freeZ(arraydef);
}

A_arraytool A_Iarray(int size, A_cnumlist list)
{
	A_arraytool p = (A_arraytool)checked_malloc(sizeof(*p));
	p->kind = A_iarray;
	p->u.array.size = size;
	p->u.array.list = list;
	return p;
}

A_arraytool A_Farray(int size, A_cnumlist list)
{
	A_arraytool p = (A_arraytool)checked_malloc(sizeof(*p));
	p->kind = A_farray;
	p->u.array.size = size;
	p->u.array.list = list;
	return p;
}

A_arraytool A_Carray(int size)
{
	A_arraytool p = (A_arraytool)checked_malloc(sizeof(*p));
	p->kind = A_carray;

	p->u.cnum = size;
	return p;
}

A_arraytool A_Arraycopy(S_symbol id)
{
	A_arraytool p = (A_arraytool)checked_malloc(sizeof(*p));
	p->kind = A_arraycopy;
	p->u.id = id;
	return p;
}

static void DelArrayTool(A_arraytool *arraytool)
{
	A_arraytool at = *arraytool;
	if (at == NULL) return;
	switch (at->kind){
	case A_iarray: case A_farray:
		DelCnumlist(&(at->u.array.list));
		break;
	case A_arraycopy: case A_carray: 
		break;
	default: return; break;
	}
	freeZ(arraytool);
}

///////////////////////////////////////////////////////////////////////////////////

A_exp A_OpExp(A_pos pos, A_op ope)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_opExp;
	p->u.ope = ope;
	
	return p;
}

A_exp A_NotExp(A_pos pos, A_exp exp)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;

	p->u.notexp = exp;
	p->kind = A_notExp;
	return p;
}

A_exp A_FuncallExp(A_pos pos, A_funcall funcall)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_funcallExp;
	p->u.funcall = funcall;
	return p;	
}

A_exp A_ThreadcallExp(A_pos pos, S_symbol name)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_threadcallExp;
	p->u.threadcall = name;
	return p;	
}

A_exp A_ParentExp(A_pos pos, A_exp exp)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_parentExp;
	p->u.parexp = exp;
	return p;	
}

A_exp A_VariateExp(A_pos pos, A_variate var)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_variateExp;
	p->u.var = var;
	return p;	
}

A_exp A_AssignExp(A_pos pos, A_assign assign)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_assignExp;
	p->u.assign = assign;
	return p;	
}

A_exp A_CnumExp(A_pos pos, A_cnum cnum)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_cnumExp;
	p->u.cnum = cnum;
	return p;	
}

A_exp A_ClistExp(A_pos pos, A_cnumlist clist)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_clistExp;
	p->u.clist = clist;
	return p;	
}

A_exp A_StringExp(A_pos pos, string s)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	unsigned int len = strlen(s) + 1;
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_stringExp;
	p->u.str = A_String(s, len);
	return p;	
}

A_exp A_Pose(A_pos pos)
{
	A_exp p = (A_exp)checked_malloc(sizeof(*p));
	p->pos = pos;
	p->type = NULL;
	
	p->kind = A_pose;
	return p;
}

void DelExp(A_exp *exp)
{
	A_exp e = *exp;
	if (!e) return;
	switch (e->kind){
	case A_opExp: 
		DelOp(&(e->u.ope)); break;
	case A_notExp: 
		DelExp(&(e->u.notexp)); break;
	case A_funcallExp: 
		DelFuncall(&(e->u.funcall)); break;
	case A_threadcallExp: 
		// S_symbol在销毁符号表的时候一同删除，删除语法树的时候不需要删除
		break;						
	case A_parentExp: 
		DelExp(&(e->u.parexp)); break;
	case A_variateExp: 
		DelVar(&(e->u.var)); break;
	case A_assignExp: 
		DelAssign(&(e->u.assign)); break;
	case A_cnumExp: 
		DelCnum(&(e->u.cnum));	break;
	case A_clistExp: 
		DelCnumlist(&(e->u.clist)); break;
	case A_stringExp: 
		DelString(&(e->u.str)); break;
	case A_pose:  break;
	default: return; break;
	}
	freeZ(exp);
}

///////////////////////////////////////////////////////////////////////////////////

A_op A_Op(A_oper oper, A_exp left, A_exp right)
{
	A_op p = (A_op)checked_malloc(sizeof(*p));
	p->oper = oper;
	p->left = left;
	p->right = right;
	return p;	
}

static void DelOp(A_op *op)
{
	if ((*op) == NULL) return;
	DelExp(&((*op)->left));
	DelExp(&((*op)->right));

	freeZ(op);
}

///////////////////////////////////////////////////////////////////////////////////

A_funcall A_Funcall(S_symbol func, A_args args)
{
	A_funcall p = (A_funcall)checked_malloc(sizeof(*p));
	p->func = func;
	p->args = args;
	return p;
}

static void DelFuncall(A_funcall *func)
{
	if (*func == NULL) return;
	DelArgs(&((*func)->args));

	freeZ(func);
}

A_args A_Args(A_exp head, A_args tail)
{
	A_args p = (A_args)checked_malloc(sizeof(*p));
	p->head = head;
	p->tail = tail;
	return p;
}

static void DelArgs(A_args *args)
{
	A_args as = *args, tas = NULL;
	
	while (as){
		tas = as;
		as = as->tail;
		DelExp(&(tas->head));
		free(tas);
	}

	*args = NULL;
}

///////////////////////////////////////////////////////////////////////////////////

A_variate A_SimpleVar(S_symbol simple)
{
	A_variate p = (A_variate)checked_malloc(sizeof(*p));
	p->kind = A_simpleVar;
	p->u.simple = simple;
	return p;
}

A_variate A_SubscriptVar(S_symbol var, A_exp e)
{
	A_variate p = (A_variate)checked_malloc(sizeof(*p));
	p->kind = A_subscriptVar;
	p->u.subscript.var = var;
	p->u.subscript.e = e;
	return p;
}

static void DelVar(A_variate *var)
{
	A_variate v = *var;
	if ( v == NULL ) return;
	switch (v->kind){
	case A_simpleVar: break;
	case A_subscriptVar: 
		DelExp(&(v->u.subscript.e));
		break;
	default: return; break;
	}

	freeZ(var);
}

///////////////////////////////////////////////////////////////////////////////////

A_assign A_Assign(A_variate var, A_exp exp)
{
	A_assign p = (A_assign)checked_malloc(sizeof(*p));
	p->var = var;
	p->exp = exp;
	return p;
}

static void DelAssign(A_assign *assign)
{
	A_assign ass = *assign;
	if (ass == NULL) return;
	DelVar(&(ass->var));
	DelExp(&(ass->exp));

	freeZ(assign);
}

///////////////////////////////////////////////////////////////////////////////////

A_cnum A_Cint(int i)
{
	A_cnum p = (A_cnum)checked_malloc(sizeof(*p));
	p->kind = A_int;
	p->d = i;
	return p;	
}

A_cnum A_Cdouble(double d)
{
	A_cnum p = (A_cnum)checked_malloc(sizeof(*p));
	p->kind = A_double;
	p->d = d;
	return p;		
}

A_cnum A_Cbool(char b)
{
	A_cnum p = (A_cnum)checked_malloc(sizeof(*p));
	p->kind = A_char;
	p->d = b;
	return p;		
}

static void DelCnum(A_cnum *cnum)
{
	if (*cnum == NULL) return;
	if ((*cnum)->kind == A_int || (*cnum)->kind == A_double || (*cnum)->kind == A_char)
		freeZ(cnum);
}

A_cnumlist A_Cnumlist(A_cnum num, A_cnumlist tail)
{
	A_cnumlist p = (A_cnumlist)checked_malloc(sizeof(*p));
	p->num = num;	
	p->tail = tail;
	return p;
}

static void DelCnumlist(A_cnumlist *cnuml)
{
	A_cnumlist cnl = *cnuml, dcnl = NULL;
	while (cnl){
		dcnl = cnl;
		cnl = cnl->tail;
		DelCnum(&(dcnl->num));
		free(dcnl);
	}
	(*cnuml) = NULL;
}

///////////////////////////////////////////////////////////////////////////////////

A_string A_String(string str, unsigned int len)
{
	A_string p = (A_string)checked_malloc(sizeof(*p));
	p->len = len;
	p->str = str;
	return p;
}

static void DelString(A_string *str)
{
	if (*str == NULL) return;
	free((*str)->str);
	freeZ(str);
}

///////////////////////////////////////////////////////////////////////////////////
