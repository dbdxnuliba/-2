/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "7dof.y"

#include "sysutil.h"
#include "symbol.h"
#include "absyn.h"
#include "errmsg.h"
#include "enventry.h"

#define TOKPOS EM_tokPos

#define PEXP(x) /*printf(x"\n")*/
#define PSTM(x) /*printf(x", line = %d\n", EM_getLineNum(EM_tokPos))*/

A_stmList absyn_root = NULL; //the root of  grammar tree

int yylex(void);
void yyerror(char *s)
{
	EM_error(EM_tokPos, "%s", s);
}


/* Line 371 of yacc.c  */
#line 90 "7dof.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "7dof.tab.h".  */
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
     GLOBAL = 302,
     ATAT = 303,
     PPOUND = 304
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 23 "7dof.y"

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
	E_param param;


/* Line 387 of yacc.c  */
#line 214 "7dof.tab.c"
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

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 242 "7dof.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  60
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   501

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNRULES -- Number of states.  */
#define YYNSTATES  197

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   304

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    12,    15,    21,    24,
      28,    30,    32,    34,    39,    42,    45,    48,    57,    59,
      61,    65,    66,    70,    72,    76,    82,    86,    88,    96,
     103,   109,   118,   128,   130,   133,   138,   145,   154,   161,
     170,   177,   182,   186,   190,   194,   198,   201,   205,   209,
     213,   217,   221,   225,   229,   233,   237,   240,   242,   244,
     248,   250,   254,   256,   258,   260,   264,   269,   271,   275,
     276,   281,   283,   288,   292,   294,   298,   300,   303,   305,
     308,   310,   312,   314,   317,   319,   321,   325,   328,   331,
     333,   336,   341
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      51,     0,    -1,    52,    -1,    74,    -1,    53,    -1,    53,
      52,    -1,    65,    31,    -1,    41,    65,    24,    52,    40,
      -1,    42,    31,    -1,    43,    65,    31,    -1,    54,    -1,
      60,    -1,    61,    -1,    69,     3,    64,    31,    -1,    47,
      73,    -1,     1,    31,    -1,     1,    40,    -1,    44,    73,
      28,    55,    29,    24,    52,    40,    -1,    57,    -1,    59,
      -1,    57,    25,    59,    -1,    -1,    73,    24,    73,    -1,
      56,    -1,    56,    25,    57,    -1,    73,    24,    73,     3,
      65,    -1,    58,    25,    59,    -1,    58,    -1,    45,    73,
      28,    29,    24,    52,    40,    -1,    37,    65,    24,    52,
      62,    40,    -1,    37,    65,    24,    52,    40,    -1,    37,
      65,    24,    52,    39,    24,    52,    40,    -1,    37,    65,
      24,    52,    62,    39,    24,    52,    40,    -1,    63,    -1,
      63,    62,    -1,    38,    65,    24,    52,    -1,    33,    28,
      22,    25,    34,    29,    -1,    33,    28,    22,    25,    34,
      25,    70,    29,    -1,    33,    28,    22,    25,    35,    29,
      -1,    33,    28,    22,    25,    35,    25,    70,    29,    -1,
      33,    28,    22,    25,    36,    29,    -1,    33,    28,    73,
      29,    -1,    65,    14,    65,    -1,    65,    13,    65,    -1,
      65,    16,    65,    -1,    65,    15,    65,    -1,    13,    65,
      -1,    65,    12,    65,    -1,    65,    11,    65,    -1,    65,
       8,    65,    -1,    65,     7,    65,    -1,    65,    10,    65,
      -1,    65,     9,    65,    -1,    65,     6,    65,    -1,    65,
       5,    65,    -1,    65,     4,    65,    -1,    17,    65,    -1,
      66,    -1,    68,    -1,    28,    65,    29,    -1,    69,    -1,
      69,     3,    65,    -1,    72,    -1,    70,    -1,    21,    -1,
      26,    32,    27,    -1,    73,    28,    67,    29,    -1,    65,
      -1,    65,    25,    67,    -1,    -1,    46,    73,    28,    29,
      -1,    73,    -1,    73,    26,    65,    27,    -1,    26,    71,
      27,    -1,    72,    -1,    72,    25,    71,    -1,    22,    -1,
      13,    22,    -1,    23,    -1,    13,    23,    -1,    19,    -1,
      20,    -1,    75,    -1,    75,    74,    -1,    76,    -1,    77,
      -1,    48,    73,    78,    -1,    48,    73,    -1,    49,    73,
      -1,    79,    -1,    79,    78,    -1,    10,    73,    73,     8,
      -1,    10,    73,    73,    24,    65,     8,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   101,   101,   102,   105,   106,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   120,   122,   123,
     124,   125,   127,   129,   130,   132,   134,   135,   137,   139,
     140,   141,   142,   144,   145,   147,   149,   150,   151,   152,
     153,   154,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   181,   183,   184,   185,
     187,   189,   190,   192,   194,   195,   197,   198,   199,   200,
     201,   203,   206,   207,   210,   211,   214,   215,   218,   221,
     222,   225,   226
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ASSIGN", "XOR", "OR", "AND", "GE", "GT",
  "LE", "LT", "NEQ", "EQ", "MINUS", "PLUS", "DIVIDE", "TIMES", "NOT",
  "UMINUS", "BOOL", "ID", "STRING", "INT", "DOUBLE", "COLON", "COMMA",
  "LBRACK", "RBRACK", "LPAREN", "RPAREN", "POUND", "SEMICOLON", "POSE",
  "ARRAY", "SINT", "SFLOAT", "SCHAR", "IF", "ELIF", "ELSE", "END", "WHILE",
  "BREAK", "RETURN", "DEF", "THREAD", "RUN", "GLOBAL", "ATAT", "PPOUND",
  "$accept", "program", "stmlist", "stm", "fundec", "typefields", "field",
  "fieldl", "fieldd", "fielddl", "threaddec", "ifstm", "eliflist",
  "elifexp", "array", "exp", "funcall", "args", "threadcall", "variate",
  "clist", "cnumlist", "cnum", "id", "env_regs", "env_reg", "env_funreg",
  "env_thrreg", "env_pal", "env_pa", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    51,    51,    52,    52,    53,    53,    53,    53,
      53,    53,    53,    53,    53,    53,    53,    54,    55,    55,
      55,    55,    56,    57,    57,    58,    59,    59,    60,    61,
      61,    61,    61,    62,    62,    63,    64,    64,    64,    64,
      64,    64,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    66,    67,    67,    67,
      68,    69,    69,    70,    71,    71,    72,    72,    72,    72,
      72,    73,    74,    74,    75,    75,    76,    76,    77,    78,
      78,    79,    79
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     2,     5,     2,     3,
       1,     1,     1,     4,     2,     2,     2,     8,     1,     1,
       3,     0,     3,     1,     3,     5,     3,     1,     7,     6,
       5,     8,     9,     1,     2,     4,     6,     8,     6,     8,
       6,     4,     3,     3,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     1,     1,     3,
       1,     3,     1,     1,     1,     3,     4,     1,     3,     0,
       4,     1,     4,     3,     1,     3,     1,     2,     1,     2,
       1,     1,     1,     2,     1,     1,     3,     2,     2,     1,
       2,     4,     6
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,    80,    81,    64,    76,    78,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     2,     0,    10,    11,    12,     0,    57,    58,
      60,    63,    62,    71,     3,    82,    84,    85,    15,    16,
      76,    78,    46,    60,    56,     0,     0,     0,    74,     0,
       0,     0,     8,     0,     0,     0,     0,    14,    87,    88,
       1,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     6,     0,     0,    69,    83,
       0,    77,    79,    65,    73,     0,    59,     0,     0,     9,
      21,     0,     0,     0,    86,    89,    55,    54,    53,    50,
      49,    52,    51,    48,    47,    43,    42,    45,    44,     0,
       0,    61,     0,    67,     0,    75,     0,     0,     0,    23,
      18,    27,    19,     0,     0,    70,     0,    90,     0,    13,
      72,    69,    66,     0,     0,    30,     0,    33,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    68,     0,
       0,     0,    29,    34,     0,    24,     0,    20,     0,    26,
      22,     0,    91,     0,     0,    41,     0,     0,     0,     0,
       0,     0,     0,    28,     0,     0,     0,     0,    35,    31,
       0,    17,    22,     0,    25,    92,     0,    36,     0,    38,
      40,    32,     0,     0,     0,    37,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    21,    22,    23,    24,   118,   119,   120,   121,   122,
      25,    26,   136,   137,   110,    27,    28,   114,    29,    43,
      31,    47,    32,    33,    34,    35,    36,    37,    94,    95
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -144
static const yytype_int16 yypact[] =
{
     168,   -27,    27,   253,  -144,  -144,  -144,  -144,  -144,    19,
     253,   253,   253,   -24,   253,    13,    13,    13,    13,    13,
      13,    52,  -144,   206,  -144,  -144,  -144,   296,  -144,  -144,
      51,  -144,  -144,     8,  -144,     9,  -144,  -144,  -144,  -144,
    -144,  -144,  -144,    53,  -144,    58,    32,    55,    65,   337,
     409,   430,  -144,   309,    63,    67,    68,  -144,    92,  -144,
    -144,  -144,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,  -144,    88,   253,   253,  -144,
     253,  -144,  -144,  -144,  -144,    64,  -144,   241,   241,  -144,
      13,    69,    74,    13,  -144,    92,   282,   282,   282,   321,
     321,   321,   321,   321,   321,    10,    10,  -144,  -144,    76,
      75,   472,   363,   387,    83,  -144,    -9,    73,    86,    93,
      94,    95,  -144,    98,    99,  -144,    13,  -144,    17,  -144,
    -144,   253,  -144,   253,   100,  -144,    45,    79,  -144,   101,
      13,    13,    13,    13,   241,     3,   102,    97,  -144,   451,
     241,   104,  -144,  -144,   241,  -144,   109,  -144,   111,  -144,
     133,   103,  -144,   253,   -19,  -144,   241,   108,   241,   110,
      13,    13,   253,  -144,   485,   -23,   -11,   112,  -144,  -144,
     113,  -144,  -144,   133,   472,   253,   114,  -144,   114,  -144,
    -144,  -144,    64,   122,   123,  -144,  -144
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -144,  -144,    12,  -144,  -144,  -144,  -144,    -3,  -144,   -53,
    -144,  -144,     1,  -144,  -144,    -2,  -144,    11,  -144,     5,
    -143,    54,    -6,     4,   119,  -144,  -144,  -144,    62,  -144
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -5
static const yytype_int16 yytable[] =
{
      42,    44,   186,    48,    38,    30,   187,    52,    49,    50,
      51,   162,    53,    39,   188,   175,   176,   177,   189,    54,
      55,    56,    57,    58,    59,    73,    74,   163,    30,   133,
     134,   135,    45,     5,    77,    61,    78,     5,     4,   146,
       2,     7,     8,   193,     3,   194,     4,     5,     6,    40,
      41,    46,    60,     9,    76,    10,    80,    19,    20,    83,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,    17,   111,   112,   113,    45,   111,    48,
      81,    82,    84,     4,   151,   152,     7,     8,   157,   159,
      85,    90,    30,    30,   123,    91,    92,   126,   124,   116,
     117,     2,    93,   125,   128,     3,   129,     4,     5,     6,
       7,     8,   132,   138,     9,   139,    10,   133,   140,   141,
     142,   109,   143,   144,   150,   154,   165,   164,   168,   113,
     145,   149,   147,   170,    17,   171,   172,   155,   153,   115,
     192,   190,   148,   173,   156,   158,   158,   160,   179,    30,
     181,   195,   196,   191,    79,    30,   161,   127,     0,    30,
       0,   174,   167,     0,     0,     0,   169,     0,     0,     1,
     184,    30,     0,    30,   182,   183,     0,     0,   178,     0,
     180,     2,     0,   100,     0,     3,    48,     4,     5,     6,
       7,     8,     0,     0,     9,     0,    10,     0,     0,     0,
       0,     0,     0,     0,     0,    11,    -4,     1,     0,    12,
      13,    14,    15,    16,    17,    18,    19,    20,     0,     2,
       0,     0,     0,     3,     0,     4,     5,     6,     7,     8,
       0,     0,     9,     0,    10,     0,     0,     0,     0,     0,
       0,     0,     1,    11,    -4,    -4,    -4,    12,    13,    14,
      15,    16,    17,    18,     2,     0,     0,     0,     3,     0,
       4,     5,     6,     7,     8,     0,     2,     9,     0,    10,
       3,     0,     4,     5,     6,     7,     8,     0,    11,     9,
       0,    10,    12,    13,    14,    15,    16,    17,    18,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    17,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,     0,    75,    -5,    -5,
      -5,    -5,    -5,    -5,    71,    72,    73,    74,     0,     0,
      89,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    86,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     130,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,     0,     0,     0,     0,     0,     0,
       0,     0,   131,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,     0,     0,     0,     0,
       0,     0,     0,    87,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,     0,     0,     0,
       0,     0,     0,     0,    88,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,     0,     0,
       0,     0,     0,     0,     0,   166,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    62,
      63,    64,    65,   185,    67,    68,    69,    70,    71,    72,
      73,    74
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-144)))

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-5)))

