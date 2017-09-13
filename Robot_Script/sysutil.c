#include "sysutil.h"

#include "log.h"
#include "errmsg.h"
#include <string.h>

static unsigned long int _malloc_size = 0;

//malloc a "len" long space and return it
void* checked_malloc(int len)
{
	void *p = malloc(len);
	if (!p){
#ifdef _MSC_VER
		EM_errorE(RI_ERROR_SYSERR, "system error £ºmalloc failed£¬ Ran out of memory!  "
			"total malloc size = %ld\n", U_ToolGetMSize());
#else
		exit(1); // Linux enivronment
#endif
	}
	_malloc_size += len;

	return p;
}

string String(char *s)
{
	string p = checked_malloc(strlen(s) + 1);
	strcpy(p, s);
	return p;
}

U_boolList U_BoolList(bool head, U_boolList tail)
{
	U_boolList list = (U_boolList)checked_malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

void U_D_BoolList(U_boolList *bl)
{
	U_boolList ubl = *bl, tubl = NULL;
	while (ubl){
		tubl = ubl;
		ubl = ubl->tail;
		free(tubl);
	}
	(*bl) = NULL;
}

U_intList U_IntList(int i, U_intList il)
{
	U_intList pil = (U_intList)checked_malloc(sizeof(*pil));
	pil->i = i;
	pil->rest = il;
	return pil;
}

void U_D_IntList(U_intList *il)
{
	U_intList uil = *il, tuil = NULL;
	while (uil)
	{
		tuil = uil;
		uil = uil->rest;
		free(tuil);
	}
	(*il) = NULL;
}

unsigned long U_ToolGetMSize()
{
	return _malloc_size;
}
