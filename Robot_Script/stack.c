#include "stack.h"

#include "symbol.h"
#include "datatype.h"
#include "errmsg.h"

#define STACKSIZE 1024											//栈的存储大小1024B

static sbinder SBinder(S_symbol name, datatype type, void *addr, sbinder next, int stanum);
static void Stack_pushValList(stack s, S_symbol name, datatype ty, A_cnumlist list, int len, int count, int stanum, int(*setval)(void *, double));
static __inline int setIntVal(void *des, double val);
static __inline int setDouVal(void *des, double val);
static void Stack_pushVal(stack s, S_symbol name, datatype type, void *value, int stanum, int len);
static void douAtointA(int *des, double *src, int len);
static void intAtodouA(double *des, int *src, int len);
static int _D_Sbinder(sbinder sb);

static sbinder SBinder(S_symbol name, datatype type, void *addr, sbinder next, int stanum) // DONE
{
	sbinder sb = checked_malloc(sizeof(*sb));
	
	sb->name = name;
	sb->type = type;
	sb->addr = addr;
	sb->next = next;
	sb->stanum = stanum;
	return sb;
}

stack Stack(void) // DONE
{
	int i = 0;
	stack s = checked_malloc(sizeof(*s));

	for ( i =0; i < VALCOUNT; i++){
		(s->tab)[i] = NULL;
	}
	s->memoryspace = checked_malloc(STACKSIZE);
	s->top = s->memoryspace;
	s->size = 0;
	return s;
}

sbinder Stack_getSbinder(stack s, S_symbol name, int stackdeep) // DONE
{
	int index = 0;
	sbinder sb = NULL;
	if (!s) return NULL;

	index = ((unsigned)name) % VALCOUNT;
	sb = (s->tab)[index];
	while (sb && sb->stanum == stackdeep){
		if (sb->name == name)
			return sb;
		sb = sb->next;
	}
	return NULL;
}

int Stack_setSbinderIntVal(sbinder sb, int val) // DONE
{
	datatype ty;
	if (!sb)
		return FALSE;
	ty = sb->type;

	switch (ty->kind){
	case T_INT: *((int *)(sb->addr)) = val; break;
	case T_DOU: *((double*)(sb->addr)) = val; break;
	case T_CHAR: *((char *)(sb->addr)) = val; break;
	default: return FALSE;  break;
	}
	return TRUE;
}

int Stack_setSbinderDouVal(sbinder sb, double val) // DONE
{
	datatype ty;
	if (!sb) return FALSE;
	ty = sb->type;

	switch (ty->kind){
	case T_INT: *((int *)(sb->addr)) = (int)val;  break;
	case T_DOU: *((double*)(sb->addr)) = val; break;
	case T_CHAR: *((char *)(sb->addr)) = (char)val; break;
	default:  return FALSE;  break;
	}
	return TRUE;
}

int Stack_setSbinderCharVal(sbinder sb, char val) // DONE
{
	datatype ty;
	if (!sb) return FALSE;
	ty = sb->type;

	switch (ty->kind){
	case T_INT: *((int *)(sb->addr)) = val;  break;
	case T_DOU: *((double*)(sb->addr)) = val; break;
	case T_CHAR: *((char *)(sb->addr)) = val; break;
	default:  return FALSE;  break;
	}
	return TRUE;
}

// D: 传入的字符串长度len不涵盖‘\0’
// D: 只支持长度相等的字符串进行设置
int Stack_setSbinderCStringVal(sbinder sb, char *val, int len)	// DONE
{
	datatype ty;
	if (!sb) return FALSE;
	ty = sb->type;

	switch (ty->kind){
	case T_STRING: case T_RSTRING:
		if (ty->count == (len + 1)){
			strcpy(sb->addr, val);
		} else{
			return FALSE;
		}
	default: return FALSE; break;
	}
	return TRUE;
}

static void intAtodouA(double *des, int *src, int len)
{
	// not safety function
	int i = 0;
	for (i = 0; i < len; i++){
		des[i] = src[i];
	}
}

static void douAtointA(int *des, double *src, int len)
{
	int i = 0;
	for (i = 0; i < len; i++){
		des[i] = (int)src[i];
	}
}