static const yytype_int16 yycheck[] =
{
       2,     3,    25,     9,    31,     0,    29,    31,    10,    11,
      12,     8,    14,    40,    25,    34,    35,    36,    29,    15,
      16,    17,    18,    19,    20,    15,    16,    24,    23,    38,
      39,    40,    13,    20,    26,    23,    28,    20,    19,    22,
      13,    22,    23,   186,    17,   188,    19,    20,    21,    22,
      23,    32,     0,    26,     3,    28,     3,    48,    49,    27,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    46,    76,    77,    78,    13,    80,    85,
      22,    23,    27,    19,    39,    40,    22,    23,   141,   142,
      25,    28,    87,    88,    90,    28,    28,    93,    29,    87,
      88,    13,    10,    29,    28,    17,    31,    19,    20,    21,
      22,    23,    29,    40,    26,    29,    28,    38,    25,    25,
      25,    33,    24,    24,    24,    24,    29,    25,    24,   131,
     126,   133,   128,    24,    46,    24,     3,   140,   137,    85,
      26,    29,   131,    40,   140,   141,   142,   143,    40,   144,
      40,    29,    29,    40,    35,   150,   144,    95,    -1,   154,
      -1,   163,   150,    -1,    -1,    -1,   154,    -1,    -1,     1,
     172,   166,    -1,   168,   170,   171,    -1,    -1,   166,    -1,
     168,    13,    -1,   185,    -1,    17,   192,    19,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,     0,     1,    -1,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    -1,    13,
      -1,    -1,    -1,    17,    -1,    19,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    28,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    13,    -1,    -1,    -1,    17,    -1,
      19,    20,    21,    22,    23,    -1,    13,    26,    -1,    28,
      17,    -1,    19,    20,    21,    22,    23,    -1,    37,    26,
      -1,    28,    41,    42,    43,    44,    45,    46,    47,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    46,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    -1,    31,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
      31,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    29,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    24,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    24,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    24,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    13,    17,    19,    20,    21,    22,    23,    26,
      28,    37,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    51,    52,    53,    54,    60,    61,    65,    66,    68,
      69,    70,    72,    73,    74,    75,    76,    77,    31,    40,
      22,    23,    65,    69,    65,    13,    32,    71,    72,    65,
      65,    65,    31,    65,    73,    73,    73,    73,    73,    73,
       0,    52,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    31,     3,    26,    28,    74,
       3,    22,    23,    27,    27,    25,    29,    24,    24,    31,
      28,    28,    28,    10,    78,    79,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    33,
      64,    65,    65,    65,    67,    71,    52,    52,    55,    56,
      57,    58,    59,    73,    29,    29,    73,    78,    28,    31,
      27,    25,    29,    38,    39,    40,    62,    63,    40,    29,
      25,    25,    25,    24,    24,    73,    22,    73,    67,    65,
      24,    39,    40,    62,    24,    57,    73,    59,    73,    59,
      73,    52,     8,    24,    25,    29,    24,    52,    24,    52,
      24,    24,     3,    40,    65,    34,    35,    36,    52,    40,
      52,    40,    73,    73,    65,     8,    25,    29,    25,    29,
      29,    40,    26,    70,    70,    29,    29
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 101 "7dof.y"
    {absyn_root = (yyvsp[(1) - (1)].stml) ;}
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 105 "7dof.y"
    {(yyval.stml) = A_StmList((yyvsp[(1) - (1)].stm), NULL); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 106 "7dof.y"
    {(yyval.stml) = A_StmList((yyvsp[(1) - (2)].stm), (yyvsp[(2) - (2)].stml)); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 108 "7dof.y"
    {(yyval.stm) = A_ExpStm(TOKPOS, (yyvsp[(1) - (2)].e)); PSTM("Se");}
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 109 "7dof.y"
    { (yyval.stm) = A_WhileStm(TOKPOS, A_While((yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].stml))); PSTM("Swhl");}
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 110 "7dof.y"
    {(yyval.stm) = A_BreakStm(TOKPOS); PSTM("Sbk");}
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 111 "7dof.y"
    {(yyval.stm) = A_ReturnStm(TOKPOS, (yyvsp[(2) - (3)].e)); PSTM("Sre");}
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 112 "7dof.y"
    {(yyval.stm) = A_FunDecStm(TOKPOS, (yyvsp[(1) - (1)].fund)); PSTM("Sfd");}
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 113 "7dof.y"
    {(yyval.stm) = A_ThreadDecStm(TOKPOS, (yyvsp[(1) - (1)].thrd)); PSTM("Std");}
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 114 "7dof.y"
    {(yyval.stm) = A_IfStm(TOKPOS, (yyvsp[(1) - (1)].iff)); PSTM("Sif");}
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 115 "7dof.y"
    {(yyval.stm) = A_ArraydefStm(TOKPOS, A_Arraydef((yyvsp[(1) - (4)].var), (yyvsp[(3) - (4)].arrt))); PSTM("Sarrd"); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 116 "7dof.y"
    {(yyval.stm) = A_GlobalStm(TOKPOS, (yyvsp[(2) - (2)].sym)); PSTM("Sglo"); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 117 "7dof.y"
    {(yyval.stm) = NULL;  yyclearin ; yyerrok; /*printf("Enter another command:\n");*/}
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 118 "7dof.y"
    {(yyval.stm) = NULL; yyclearin ; yyerrok; /*printf("Enter another command:\n");*/}
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 120 "7dof.y"
    {(yyval.fund) = A_FunDec((yyvsp[(2) - (8)].sym), (yyvsp[(4) - (8)].tyf), (yyvsp[(7) - (8)].stml)); }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 122 "7dof.y"
    {(yyval.tyf) = A_TypeFields((yyvsp[(1) - (1)].fl), NULL); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 123 "7dof.y"
    {(yyval.tyf) = A_TypeFields(NULL, (yyvsp[(1) - (1)].fdl)); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 124 "7dof.y"
    {(yyval.tyf) = A_TypeFields((yyvsp[(1) - (3)].fl), (yyvsp[(3) - (3)].fdl)); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 125 "7dof.y"
    {(yyval.tyf) = NULL; }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 127 "7dof.y"
    {(yyval.f) = A_Field((yyvsp[(1) - (3)].sym), (yyvsp[(3) - (3)].sym)); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 129 "7dof.y"
    {(yyval.fl) = A_Fieldl((yyvsp[(1) - (1)].f), NULL); }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 130 "7dof.y"
    {(yyval.fl) = A_Fieldl((yyvsp[(1) - (3)].f), (yyvsp[(3) - (3)].fl)); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 132 "7dof.y"
    {(yyval.fd) = A_Fieldd((yyvsp[(1) - (5)].sym), (yyvsp[(3) - (5)].sym), (yyvsp[(5) - (5)].e)); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 134 "7dof.y"
    {(yyval.fdl) = A_Fielddl((yyvsp[(1) - (3)].fd), (yyvsp[(3) - (3)].fdl)); }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 135 "7dof.y"
    {(yyval.fdl) = A_Fielddl((yyvsp[(1) - (1)].fd), NULL); }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 137 "7dof.y"
    {(yyval.thrd) = A_ThreadDec((yyvsp[(2) - (7)].sym), (yyvsp[(6) - (7)].stml));}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 139 "7dof.y"
    {(yyval.iff) = A_NoelseIfstm((yyvsp[(2) - (6)].e), (yyvsp[(4) - (6)].stml), (yyvsp[(5) - (6)].elifl));}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 140 "7dof.y"
    {(yyval.iff) = A_NoelseIfstm((yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].stml), NULL);}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 141 "7dof.y"
    {(yyval.iff) = A_ElseIfstm((yyvsp[(2) - (8)].e), (yyvsp[(4) - (8)].stml), NULL, (yyvsp[(7) - (8)].stml));}
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 142 "7dof.y"
    {(yyval.iff) = A_ElseIfstm((yyvsp[(2) - (9)].e), (yyvsp[(4) - (9)].stml), (yyvsp[(5) - (9)].elifl), (yyvsp[(8) - (9)].stml));}
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 144 "7dof.y"
    {(yyval.elifl) = A_ElifList((yyvsp[(1) - (1)].elif), NULL); }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 145 "7dof.y"
    {(yyval.elifl) = A_ElifList((yyvsp[(1) - (2)].elif), (yyvsp[(2) - (2)].elifl));}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 147 "7dof.y"
    {(yyval.elif) = A_ElifExp((yyvsp[(2) - (4)].e), (yyvsp[(4) - (4)].stml)); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 149 "7dof.y"
    {(yyval.arrt) = A_Iarray((yyvsp[(3) - (6)].ival), NULL);}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 150 "7dof.y"
    {(yyval.arrt) = A_Iarray((yyvsp[(3) - (8)].ival), (yyvsp[(7) - (8)].cnuml));}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 151 "7dof.y"
    {(yyval.arrt) = A_Farray((yyvsp[(3) - (6)].ival), NULL);}
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 152 "7dof.y"
    {(yyval.arrt) = A_Farray((yyvsp[(3) - (8)].ival), (yyvsp[(7) - (8)].cnuml));}
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 153 "7dof.y"
    {(yyval.arrt) = A_Carray((yyvsp[(3) - (6)].ival));}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 154 "7dof.y"
    {(yyval.arrt) = A_Arraycopy((yyvsp[(3) - (4)].sym));}
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 156 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_plusOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E+"); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 157 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_minusOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E-"); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 158 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_timesOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E*"); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 159 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_divideOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E/"); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 160 "7dof.y"
    { (yyval.e) = A_OpExp(TOKPOS, A_Op(A_minusOp , A_CnumExp(TOKPOS, A_Cint(0)), (yyvsp[(2) - (2)].e)) ); PEXP("-E"); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 161 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_eqOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E=="); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 162 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_neqOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E!="); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 163 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_gtOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E>"); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 164 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_geOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E>="); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 165 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_ltOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E<"); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 166 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_leOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("E<="); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 167 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_andOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("Eand"); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 168 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_orOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("Eor"); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 169 "7dof.y"
    {(yyval.e) = A_OpExp(TOKPOS, A_Op(A_xorOp , (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e))); PEXP("Exor"); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 170 "7dof.y"
    {(yyval.e) = A_NotExp(TOKPOS, (yyvsp[(2) - (2)].e)); PEXP("Enot"); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 171 "7dof.y"
    {(yyval.e) = A_FuncallExp(TOKPOS, (yyvsp[(1) - (1)].func)); PEXP("Efc"); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 172 "7dof.y"
    {(yyval.e) = A_ThreadcallExp(TOKPOS, (yyvsp[(1) - (1)].sym)); PEXP("Etc"); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 173 "7dof.y"
    {(yyval.e) = (yyvsp[(2) - (3)].e); PEXP("E()"); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 174 "7dof.y"
    {(yyval.e) = A_VariateExp(TOKPOS, (yyvsp[(1) - (1)].var)); PEXP("Evar"); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 175 "7dof.y"
    {(yyval.e) = A_AssignExp(TOKPOS, A_Assign((yyvsp[(1) - (3)].var), (yyvsp[(3) - (3)].e))); PEXP("Evara"); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 176 "7dof.y"
    {(yyval.e) = A_CnumExp(TOKPOS, (yyvsp[(1) - (1)].cnum)); PEXP("Ecnum"); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 177 "7dof.y"
    {(yyval.e) = A_ClistExp(TOKPOS, (yyvsp[(1) - (1)].cnuml)); PEXP("Eclist"); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 178 "7dof.y"
    {(yyval.e) = A_StringExp(TOKPOS, (yyvsp[(1) - (1)].sval)); PEXP("Estr"); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 179 "7dof.y"
    {(yyval.e) = A_Pose(TOKPOS); PEXP("E[P]"); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 181 "7dof.y"
    {(yyval.func) = A_Funcall((yyvsp[(1) - (4)].sym), (yyvsp[(3) - (4)].args));}
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 183 "7dof.y"
    {(yyval.args) = A_Args((yyvsp[(1) - (1)].e), NULL); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 184 "7dof.y"
    {(yyval.args) = A_Args((yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].args));}
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 185 "7dof.y"
    {(yyval.args) = NULL;}
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 187 "7dof.y"
    {(yyval.sym) = (yyvsp[(2) - (4)].sym); }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 189 "7dof.y"
    {(yyval.var) = A_SimpleVar((yyvsp[(1) - (1)].sym));}
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 190 "7dof.y"
    {(yyval.var) = A_SubscriptVar((yyvsp[(1) - (4)].sym), (yyvsp[(3) - (4)].e));}
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 192 "7dof.y"
    {(yyval.cnuml) = (yyvsp[(2) - (3)].cnuml); }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 194 "7dof.y"
    {(yyval.cnuml) = A_Cnumlist((yyvsp[(1) - (1)].cnum), NULL);}
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 195 "7dof.y"
    {(yyval.cnuml) = A_Cnumlist((yyvsp[(1) - (3)].cnum), (yyvsp[(3) - (3)].cnuml));}
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 197 "7dof.y"
    {(yyval.cnum) = A_Cint((yyvsp[(1) - (1)].ival));}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 198 "7dof.y"
    {(yyval.cnum) = A_Cint(0 - (yyvsp[(2) - (2)].ival));}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 199 "7dof.y"
    {(yyval.cnum) = A_Cdouble((yyvsp[(1) - (1)].dval));}
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 200 "7dof.y"
    {(yyval.cnum) = A_Cdouble(0 - (yyvsp[(2) - (2)].dval));}
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 201 "7dof.y"
    {(yyval.cnum) = A_Cint((yyvsp[(1) - (1)].bval));}
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 203 "7dof.y"
    {(yyval.sym) = S_Symbol((yyvsp[(1) - (1)].sval)); }
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 214 "7dof.y"
    { E_enter_SysFE_REG((yyvsp[(2) - (3)].sym), (yyvsp[(3) - (3)].param)); }
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 215 "7dof.y"
    { E_enter_SysFE_REG((yyvsp[(2) - (2)].sym), NULL); }
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 218 "7dof.y"
    { }
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 221 "7dof.y"
    { (yyval.param) = (yyvsp[(1) - (1)].param); }
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 222 "7dof.y"
    { (yyvsp[(1) - (2)].param)->next = (yyvsp[(2) - (2)].param); (yyval.param) = (yyvsp[(1) - (2)].param); }
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 225 "7dof.y"
    { (yyval.param) = E_Param_REG((yyvsp[(2) - (4)].sym), (yyvsp[(3) - (4)].sym), NULL, NULL); }
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 226 "7dof.y"
    { (yyval.param) = E_Param_REG((yyvsp[(2) - (6)].sym), (yyvsp[(3) - (6)].sym), (yyvsp[(5) - (6)].e), NULL);}
    break;


/* Line 1792 of yacc.c  */
#line 2182 "7dof.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


