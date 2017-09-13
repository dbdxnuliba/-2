%{
#include"util.h"
#include"symbol.h"
#include"absyn.h"
#include"errormsg.h"
#include"sysmacro.h"


#define TOKPOS 0

#define PEXP(x) printf(x"\n")
#define PSTM(x) /*printf(x"\n")*/

A_stmList absyn_root; //the root of  grammar tree

int yylex(void);
void yyerror(char *s)
{
	EM_error(EM_tokPos, "%s\n", s);
}

%}

%union{
	int ival;
	double dval;
	string sval;
	bool bval;
	S_symbol sym;
	A_stmList stml;
	A_stm stm;
	A_while wh;
	A_funDec fund;
	A_typeFields tyf;
	A_field f;
	A_fieldl fl;
	A_fieldd fd;
	A_fielddl fdl;
	A_threadDec thrd;
	A_if iff;
	A_elifExp elif;
	A_elifList elifl;
	A_exp e;
	A_op op;
	A_funcall func;
	A_args args;
	A_variate var;
	A_assign assign;
	A_cnum cnum;
	A_cnumlist cnuml;
	A_arraytool arrt;
}

%right ASSIGN
%left AND OR XOR 
%nonassoc EQ NEQ LT LE GT GE
%left PLUS MINUS
%left TIMES DIVIDE
%right NOT
%left UMINUS

%token <bval> BOOL 
%token <sval> ID STRING
%token <ival> INT 
%token <dval> DOUBLE

%token 
	COLON COMMA LBRACK RBRACK LPAREN RPAREN POUND SEMICOLON 
	PLUS MINUS TIMES DIVIDE LT LE GT GE EQ NEQ 
	ASSIGN POSE ARRAY SINT SFLOAT SCHAR
	AND OR XOR NOT 
	IF ELIF ELSE END WHILE BREAK RETURN 
	DEF THREAD RUN GLOBAL

%type <stml> stmlist
%type <stm> stm
%type <fund> fundec
%type <tyf> typefields
%type <f> field
%type <fl> fieldl
%type <fd> fieldd
%type <fdl> fielddl
%type <thrd> threaddec
%type <iff> ifstm
%type <elifl> eliflist
%type <elif> elifexp	
%type <e> exp
%type <arrt> array
%type <func> funcall
%type <args> args
%type <sym> id threadcall
%type <var> variate
%type <cnuml> clist cnumlist		
%type <cnum> cnum

%start program

%%
program: stmlist {absyn_root = $1 ;}

stmlist: 	stm {$$ = A_StmList($1, NULL); }
	|		stm stmlist{$$ = A_StmList($1, $2); }
	;
stm:	exp SEMICOLON {$$ = A_ExpStm(TOKPOS, $1); PSTM("expstm");}
	|	WHILE LPAREN exp RPAREN COLON stmlist END { $$ = A_WhileStm(TOKPOS, A_While($3, $6)); PSTM("whilestm");}
	|	BREAK SEMICOLON {$$ = A_BreakStm(TOKPOS); PSTM("breakstm");}
	|	RETURN exp SEMICOLON {$$ = A_ReturnStm(TOKPOS, $2); PSTM("returnstm");}
	|	fundec {$$ = A_FunDecStm(TOKPOS, $1); PSTM("fundecstm");}
	|	threaddec {$$ = A_ThreadDecStm(TOKPOS, $1); PSTM("threadstm");}
	|	ifstm {$$ = A_IfStm(TOKPOS, $1); PSTM("ifstm");}
	|	variate ASSIGN array SEMICOLON{$$ = A_ArraydefStm(TOKPOS, A_Arraydef($1, $3)); PSTM("arraydefStm"); }
	|	GLOBAL id {$$ = A_GlobalStm(TOKPOS, $2);}
	|	error SEMICOLON {yyclearin ; yyerrok; printf("Enter another command:\n");}
	|	error END {yyclearin ; yyerrok; printf("Enter another command:\n");}
	;
fundec:	DEF id LPAREN typefields RPAREN COLON stmlist END {$$ = A_FunDec($2, $4, $7); }
	;
typefields:	fieldl {$$ = A_TypeFields($1, NULL); }
	|		fielddl {$$ = A_TypeFields(NULL, $1); }
	|		fieldl COMMA fielddl {$$ = A_TypeFields($1, $3); }
	|		{$$ = NULL; }
	;
field:	id COLON id {$$ = A_Field($1, $3); }	
	;
fieldl:	field {$$ = A_Fieldl($1, NULL); }
	|	field COMMA fieldl {$$ = A_Fieldl($1, $3); }
	;
fieldd:	id COLON id ASSIGN exp {$$ = A_Fieldd($1, $3, $5); }
	;
fielddl:	fieldd COMMA fielddl {$$ = A_Fielddl($1, $3); }
		|	fieldd {$$ = A_Fielddl($1, NULL); }
		;
threaddec: THREAD id LPAREN RPAREN COLON stmlist END {$$ = A_ThreadDec($2, $6);}

ifstm:	IF exp COLON stmlist eliflist END  {$$ = A_NoelseIfstm($2, $4, $5);}
	|	IF exp COLON stmlist END {$$ = A_NoelseIfstm($2, $4, NULL);}
	|	IF exp COLON stmlist ELSE COLON stmlist END {$$ = A_ElseIfstm($2, $4, NULL, $7);}
	|	IF exp COLON stmlist eliflist ELSE COLON stmlist END  {$$ = A_ElseIfstm($2, $4, $5, $8);} 
	;
