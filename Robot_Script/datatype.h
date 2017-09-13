#ifndef DATATYPE_H
#define DATATYPE_H

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/22
// Description: Abstract datatype M
//				a. ����ϵͳ(�ڴ�ռ�)�б�����Ӧ����������
//				b. ���������β�����
// Environment: windows(vs) / gcc
//
///////////////////////////////////////////////////////////////////////////////////////////

typedef struct datatype_ *datatype;

//
// struct datatype_
//		kind :	T_INT, T_DOU, T_CHAR ������������
//				T_INTARRAY, T_RINTARRAY, T_DOUARRAY, T_RDOUARRAY, T_STRING, T_RSTRING, T_POSE, T_RPOSE, ���鼰��������
//				T_PPOSE, T_PARRAY, T_PSTR ��������������
//				T_VOID 
//				T_NIL
//		count:
//				T_INT T_DOU T_BOOL T_VOID T_NIL T_POSE T_RPOSE T_PPOSE  T_PARRAY T_PSTR = DT_DEFAULT(-1)
//				T_POSE = POSESIZE
//				T_INTARRAY T_DOUARRAY T_STRING T_RINTARRAY T_RDOUARRAY T_RSTRING = ����������������Ԫ��ʵ�ʸ�����string����'\0'
//

//�������Ͷ���
struct datatype_{
	enum{
		T_INT, T_DOU, T_CHAR, 
		T_INTARRAY, T_RINTARRAY, T_DOUARRAY, T_RDOUARRAY, T_STRING, T_RSTRING, 
		T_POSE, T_RPOSE, T_PARRAY, T_PPOSE, T_PSTR, T_VOID, T_NIL
	} kind;
	int count;
};

//��������
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

//�������
void D_Ty(datatype *ty);

void OutputTS_Ty(datatype ty);

#endif
