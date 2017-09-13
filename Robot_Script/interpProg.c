#include "interpProg.h"

#include "absyn.h"
#include "errmsg.h"
#include "tool.h"
#include "7dof_media.h"
#include "7doflib\7dof_robot.h"
#include "7doflib\7dof_socket.h"
#include "7doflib\7dof_camera.h"

#define RSI_FUNPARAM_COUNT 30											// 程序支持的最大参数个数
static void *arr[RSI_FUNPARAM_COUNT];									// 系统函数调用参数传递

static void I_interpExp(interpEnv sysenv, A_exp exp, E_val reval);
static void I_interpNum(interpEnv sysenv, A_cnum cnum, E_val reval);	// DNOE

static void OpPlus(E_val elx, E_val erx, E_val reval);
static void OpMinus(E_val elx, E_val erx, E_val reval);
static void OpTimes(E_val elx, E_val erx, E_val reval);
static void OpDiv(E_val elx, E_val erx, E_val reval);
static void Opeq(E_val elx, E_val erx, E_val reval);
static void Opneq(E_val elx, E_val erx, E_val reval);
static void Oplt(E_val elx, E_val erx, E_val reval);
static void Ople(E_val elx, E_val erx, E_val reval);
static void Opgt(E_val elx, E_val erx, E_val reval);
static void Opge(E_val elx, E_val erx, E_val reval);
static void Opand(E_val elx, E_val erx, E_val reval);
static void Opor(E_val elx, E_val erx, E_val reval);
static void Opxor(E_val elx, E_val erx, E_val reval);

static void I_interpOp(interpEnv sysenv, A_op ope, E_val reval);
static void I_interNotOp(interpEnv sysenv, A_exp exp, E_val reval);
static void I_interpAssign(interpEnv sysenv, A_assign asexp, E_val reval);
static void I_interpVariate(interpEnv sysenv, A_variate variate, E_val reval);
static void I_interpFundecStm(interpEnv sysenv, A_funDec fundec);
static void I_interpArrDefStm(interpEnv sysenv, A_arraydef arraydef, E_val reval);
static void I_interpWhileStm(interpEnv sysenv, A_while wstm, E_val reval);
static void I_interpWhileStm_N(interpEnv sysenv, A_while wstm, E_val reval);
static void I_interpStm(interpEnv sysenv, A_stm stm);
static void I_interpIfStm(interpEnv sysenv, A_if ifstm, E_val reval);
static void I_interpIfStm_N(interpEnv sysenv, A_if ifstm, E_val reval);
static void I_interpBreakStm(interpEnv sysenv);
static void I_interpFuncCall(interpEnv sysenv, A_funcall funcall, E_val reval);

static int _searchParams(E_param ps, S_symbol name);
static E_param _makeParams(interpEnv sysenv, A_typeFields params);

static void I_interpSysFuncCall(interpEnv sysenv, S_symbol funcname, E_val reval, E_param params);
static void I_interpReturn(interpEnv sysenv, A_exp exp);
static void I_interpGloVal(interpEnv sysenv, S_symbol var);

//
// D: 单句解释模块, signal Mode
//
static sta_tool sm_statool = NULL;
static A_stmList sm_pcounter = NULL;
struct rsi_media sm_media;

// D: 函数参数返回值控制符
static int sdeep = 0;																	// TODO: 函数参数返回 (TEST)
static struct E_val_ funreval;

void I_init(interpEnv sysenv, A_stmList prog)
{
	sm_statool = T_Sta_tool();
	sm_pcounter = prog;

	sm_media.linenum = 0;
	sm_media.counter = 0;
	return ;
}

void I_end()
{
	T_D_staTool(&sm_statool);
}

void *I_interpNext(interpEnv sysenv)
{
	if (sm_pcounter == NULL){
		if ((sm_pcounter = T_staPop(sm_statool)) == NULL){
			sm_media.kind = M_END;
			return &sm_media;						// 程序执行结束
		}
	}

	sm_media.counter++;
	sm_media.linenum = EM_getLineNum(sm_pcounter->stm->pos);
	//每次执行之前将返回设置为NOACT， 只有遇到运动控制模块时，调用函数将其设置为相应的动作
	sm_media.kind = M_NOACT;

	I_interpStm(sysenv, sm_pcounter->stm);
	return &sm_media;
}

// N: I_interpStm_N  
// D: 在正常模式下解释程序（主要用在函数调用阶段）
// D: N(normal）SM( signal mode 单句解释模式)
static void I_interpStm_N(interpEnv sysenv, A_stm stm)
{
	if (sysenv->stackdeep != sdeep) return;													// TODO: 函数参数返回 (TEXT)

	switch (stm->kind){
	case A_expStm: I_interpExp(sysenv, stm->u.expStm, sysenv->etx); break;					// DONE
	case A_whileStm: I_interpWhileStm_N(sysenv, stm->u.whileStm, sysenv->etx); break;		// DONE
	case A_breakStm: break;																	// DONE interp in whileStm
	case A_fundecStm: I_interpFundecStm(sysenv, stm->u.fundStm); break;						// DONE
	case A_threaddecStm:break;																// TODO: -多线程
	case A_ifStm: I_interpIfStm_N(sysenv, stm->u.ifStm, sysenv->etx); break;				// DONE
	case A_arraydefStm: I_interpArrDefStm(sysenv, stm->u.arraydef, sysenv->etx); break;		// DONE
	case A_returnStm: I_interpReturn(sysenv, stm->u.reStm); break;							// TODO: 函数参数返回 (TEXT)
	case A_globalStm: I_interpGloVal(sysenv, stm->u.gid); break;				// DONE
	default: break;
	}
}

static void I_interpGloVal(interpEnv sysenv, S_symbol var)
{
	sbinder sb = NULL;
	if (sb = Stack_getSbinder(sysenv->global_sta, var, 0)){
		Stack_pushValR(sysenv->local_sta, var, sb, sysenv->stackdeep);
	} else { EM_errorE(0, RSIERR_FUNCALL_DLOREF_ERR_INFO); }

}

static void I_interpReturn(interpEnv sysenv, A_exp exp)										// TODO: 函数参数返回 (TEXT)
{
	sdeep--;
	I_interpExp(sysenv, exp, &funreval);
}

