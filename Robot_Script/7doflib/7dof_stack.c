#include"7dof_stack.h"
#include"../sysutil.h"

#define INITSIZE 20

dof_stack Dof_stack()
{
	int i = 0;
	dof_stack ps = checked_malloc(sizeof(*ps));
	ps->maxsize = INITSIZE;
	ps->size = 0;

	ps->sta = checked_malloc(INITSIZE * sizeof(void*));
	for (i = 0; i < INITSIZE; i++){
		(ps->sta)[i] = NULL;
	}
	ps->top = ps->sta;

	return ps;
}

void *D_pops(dof_stack sta)
{
	if (sta->size == 0){
		return NULL;
	}else{
		(sta->top)--;
		sta->size--;
		return *(sta->top);
	}
}

void D_pushs(dof_stack sta, void *p)
{
	if (sta->maxsize == sta->size + 1){
		sta->sta = realloc(sta->sta, sta->maxsize * 2 * sizeof(void*));
		sta->top = sta->sta + sta->maxsize;
	}

	(*(sta->top)) = p;
	sta->top++;
	sta->size++;
}

void* D_getTops(dof_stack sta)
{
	return *(sta->top - 1);
}

void D_Dof_stack(dof_stack *sta)
{
	if (*sta == NULL) return;

	free((*sta)->sta);
	free(*sta);
	*sta = NULL;
}

void C_Dof_stack(dof_stack sta)
{
	int i = 0;
	if (sta == NULL) return;
	for (; i < INITSIZE; i++){
		(sta->sta)[i] = NULL;
	}
	sta->top = sta->sta;
	sta->size = 0;
}