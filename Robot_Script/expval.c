#include"sysutil.h"
#include"expval.h"

E_val E_Val(void)
{
	E_val ev = (E_val)checked_malloc(sizeof(*ev));
	ev->kind = E_NULL;
	return ev;
}

void E_setIntVal(E_val e, int i)
{
	e->kind = E_INT;
	e->u.i = i;
}

void E_setDouVal(E_val e, double d)
{
	e->kind = E_DOU;
	e->u.d = d;
}

void E_setCharVal(E_val e, char c)
{
	e->kind = E_CHAR;
	e->u.c = c;
}

void E_setCPoseVal(E_val e)
{
	e->kind = E_CPOSE;
	e->u.p = NULL;
}

void E_setCNumList(E_val e, void *p)
{
	e->kind = E_CNUMLISTP;
	e->u.p = p;
}

void E_setCStringVal(E_val e, void *p)
{
	e->kind = E_PSTRING;
	e->u.p = p;
}

void E_setArraypVal(E_val e, void *p)
{
	e->kind = E_ARRAYP;
	e->u.p = p;
}

void E_setNilVal(E_val e)
{
	e->kind = E_NIL;
}

double E_getDouVal(E_val e)
{
	switch (e->kind){
	case E_INT: return (double)e->u.i;
	case E_DOU: return (double)e->u.d;
	case E_CHAR:return (double)e->u.c;
	}
	return 0;
}

int E_getIntVal(E_val e)
{
	switch (e->kind){
	case E_INT: return (int)e->u.i;
	case E_DOU: return (int)e->u.d;
	case E_CHAR:return (int)e->u.c;
	}
	return 0;
}

char E_getCharVal(E_val e)
{
	switch (e->kind){
	case E_INT: return (char)e->u.i;
	case E_DOU: return (char)e->u.d;
	case E_CHAR:return (char)e->u.c;
	}
	return 0;
}

void *E_getPointVal(E_val e)
{
	switch (e->kind){
	case E_INT: case E_DOU: case E_CHAR: case E_NULL: case E_NIL:
		return NULL;
	default: return e->u.p;
	}
	return e->u.p;
}

void E_ValCopy(E_val dst, E_val src)
{
	dst->kind = src->kind;
	dst->u = src->u;
}

void E_reverseVal(E_val e)
{
	switch (e->kind){
	case E_CHAR: e->u.c = !(e->u.c); break;
	case E_DOU: e->u.d = !(e->u.d); break;
	case E_INT: e->u.i = !(e->u.i); break;
	default: break;
	}
}

void E_D_Val(E_val *ev)
{
	free(*ev);
	*ev = NULL;
}

void E_C_Val(E_val rv)
{
	rv->kind = E_NULL;
}

int E_TF_Val(E_val rv)
{
	double d;
	switch (rv->kind){
	case E_INT: d = (double)rv->u.i; break;
	case E_DOU: d = (double)rv->u.d; break;
	case E_CHAR: d = (double)rv->u.c; break;
	default: d = -1; break;
	}
	if (d <= 0) return FALSE;
	else return TRUE;
}