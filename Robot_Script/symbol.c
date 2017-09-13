#include "symbol.h"

#include <string.h>

#define SIZE 128

//创建哈希表
static S_symbol hashtable[SIZE]; 

//符号表结构
struct S_symbol_{
	string name; 
	S_symbol next;
};

//创建符号表
static S_symbol mksymbol(string name, S_symbol next)
{
	S_symbol s = (S_symbol)checked_malloc(sizeof(*s));
	s->name = checked_malloc(strlen(name) + 1);

	strcpy(s->name, name);
	s->next = next;
	return s; 
}

//查找哈希表对应序号
static unsigned int hash(char *s0)
{
	unsigned int h = 0; 
	char *s;
	for (s = s0; *s; s++){
		h = h * 65599 + *s;
	}
    return h;
}
 
static int streq(string a, string b)
{
    return !strcmp(a, b);
}

S_symbol S_Symbol(string name)
{
    //查找哈希表对应序号
	int index= hash(name) % SIZE;
	S_symbol syms = hashtable[index], sym;

    //若已经存在则返回
	for(sym = syms; sym; sym = sym->next)
		if (streq(sym->name, name)) 
			return sym;

    //不存在则创建新的符号表，存入哈希表
	sym = mksymbol(name, syms);
	hashtable[index] = sym;

	return sym;
}

string S_name(S_symbol sym)
{
    return sym->name;
}

void S_D_Symbols(void)  // 销毁系统中所有标识符
{
	int i = 0;
	S_symbol sym = NULL, tsym = NULL;
	for (i = 0; i < SIZE; i++){
		sym = hashtable[i];
		while (sym){
			tsym = sym;
			sym = sym->next;
			free(tsym->name);
			free(tsym);
		}
		hashtable[i] = NULL;
	}
}

S_table S_empty(void)
{ 
    return TAB_empty();
}

void S_enter(S_table t, S_symbol sym, void *value)
{
	//printf("table enter\n");
    TAB_enter(t, sym, value);
}

void *S_look(S_table t, S_symbol sym)
{
    return TAB_look(t, sym);
}
/*
static struct S_symbol_ marksym = {"<mark>", 0}; //a mark rember the position into a scope

void *S_look_s(S_table t, S_symbol sym)
{
	return TAB_Sign_look(t, sym, &marksym);
}

void S_beginScope(S_table t)
{ 
    S_enter(t, &marksym, NULL);
}

void S_endScope(S_table t)
{
    S_symbol s;
    do 
	    s = TAB_pop(t);
    while(s != &marksym);
}

void S_dump(S_table t, void (*show)(S_symbol sym, void *binding))
{
    TAB_dump(t, (void (*)(void *, void *)) show);
}

*/