static void I_interpStm(interpEnv sysenv, A_stm stm)
{
	A_stmList tpcounter = NULL;
	switch (stm->kind){
	case A_expStm:																			// DONE
		I_interpExp(sysenv, stm->u.expStm, sysenv->etx);
		sm_pcounter = sm_pcounter->tail;
		break;
	case A_whileStm: I_interpWhileStm(sysenv, stm->u.whileStm, sysenv->etx); break;			// DONE
	case A_breakStm: I_interpBreakStm(sysenv); break;										// DONE
	case A_fundecStm: 
		I_interpFundecStm(sysenv, stm->u.fundStm);											// DONE
		sm_pcounter = sm_pcounter->tail;
		break;						
	case A_threaddecStm: break;																// TODO: -多线程
	case A_ifStm: I_interpIfStm(sysenv, stm->u.ifStm, sysenv->etx); break;					// DONE
	case A_arraydefStm:																		// DONE
		I_interpArrDefStm(sysenv, stm->u.arraydef, sysenv->etx); 
		sm_pcounter = sm_pcounter->tail;
		break;		
	case A_globalStm: case A_returnStm: break;	// 函数调用表达式内布解析					// DONE
	default: break;
	}
}

static void I_interpIfStm_N(interpEnv sysenv, A_if ifstm, E_val reval)
{
	A_exp exp = NULL; A_stmList stml = NULL; A_elifList eliflist = NULL; A_stmList stml_el = NULL;
	A_stm stm = NULL;
	A_elifExp elif = NULL;
	switch (ifstm->kind){
	case A_noelseIfStm:
		exp = ifstm->u.noelse.exp;	stml = ifstm->u.noelse.stmlist;	eliflist = ifstm->u.noelse.eliflist;
		break;
	case A_elseIfStm:
		exp = ifstm->u.elsee.exp; stml = ifstm->u.elsee.stmlist; eliflist = ifstm->u.elsee.eliflist;
		stml_el = ifstm->u.elsee.elsestmlist;
		break;
	default: break;
	}

	I_interpExp(sysenv, exp, reval);							// if
	if (reval->kind != E_INT && reval->kind != E_CHAR && reval->kind != E_DOU) 	EM_errorE(0, RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO);

	if (E_TF_Val(reval)){
		while (stml){
			I_interpStm_N(sysenv, stml->stm);
			stml = stml->tail;
		}
		goto OUTIF;
	}

	if (eliflist){												// else if
		while (eliflist){
			I_interpExp(sysenv, eliflist->head->test, reval);
			if (reval->kind != E_INT && reval->kind != E_CHAR && reval->kind != E_DOU) 	EM_errorE(0, RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO);
			if (E_TF_Val(reval)){
				stml = eliflist->head->body;
				while (stml){
					I_interpStm_N(sysenv, stml->stm);
					stml = stml->tail;
				}
				goto OUTIF;
			} else{
				eliflist = eliflist->tail;
			}
		}
	}

	while (stml_el){											// else
		stm = stml_el->stm;
		I_interpStm_N(sysenv, stm);
		stml_el = stml_el->tail;
	}
OUTIF:
	return;
}

static void I_interpIfStm(interpEnv sysenv, A_if ifstm, E_val reval)
{
	A_exp exp = NULL; A_stmList stml = NULL; A_elifList eliflist = NULL; A_stmList stml_el = NULL;

	switch (ifstm->kind){
	case A_noelseIfStm:
		exp = ifstm->u.noelse.exp;	stml = ifstm->u.noelse.stmlist;	eliflist = ifstm->u.noelse.eliflist;
		break;
	case A_elseIfStm:
		exp = ifstm->u.elsee.exp; stml = ifstm->u.elsee.stmlist; eliflist = ifstm->u.elsee.eliflist;
		stml_el = ifstm->u.elsee.elsestmlist;
		break;
	default: break;
	}

	T_staPush(sm_statool, sm_pcounter->tail);

	I_interpExp(sysenv, exp, reval);							// if
	if (reval->kind != E_INT && reval->kind != E_CHAR && reval->kind != E_DOU) 	EM_errorE(0, RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO);
						
	if (E_TF_Val(reval)){
		sm_pcounter = stml;
		goto OUTIF;
	}

	if (eliflist){												// else if
		while (eliflist){
			I_interpExp(sysenv, eliflist->head->test, reval);
			if (reval->kind != E_INT && reval->kind != E_CHAR && reval->kind != E_DOU) 	EM_errorE(0, RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO);
			if (E_TF_Val(reval)){
				stml = eliflist->head->body;
				sm_pcounter = stml;
				goto OUTIF;
			} else{
				eliflist = eliflist->tail;
			}
		}
	}
	sm_pcounter = stml_el;										// else
OUTIF:
	return;
}

static void I_interpWhileStm_N(interpEnv sysenv, A_while wstm, E_val reval)			// DONE
{
	A_stmList stml = wstm->stml;
	A_stm stm = NULL;
	I_interpExp(sysenv, wstm->cond, reval);
	if (reval->kind != E_INT && reval->kind != E_CHAR && reval->kind != E_DOU) 	EM_errorE(0, RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO);
	
	while (E_TF_Val(reval)){
		stml = wstm->stml;
		while (stml){
			stm = stml->stm;
			if (stm->kind == A_breakStm)
				goto OUTWHILE;
			else{
				I_interpStm_N(sysenv, stm);
			}
			stml = stml->tail;
		}
		if (sysenv->stackdeep != sdeep) break;										// TODO: 函数参数返回 (TEXT)
		I_interpExp(sysenv, wstm->cond, reval);
	}

OUTWHILE:
	return;
}

static void I_interpWhileStm(interpEnv sysenv, A_while wstm, E_val reval)			// DONE
{
	A_stmList stml = wstm->stml;
	I_interpExp(sysenv, wstm->cond, reval);

	if (reval->kind != E_INT && reval->kind != E_CHAR && reval->kind != E_DOU) 	EM_errorE(0, RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO);

	if (E_TF_Val(reval)){
		T_staPush(sm_statool, sm_pcounter);
		sm_pcounter = stml;
	} else{
		sm_pcounter = sm_pcounter->tail;
	}
}

static void I_interpBreakStm(interpEnv sysenv)										// DONE
{
	sm_pcounter = T_staPop(sm_statool);
	if (sm_pcounter->stm->kind == A_whileStm){
		sm_pcounter = sm_pcounter->tail;
	} else{
		T_staPush(sm_statool, sm_pcounter);
	}
}

// N: _searchParams
// D: 查看当前参数名是否在参数列表中，如果在返回TRUE, 反之返回FALSE
static int _searchParams(E_param ps, S_symbol name)
{
	while (ps){
		if (ps->name == name){
			return TRUE;
		}
		ps = ps->next;
	}
	return FALSE;
}

static __inline int _getCnumlistCount(A_cnumlist cnl){
	int count = 0;
	while (cnl){
		count++;
		cnl = cnl->tail;
	}
	return count;
}