eliflist:	elifexp {$$ = A_ElifList($1, NULL); }
	|		elifexp eliflist {$$ = A_ElifList($1, $2);}
	;
elifexp:	ELIF exp COLON stmlist  {$$ = A_ElifExp($2, $4); }	
	;
array:	ARRAY LPAREN INT COMMA SINT RPAREN {$$ = A_Iarray($3, NULL);}
	|	ARRAY LPAREN INT COMMA SINT COMMA clist RPAREN {$$ = A_Iarray($3, $7);}
	|	ARRAY LPAREN INT COMMA SFLOAT RPAREN {$$ = A_Farray($3, NULL);}
	|	ARRAY LPAREN INT COMMA SFLOAT COMMA clist RPAREN {$$ = A_Farray($3, $7);}
	|	ARRAY LPAREN INT COMMA SCHAR RPAREN {$$ = A_Carray($3);}
	|	ARRAY LPAREN id RPAREN {$$ = A_Arraycopy($3);}
	;
exp:	exp PLUS exp {$$ = A_OpExp(TOKPOS, A_Op(A_plusOp , $1, $3)); PEXP("plusExp"); }
	|	exp MINUS exp {$$ = A_OpExp(TOKPOS, A_Op(A_minusOp , $1, $3)); PEXP("minusExp"); }
	|	exp TIMES exp {$$ = A_OpExp(TOKPOS, A_Op(A_timesOp , $1, $3)); PEXP("timesExp"); }
	|	exp DIVIDE exp {$$ = A_OpExp(TOKPOS, A_Op(A_divideOp , $1, $3)); PEXP("divExp"); }
	|	MINUS exp %prec UMINUS { $$ = A_ParentExp(TOKPOS, A_OpExp(TOKPOS, A_Op(A_minusOp , A_CnumExp(TOKPOS, A_Cint(0)), $2) ) ); PEXP("minuExp"); }
	|	exp EQ exp {$$ = A_OpExp(TOKPOS, A_Op(A_eqOp , $1, $3)); PEXP("eqExp"); }
	|	exp NEQ exp {$$ = A_OpExp(TOKPOS, A_Op(A_neqOp , $1, $3)); PEXP("neqExp"); }
	|	exp GT exp {$$ = A_OpExp(TOKPOS, A_Op(A_gtOp , $1, $3)); PEXP("gtExp"); }
	|	exp GE exp {$$ = A_OpExp(TOKPOS, A_Op(A_geOp , $1, $3)); PEXP("geExp"); }
	|	exp LT exp {$$ = A_OpExp(TOKPOS, A_Op(A_ltOp , $1, $3)); PEXP("ltExp"); }
	|	exp LE exp {$$ = A_OpExp(TOKPOS, A_Op(A_leOp , $1, $3)); PEXP("leExp"); }
	|	exp AND exp {$$ = A_OpExp(TOKPOS, A_Op(A_andOp , $1, $3)); PEXP("andExp"); }
	|	exp OR exp {$$ = A_OpExp(TOKPOS, A_Op(A_orOp , $1, $3)); PEXP("orExp"); }
	|	exp XOR exp {$$ = A_OpExp(TOKPOS, A_Op(A_xorOp , $1, $3)); PEXP("xorExp"); }
	|	NOT exp {$$ = A_NotExp(TOKPOS, $2); PEXP("notExp"); }
	|	funcall {$$ = A_FuncallExp(TOKPOS, $1); PEXP("funcallExp"); }
	|	threadcall {$$ = A_ThreadcallExp(TOKPOS, $1); PEXP("threadcallExp"); }
	|	LPAREN exp RPAREN {$$ = $2; PEXP("parentExp"); }
	|	variate {$$ = A_VariateExp(TOKPOS, $1); PEXP("variateExp"); }
	|	variate ASSIGN exp {$$ = A_AssignExp(TOKPOS, A_Assign($1, $3)); PEXP("variate_assignExp"); }
	|	cnum {$$ = A_CnumExp(TOKPOS, $1); PEXP("cnumExp"); }
	|	clist {$$ = A_ClistExp(TOKPOS, $1); PEXP("clistExp"); }
	|	STRING {$$ = A_StringExp(TOKPOS, $1); PEXP("stringExp"); }
	|	LBRACK POSE RBRACK {$$ = A_Pose(TOKPOS); PEXP("[POSE_Exp]"); }
	;
funcall:	id LPAREN args RPAREN {$$ = A_Funcall($1, $3);}
	;
args:	exp {$$ = A_Args($1, NULL); }
	|	exp COMMA args {$$ = A_Args($1, $3);}
	| 	{$$ = NULL;}
	;
threadcall:	RUN id LPAREN RPAREN {$$ = $2; }
	;
variate:	id {$$ = A_SimpleVar($1);}
	|		id LBRACK exp RBRACK {$$ = A_SubscriptVar($1, $3);}
	;
clist:	LBRACK cnumlist RBRACK {$$ = $2; }
	;
cnumlist:	cnum {$$ = A_Cnumlist($1, NULL);}
	|		cnum COMMA cnumlist {$$ = A_Cnumlist($1, $3);}
	;	
cnum:	INT {$$ = A_Cint($1);}
	|	DOUBLE {$$ = A_Cdouble($1);}
	|	BOOL {$$ = A_Cint($1);}
	;
id:		ID  {$$ = S_Symbol($1); }
	;