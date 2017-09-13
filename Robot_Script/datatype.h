#ifndef DATATYPE_H
#define DATATYPE_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/22
// Description: Abstract datatype M
//				a. 描述系统(内存空间)中变量对应的数据类型
//				b. 描述函数形参类型
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

typedef struct datatype_ *datatype;

//
// struct datatype_
//		kind :	T_INT, T_DOU, T_CHAR 基础数据类型
//				T_INTARRAY, T_RINTARRAY, T_DOUARRAY, T_RDOUARRAY, T_STRING, T_RSTRING, T_POSE, T_RPOSE, 数组及引用类型
//				T_PPOSE, T_PARRAY, T_PSTR 参数类型描述符
//				T_VOID 
//				T_NIL
//		count:
//				T_INT T_DOU T_BOOL T_VOID T_NIL T_POSE T_RPOSE T_PPOSE  T_PARRAY T_PSTR = DT_DEFAULT(-1)
//				T_POSE = POSESIZE
//				T_INTARRAY T_DOUARRAY T_STRING T_RINTARRAY T_RDOUARRAY T_RSTRING = 包含类型中所含的元素实际个数，string包含'\0'
//

//变量类型定义
struct datatype_{
	enum{
		T_INT, T_DOU, T_CHAR, 
		T_INTARRAY, T_RINTARRAY, T_DOUARRAY, T_RDOUARRAY, T_STRING, T_RSTRING, 
		T_POSE, T_RPOSE, T_PARRAY, T_PPOSE, T_PSTR, T_VOID, T_NIL
	} kind;
	int count;
};

//创建类型
datatype IntTy(void);
datatype DouTy(void);
datatype CharTy(void);
datatype IntArrTy(int count);
datatype DouArrTy(int count);
datatype StringTy(int count);
datatype RIntArrTy(int count);
datatype RDouArrTy(int count);
datatype RStringTy(int count);
datatype VoidTy(void);
datatype NilTy(void);
datatype PoseTy(void);
datatype RPoseTy(void);

datatype PPoseTy(void);
datatype PArrayTy(void);
datatype PStrTy(void);

//清除类型
void D_Ty(datatype *ty);

void OutputTS_Ty(datatype ty);

#endif