static E_param _makeParams(interpEnv sysenv, A_typeFields params)
{
	S_table typetab = sysenv->ty_tab;
	A_fieldl fl = params->fl;									// 不带默认参数的形参列表
	A_fielddl fdl = params->fdl;								// 带默认参数的形参列表
	E_param ps = NULL, tps = NULL;
	S_symbol name = NULL;
	datatype daty = NULL;
	A_exp defval = NULL;

	// 链表用尾插法
	while (fl){
		// 检查：类型标识符是否合法
		if ( !(daty = S_look(typetab, fl->head->typ)) ) EM_errorE(0, RSIERR_PRO_UNKNOWN_TY_INFO);
		// 检查：形参是否重复定义
		name = fl->head->name;
		if (_searchParams(ps, name)) EM_errorE(0, RSIERR_PRO_PA_REDEFINE_INFO);

		if (ps == NULL){
			ps = E_Param(fl->head->name, daty, NULL, NULL);
			tps = ps;
		} else{
			tps->next = E_Param(fl->head->name, daty, NULL, NULL);
			tps = tps->next;
		}
		fl = fl->tail;
	}

	while (fdl){
		// 检查: 默认参数类型合法性
		if (!(daty = S_look(typetab, fdl->head->typ))) EM_errorE(0, RSIERR_PRO_UNKNOWN_TY_INFO);
		// 检查：形参是否重复定义
		name = fdl->head->name;
		if (_searchParams(ps, name)) EM_errorE(0, RSIERR_PRO_PA_REDEFINE_INFO);

		defval = fdl->head->exp;

		switch (daty->kind){
		case T_PSTR: 
			if (defval->kind != A_stringExp) EM_errorE(0, RSIERR_PRO_PA_DEFAULTPA_ERR_INFO);
			break;
		case T_PARRAY: 	case T_PPOSE:
			if (defval->kind != A_clistExp) EM_errorE(0, RSIERR_PRO_PA_DEFAULTPA_ERR_INFO);
			break;
		case T_INT: case T_DOU:
			if (defval->kind != A_cnumExp) EM_errorE(0, RSIERR_PRO_PA_DEFAULTPA_ERR_INFO);
			break;
		default: EM_errorE(0, RSIERR_PRO_PA_TY_ERR_INFO); break;
		}

		if (ps == NULL){
			ps = E_Param(fdl->head->name, daty, defval, NULL);
			tps = ps;
		} else{
			tps->next = E_Param(fdl->head->name, daty, defval, NULL);
			tps = tps->next;
		}
		fdl = fdl->tail;
	}
	return ps;
}

static void I_interpFundecStm(interpEnv sysenv, A_funDec fundec)
{
	S_symbol name = fundec->name;
	A_stmList body = fundec->body;
	E_param param = NULL;

	// 合法性判断 -- 函数不允许嵌套定义
	if (sysenv->stackdeep != 0)	EM_errorE(0, RSIERR_PRO_FUNDEF_NESTED_INFO);
	if (S_look(sysenv->ufunc_tab, name) != NULL) EM_errorE(0, RSIERR_PRO_FUN_REDEFINED_INFO);
	if (S_look(sysenv->sfunc_tab, name) != NULL) EM_errorE(0, RSIERR_PRO_FUN_CONFLICT_SYSF_INFO);

	param = _makeParams(sysenv, fundec->params);
	S_enter(sysenv->ufunc_tab, name, E_FunEnventry(param, body));
}

static void I_interpExp(interpEnv sysenv, A_exp exp, E_val reval)
{
	switch (exp->kind){
	case A_opExp: I_interpOp(sysenv, exp->u.ope, reval); break;						// DONE
	case A_notExp: I_interNotOp(sysenv, exp, reval); break;							// DONE
	case A_funcallExp: I_interpFuncCall(sysenv, exp->u.funcall, reval);  break;		// DONE
	case A_threadcallExp: break;													// TODO: - 多线程
	case A_parentExp: break;														// DONE
	case A_variateExp: I_interpVariate(sysenv, exp->u.var, reval); break;			// DONE
	case A_assignExp: I_interpAssign(sysenv, exp->u.assign, reval);	break;			// DONE
	case A_cnumExp: I_interpNum(sysenv, exp->u.cnum, reval); break;					// DONE
	case A_clistExp:  E_setCNumList(reval, (void*)exp->u.clist); break;				// DONE
	case A_stringExp: E_setCStringVal(reval, (void*)exp->u.str->str); break;		// DONE
	case A_pose: E_setCPoseVal(reval); break;										// DONE
	default: break;																	// DONE
	}
}

static void _pushParams(interpEnv sysenv, E_param params, A_args args, E_val reval)
{
	datatype paraty = NULL; A_exp exp = NULL;  sbinder sb = NULL; stack sta = NULL;

	while (params){
		paraty = params->ty;
		if (args){
			exp = args->head;
			args = args->tail;
		} else if (params->defval){
			exp = params->defval;
		} else{
			EM_errorE(0, RSIERR_FUN_LACKPARAM_INFO);
		}

		sysenv->stackdeep--; sdeep--;
		I_interpExp(sysenv, exp, reval);
		sysenv->stackdeep++; sdeep++;

		switch (paraty->kind)
		{
		case T_INT:
			if (reval->kind == E_INT || reval->kind == E_DOU || reval->kind == E_CHAR)
				Stack_pushIntVal(sysenv->local_sta, params->name, E_getIntVal(reval), sysenv->stackdeep);
			else EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
			break;
		case T_DOU:
			if (reval->kind == E_INT || reval->kind == E_DOU || reval->kind == E_CHAR)
				Stack_pushDouVal(sysenv->local_sta, params->name, E_getDouVal(reval), sysenv->stackdeep);
			else EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
			break;
		case T_PARRAY:
			if (reval->kind == E_CNUMLISTP){
				Stack_pushDouArrCNListVal(sysenv->local_sta, params->name, reval->u.p, _getCnumlistCount((A_cnumlist)reval->u.p), sysenv->stackdeep);
			} else if (reval->kind == E_ARRAYP){
				sb = reval->u.p;
				if ((sb->type)->kind == T_INTARRAY || (sb->type)->kind == T_RINTARRAY ||
					(sb->type)->kind == T_DOUARRAY || (sb->type)->kind == T_RDOUARRAY){
					Stack_pushValR(sysenv->local_sta, params->name, sb, sysenv->stackdeep);
				} else if ((sb->type)->kind == T_POSE || ((sb->type))->kind == T_RPOSE)
					Stack_pushDouArrayR(sysenv->local_sta, params->name, sb->addr, POSECOUNT, sysenv->stackdeep);
				else
					EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
			} else
				EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
			break;
		case T_PPOSE:
			if (reval->kind == E_CNUMLISTP){
				Stack_pushPoseCNListVal(sysenv->local_sta, params->name, reval->u.p, sysenv->stackdeep);
			} else if (reval->kind == E_ARRAYP){
				sb = reval->u.p;
				if ((sb->type)->kind == T_POSE || ((sb->type))->kind == T_RPOSE)
					Stack_pushValR(sysenv->local_sta, params->name, sb, sysenv->stackdeep);
				else if ((sb->type)->kind == T_DOUARRAY || (sb->type)->kind == T_RDOUARRAY){
					if (sb->type->count >= POSECOUNT)
						Stack_pushPoseR(sysenv->local_sta, params->name, sb->addr, sysenv->stackdeep);
					else EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
				}
				else
					EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
			} else
				EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
			break;
		case T_PSTR:
			if (reval->kind == E_PSTRING){
				Stack_pushStringVal(sysenv->local_sta, params->name, reval->u.p, sysenv->stackdeep);
			} else if (reval->kind == E_ARRAYP){
				sb = reval->u.p;
				if ((sb->type)->kind == T_STRING || (sb->type)->kind == T_RSTRING)
					Stack_pushValR(sysenv->local_sta, params->name, sb, sysenv->stackdeep);
				else{
					EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
				}
			} else
				EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO);
			break;
		default: break;
		}
		params = params->next;
	}
}

