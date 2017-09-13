#ifndef TABLE_H
#define TABLE_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/20
// Description: Table Struct
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

#define _TABSIZE 127							// 记录Table的大小
typedef struct TAB_table_ *TAB_table;
typedef struct binder_ *binder;

struct binder_{
	void *key;
	void *value;
	binder next;
	void *prevtop;								// key
};

//函数表
struct TAB_table_{
	binder table[_TABSIZE];
	void *top;									// key
};

TAB_table TAB_empty(void);

void TAB_enter(TAB_table t, void *key, void *value);
void* TAB_look(TAB_table t, void *key);			// return kind of value

//void* TAB_pop(TAB_table t);
//void TAB_dump(TAB_table t, void (*show)(void *key, void *value));

#endif
