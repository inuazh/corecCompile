/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "corec.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "corec.tab.h"

extern int yylineno; 
int yylex();           
void yyerror(const char* s); 

FILE *output_file; 

// -------------------------------------------------
// Таблица символов
typedef enum { SYM_VAR, SYM_ARR /* , SYM_FUNC, ... */ } SymType;

typedef struct Symbol {
    char* name;
    SymType kind;
    struct Symbol* next;
} Symbol;

Symbol* symbolTable = NULL;

Symbol* addSymbol(const char* name, SymType kind) {
    Symbol* p = symbolTable;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
        p = p->next;
    }
    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->kind = kind;
    s->next = symbolTable;
    symbolTable = s;
    return s;
}

extern int yylineno; 
int yylex();           
void yyerror(const char* s);

FILE *output_file; 

// -------------------------------------------------
// Для вывода в MIPS
// -------------------------------------------------
void emitDataSection(FILE* out) {
    fprintf(out, ".data\n");
    Symbol* p = symbolTable;
    while (p) {
        if (p->kind == SYM_VAR) {
            fprintf(out, "%s: .word 0\n", p->name);
        }
        else if (p->kind == SYM_ARR) {
            fprintf(out, "%s: .space 40\n", p->name);
        }
        p = p->next;
    }
    // Остальную инфо добавим позже
}

// Генерация MIPS для =, +=, -=, ...
void genAssign(const char* varName) {
    fprintf(output_file, "# %s = <expr>\n", varName);
    fprintf(output_file, "sw $t0, %s\n", varName);
}

void genPlusAssign(const char* varName) {
    fprintf(output_file, "# %s += <expr>\n", varName);
    fprintf(output_file, "lw $t1, %s\n", varName);
    fprintf(output_file, "add $t1, $t1, $t0\n");
    fprintf(output_file, "sw $t1, %s\n", varName);
}

void genMinusAssign(const char* varName) {
    fprintf(output_file, "# %s -= <expr>\n", varName);
    fprintf(output_file, "lw $t1, %s\n", varName);
    fprintf(output_file, "sub $t1, $t1, $t0\n");
    fprintf(output_file, "sw $t1, %s\n", varName);
}

void genMultAssign(const char* varName) {
    fprintf(output_file, "# %s *= <expr>\n", varName);
    fprintf(output_file, "lw $t1, %s\n", varName);
    fprintf(output_file, "mul $t1, $t1, $t0\n");
    fprintf(output_file, "sw $t1, %s\n", varName);
}

void genDivAssign(const char* varName) {
    fprintf(output_file, "# %s /= <expr>\n", varName);
    fprintf(output_file, "lw $t1, %s\n", varName);
    fprintf(output_file, "div $t1, $t0\n");
    fprintf(output_file, "mflo $t1\n");
    fprintf(output_file, "sw $t1, %s\n", varName);
}

void genRead(const char* varName) {
    fprintf(output_file, "# read %s\n", varName);
    fprintf(output_file, "li $v0, 5\n");
    fprintf(output_file, "syscall\n");
    fprintf(output_file, "sw $v0, %s\n", varName);
}

void genPrint(const char* varName) {
    fprintf(output_file, "# print %s\n", varName);
    fprintf(output_file, "lw $a0, %s\n", varName);
    fprintf(output_file, "li $v0, 1\n");
    fprintf(output_file, "syscall\n");
    // перевод строки
    fprintf(output_file, "la $a0, newline\n");
    fprintf(output_file, "li $v0, 4\n");
    fprintf(output_file, "syscall\n");
}

void genPrintStr(const char* str) {
    static int strCount = 0;
    char label[32];
    sprintf(label, "strlbl_%d", strCount++);
    fprintf(output_file, ".data\n");
    fprintf(output_file, "%s: .asciiz \"%s\"\n", label, str);
    fprintf(output_file, ".text\n");
    fprintf(output_file, "# print string\n");
    fprintf(output_file, "la $a0, %s\n", label);
    fprintf(output_file, "li $v0, 4\n");
    fprintf(output_file, "syscall\n");
}

enum {
    OP_NONE=0,
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV
};

