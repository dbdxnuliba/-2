#include"table.h"

#include<stdio.h>
#include"sysutil.h"

//生成binder
static binder Binder(void *key, void *value, binder next, void *prevtop)
{
	binder b = checked_malloc(sizeof(*b));
	b->key = key;
	b->next = next;
	b->value = value;
	b->prevtop = prevtop;
	return b;
}

//得到一个空Table
TAB_table TAB_empty(void)
{
	TAB_table t = checked_malloc(sizeof(*t));
	int i;
	t->top = NULL;
	for (i = 0; i < _TABSIZE; i++){
		t->table[i] = NULL;
	}
	return t;
}

//为table添加binder
void TAB_enter(TAB_table t, void *key, void *value)
{
	int index;
	assert(t && key);

	index = ((unsigned)key) % _TABSIZE;
	t->table[index] = Binder(key, value, t->table[index], t->top);
	t->top = key;
}

//查找在table中key对应的value
void *TAB_look(TAB_table t, void *key)
{
	int index;
	binder b;
	assert(t && key);
	index = ((unsigned)key) % _TABSIZE;
	for (b = t->table[index]; b; b = b->next){
		if (b->key == key)
			return b->value;
	}
	return NULL;
}

//未使用
void *TAB_pop(TAB_table t){
	void *k;
	binder b;
	int index;
	assert(t);
	k = t->top;
	assert(k);
	index = ((unsigned)k) % _TABSIZE;
	b = t->table[index];
	assert(b);
	t->table[index] = b->next;
	//TODO: value b should be free
	t->top = b->prevtop;
	return b->key;
}

//未使用
void TAB_dump(TAB_table t, void (*show)(void *key, void *value))
{
	void *k = t->top;
	int index = ((unsigned)k) % _TABSIZE;
	binder b = t->table[index];
	if (NULL == b)
		return;
	t->table[index] = b->next;
	t->top = b->prevtop;
	show(b->key, b->value);
	TAB_dump(t, show);
	assert(t->top == b->prevtop && t->table[index] == b->next);
	t->top = k;
	t->table[index] = b;
}