static void _popParams(stack sta, int stackdeep)
{
	Stack_popOneLeval(sta, stackdeep);
}


static void I_interpFuncCall(interpEnv sysenv, A_funcall funcall, E_val reval)
{
	S_symbol funcname = funcall->func;
	A_args args = funcall->args;
	E_funEnventry funenv = NULL;
	E_param params = NULL;
	A_stmList body = NULL;
	A_stm stm = NULL;
	sbinder sb = NULL;

	int returnsign = 0;
	int sysfuncsign = 0;	// 区分是系统函数还是用户自定义函数

	if ((funenv = S_look(sysenv->sfunc_tab, funcname)) != NULL){				// 系统函数调用
		params = funenv->params;
		sysfuncsign = 1;
	} else if ((funenv = S_look(sysenv->ufunc_tab, funcname)) != NULL){			// 用户函数调用
		params = funenv->params;
		body = funenv->funcbody;
	} else{
		EM_errorE(0, RSIERR_FUNCALL_NOTDEF_INFO);
	}

	sysenv->stackdeep++; sdeep++;

	_pushParams(sysenv, params, args, reval);									// 参数压栈

	if (sysfuncsign == 1){														// 执行函数体
		I_interpSysFuncCall(sysenv, funcname, reval, params);
	} else{
		/*while (body){
			stm = body->stm;
			if (stm->kind == A_returnStm){
				I_interpExp(sysenv, stm->u.reStm, reval);
				returnsign = 1;
				break;
			} else if (stm->kind == A_globalStm){
				sb = Stack_getSbinder(sysenv->global_sta, stm->u.gid, 0);
				if (sb){
					Stack_pushValR(sysenv->local_sta, stm->u.gid, sb, sysenv->stackdeep);
				} else {
					EM_errorE(0, RSIERR_FUNCALL_DLOREF_ERR_INFO);
				}
			} else{
				I_interpStm_N(sysenv, stm);
			}
			body = body->tail;
		}
		if (returnsign == 0){
			reval->kind = E_NIL;
		}*/
		while (body){
			stm = body->stm;
			I_interpStm_N(sysenv, stm);
			body = body->tail;
		}
		if (sysenv->stackdeep != sdeep){
			E_ValCopy(reval, &funreval);
			sdeep++;
		} else{
			reval->kind = E_NIL;
		}
	}
	_popParams(sysenv->local_sta, sysenv->stackdeep);
	
	sysenv->stackdeep--; sdeep--;														// 返回上一级的调用深度， 调用深度减一
}



static void I_interpArrDefStm(interpEnv sysenv, A_arraydef arraydef, E_val reval)
{
	S_symbol var = NULL; sbinder sb = NULL; stack nowsta = NULL;
	A_arraytool arrt = arraydef->array;

	if (arraydef->var->kind != A_simpleVar) EM_errorE(0, RSIERR_PRO_ARRAYDEF_ERR_INFO);

	var = arraydef->var->u.simple;
	nowsta = sysenv->stackdeep ? sysenv->local_sta : sysenv->global_sta;

	if (Stack_getSbinder(nowsta, var, sysenv->stackdeep)) EM_errorE(0, RSIERR_PRO_ARRAY_RED_INFO);

	switch (arrt->kind){
	case A_iarray:
		if (arrt->u.array.list == NULL) Stack_pushIntArrayVal(nowsta, var, NULL, arrt->u.array.size, sysenv->stackdeep);
		else Stack_pushIntArrCNListVal(nowsta, var, arrt->u.array.list, arrt->u.array.size, sysenv->stackdeep);
		break;
	case A_farray:
		if (arrt->u.array.list == NULL) Stack_pushDouArrayVal(nowsta, var, NULL, arrt->u.array.size, sysenv->stackdeep);
		else Stack_pushDouArrCNListVal(nowsta, var, arrt->u.array.list, arrt->u.array.size, sysenv->stackdeep);
		break;
	case A_carray:
		Stack_pushStrArrayVal(nowsta, var, NULL, arrt->u.array.size, sysenv->stackdeep); break;
	case A_arraycopy:
		sb = sysenv->stackdeep ? Stack_getSbinder(sysenv->local_sta, arrt->u.id, sysenv->stackdeep) :
								Stack_getSbinder(sysenv->global_sta, arrt->u.id, sysenv->stackdeep);
		if (!sb) EM_errorE(0, RSIERR_PRO_ASSIGN_FAILED_INFO);
		Stack_pushSbinderVal(nowsta, var, sb, sysenv->stackdeep);
		break;
	default:
		break;
	}
}

static void I_interpSimpVar(interpEnv sysenv, S_symbol name, E_val reval)
{
	sbinder sb = NULL; datatype dt = NULL;

	if (!(sb = sysenv->stackdeep ? Stack_getSbinder(sysenv->local_sta, name, sysenv->stackdeep) :
		Stack_getSbinder(sysenv->global_sta, name, sysenv->stackdeep))){
		EM_errorE(0, RSIERR_PRO_VAR_DEFERR_INFO);
	}
	dt = sb->type;

	switch (dt->kind){
	case T_INT: E_setIntVal(reval, *(int*)(sb->addr)); break;
	case T_DOU: E_setDouVal(reval, *(double*)(sb->addr)); break;
	case T_CHAR: E_setCharVal(reval, *(char*)(sb->addr)); break;
	case T_INTARRAY: case T_RINTARRAY: case T_DOUARRAY: case T_RDOUARRAY: 
	case T_STRING: case T_RSTRING: case T_POSE: case T_RPOSE:
		E_setArraypVal(reval, (void*)sb);
	reval->kind = E_ARRAYP; reval->u.p = sb; break;
	default: EM_errorE(0, RSIERR_UNKNOWN_EXCEPTION_INFO);  break;
	}
}

