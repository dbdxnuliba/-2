%{

///////////////////////////////////////////////////////////////////////////////////////////
// 
// Date: 2017/03/24
// Description: Lexical Rules
// Environment: flex(gnu) / win-flex
//
///////////////////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "sysutil.h"
#include "symbol.h"
#include "absyn.h"
#include "errmsg.h"
#include "enventry.h"
#include "7dof.tab.h"


extern char myinput[];
extern char *myinputptr;
extern char *myinputlim;

#define YY_NO_UNISTD_H
#define ADJ adjust()

int charPos = 1;

void adjust()
{
	EM_tokPos = charPos;
	charPos += yyleng;
}

//
// change default input stream
//
 
#undef YY_INPUT
#define YY_INPUT(b, r, ms) {r = my_yyinput(b, ms);}
int my_yyinput(char *buf, int max_size)
{
	int n = min(max_size, myinputlim - myinputptr);
	if(n > 0){
		memcpy(buf, myinputptr, n);
		myinputptr += n;
	}
	return n;
}

int yywrap(void){ 
	charPos=1;
	return 1; 
}

//
// this part is used to save the string in pair of " "
//
 
#define OVER_MEM_ERR printf("%s (max_length: %d)","usage:string out of memrory!",MAX_LENGTH);\
		exit(1)
#define MAX_LENGTH 512

static char *str_ptr;
static char str[MAX_LENGTH];
static char ch;
static int remain;

void init_string()
{
	remain = MAX_LENGTH - 1;
	str_ptr = str;
}

void append_char_to_string(int ch)
{
	if(!remain){
		OVER_MEM_ERR;
	}
	*str_ptr++ = ch;
	remain--;
}

void append_str_to_string(char *s)
{
	int l = strlen(s);
	if(l > remain){
		OVER_MEM_ERR;
	}
	while(*str_ptr++ = *s++, *s)
		;
	remain -= 1;
}

void end_string(void)
{
	if(!remain){
		OVER_MEM_ERR;
	}
	*str_ptr++ = '\0';
}

%}

%x string
id [A-Za-z][_A-Za-z0-9]*
digits [0-9]+
ws	[ \t]+

%%
{ws}		{ADJ; continue;}
\n			{ADJ; EM_newline(); continue;}
":"			{ADJ; return COLON;}
","			{ADJ; return COMMA;}
"["			{ADJ; return LBRACK;}
"]"			{ADJ; return RBRACK;}
"("			{ADJ; return LPAREN;}
")"			{ADJ; return RPAREN;}
"#"			{ADJ; return POUND;}
";"			{ADJ; return SEMICOLON;}
"+"			{ADJ; return PLUS;}
"-"			{ADJ; return MINUS;}
"*"			{ADJ; return TIMES;}
"/"			{ADJ; return DIVIDE;}
"<"			{ADJ; return LT;}
"<="		{ADJ; return LE;}
">"			{ADJ; return GT;}
">="		{ADJ; return GE;} 
"=="		{ADJ; return EQ;}
"!="		{ADJ; return NEQ;}
"="			{ADJ; return ASSIGN;}
"@@"		{ADJ; return ATAT;}
"##"		{ADJ; return PPOUND;}
POSE		{ADJ; return POSE;}
Array		{ADJ; return ARRAY;}
INT			{ADJ; return SINT;}
FLO			{ADJ; return SFLOAT;}
CHAR		{ADJ; return SCHAR;}
and			{ADJ; return AND;}
or			{ADJ; return OR;}
xor			{ADJ; return XOR;}
not			{ADJ; return NOT;}
if			{ADJ; return IF;}
elif		{ADJ; return ELIF;}
else		{ADJ; return ELSE;}
end			{ADJ; return END;}
while		{ADJ; return WHILE;}
break		{ADJ; return BREAK;}
return		{ADJ; return RETURN;}
def			{ADJ; return DEF;}
thread		{ADJ; return THREAD;}
run			{ADJ; return RUN;}
global		{ADJ; return GLOBAL;}
True		{ADJ; yylval.bval = TRUE; yylval.bval = TRUE; return BOOL;}
False		{ADJ; yylval.bval = FALSE; yylval.bval = FALSE; return BOOL;}
{id}		{ADJ; yylval.sval = yytext;return ID;}
{digits}	{ADJ; yylval.ival=atoi(yytext); return INT;}
{digits}(\.{digits})	{ADJ; yylval.dval = atof(yytext); return DOUBLE;}
(#.*)/\n	{ADJ; continue;}
(#.*) 		{ADJ; continue;}
\"			{ADJ; init_string(); BEGIN string;}
<string>{
\\			{ADJ; append_char_to_string(0x5c);}
"\\\""		{ADJ; append_char_to_string(0x22);}
\\n			{ADJ; append_char_to_string(0x0A);}
\\t			{ADJ; append_char_to_string(0x09);}
\\[0-9][3]	{ADJ; append_char_to_string(atoi(yytext));}
\"			{ADJ; end_string(); yylval.sval = _strdup(str);  BEGIN 0; return STRING;}
\n			{ADJ; continue;}
{ws}		{ADJ; append_str_to_string(yytext);}
[^\\" \t\n]+	{ADJ; append_str_to_string(yytext);}
}
.			{ADJ; EM_error(EM_tokPos,"illegal token");}
%%