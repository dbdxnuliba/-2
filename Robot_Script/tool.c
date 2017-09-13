#include "tool.h"

#include "sysutil.h"

#define STATOOL_INITSIZE 20

sta_tool T_Sta_tool()
{
	int i = 0;
	sta_tool ps = checked_malloc(sizeof(*ps));
	ps->maxsize = STATOOL_INITSIZE;
	ps->size = 0;

	ps->sta = checked_malloc(STATOOL_INITSIZE * sizeof(void*));
	for (i = 0; i < STATOOL_INITSIZE; i++){
		(ps->sta)[i] = NULL;
	}
	ps->top = ps->sta;

	return ps;
}

void *T_staPop(sta_tool sta)
{
	if (sta->size == 0){
		return NULL;
	} else{
		(sta->top)--;
		sta->size--;
		return *(sta->top);
	}
}

void T_staPush(sta_tool sta, void *p)
{
	if (p == NULL) return;											// TODO:

	if (sta->maxsize == sta->size + 1){
		sta->sta = realloc(sta->sta, sta->maxsize * 2 * sizeof(void*));
		sta->top = sta->sta + sta->maxsize;
	}

	(*(sta->top)) = p;
	sta->top++;
	sta->size++;
}

void T_D_staTool(sta_tool *sta)
{
	if (*sta == NULL) return;

	free((*sta)->sta);
	free(*sta);
	*sta = NULL;
}

void T_C_staTool(sta_tool sta)
{
	int i;
	if (sta == NULL) return;
	for (i =0; i < sta->maxsize; i++){
		(sta->sta)[i] = NULL;
	}
	sta->top = sta->sta;
	sta->size = 0;
}