static void I_interpSubVar(interpEnv sysenv, S_symbol name, A_exp sub, E_val reval)  // DONE
{
	sbinder sb = NULL; datatype dt = NULL;
	int tykind = -1;
	E_val subv = IE_getNullReg(sysenv);

	I_interpExp(sysenv, sub, subv);
	if (subv->kind != E_INT) EM_errorE(0, RSIERR_PRO_SUB_TYPEERROR_INFO);

	if (!(sb = sysenv->stackdeep ? Stack_getSbinder(sysenv->local_sta, name, sysenv->stackdeep) :
		Stack_getSbinder(sysenv->global_sta, name, sysenv->stackdeep))){
		EM_errorE(0, RSIERR_PRO_VAR_DEFERR_INFO);
	}
	dt = sb->type; tykind = dt->kind;

	if (tykind == T_INT || tykind == T_DOU || tykind == T_CHAR || tykind == T_VOID || tykind == T_NIL ||
		tykind == T_PARRAY || tykind == T_PPOSE || tykind == T_PSTR)
		EM_errorE(0, RSIERR_PRO_TYPE_OPNOTSUPPORT_INFO);
	if (E_getIntVal(subv) > (dt->count - 1))
		EM_errorE(0, RSIERR_PRO_SUB_OVERFLOW_INFO);

	switch (tykind){
	case T_INTARRAY: case T_RINTARRAY: E_setIntVal(reval, ((int*)sb->addr)[E_getIntVal(subv)]); break;
	case T_STRING:	case T_RSTRING: E_setCharVal(reval, ((char*)sb->addr)[E_getIntVal(subv)]); break;
	case T_DOUARRAY: case T_RDOUARRAY: case T_POSE: case T_RPOSE:
		E_setDouVal(reval, ((double*)sb->addr)[E_getIntVal(subv)]); break;
	default: break;
	}
	IE_freeReg(sysenv, subv);
}

static void I_interpVariate(interpEnv sysenv, A_variate variate, E_val reval)		// DONE
{
	switch (variate->kind){
	case A_simpleVar: I_interpSimpVar(sysenv, variate->u.simple, reval); break;
	case A_subscriptVar: I_interpSubVar(sysenv, variate->u.subscript.var, variate->u.subscript.e, reval); break;
	default: EM_errorE(0, RSIERR_UNKNOWN_EXCEPTION_INFO);
	}
}

static void I_interpAssign(interpEnv sysenv, A_assign asexp, E_val reval)
{
	sbinder sbfun = NULL; E_val rx = NULL; int err = TRUE; S_symbol name = NULL; stack sta = NULL;

	name = asexp->var->kind == A_simpleVar ? asexp->var->u.simple : asexp->var->u.subscript.var;
	sta = sysenv->stackdeep ? sysenv->local_sta : sysenv->global_sta;
	sbfun = Stack_getSbinder(sta, name, sysenv->stackdeep);

	// 1. 解释右值,暂时保存在临时寄存器rx中
	I_interpExp(sysenv, asexp->exp, reval);

	// 2. 分析左值
	if (sbfun){																	// 变量赋值
		if (asexp->var->kind == A_simpleVar){									// 如果左值是一个简单变量
			switch (reval->kind)	
			{
			case E_CHAR: err = Stack_setSbinderCharVal(sbfun, reval->u.c); break;
			case E_INT: err = Stack_setSbinderIntVal(sbfun, reval->u.i); break;
			case E_DOU: err = Stack_setSbinderDouVal(sbfun, reval->u.d); break;
			case E_ARRAYP: err = Stack_setSbinderSbinderVal(sbfun, (sbinder)reval->u.p); break;
			case E_CNUMLISTP: err = Stack_setSbinderCNumListVal(sbfun, (A_cnumlist)reval->u.p); break;
			case E_PSTRING: err = Stack_setSbinderCStringVal(sbfun, ((A_string)reval->u.p)->str, ((A_string)reval->u.p)->len - 1); break;
			case E_CPOSE: err = Stack_setSbinderPoseZero(sbfun); break;
			default: err = FALSE; break;
			}
		} else if (asexp->var->kind == A_subscriptVar ){						// 如果左值是一个带有下标的变量
			rx = IE_getNullReg(sysenv);
			I_interpExp(sysenv, asexp->var->u.subscript.e, rx);
			if (rx->kind != E_INT) EM_errorE(0, RSIERR_PRO_SUB_TYPEERROR_INFO);

			switch (reval->kind){
			case E_CHAR: err = Stack_setArraySbinderCharVal(sbfun, reval->u.c, rx->u.i); break;
			case E_INT: err = Stack_setArraySbinderIntVal(sbfun, reval->u.i, rx->u.i); break;
			case E_DOU: err = Stack_setArraySbinderDouVal(sbfun, reval->u.d, rx->u.i); break;
			default: err = FALSE; break;
			}
			IE_freeReg(sysenv, rx);
		}
		if (FALSE == err) EM_errorE(0, RSIERR_PRO_ASSIGN_FAILED_INFO);
	} else{																		// 变量定义
		if (asexp->var->kind == A_subscriptVar) EM_errorE(0, RSIERR_PRO_VAR_UNDEF_INFO);

		switch (reval->kind) {
		case E_CHAR:Stack_pushCharVal(sta, name, reval->u.c, sysenv->stackdeep); break;
		case E_INT: Stack_pushIntVal(sta, name, reval->u.i, sysenv->stackdeep); break;
		case E_DOU: Stack_pushDouVal(sta, name, reval->u.d, sysenv->stackdeep); break;
		case E_CPOSE: Stack_pushPoseZeroVal(sta, name, sysenv->stackdeep); break;
		case E_CNUMLISTP: Stack_pushPoseCNListVal(sta, name, (A_cnumlist)reval->u.p, sysenv->stackdeep); break;
		case E_PSTRING: Stack_pushStringVal(sta, name, reval->u.p, sysenv->stackdeep); break;
		default: EM_errorE(0, RSIERR_PRO_VAR_DEFERR_INFO); break;
		}
		reval->kind = E_NIL;													// 变量定义表达式返回E_NIL类型， 不支持任何运算
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
//the module below helps to interp : 1. Operater: + - * / and or xor == >= > <= <
//                                   2. const number
///////////////////////////////////////////////////////////////////////////////////////////

static void I_interpNum(interpEnv sysenv, A_cnum cnum, E_val reval) //DNOE
{
	switch (cnum->kind){
	case A_int:	E_setIntVal(reval, (int)cnum->d); break;
	case A_double: E_setDouVal(reval, cnum->d); break;
	case A_char:  E_setCharVal(reval, (char)cnum->d); break;
	}
	return;
}

static void I_interNotOp(interpEnv sysenv, A_exp exp, E_val reval) //DONE
{
	I_interpExp(sysenv, exp->u.notexp, reval);
	E_reverseVal(reval);
	return;
}

static void I_interpOp(interpEnv sysenv, A_op ope, E_val reval) //DONE
{
	E_val rx = NULL;
	E_val lx = reval;

	// 二元操作解释顺序不可以更改
	I_interpExp(sysenv, ope->left, lx);
	rx = IE_getNullReg(sysenv);
	I_interpExp(sysenv, ope->right, rx);

	switch (ope->oper){
	case A_plusOp: OpPlus(lx, rx, reval); break;
	case A_minusOp: OpMinus(lx, rx, reval); break;
	case A_timesOp: OpTimes(lx, rx, reval); break;
	case A_divideOp: OpDiv(lx, rx, reval); break;
	case A_eqOp: Opeq(lx, rx, reval);  break;
	case A_neqOp: Opneq(lx, rx, reval); break;
	case A_ltOp: Oplt(lx, rx, reval);  break;
	case A_leOp: Ople(lx, rx, reval); break;
	case A_gtOp: Opgt(lx, rx, reval); break;
	case A_geOp: Opge(lx, rx, reval); break;
	case A_andOp: Opand(lx, rx, reval); break;
	case A_orOp: Opor(lx, rx, reval); break;
	case A_xorOp: Opxor(lx, rx, reval); break;
	default: break;
	}
	IE_freeReg(sysenv, rx);
}

static void Opeq(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: re = (E_getIntVal(elx) == E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) == E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) == E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
	E_setIntVal(reval, re);
}

static void Opneq(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: re = (E_getIntVal(elx) != E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) != E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) != E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
	E_setIntVal(reval, re);
}

