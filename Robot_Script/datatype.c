#include "datatype.h"

#include "sysutil.h"

#define DT_DEFAULT -1

static struct datatype_ intTy = { T_INT, DT_DEFAULT };
datatype IntTy(void)
{
	return &intTy;
}

static struct datatype_ douTy = { T_DOU, DT_DEFAULT };
datatype DouTy(void)
{
	return &douTy;
}

static struct datatype_ charTy = { T_CHAR, DT_DEFAULT };
datatype CharTy(void)
{
	return &charTy;
}

datatype IntArrTy(int count)
{
	datatype ty = checked_malloc(sizeof(*ty));
	ty->kind = T_INTARRAY;
	ty->count = count;
	return ty;
}

datatype DouArrTy(int count)
{
	datatype ty = checked_malloc(sizeof(*ty));
	ty->kind = T_DOUARRAY;
	ty->count = count;
	return ty;
}

datatype StringTy(int count)
{
	datatype ty = checked_malloc(sizeof(*ty));
	ty->kind = T_STRING;
	ty->count = count;
	return ty;
}

datatype RIntArrTy(int count)
{
	datatype ty = checked_malloc(sizeof(*ty));
	ty->kind = T_RINTARRAY;
	ty->count = count;
	return ty;
}

datatype RDouArrTy(int count)
{
	datatype ty = checked_malloc(sizeof(*ty));
	ty->kind = T_RDOUARRAY;
	ty->count = count;
	return ty;
}

datatype RStringTy(int count)
{
	datatype ty = checked_malloc(sizeof(*ty));
	ty->kind = T_RSTRING;
	ty->count = count;
	return ty;
}

static struct datatype_ voidTy = { T_VOID, DT_DEFAULT };
datatype VoidTy(void)
{
	return &voidTy;
}

static struct datatype_ nilTy = { T_NIL, DT_DEFAULT };
datatype NilTy(void)
{
	return &nilTy;
}

static struct datatype_ poseTy = { T_POSE, POSECOUNT };
datatype PoseTy(void)
{
	return &poseTy;
}

static struct datatype_ rposeTy = { T_RPOSE, DT_DEFAULT };
datatype RPoseTy(void)
{
	return &rposeTy;
}

static struct datatype_ pposeTy = { T_PPOSE, DT_DEFAULT };
datatype PPoseTy(void)
{
	return &pposeTy;
}

static struct datatype_ parrayTy = { T_PARRAY, DT_DEFAULT };
datatype PArrayTy(void)
{
	return &parrayTy;
}

static struct datatype_ pstrTy = { T_PSTR, DT_DEFAULT };
datatype PStrTy(void)
{
	return &pstrTy;
}

void D_Ty(datatype *ty)
{
	// ����T_POSE�������⣬����ty->size = DT_DEFAULT�Ľṹ��ֻ��Ҫ�������ͣ�����Ҫ�洢��С��
	// ���Ǳ�����Ϊstatic������T_POSE��һ��������������size�Ĵ�С������ȴ��static���εľ�̬����
	datatype t = *ty;
	if (t->count != DT_DEFAULT && t->kind != T_POSE){
		free(t);
	}
	(*ty) = NULL;
}

void OutputTS_Ty(datatype ty)
{
	switch (ty->kind){
	case T_PARRAY: printf("array "); break;
	case T_PPOSE: printf("pose "); break;
	case T_PSTR: printf("string "); break;
	case T_INT: printf("int "); break;
	case T_DOU: printf("float "); break;
	case T_CHAR: printf("char "); break;
	case T_VOID: printf("void "); break;
	default: break;
	}
}