// D: array / array， array / pose，string / string 在长度相等的情况下可以相互赋值
int Stack_setSbinderSbinderVal(sbinder des, sbinder src)	// DONE
{ 
	datatype dty, sty;
	if (des == NULL || src == NULL) return FALSE;

	dty = des->type, sty = src->type;
	if (dty->count != sty->count )							// 如果大小不相等， 不能赋值
		return FALSE;

	switch (dty->kind){
	case T_INTARRAY: case T_RINTARRAY:
		if (sty->kind == T_INTARRAY || sty->kind == T_RINTARRAY){
			memcpy(des->addr, src->addr, dty->count * RSI_INTSIZE);
		} else if (sty->kind == T_DOUARRAY || sty->kind == T_RDOUARRAY ||
					sty->kind == T_POSE || sty->kind == T_RPOSE){
			douAtointA(des->addr, src->addr, dty->count);
		} break;
	case T_DOUARRAY: case T_RDOUARRAY:	case T_POSE: case T_RPOSE:
		if (sty->kind == T_INTARRAY || sty->kind == T_RINTARRAY){
			intAtodouA(des->addr, src->addr, dty->count);
		} else if (sty->kind == T_DOUARRAY || sty->kind == T_RDOUARRAY ||
			sty->kind == T_POSE || sty->kind == T_RPOSE){
			memcpy(des->addr, src->addr, dty->count * RSI_DOUBLESIZE);
		} break;
	case T_STRING: case T_RSTRING: 
		if (sty->kind == T_STRING || sty->kind == T_RSTRING){
			strcpy(des->addr, src->addr);
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

// D: cnumlist -> array / pose / string
// D: cnumlist -> string 须慎用
int Stack_setSbinderCNumListVal(sbinder sb, A_cnumlist cnl) // DONE
{
	int *iaddr = NULL; double *daddr = NULL; char *caddr = NULL;
	int i = 0, count;
	datatype ty;
	if (!sb) return FALSE;
	
	ty = sb->type;
	count = ty->count;

	switch (ty->kind){
	case T_RINTARRAY: case T_INTARRAY:
		iaddr = sb->addr;
		while (i++ < count && cnl){ *(iaddr++) = (int)(cnl->num->d); cnl = cnl->tail; } break;
	case T_RDOUARRAY: case T_DOUARRAY: case T_POSE: case T_RPOSE:
		daddr = sb->addr;
		while (i++ < count && cnl){ *(daddr++) = cnl->num->d; cnl = cnl->tail; } break;
	case T_STRING: case T_RSTRING: 
		caddr = sb->addr;
		while (i++ < count && cnl){ *(caddr++) = (char)cnl->num->d; cnl = cnl->tail; } break;
	default: return FALSE; break;
	}
	return TRUE;
}

int Stack_setSbinderPoseZero(sbinder sb) // DONE
{
	int i = 0;
	datatype ty;
	if (!sb) return FALSE;
	ty = sb->type;

	switch (ty->kind){
	case T_POSE:case T_RPOSE: 
		while (i < ty->count){ 	*((double*)(sb->addr) + i++) = 0; } break;
	default: return FALSE; break;
	}
	return TRUE;
}

int Stack_setArraySbinderDouVal(sbinder sb, double val, int offset) // DONE
{
	datatype ty;
	if (!sb) return FALSE;
	ty = sb->type;
	if (offset >= ty->count) return FALSE;

	switch (ty->kind){
	case T_POSE:case T_RPOSE: case T_DOUARRAY: case T_RDOUARRAY:
		((double *)(sb->addr))[offset] = val;
		break;
	case T_RINTARRAY: case T_INTARRAY:
		((int *)(sb->addr))[offset] = (int)val;
		break;
	case T_STRING:case T_RSTRING:
		((char *)(sb->addr))[offset] = (char)val;
		break;
	default: return FALSE; break;
	}
	return TRUE;
}

int Stack_setArraySbinderIntVal(sbinder sb, int val, int offset) // DONE
{
	return Stack_setArraySbinderDouVal(sb, (double)val, offset);
}

int Stack_setArraySbinderCharVal(sbinder sb, char val, int offset) // DONE
{
	return Stack_setArraySbinderDouVal(sb, (double)val, offset);
}

// D: 参数说明
//		name: 变量名
//		type: 变量值
//		value: 变量值， 指向内存区的指针
//		stanum: 栈内编号
//		len: 实际需要的栈内存储空间， 单位：Byte
// D: 如果value != NULL, 则将value中的长度的len内容拷贝至stack，否则将长度为len的内容置为0
static void Stack_pushVal(stack s, S_symbol name, datatype type, void *value, int stanum, int len) // DONE
{
	int index; void *taddr = NULL;
	if ((s->size + len) >= STACKSIZE) EM_errorE(0, RSIERR_STA_OVERFLOW_INFO); // RSIERR_STA_OVERFLOW

	index = ((unsigned)name) % VALCOUNT;
	taddr = s->top;

	if (value)	memcpy(taddr, value, len);
	else		memset(taddr, 0, len);

	s->top = (char*)s->top + len;
	s->size += len;

	s->tab[index] = SBinder(name, type, taddr, s->tab[index], stanum);
}

void Stack_pushIntVal(stack s, S_symbol name, int val, int stanum) // DONE
{
	datatype type = IntTy();
	Stack_pushVal(s, name, type, &val, stanum, RSI_INTSIZE);
}

void Stack_pushDouVal(stack s, S_symbol name, double val, int stanum)
{
	datatype type = DouTy();
	Stack_pushVal(s, name, type, &val, stanum, RSI_DOUBLESIZE); // DONE
}

void Stack_pushCharVal(stack s, S_symbol name, char val, int stanum)
{
	datatype type = CharTy();
	Stack_pushVal(s, name, type, &val, stanum, RSI_CHARSIZE); // DONE
}

void Stack_pushPoseZeroVal(stack s, S_symbol name, int stanum) // DONE
{
	datatype type = PoseTy();
	Stack_pushVal(s, name, type, NULL, stanum, POSESIZE);
}

void Stack_pushStringVal(stack s, S_symbol name, char *str, int stanum) // DONE
{
	datatype ty = StringTy(strlen(str) + 1);
	Stack_pushVal(s, name, ty, str, stanum, ty->count * RSI_CHARSIZE);
}

void Stack_pushIntArrayVal(stack s, S_symbol name, int *value, int count, int stanum) // DONE
{
	datatype ty = IntArrTy(count);
	Stack_pushVal(s, name, ty, value, stanum, count * RSI_INTSIZE);
}

void Stack_pushDouArrayVal(stack s, S_symbol name, double *value, int count, int stanum) // DONE
{
	datatype ty = DouArrTy(count);
	Stack_pushVal(s, name, ty, value, stanum, count * RSI_DOUBLESIZE);
}

void Stack_pushStrArrayVal(stack s, S_symbol name, char * value, int count, int stanum) // DONE
{
	datatype ty = StringTy(count);
	Stack_pushVal(s, name, ty, value, stanum, count * RSI_CHARSIZE);
}

static __inline int setDouVal(void *des, double val) // DONE
{
	*((double*)(des)) = val;
	return RSI_DOUBLESIZE;
}

static __inline int setIntVal(void *des, double val) // DONE
{
	*((int*)(des)) = (int)val;
	return RSI_INTSIZE;
}

static __inline int setCharVal(void *des, double val) // DONE
{
	*((char*)(des)) = (char)val;
	return RSI_CHARSIZE;
}

// D: 参数说明
//		name: 变量名
//		ty: 变量值
//		list: 源numlist列表
//		len: 变量的实际占用内存空间大小（需要的栈内空间大小）
//		count: 变量实际个数（list的长度可以大于count也可以小于count）
//		stanum: 栈内编号
static void Stack_pushValList(stack s, S_symbol name, datatype ty, A_cnumlist list, int len, int count, int stanum , int (*setval)(void *, double))
{
	int i = 0; int index; char *taddr = NULL;
	if ((s->size + len) >= STACKSIZE)
		EM_errorE(0, RSIERR_STA_OVERFLOW_INFO);	// RSIERR_STA_OVERFLOW

	index = ((unsigned)name) % VALCOUNT;
	taddr = s->top;

	for (i = 0; i < count; i++){
		if (list){
			taddr += setval(taddr, list->num->d);
			list = list->tail;
		} else{
			taddr += setval(taddr, 0);
		}
	}

	taddr = s->top;
	s->top = (char*)s->top + len;
	s->size += len;

	s->tab[index] = SBinder(name, ty, taddr, s->tab[index], stanum);
}

void Stack_pushPoseCNListVal(stack s, S_symbol name, A_cnumlist list, int stanum) // DONE
{
	datatype ty = PoseTy();
	Stack_pushValList(s, name, ty, list, POSESIZE, POSECOUNT, stanum, setDouVal);
}

void Stack_pushIntArrCNListVal(stack s, S_symbol name, A_cnumlist list, int count, int stanum) // DONE
{
	datatype ty =IntArrTy(count);
	Stack_pushValList(s, name, ty, list, count * RSI_INTSIZE, count, stanum, setIntVal);
}

void Stack_pushDouArrCNListVal(stack s, S_symbol name, A_cnumlist list, int count, int stanum) // DONE
{
	datatype ty = DouArrTy(count);
	Stack_pushValList(s, name, ty, list, count * RSI_DOUBLESIZE, count, stanum, setDouVal);
}

// N: Stack_pushSbinderVal
// D: 用作变量拷贝初始化, 在调用阶段要保证 sb != NULL
// D: 是否需要支持字符串变量的拷贝初始化？暂时不支持
void Stack_pushSbinderVal(stack s, S_symbol name, sbinder sb, int stanum) // DONE
{
	datatype dty = checked_malloc(sizeof(*dty));
	datatype sty = sb->type;
	dty->count = sty->count;
	int len = 0;
	switch (sty->kind){
	case T_INTARRAY: case T_RINTARRAY:
		dty->kind = T_INTARRAY;
		len = (dty->count) * RSI_INTSIZE; break;
	case T_DOUARRAY: case T_RDOUARRAY:
		dty->kind = T_DOUARRAY;
		len = (dty->count) * RSI_DOUBLESIZE; break;
	default: EM_errorE(0, RSIERR_STA_ALLOC_FAILUER_INFO); break;
	}
	Stack_pushVal(s, name, dty, sb->addr, stanum, len);
}

// N: Stack_pushValR
// D: 将引用类型的变量压栈, 在调用阶段要保证 sb != NULL
void Stack_pushValR(stack s, S_symbol name, sbinder sb, int stanum) // DONE
{
	int index = ((unsigned)name) % VALCOUNT;
	sbinder dsb = checked_malloc(sizeof(*sb));
	datatype ty = NULL;
 
	switch ((sb->type)->kind){
	case T_INTARRAY: case T_RINTARRAY: ty = RIntArrTy(sb->type->count); break;
	case T_DOUARRAY: case T_RDOUARRAY: ty = RDouArrTy(sb->type->count); break;
	case T_POSE: case T_RPOSE: ty = RPoseTy( ); break;
	case T_STRING: case T_RSTRING: ty = RStringTy(sb->type->count); break;
	default: EM_errorE(0, RSIERR_STA_PUSHREF_FAILURE_INFO);
	}
	dsb->type = ty;

	dsb->name = name;
	dsb->addr = sb->addr;
	dsb->stanum = stanum;
	dsb->next = s->tab[index];

	s->tab[index] = dsb;
}

void Stack_pushPoseR(stack s, S_symbol name, void *addr, int stanum)
{
	int index = ((unsigned)name) % VALCOUNT;
	sbinder dsb = checked_malloc(sizeof(*dsb));
	dsb->type = RPoseTy();

	dsb->name = name;
	dsb->addr = addr;
	dsb->stanum = stanum;
	dsb->next = s->tab[index];

	s->tab[index] = dsb;
}

void Stack_pushDouArrayR(stack s, S_symbol name, void *addr, int count, int stanum)
{
	int index = ((unsigned)name) % VALCOUNT;
	sbinder dsb = checked_malloc(sizeof(*dsb));
	dsb->type = RDouArrTy(count);

	dsb->name = name;
	dsb->addr = addr;
	dsb->stanum = stanum;
	dsb->next = s->tab[index];

	s->tab[index] = dsb;
}

void Stack_pushIntArrayR(stack s, S_symbol name, void *addr, int count, int stanum)
{
	int index = ((unsigned)name) % VALCOUNT;
	sbinder dsb = checked_malloc(sizeof(*dsb));
	dsb->type = RIntArrTy(count);

	dsb->name = name;
	dsb->addr = addr;
	dsb->stanum = stanum;
	dsb->next = s->tab[index];

	s->tab[index] = dsb;
}

// N: Stack_printfSbinder
// D: 打印出变量信息
//    str: 信息存放地址
//    len: str长度
int Stack_printfSbinder(char *str, sbinder sb, int len) // DONE
{
#define PUT_CONTROL 24			// 假设一个double类型最多占16位
#define MIX_LEN 64				// str的最小空间
	datatype ty;
	int i = 0, c = 0; int *pi = NULL; double *pd = NULL; char *pc = NULL;
	int maxsign = len - PUT_CONTROL;

	if (sb == NULL || len < MIX_LEN) return -1;
	ty = sb->type;

	switch (ty->kind){
	case T_INTARRAY: case T_RINTARRAY:
		pi = sb->addr;
		c += sprintf(str + c, "[%s kind: INTARRAY , size: %d] [", S_name(sb->name), ty->count);
		while (i++ < ty->count && c < maxsign) 
			c += sprintf(str + c, "%d, ", *(pi++));
		if ((i - 1) != ty->count) c += sprintf(str + c, "...");
		c += printf(str + c, "]\n");
		break;
	case T_DOUARRAY: case T_RDOUARRAY: 
		pd = sb->addr;
		c += sprintf(str + c, "[%s kind: DOURRAY , size: %d] [", S_name(sb->name), ty->count);
		while (i++ < ty->count && c < maxsign)
			c += sprintf(str + c, "%lf, ", *(pd++));
		if ((i - 1) != ty->count) c += sprintf(str + c, "...");
		c += printf(str + c, "]\n");
		break;
	case T_POSE: case T_RPOSE: 
		pd = sb->addr;
		c += sprintf(str + c, "[%s kind: POSE , size: 6] [", S_name(sb->name));
		while (i++ < ty->count && c < maxsign)
			c += sprintf(str + c, "%lf, ", *(pd++));
		if ((i - 1) != ty->count) c += sprintf(str + c, "...");
		c += printf(str + c, "]\n");
		break;
	case T_STRING: case T_RSTRING: 
		pc = sb->addr;
		c += sprintf(str + c, "[%s kind: STRING , size: %d] [\"", S_name(sb->name), ty->count);
		while (i++ < ty->count && c < maxsign){
			c += sprintf(str + c, "%c", *(pc++));
		}
		if ((i - 1) != ty->count) c += sprintf(str + c, "...");
		c += sprintf(str + c, "\"]\n");
		break;
	default: break;
	}

	return c;
}

void Stack_popOneLeval(stack s, int stanum)
{
	int i = 0, size = 0;
	sbinder sb = NULL, tsb = NULL;
	if (s == NULL) return;

	for (i = 0; i < VALCOUNT; i++){
		sb = s->tab[i];
		//printf("sb->stanum = %d, stanum = %d\n", sb->stanum, stanum);
		if (sb != NULL && sb->stanum > stanum)
			EM_errorE(0, RSIERR_STA_STACK_DISORDERED_INFO);

		while (s->tab[i]){
			if (s->tab[i]->stanum != stanum) break;
			tsb = s->tab[i]->next;
			size = size + _D_Sbinder(s->tab[i]);
			s->tab[i] = tsb;
		}
	}
	if (size > s->size)
		EM_errorE(0, RSIERR_STA_STACK_DISORDERED_INFO);

	s->top = (char*)s->top - size;
	s->size -= size;
}



static int _D_Sbinder(sbinder sb)
{
	datatype ty = sb->type;
	int size = 0;
	switch (ty->kind)
	{
	case T_INT: size = RSI_INTSIZE; break;
	case T_DOU: size = RSI_DOUBLESIZE; break;
	case T_CHAR: size = RSI_CHARSIZE; break;
	case T_INTARRAY: size = ty->count * RSI_INTSIZE; break;
	case T_DOUARRAY: size = ty->count * RSI_DOUBLESIZE; break;
	case T_STRING: size = ty->count * RSI_CHARSIZE; break;
	case T_POSE: size = POSESIZE; break;
	case T_RINTARRAY: case T_RDOUARRAY: case T_RSTRING: case T_RPOSE: size = 0; break;
	case T_PARRAY:case T_PPOSE:case T_PSTR:		// 表示形参类型， 不入栈
	case T_VOID: case T_NIL:					// 无意义不入栈
	default: return 0; break;
	}
	D_Ty(&ty);
	free(sb);
	return size;
}

void D_Stack(stack *stk)
{
	if ((*stk) == NULL) return;
	C_Stack(*stk);

	free((*stk)->memoryspace);

	free(*stk);
	*stk = NULL;
}

void C_Stack(stack stk)
{
	int i = 0;
	sbinder sb, tsb;

	if (stk == NULL) return;

	for (i = 0; i < VALCOUNT; i++ ){
		sb = stk->tab[i];
		while (sb){
			tsb = sb;
			sb = sb->next;
			D_Ty(&(tsb->type));
			free(tsb);
		}
		stk->tab[i] = NULL;
	}
	stk->top = stk->memoryspace;
	stk->size = 0;
}