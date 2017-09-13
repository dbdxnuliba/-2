/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_7DOF_TAB_H_INCLUDED
# define YY_YY_7DOF_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ASSIGN = 258,
     XOR = 259,
     OR = 260,
     AND = 261,
     GE = 262,
     GT = 263,
     LE = 264,
     LT = 265,
     NEQ = 266,
     EQ = 267,
     MINUS = 268,
     PLUS = 269,
     DIVIDE = 270,
     TIMES = 271,
     NOT = 272,
     UMINUS = 273,
     BOOL = 274,
     ID = 275,
     STRING = 276,
     INT = 277,
     DOUBLE = 278,
     COLON = 279,
     COMMA = 280,
     LBRACK = 281,
     RBRACK = 282,
     LPAREN = 283,
     RPAREN = 284,
     POUND = 285,
     SEMICOLON = 286,
     POSE = 287,
     ARRAY = 288,
     SINT = 289,
     SFLOAT = 290,
     SCHAR = 291,
     IF = 292,
     ELIF = 293,
     ELSE = 294,
     END = 295,
     WHILE = 296,
     BREAK = 297,
     RETURN = 298,
     DEF = 299,
     THREAD = 300,
     RUN = 301,
     GLOBAL = 302
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 24 "7dof.y"

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


/* Line 2058 of yacc.c  */
#line 135 "7dof.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_7DOF_TAB_H_INCLUDED  */