// Вспомогательные функции для склейки строк
static char* concat2(const char* s1, const char* s2) {
    if (!s1) s1 = "";
    if (!s2) s2 = "";
    size_t len = strlen(s1) + strlen(s2) + 1;
    char* result = (char*)malloc(len);
    if (!result) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

static char* concat3(const char* s1, const char* s2, const char* s3) {
    char* tmp = concat2(s1, s2);
    char* out = concat2(tmp, s3);
    free(tmp);
    return out;
}


#line 234 "corec.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "corec.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_LOC = 3,                        /* LOC  */
  YYSYMBOL_NUMBER = 4,                     /* NUMBER  */
  YYSYMBOL_FLOTTANT = 5,                   /* FLOTTANT  */
  YYSYMBOL_ID = 6,                         /* ID  */
  YYSYMBOL_CHAINE = 7,                     /* CHAINE  */
  YYSYMBOL_PROG = 8,                       /* PROG  */
  YYSYMBOL_DEF = 9,                        /* DEF  */
  YYSYMBOL_MAIN = 10,                      /* MAIN  */
  YYSYMBOL_DOM = 11,                       /* DOM  */
  YYSYMBOL_REC = 12,                       /* REC  */
  YYSYMBOL_IN = 13,                        /* IN  */
  YYSYMBOL_IN_SIMPLE = 14,                 /* IN_SIMPLE  */
  YYSYMBOL_OUT = 15,                       /* OUT  */
  YYSYMBOL_INOUT = 16,                     /* INOUT  */
  YYSYMBOL_PRINTSTR = 17,                  /* PRINTSTR  */
  YYSYMBOL_PRINT = 18,                     /* PRINT  */
  YYSYMBOL_READ = 19,                      /* READ  */
  YYSYMBOL_EQUALS = 20,                    /* EQUALS  */
  YYSYMBOL_SEMICOLON = 21,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 22,                     /* COMMA  */
  YYSYMBOL_LBRACE = 23,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 24,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 25,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 26,                  /* RBRACKET  */
  YYSYMBOL_LPAREN = 27,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 28,                    /* RPAREN  */
  YYSYMBOL_PLUS = 29,                      /* PLUS  */
  YYSYMBOL_MINUS = 30,                     /* MINUS  */
  YYSYMBOL_MULT = 31,                      /* MULT  */
  YYSYMBOL_DIV = 32,                       /* DIV  */
  YYSYMBOL_QUESTION = 33,                  /* QUESTION  */
  YYSYMBOL_COLON = 34,                     /* COLON  */
  YYSYMBOL_DOTDOT = 35,                    /* DOTDOT  */
  YYSYMBOL_PERCENT = 36,                   /* PERCENT  */
  YYSYMBOL_EQUAL = 37,                     /* EQUAL  */
  YYSYMBOL_LT = 38,                        /* LT  */
  YYSYMBOL_GT = 39,                        /* GT  */
  YYSYMBOL_LE = 40,                        /* LE  */
  YYSYMBOL_GE = 41,                        /* GE  */
  YYSYMBOL_EQ = 42,                        /* EQ  */
  YYSYMBOL_YYACCEPT = 43,                  /* $accept  */
  YYSYMBOL_program = 44,                   /* program  */
  YYSYMBOL_lfonc = 45,                     /* lfonc  */
  YYSYMBOL_fonc = 46,                      /* fonc  */
  YYSYMBOL_loc_section = 47,               /* loc_section  */
  YYSYMBOL_ldecl = 48,                     /* ldecl  */
  YYSYMBOL_decl = 49,                      /* decl  */
  YYSYMBOL_array = 50,                     /* array  */
  YYSYMBOL_dlist = 51,                     /* dlist  */
  YYSYMBOL_args = 52,                      /* args  */
  YYSYMBOL_io_sections = 53,               /* io_sections  */
  YYSYMBOL_sectionlist = 54,               /* sectionlist  */
  YYSYMBOL_section = 55,                   /* section  */
  YYSYMBOL_in_section = 56,                /* in_section  */
  YYSYMBOL_out_section = 57,               /* out_section  */
  YYSYMBOL_inout_section = 58,             /* inout_section  */
  YYSYMBOL_param_list = 59,                /* param_list  */
  YYSYMBOL_param = 60,                     /* param  */
  YYSYMBOL_id_list = 61,                   /* id_list  */
  YYSYMBOL_dom_section = 62,               /* dom_section  */
  YYSYMBOL_domlist = 63,                   /* domlist  */
  YYSYMBOL_d = 64,                         /* d  */
  YYSYMBOL_rec_section = 65,               /* rec_section  */
  YYSYMBOL_blockinst = 66,                 /* blockinst  */
  YYSYMBOL_listei = 67,                    /* listei  */
  YYSYMBOL_i = 68,                         /* i  */
  YYSYMBOL_op = 69,                        /* op  */
  YYSYMBOL_call = 70,                      /* call  */
  YYSYMBOL_lcall = 71,                     /* lcall  */
  YYSYMBOL_precinst = 72,                  /* precinst  */
  YYSYMBOL_arrayref = 73,                  /* arrayref  */
  YYSYMBOL_elist = 74,                     /* elist  */
  YYSYMBOL_e = 75,                         /* e  */
  YYSYMBOL_t = 76,                         /* t  */
  YYSYMBOL_f = 77,                         /* f  */
  YYSYMBOL_cond = 78,                      /* cond  */
  YYSYMBOL_b = 79,                         /* b  */
  YYSYMBOL_oprel = 80                      /* oprel  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
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
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   213

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  91
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  171

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   297


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   198,   198,   219,   220,   224,   234,   235,   236,   240,
     241,   245,   249,   256,   263,   267,   268,   273,   274,   278,
     282,   283,   287,   288,   289,   293,   297,   301,   305,   306,
     310,   311,   315,   316,   320,   321,   322,   326,   327,   331,
     335,   339,   340,   343,   344,   345,   350,   362,   379,   384,
     388,   392,   393,   394,   395,   400,   401,   402,   403,   404,
     408,   409,   410,   414,   415,   416,   417,   421,   426,   433,
     434,   439,   448,   457,   462,   469,   478,   487,   496,   503,
     507,   513,   519,   523,   527,   534,   538,   542,   543,   544,
     545,   546
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "LOC", "NUMBER",
  "FLOTTANT", "ID", "CHAINE", "PROG", "DEF", "MAIN", "DOM", "REC", "IN",
  "IN_SIMPLE", "OUT", "INOUT", "PRINTSTR", "PRINT", "READ", "EQUALS",
  "SEMICOLON", "COMMA", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET",
  "LPAREN", "RPAREN", "PLUS", "MINUS", "MULT", "DIV", "QUESTION", "COLON",
  "DOTDOT", "PERCENT", "EQUAL", "LT", "GT", "LE", "GE", "EQ", "$accept",
  "program", "lfonc", "fonc", "loc_section", "ldecl", "decl", "array",
  "dlist", "args", "io_sections", "sectionlist", "section", "in_section",
  "out_section", "inout_section", "param_list", "param", "id_list",
  "dom_section", "domlist", "d", "rec_section", "blockinst", "listei", "i",
  "op", "call", "lcall", "precinst", "arrayref", "elist", "e", "t", "f",
  "cond", "b", "oprel", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-104)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-60)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       7,    14,     8,     4,  -104,  -104,    22,    78,  -104,    29,
      35,    79,    58,    49,    58,     6,   112,    17,    17,    17,
    -104,    82,   105,  -104,  -104,  -104,  -104,   112,   108,   131,
     138,  -104,  -104,   153,   149,  -104,   156,   141,  -104,   141,
     141,  -104,   137,   149,     5,   142,     6,   151,   144,  -104,
      68,   143,  -104,     2,    17,  -104,   145,  -104,  -104,    51,
       5,     5,  -104,  -104,    38,   115,  -104,     5,  -104,   146,
     153,   109,   147,   148,   150,    36,  -104,  -104,   101,   113,
      41,  -104,   152,   162,  -104,  -104,  -104,     5,     1,   102,
    -104,    12,    12,    12,    12,    12,    21,    38,     5,  -104,
    -104,  -104,  -104,  -104,   158,   163,    -2,   166,    26,  -104,
     159,  -104,  -104,  -104,  -104,  -104,     5,  -104,  -104,    74,
      38,   154,  -104,   169,  -104,    23,  -104,  -104,   115,   115,
    -104,  -104,  -104,     5,  -104,   -16,     5,   155,    62,   157,
     160,   161,    97,  -104,     5,    88,     5,  -104,  -104,     0,
    -104,    38,     5,    38,  -104,  -104,  -104,  -104,  -104,  -104,
      38,    68,    38,  -104,  -104,    80,  -104,   164,  -104,    68,
    -104
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     1,     4,     0,     0,     3,     0,
       0,     0,     8,     0,     8,     7,    36,     0,     0,     0,
      17,     0,    19,    21,    22,    23,    24,    36,    11,     0,
       6,    10,    13,    35,     0,    30,     0,    25,    29,    26,
      27,    18,     0,     0,    60,     0,     0,     0,    34,    38,
      60,     0,    33,     0,     0,    20,     0,    80,    84,    81,
      60,    60,    82,    83,    12,    74,    78,    60,     9,     0,
       0,    81,     0,     0,     0,    60,    40,    42,    82,    83,
       0,    53,     0,     0,    31,    28,     5,    60,    60,     0,
      73,    60,    60,    60,    60,    60,     0,    16,    60,    37,
      55,    56,    57,    58,     0,     0,    60,     0,     0,    45,
       0,    87,    88,    89,    90,    91,    60,     2,    32,     0,
      70,     0,    62,     0,    65,     0,    66,    79,    71,    72,
      75,    76,    77,    60,    14,     0,    60,     0,     0,     0,
       0,     0,    60,    41,    60,     0,    60,    68,    67,    60,
      61,    15,    60,    47,    50,    49,    54,    52,    48,    44,
      46,    60,    69,    63,    64,     0,    86,     0,    39,    60,
      85
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -104,  -104,  -104,  -104,   168,  -104,   134,  -104,  -104,  -104,
    -104,  -104,   165,  -104,  -104,  -104,   130,   132,  -104,   167,
    -104,   114,   170,   140,  -104,   -73,   116,   -50,  -104,  -103,
     -49,  -104,   -39,    66,    61,  -104,    18,  -104
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     6,     8,    16,    30,    31,    32,    96,    14,
      21,    22,    23,    24,    25,    26,    37,    38,    53,    34,
      48,    49,    51,   166,   108,    77,   104,    62,   125,   126,
      63,   119,    80,    65,    66,    81,   167,   116
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      78,    79,   109,   140,   138,    64,   121,   121,     4,    57,
      58,    59,    28,    91,    92,     1,    57,    58,    59,   152,
       3,    89,    90,    35,    83,    78,    79,     5,    97,   122,
      84,     7,    60,    29,   123,    61,   123,   123,   124,    60,
      57,    58,    71,   133,    36,   149,   164,   142,   120,   134,
     143,   150,    11,    72,    73,    74,   139,   -43,    12,   135,
     -43,    15,    17,    60,    18,    19,    61,    91,    92,   159,
      91,    92,    57,    58,    71,    20,    87,   145,    88,   111,
     112,   113,   114,   115,     9,    72,    73,    74,    10,    88,
     155,    75,    78,    79,   151,    60,   146,   153,    61,   163,
     147,    57,    58,    71,    13,   160,   168,   162,    41,    91,
      92,    78,    79,   165,    72,    73,    74,    91,    92,    78,
      79,   161,   -51,    33,    60,   -51,    42,    61,    44,   -59,
     127,    91,    92,   -59,    87,   -51,    88,    45,   100,   101,
     102,   103,   100,   101,   102,   103,    93,    94,    39,    40,
      17,    95,    18,    19,   130,   131,   132,   128,   129,    47,
      46,    50,    52,    54,    67,    69,    70,    82,   118,    86,
     137,    98,   141,   148,   105,   106,   117,   107,   136,   144,
      68,    88,    27,   154,    99,   156,    85,   170,   157,   158,
      76,     0,     0,     0,    43,   110,     0,     0,   169,     0,
       0,     0,     0,     0,     0,     0,     0,    55,     0,     0,
       0,     0,     0,    56
};

static const yytype_int16 yycheck[] =
{
      50,    50,    75,   106,     6,    44,     6,     6,     0,     4,
       5,     6,     6,    29,    30,     8,     4,     5,     6,    35,
       6,    60,    61,     6,    22,    75,    75,    23,    67,    28,
      28,     9,    27,    27,    36,    30,    36,    36,    88,    27,
       4,     5,     6,    22,    27,    22,   149,    21,    87,    28,
      24,    28,    23,    17,    18,    19,   106,    21,    23,    98,
      24,     3,    13,    27,    15,    16,    30,    29,    30,   142,
      29,    30,     4,     5,     6,    26,    25,   116,    27,    38,
      39,    40,    41,    42,     6,    17,    18,    19,    10,    27,
      28,    23,   142,   142,   133,    27,    22,   136,    30,   149,
      26,     4,     5,     6,    25,   144,    26,   146,    26,    29,
      30,   161,   161,   152,    17,    18,    19,    29,    30,   169,
     169,    33,    21,    11,    27,    24,    21,    30,    20,    20,
      28,    29,    30,    20,    25,    34,    27,     6,    29,    30,
      31,    32,    29,    30,    31,    32,    31,    32,    18,    19,
      13,    36,    15,    16,    93,    94,    95,    91,    92,     6,
      22,    12,     6,    22,    22,    14,    22,    24,     6,    24,
       7,    25,     6,     4,    27,    27,    24,    27,    20,    20,
      46,    27,    14,    28,    70,    28,    54,   169,    28,    28,
      50,    -1,    -1,    -1,    27,    79,    -1,    -1,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    43
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     8,    44,     6,     0,    23,    45,     9,    46,     6,
      10,    23,    23,    25,    52,     3,    47,    13,    15,    16,
      26,    53,    54,    55,    56,    57,    58,    47,     6,    27,
      48,    49,    50,    11,    62,     6,    27,    59,    60,    59,
      59,    26,    21,    62,    20,     6,    22,     6,    63,    64,
      12,    65,     6,    61,    22,    55,    65,     4,     5,     6,
      27,    30,    70,    73,    75,    76,    77,    22,    49,    14,
      22,     6,    17,    18,    19,    23,    66,    68,    70,    73,
      75,    78,    24,    22,    28,    60,    24,    25,    27,    75,
      75,    29,    30,    31,    32,    36,    51,    75,    25,    64,
      29,    30,    31,    32,    69,    27,    27,    27,    67,    68,
      69,    38,    39,    40,    41,    42,    80,    24,     6,    74,
      75,     6,    28,    36,    70,    71,    72,    28,    76,    76,
      77,    77,    77,    22,    28,    75,    20,     7,     6,    70,
      72,     6,    21,    24,    20,    75,    22,    26,     4,    22,
      28,    75,    35,    75,    28,    28,    28,    28,    28,    68,
      75,    33,    75,    70,    72,    75,    66,    79,    26,    34,
      79
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    47,    47,    47,    48,
      48,    49,    49,    49,    50,    51,    51,    52,    52,    53,
      54,    54,    55,    55,    55,    56,    57,    58,    59,    59,
      60,    60,    61,    61,    62,    62,    62,    63,    63,    64,
      65,    66,    66,    67,    67,    67,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    69,    69,    69,    69,    69,
      70,    70,    70,    71,    71,    71,    71,    72,    73,    74,
      74,    75,    75,    75,    75,    76,    76,    76,    76,    77,
      77,    77,    77,    77,    77,    78,    79,    80,    80,    80,
      80,    80
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,    12,     2,     0,     8,     2,     1,     0,     3,
       1,     1,     3,     1,     5,     3,     1,     2,     3,     1,
       3,     1,     1,     1,     1,     2,     2,     2,     3,     1,
       1,     3,     3,     1,     2,     1,     0,     3,     1,     7,
       2,     3,     1,     0,     3,     1,     4,     4,     4,     4,
       4,     1,     4,     1,     4,     1,     1,     1,     1,     0,
       0,     4,     3,     3,     3,     1,     1,     2,     4,     3,
       1,     3,     3,     2,     1,     3,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     7,     1,     1,     1,     1,
       1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: PROG ID LBRACE lfonc DEF MAIN LBRACE loc_section dom_section rec_section RBRACE RBRACE  */
#line 199 "corec.y"
    {
        emitDataSection(output_file);

        fprintf(output_file, "newline: .asciiz \"\\n\"\n");
        fprintf(output_file, ".text\n.globl main\n");
        fprintf(output_file, "main:\n");

        // Завершаем программу
        fprintf(output_file, "\n# End of main\n");
        fprintf(output_file, "li $v0, 10\n");
        fprintf(output_file, "syscall\n");
    }
#line 1438 "corec.tab.c"
    break;

  case 5: /* fonc: DEF ID LBRACE args loc_section dom_section rec_section RBRACE  */
#line 225 "corec.y"
    {
      // В конце описания функции выведем метку <ID> и "jr $ra"
      fprintf(output_file, "\n# function %s\n", (yyvsp[-6].str));
      fprintf(output_file, "%s:\n", (yyvsp[-6].str));
      fprintf(output_file, "jr $ra\n");
    }
#line 1449 "corec.tab.c"
    break;

  case 11: /* decl: ID  */
#line 246 "corec.y"
    {
      addSymbol((yyvsp[0].str), SYM_VAR);
    }
#line 1457 "corec.tab.c"
    break;

  case 12: /* decl: ID EQUALS e  */
#line 250 "corec.y"
    {
      addSymbol((yyvsp[-2].str), SYM_VAR);
      fprintf(output_file, "%s", (yyvsp[0].str));
      genAssign((yyvsp[-2].str));
      free((yyvsp[0].str));
    }
#line 1468 "corec.tab.c"
    break;

  case 13: /* decl: array  */
#line 257 "corec.y"
    {
      // addSymbol(..., SYM_ARR);
    }
#line 1476 "corec.tab.c"
    break;

  case 46: /* i: arrayref op EQUALS e  */
#line 351 "corec.y"
      {
        /* Печатаем код expr, затем arrayref assign (индекс = 0, заглушка) */
        fprintf(output_file, "%s", (yyvsp[0].str));
        fprintf(output_file, "# arrayref assign\n");
        fprintf(output_file, "la $t2, %s\n", (yyvsp[-3].str));
        fprintf(output_file, "li $t1, 0\n");
        fprintf(output_file, "add $t2, $t2, $t1\n");
        fprintf(output_file, "sw $t0, 0($t2)\n");
        free((yyvsp[-3].str));
        free((yyvsp[0].str));
      }
#line 1492 "corec.tab.c"
    break;

  case 47: /* i: ID op EQUALS e  */
#line 363 "corec.y"
      {
        addSymbol((yyvsp[-3].str), SYM_VAR);
        fprintf(output_file, "%s", (yyvsp[0].str));
        if ((yyvsp[-2].op_type) == OP_NONE) {
          genAssign((yyvsp[-3].str));
        } else if ((yyvsp[-2].op_type) == OP_PLUS) {
          genPlusAssign((yyvsp[-3].str));
        } else if ((yyvsp[-2].op_type) == OP_MINUS) {
          genMinusAssign((yyvsp[-3].str));
        } else if ((yyvsp[-2].op_type) == OP_MULT) {
          genMultAssign((yyvsp[-3].str));
        } else if ((yyvsp[-2].op_type) == OP_DIV) {
          genDivAssign((yyvsp[-3].str));
        }
        free((yyvsp[0].str));
      }
#line 1513 "corec.tab.c"
    break;

  case 48: /* i: READ LPAREN ID RPAREN  */
#line 380 "corec.y"
      {
        addSymbol((yyvsp[-1].str), SYM_VAR);
        genRead((yyvsp[-1].str));
      }
#line 1522 "corec.tab.c"
    break;

  case 49: /* i: PRINT LPAREN ID RPAREN  */
#line 385 "corec.y"
      {
        genPrint((yyvsp[-1].str));
      }
#line 1530 "corec.tab.c"
    break;

  case 50: /* i: PRINTSTR LPAREN CHAINE RPAREN  */
#line 389 "corec.y"
      {
        genPrintStr((yyvsp[-1].str));
      }
#line 1538 "corec.tab.c"
    break;

  case 55: /* op: PLUS  */
#line 400 "corec.y"
         { (yyval.op_type) = OP_PLUS; }
#line 1544 "corec.tab.c"
    break;

  case 56: /* op: MINUS  */
#line 401 "corec.y"
          { (yyval.op_type) = OP_MINUS; }
#line 1550 "corec.tab.c"
    break;

  case 57: /* op: MULT  */
#line 402 "corec.y"
         { (yyval.op_type) = OP_MULT; }
#line 1556 "corec.tab.c"
    break;

  case 58: /* op: DIV  */
#line 403 "corec.y"
        { (yyval.op_type) = OP_DIV; }
#line 1562 "corec.tab.c"
    break;

  case 59: /* op: %empty  */
#line 404 "corec.y"
                  { (yyval.op_type) = OP_NONE; }
#line 1568 "corec.tab.c"
    break;

  case 68: /* arrayref: ID LBRACKET elist RBRACKET  */
#line 427 "corec.y"
  {
    (yyval.str) = (yyvsp[-3].str);
  }
#line 1576 "corec.tab.c"
    break;

  case 71: /* e: e PLUS t  */
#line 440 "corec.y"
    {
      char* part1 = concat2((yyvsp[-2].str), "  # e PLUS t\n  move $t1, $t0\n");
      char* part2 = concat2(part1, (yyvsp[0].str));
      free(part1);
      char* final = concat2(part2, "  add $t0, $t1, $t0\n");
      free(part2);
      (yyval.str) = final;
    }
#line 1589 "corec.tab.c"
    break;

  case 72: /* e: e MINUS t  */
#line 449 "corec.y"
    {
      char* p1 = concat2((yyvsp[-2].str), "  # e MINUS t\n  move $t1, $t0\n");
      char* p2 = concat2(p1, (yyvsp[0].str));
      free(p1);
      char* fin = concat2(p2, "  sub $t0, $t1, $t0\n");
      free(p2);
      (yyval.str) = fin;
    }
#line 1602 "corec.tab.c"
    break;

  case 73: /* e: MINUS e  */
#line 458 "corec.y"
    {
      char* fin = concat2((yyvsp[0].str), "  # unary minus\n  neg $t0, $t0\n");
      (yyval.str) = fin;
    }
#line 1611 "corec.tab.c"
    break;

  case 74: /* e: t  */
#line 463 "corec.y"
    {
      (yyval.str) = (yyvsp[0].str);
    }
#line 1619 "corec.tab.c"
    break;

  case 75: /* t: t MULT f  */
#line 470 "corec.y"
    {
      char* p1 = concat2((yyvsp[-2].str), "  # t MULT f\n  move $t1, $t0\n");
      char* p2 = concat2(p1, (yyvsp[0].str));
      free(p1);
      char* fin = concat2(p2, "  mul $t0, $t1, $t0\n");
      free(p2);
      (yyval.str) = fin;
    }
#line 1632 "corec.tab.c"
    break;

  case 76: /* t: t DIV f  */
#line 479 "corec.y"
    {
      char* p1 = concat2((yyvsp[-2].str), "  # t DIV f\n  move $t1, $t0\n");
      char* p2 = concat2(p1, (yyvsp[0].str));
      free(p1);
      char* fin = concat2(p2, "  div $t1, $t0\n  mflo $t0\n");
      free(p2);
      (yyval.str) = fin;
    }
#line 1645 "corec.tab.c"
    break;

  case 77: /* t: t PERCENT f  */
#line 488 "corec.y"
    {
      char* p1 = concat2((yyvsp[-2].str), "  # t %% f\n  move $t1, $t0\n");
      char* p2 = concat2(p1, (yyvsp[0].str));
      free(p1);
      char* fin = concat2(p2, "  div $t1, $t0\n  mfhi $t0\n");
      free(p2);
      (yyval.str) = fin;
    }
#line 1658 "corec.tab.c"
    break;

  case 78: /* t: f  */
#line 497 "corec.y"
    {
      (yyval.str) = (yyvsp[0].str);
    }
#line 1666 "corec.tab.c"
    break;

  case 79: /* f: LPAREN e RPAREN  */
#line 504 "corec.y"
    {
      (yyval.str) = (yyvsp[-1].str); 
    }
#line 1674 "corec.tab.c"
    break;

  case 80: /* f: NUMBER  */
#line 508 "corec.y"
    {
      char buf[64];
      sprintf(buf, "  li $t0, %d\n", (yyvsp[0].num));
      (yyval.str) = strdup(buf);
    }
#line 1684 "corec.tab.c"
    break;

  case 81: /* f: ID  */
#line 514 "corec.y"
    {
      char buf[64];
      sprintf(buf, "  lw $t0, %s\n", (yyvsp[0].str));
      (yyval.str) = strdup(buf);
    }
#line 1694 "corec.tab.c"
    break;

  case 82: /* f: call  */
#line 520 "corec.y"
    {
      (yyval.str) = strdup("# call - not implemented\n");
    }
#line 1702 "corec.tab.c"
    break;

  case 83: /* f: arrayref  */
#line 524 "corec.y"
    {
      (yyval.str) = strdup("# f: arrayref - not implemented\n");
    }
#line 1710 "corec.tab.c"
    break;

  case 84: /* f: FLOTTANT  */
#line 528 "corec.y"
    {
      (yyval.str) = strdup("# float not fully handled\n");
    }
#line 1718 "corec.tab.c"
    break;


#line 1722 "corec.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 549 "corec.y"


void yyerror(const char* s) {
    fprintf(stderr, "Error: %s at line %d\n", s, yylineno);
}