static void Oplt(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: re = (E_getIntVal(elx) < E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) < E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) < E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
	E_setIntVal(reval, re);
}

static void Ople(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (reval->kind){
	case E_INT: re = (E_getIntVal(elx) <= E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) <= E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) <= E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO);
	}
	E_setIntVal(reval, re);
}

static void Opgt(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: re = (E_getIntVal(elx) > E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) > E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) > E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
	E_setIntVal(reval, re);
}

static void Opge(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: re = (E_getIntVal(elx) >= E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) >= E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) >= E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
	E_setIntVal(reval, re);
}

static void Opand(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: re = (E_getIntVal(elx) && E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) && E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) && E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
	E_setIntVal(reval, re);
}

static void Opor(E_val elx, E_val erx, E_val reval) //DONE
{
	int re = 0, kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: re = (E_getIntVal(elx) || E_getIntVal(erx)); break;
	case E_CHAR: re = (E_getCharVal(elx) || E_getCharVal(erx)); break;
	case E_DOU: re = (E_getDouVal(elx) || E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
	E_setIntVal(reval, re);
}

static void Opxor(E_val elx, E_val erx, E_val reval)  //DONE
{
	EM_errorE(0, RSIERR_PRO_OP_XOR_NOTSUPPORT_INFO);
}

static void OpPlus(E_val elx, E_val erx, E_val reval)  // DONE
{
	//erx 和 reval指向相同的寄存器
	int kind = -1;
	kind = MAX(elx->kind, erx->kind);	//对寄存器中的值进行类型提升
	switch (kind){
	case E_INT: E_setIntVal(reval, E_getIntVal(elx) + E_getIntVal(erx)); break;
	case E_CHAR: E_setCharVal(reval, E_getCharVal(elx) + E_getCharVal(erx)); break;
	case E_DOU: E_setDouVal(reval, E_getDouVal(elx) + E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
}

static void OpMinus(E_val elx, E_val erx, E_val reval) //DONE
{
	int kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: E_setIntVal(reval, E_getIntVal(elx) - E_getIntVal(erx)); break;
	case E_CHAR: E_setIntVal(reval, E_getCharVal(elx) - E_getCharVal(erx)); break;
	case E_DOU: E_setDouVal(reval, E_getDouVal(elx) - E_getDouVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
}

static void OpTimes(E_val elx, E_val erx, E_val reval) //DONE
{
	int kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT: E_setIntVal(reval, E_getIntVal(elx) * E_getIntVal(erx)); break;
	case E_DOU: E_setDouVal(reval, E_getDouVal(elx) * E_getDouVal(erx)); break;
	case E_CHAR: E_setIntVal(reval, E_getCharVal(elx) * E_getCharVal(erx)); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
}

static void OpDiv(E_val elx, E_val erx, E_val reval) //DONE
{
	// 除法需要特殊处理，用来保证除数不为0
	int ir = 0; double dr = 0; char cr = 0; int kind = -1;
	kind = MAX(elx->kind, erx->kind);
	switch (kind){
	case E_INT:
		ir = E_getIntVal(erx);
		if (0 == ir) EM_errorE(0, RSIERR_PRO_DIVZERO_INFO);
		E_setIntVal(reval, E_getIntVal(elx) / ir); break;
	case E_DOU:
		dr = E_getDouVal(erx);
		if (0 == dr) EM_errorE(0, RSIERR_PRO_DIVZERO_INFO);
		E_setDouVal(reval, E_getDouVal(elx) / dr); break;
	case E_CHAR:
		cr = E_getCharVal(erx);
		if (0 == cr) EM_errorE(0, RSIERR_PRO_DIVZERO_INFO);
		E_setIntVal(reval, E_getCharVal(elx) / cr); break;
	default: EM_errorE(0, RSIERR_PRO_TYPE_MISMATCH_INFO); break;
	}
}

#define I_CASE_S( fname, act )		if ( funcname == S_Symbol(fname) ) { act }
#define I_CASE_C( fname, act )		else if (  funcname == S_Symbol(fname) ){ act }
#define I_CASE_E( act )				else { act }

static void I_interpSysFuncCall(interpEnv sysenv, S_symbol funcname, E_val reval, E_param params)
{
	int ri = 0; double rb = 0; int i = 0;
	sbinder sb = NULL;
	while (params){
		sb = Stack_getSbinder(sysenv->local_sta, params->name, sysenv->stackdeep);
		if (sb->type->kind != T_INTARRAY && sb->type->kind != T_RINTARRAY &&
			sb->type->kind != T_DOUARRAY && sb->type->kind != T_RDOUARRAY &&
			sb->type->kind != T_STRING && sb->type->kind != T_RSTRING)
			arr[i++] = sb->addr;
		else
			arr[i++] = sb;
		params = params->next;
	}
	
	E_setNilVal(reval);
	
{
	I_CASE_S("Start",
		rob_start((double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
			(double *)arr[6], (double *)arr[7], (double *)arr[15]);)
	I_CASE_C("socket_open",
		E_setIntVal(reval, rob_socket_open(((sbinder)arr[0])->addr, (int *)arr[1]));)
	I_CASE_C("socket_send_string", 
		E_setIntVal(reval, rob_socket_send_string(((sbinder)arr[0])->addr, ((sbinder)arr[0])->type->count) );)
	I_CASE_C("socket_read_string",
		E_setIntVal(reval, rob_socket_read_string( ((sbinder)arr[0])->addr, ((sbinder)arr[0])->type->count, *(int *)arr[1]) ); )
	I_CASE_C("socket_get_var", ;)
	I_CASE_C("socket_close", rob_socket_close();)            
	I_CASE_C("MoveJ", 
		rob_movej((double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
					(double *)arr[6], (double *)arr[7], (double *)arr[15]);)
	I_CASE_C("MoveC_1",
		rob_movec1((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5], 
					(double *)arr[6], (double *)arr[7], (double *)arr[15]);)
	I_CASE_C("MoveC_2", 
		rob_movec2((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5],
					(double *)arr[6], (double *)arr[7],
					(double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
					(double *)arr[15]);)
	I_CASE_C("MoveL", 
		rob_movel((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5],
				(double *)arr[6], (double *)arr[7],
				(double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
				(double *)arr[15]);)
	I_CASE_C("biaoding", 
		rob_biaoding((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5],
				(double *)arr[6], (double *)arr[7], (double *)arr[15]);)
	I_CASE_C("MovePsi", 
		rob_movepsi((double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
				(double *)arr[6], (double *)arr[7], (double *)arr[15]);)
    I_CASE_C("Sleep", rob_sleep((int *)arr[0]);)
    I_CASE_C("sleep", rob_sleep((int *)arr[0]);)
    I_CASE_C("gripper", rob_gripper((int *)arr[0]);)
	I_CASE_C("io_out", rob_io_out((int *)arr[0], (int *)arr[1]);)
	I_CASE_C("io_in", 
		ri = rob_io_in((int *)arr[0]);
		E_setIntVal(reval, ri);)
	I_CASE_C("tool_out", tool_io_out((int *)arr[0], (int *)arr[1]);)
	I_CASE_C("tool_in",
			ri = tool_io_in((int *)arr[0]);
			E_setIntVal(reval, ri);)
	I_CASE_C("start", 
		if (((sbinder)arr[0])->type->count < 7) { EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO); }			// axis数组传入参数检查， 必须保证大于 7
		if (((sbinder)arr[0])->type->kind == T_INTARRAY ||
			((sbinder)arr[0])->type->kind == T_RINTARRAY)
			rob_start_new_i(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
		else if (((sbinder)arr[0])->type->kind == T_DOUARRAY ||
			((sbinder)arr[0])->type->kind == T_RDOUARRAY)
			rob_start_new_d(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);)
	I_CASE_C("movel", 
		rob_movel_new((double *)arr[0], (double*)arr[1], (double*)arr[2], (double*)arr[3], (double*)arr[4]);)
    I_CASE_C("movej_pose",
        rob_movej_pose((double *)arr[0], (double*)arr[1], (double*)arr[2]);)
    I_CASE_C("movej",
		if (((sbinder)arr[0])->type->count < 7) { EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO); }			// axis数组传入参数检查， 必须保证大于 7
		if (((sbinder)arr[0])->type->kind == T_INTARRAY || ((sbinder)arr[0])->type->kind == T_RINTARRAY)
			rob_movej_new_i(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
		else if (((sbinder)arr[0])->type->kind == T_DOUARRAY || ((sbinder)arr[0])->type->kind == T_RDOUARRAY)
			rob_movej_new_d(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);)
	I_CASE_C("movec", 
		rob_movec_new((double *)arr[0], (double*)arr[1], (double*)arr[2], (double*)arr[3], (double*)arr[4], (double*)arr[5]);)
	I_CASE_C("movepsi", 
		if (((sbinder)arr[0])->type->count < 7) { EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO); }			// axis数组传入参数检查， 必须保证大于 7
		if (((sbinder)arr[0])->type->kind == T_INTARRAY || ((sbinder)arr[0])->type->kind == T_RINTARRAY)
			rob_movepsi_new_i(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
		else if (((sbinder)arr[0])->type->kind == T_DOUARRAY || ((sbinder)arr[0])->type->kind == T_RDOUARRAY)
			rob_movepsi_new_d(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);)
    //I_CASE_C("run_prog", rob_runprog(((sbinder)arr[0])->addr, ((sbinder)arr[0])->type->count);)
    I_CASE_C("moveCalib", moveCalib((int *)arr[0]);)
    I_CASE_C("set_tcp",  rob_set_tcp((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5]);)
    I_CASE_C("clean_tcp", rob_clean_tcp();)
    I_CASE_C("tcp_move",  move_tcp((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3]);)
    I_CASE_C("get_flange_pos", rob_get_flange_pos((double *)arr[0]);)
	I_CASE_C("get_joint_pos", 
		if (((sbinder)arr[0])->type->count < 7) { EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO); }			// axis数组传入参数检查， 必须保证大于 7
		if (((sbinder)arr[0])->type->kind == T_INTARRAY || ((sbinder)arr[0])->type->kind == T_RINTARRAY)
			rob_get_joint_pos_i(((sbinder)arr[0])->addr);
		else if (((sbinder)arr[0])->type->kind == T_DOUARRAY || ((sbinder)arr[0])->type->kind == T_RDOUARRAY)
			rob_get_joint_pos_d(((sbinder)arr[0])->addr);)
	I_CASE_C("calib_start", E_setIntVal(reval, rob_start_calib());)
	I_CASE_C("calib_finish", E_setIntVal(reval, rob_calib_finish());)
	I_CASE_C("robot_move_finished", rob_move_finished((int *)arr[0]);)
    I_CASE_C("robot_move_write", rob_move_write((int *)arr[0]);)
	I_CASE_C("robot_move", E_setIntVal(reval, rob_move());)
    I_CASE_C("calibAdjustPose",calibAdjustPose();)
    I_CASE_C("replay",replay(((int *)arr[0]),(int *)arr[1]);)
    I_CASE_C("calpos",
        if (((sbinder)arr[2])->type->count < 3) { EM_errorE(0, RSIERR_FUNCALL_ILLEGALP_INFO); }
        if (((sbinder)arr[2])->type->kind == T_INTARRAY || ((sbinder)arr[2])->type->kind == T_RINTARRAY)
                E_setIntVal(reval, rob_cam_CalPos_i((double *)arr[0], (double *)arr[1],((sbinder)arr[2])->addr,(int *)arr[3]));
        else if (((sbinder)arr[2])->type->kind == T_DOUARRAY || ((sbinder)arr[2])->type->kind == T_RDOUARRAY)
                E_setIntVal(reval, rob_cam_CalPos_d((double *)arr[0], (double *)arr[1],((sbinder)arr[2])->addr,(int *)arr[3]));)
	I_CASE_C("capture", 
		if (((sbinder)arr[2])->type->kind == T_INTARRAY || ((sbinder)arr[2])->type->kind == T_RINTARRAY)
			E_setIntVal(reval, rob_camera_capture_i(((sbinder)arr[0])->addr, *(int *)arr[1], ((sbinder)arr[2])->addr, ((sbinder)arr[2])->type->count, *(int *)arr[3], *(int *)arr[4]));
		else if (((sbinder)arr[2])->type->kind == T_DOUARRAY || ((sbinder)arr[2])->type->kind == T_RDOUARRAY)
			E_setIntVal(reval, rob_camera_capture_d(((sbinder)arr[0])->addr, *(int *)arr[1], ((sbinder)arr[2])->addr, ((sbinder)arr[2])->type->count, *(int *)arr[3], *(int *)arr[4]));)
}

	;
}





/*
if (funcname == S_Symbol("socket_open")){
//socket_open((char *)arr[0], (int*)arr[1]);
} else if (funcname == S_Symbol("socket_get_var")){
//ri = socket_get_var((char *)arr[0]);
//E_setIntVal(reval, ri);
} else if (funcname == S_Symbol("socket_close")){
socket_close();
} else if (funcname == S_Symbol("Start")){														// Start
rob_start((double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
(double *)arr[6], (double *)arr[7], (double *)arr[15]);
} else if (funcname == S_Symbol("MoveJ")){														// MoveJ
rob_movej((double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
(double *)arr[6], (double *)arr[7], (double *)arr[15]);
} else if (funcname == S_Symbol("MoveC_1")){													// MoveC_1
rob_movec1((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5],
(double *)arr[6], (double *)arr[7], (double *)arr[15]);
} else if (funcname == S_Symbol("MoveC_2")){													// MoveC_2
rob_movec2((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5],
(double *)arr[6], (double *)arr[7],
(double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
(double *)arr[15]);
} else if (funcname == S_Symbol("MoveL")){														// MoveL
rob_movel((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5],
(double *)arr[6], (double *)arr[7],
(double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
(double *)arr[15]);
} else if (funcname == S_Symbol("biaoding")){													// biaoding
rob_biaoding((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5],
(double *)arr[6], (double *)arr[7], (double *)arr[15]);
} else if (funcname == S_Symbol("MovePsi")){													// MovePsi
rob_movepsi((double *)arr[8], (double *)arr[9], (double *)arr[10], (double *)arr[11], (double *)arr[12], (double *)arr[13], (double *)arr[14],
(double *)arr[6], (double *)arr[7], (double *)arr[15]);
} else if (funcname == S_Symbol("Sleep")){														// Sleep
rob_sleep((int *)arr[0]);
} else if (funcname == S_Symbol("io_out")){														// io_out
rob_io_out((int *)arr[0], (int *)arr[1]);
} else if (funcname == S_Symbol("io_in")){														// io_in
ri = rob_io_in((int *)arr[0]);
E_setIntVal(reval, ri);
} else if (funcname == S_Symbol("start")){														// $start$
if (((sbinder)arr[0])->type->kind == T_INTARRAY ||
((sbinder)arr[0])->type->kind == T_RINTARRAY)
rob_start_new_i(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
else if (((sbinder)arr[0])->type->kind == T_DOUARRAY ||
((sbinder)arr[0])->type->kind == T_RDOUARRAY)
rob_start_new_d(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
// rob_start_new((double *)arr[0], (double*)arr[1], (double*)arr[2], (double*)arr[3]);
} else if (funcname == S_Symbol("movel")){														// movel
rob_movel_new((double *)arr[0], (double*)arr[1], (double*)arr[2], (double*)arr[3], (double*)arr[4]);
} else if (funcname == S_Symbol("movej")){														// $movej$
if (((sbinder)arr[0])->type->kind == T_INTARRAY ||
((sbinder)arr[0])->type->kind == T_RINTARRAY)
rob_movej_new_i(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
else if (((sbinder)arr[0])->type->kind == T_DOUARRAY ||
((sbinder)arr[0])->type->kind == T_RDOUARRAY)
rob_movej_new_d(((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
// rob_movej_new((double *)arr[0], (double*)arr[1], (double*)arr[2], (double*)arr[3]);
} else if (funcname == S_Symbol("movec")){														// movec
rob_movec_new((double *)arr[0], (double*)arr[1], (double*)arr[2], (double*)arr[3], (double*)arr[4], (double*)arr[5]);
} else if (funcname == S_Symbol("movepsi")){													// $movepsi$
if (((sbinder)arr[0])->type->kind == T_INTARRAY ||
((sbinder)arr[0])->type->kind == T_RINTARRAY)
rob_movepsi_new_i( ((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
else if (((sbinder)arr[0])->type->kind == T_DOUARRAY ||
((sbinder)arr[0])->type->kind == T_RDOUARRAY)
rob_movepsi_new_d( ((sbinder)arr[0])->addr, (double*)arr[1], (double*)arr[2], (double*)arr[3]);
} else if (funcname == S_Symbol("run_prog")){
rob_runprog(((sbinder)arr[0])->addr, ((sbinder)arr[0])->type->count);
} else if (funcname == S_Symbol("set_tcp")){
rob_set_tcp((double *)arr[0], (double *)arr[1], (double *)arr[2], (double *)arr[3], (double *)arr[4], (double *)arr[5]);
} else if (funcname == S_Symbol("get_flange_pos")){
rob_get_flange_pos((double *)arr[0]);
} else if (funcname == S_Symbol("get_joint_pos")){
if (((sbinder)arr[0])->type->kind == T_INTARRAY || ((sbinder)arr[0])->type->kind == T_RINTARRAY)
rob_get_joint_pos_i(((sbinder)arr[0])->addr);
else if (((sbinder)arr[0])->type->kind == T_DOUARRAY || ((sbinder)arr[0])->type->kind == T_RDOUARRAY)
rob_get_joint_pos_d(((sbinder)arr[0])->addr);
}*/
