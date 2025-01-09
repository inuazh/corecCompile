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





// -------------------
// Таблица символов
// -------------------
typedef enum { SYM_VAR, SYM_ARR /* , SYM_FUNC, ... */ } SymType;

typedef struct Symbol {
    char* name;
    SymType kind;
    struct Symbol* next;
} Symbol;

Symbol* symbolTable = NULL;

Symbol* addSymbol(const char* name, SymType kind) {
    // Проверка на дубликаты (упрощённая)
    Symbol* p = symbolTable;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            // Уже есть такой символ, вернём его
            return p;
        }
        p = p->next;
    }
    // Создаём новый
    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->kind = kind;
    s->next = symbolTable;
    symbolTable = s;
    return s;
}

// -------------------
// Для вывода в MIPS
// -------------------
extern int yylineno; 
int yylex();           
void yyerror(const char* s);

FILE *output_file; 

// Функция для генерации .data секции
void emitDataSection(FILE* out) {
    fprintf(out, ".data\n");
    Symbol* p = symbolTable;
    while (p) {
        if (p->kind == SYM_VAR) {
            // Для переменной - слово
            fprintf(out, "%s: .word 0\n", p->name);
        }
        else if (p->kind == SYM_ARR) {
            // Условно место под массив
            fprintf(out, "%s: .space 40\n", p->name);
        }
        p = p->next;
    }
}

// -------------------------------------------------------
// Упрощённая генерация MIPS для инструкций
// -------------------------------------------------------

// Для простоты, скажем, что любое выражение хранит результат в $t0.
// (На деле вы бы рекурсивно генерировали код для e, t, f, etc.)

// var = expr
void genAssign(const char* varName) {
    fprintf(output_file, "# %s = <expr>\n", varName);
    // store в память (varName — метка в .data)
    fprintf(output_file, "sw $t0, %s\n", varName);
}

// var += expr
void genPlusAssign(const char* varName) {
    fprintf(output_file, "# %s += <expr>\n", varName);
    // Загрузить старое значение
    fprintf(output_file, "lw $t1, %s\n", varName);
    // Прибавить $t0 (результат expr)
    fprintf(output_file, "add $t1, $t1, $t0\n");
    // Сохранить обратно
    fprintf(output_file, "sw $t1, %s\n", varName);
}

// Аналогично для -=, *=, /=
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
    fprintf(output_file, "div $t1, $t0\n"); // В MARS: div $t1, $t0 => hi/lo
    fprintf(output_file, "mflo $t1\n");     // Возьмём результат из lo
    fprintf(output_file, "sw $t1, %s\n", varName);
}

// read(var)
void genRead(const char* varName) {
    fprintf(output_file, "# read %s\n", varName);
    fprintf(output_file, "li $v0, 5   # syscall read_int\n");
    fprintf(output_file, "syscall\n");
    // Результат считывания в $v0
    fprintf(output_file, "sw $v0, %s\n", varName);
}

// print(var)
void genPrint(const char* varName) {
    fprintf(output_file, "# print %s\n", varName);
    // Загрузим var в $a0
    fprintf(output_file, "lw $a0, %s\n", varName);
    // print_int
    fprintf(output_file, "li $v0, 1\n");
    fprintf(output_file, "syscall\n");
    // Печать пробела или перевода строки (упростим)
    fprintf(output_file, "la $a0, newline\n"); // нужно объявить в .data?
    fprintf(output_file, "li $v0, 4\n");
    fprintf(output_file, "syscall\n");
}

// Для строк (printstr "Hello world")
void genPrintStr(const char* str) {
    // Сгенерируем метку в .data
    static int strCount = 0;
    char label[32];
    sprintf(label, "strlbl_%d", strCount++);
    // Выведем строку:
    fprintf(output_file, ".data\n");
    fprintf(output_file, "%s: .asciiz \"%s\"\n", label, str);
    // Вернёмся в текст
    fprintf(output_file, ".text\n");
    // Печать
    fprintf(output_file, "# print string\n");
    fprintf(output_file, "la $a0, %s\n", label);
    fprintf(output_file, "li $v0, 4\n");
    fprintf(output_file, "syscall\n");
}

// -------------------------------------------------------
// Простейший enum, чтобы хранить, какой оператор (op)
// -------------------------------------------------------
enum {
    OP_NONE=0,
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV
};








#line 254 "corec.tab.c"

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
  YYSYMBOL_largs = 53,                     /* largs  */
  YYSYMBOL_sectionargs = 54,               /* sectionargs  */
  YYSYMBOL_arglist = 55,                   /* arglist  */
  YYSYMBOL_eltarg = 56,                    /* eltarg  */
  YYSYMBOL_dom_section = 57,               /* dom_section  */
  YYSYMBOL_domlist = 58,                   /* domlist  */
  YYSYMBOL_d = 59,                         /* d  */
  YYSYMBOL_rec_section = 60,               /* rec_section  */
  YYSYMBOL_blockinst = 61,                 /* blockinst  */
  YYSYMBOL_listei = 62,                    /* listei  */
  YYSYMBOL_i = 63,                         /* i  */
  YYSYMBOL_op = 64,                        /* op  */
  YYSYMBOL_call = 65,                      /* call  */
  YYSYMBOL_lcall = 66,                     /* lcall  */
  YYSYMBOL_precinst = 67,                  /* precinst  */
  YYSYMBOL_arrayref = 68,                  /* arrayref  */
  YYSYMBOL_elist = 69,                     /* elist  */
  YYSYMBOL_e = 70,                         /* e  */
  YYSYMBOL_t = 71,                         /* t  */
  YYSYMBOL_f = 72,                         /* f  */
  YYSYMBOL_cond = 73,                      /* cond  */
  YYSYMBOL_b = 74,                         /* b  */
  YYSYMBOL_oprel = 75                      /* oprel  */
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
#define YYLAST   211

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  85
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  162

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
       0,   217,   217,   238,   239,   244,   253,   254,   255,   260,
     261,   265,   269,   276,   284,   288,   290,   294,   295,   300,
     301,   307,   308,   309,   313,   314,   318,   319,   325,   326,
     327,   332,   333,   337,   341,   345,   346,   349,   350,   351,
     356,   361,   386,   391,   396,   401,   402,   403,   404,   410,
     411,   412,   413,   414,   418,   419,   420,   425,   426,   427,
     428,   432,   436,   440,   441,   445,   446,   447,   448,   452,
     453,   454,   455,   459,   460,   461,   462,   463,   464,   468,
     472,   479,   480,   481,   482,   483
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
  "dlist", "args", "largs", "sectionargs", "arglist", "eltarg",
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

#define YYPACT_NINF (-96)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-54)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      15,    38,    28,   -17,   -96,   -96,    84,    67,   -96,    57,
      72,    95,   122,    40,   122,     6,   123,    16,    16,    16,
     -96,   -96,   123,   132,   152,   137,   -96,   -96,   154,   149,
     -96,   -96,   140,   -96,   140,   140,    71,   149,    54,   141,
       6,   150,   143,   -96,    12,   142,    16,   120,   -96,   144,
     -96,   -96,   113,    54,    54,   -96,   -96,    22,    47,   -96,
      54,   -96,   145,   154,   112,   146,   147,   148,    44,   -96,
     -96,    48,    56,    89,   -96,   153,   -96,   -96,   -96,    54,
      -2,   121,   -96,     5,     5,     5,     5,     5,    19,    22,
      54,   -96,   -96,   -96,   -96,   -96,   151,   160,     1,   163,
     124,   -96,   156,   -96,   -96,   -96,   -96,   -96,    54,   -96,
      79,    22,   155,   -96,   168,   -96,    42,   -96,   -96,    47,
      47,   -96,   -96,   -96,    54,   -96,   -16,    54,   157,    87,
     158,   159,   161,    94,   -96,    54,    93,    54,   -96,   -96,
       2,   -96,    22,    54,    22,   -96,   -96,   -96,   -96,   -96,
     -96,    22,    12,    22,   -96,   -96,    77,   -96,   162,   -96,
      12,   -96
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     1,     4,     0,     0,     3,     0,
       0,     0,     8,     0,     8,     7,    30,     0,     0,     0,
      18,    20,    30,    11,     0,     6,    10,    13,    29,     0,
      27,    26,    21,    25,    22,    23,     0,     0,    54,     0,
       0,     0,    28,    32,    54,     0,     0,     0,    17,     0,
      77,    78,    74,    54,    54,    75,    76,    12,    68,    72,
      54,     9,     0,     0,    74,     0,     0,     0,    54,    34,
      36,    75,    76,     0,    47,     0,    24,    19,     5,    54,
      54,     0,    67,    54,    54,    54,    54,    54,     0,    16,
      54,    31,    49,    50,    51,    52,     0,     0,    54,     0,
       0,    39,     0,    81,    82,    83,    84,    85,    54,     2,
       0,    64,     0,    56,     0,    59,     0,    60,    73,    65,
      66,    69,    70,    71,    54,    14,     0,    54,     0,     0,
       0,     0,     0,    54,    35,    54,     0,    54,    62,    61,
      54,    55,    15,    54,    41,    44,    43,    48,    46,    42,
      38,    40,    54,    63,    57,    58,     0,    80,     0,    33,
      54,    79
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -96,   -96,   -96,   -96,   164,   -96,   139,     0,   -96,   -96,
     -96,   133,   128,   135,   166,   -96,   127,   165,   167,   -96,
     -66,   111,   -44,   -96,   -95,   -43,   -96,   -33,    73,    68,
     -96,    24,   -96
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     6,     8,    16,    25,    26,    31,    88,    14,
      36,    21,    32,    33,    29,    42,    43,    45,   157,   100,
      70,    96,    55,   116,   117,    56,   110,    73,    58,    59,
      74,   158,   108
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      71,    72,   101,   131,   112,    57,     5,   129,   112,    50,
      51,    52,    23,    83,    84,    27,    50,    51,    64,   143,
      81,    82,    30,     1,    71,    72,   113,    89,     4,    65,
      66,    67,    53,    24,   114,    68,   115,   114,   114,    53,
      27,   124,    54,    24,     3,   155,   111,   125,    50,    51,
      64,    83,    84,    17,   130,    18,    19,   126,    50,    51,
      52,    65,    66,    67,   140,   -37,    20,   150,   -37,   -45,
     141,    53,   -45,     9,    54,   136,   -53,    10,    85,    86,
      11,    53,   -45,    87,    54,    92,    93,    94,    95,    71,
      72,   142,    47,     7,   144,    12,   154,    48,    50,    51,
      64,   137,   151,   159,   153,   138,    83,    84,    71,    72,
     156,    65,    66,    67,    80,   146,    71,    72,    83,    84,
      13,    53,    83,    84,    54,    15,   152,   103,   104,   105,
     106,   107,   -53,    17,    28,    18,    19,    79,    79,    80,
      80,    92,    93,    94,    95,   133,    34,    35,   134,   118,
      83,    84,    38,   121,   122,   123,   119,   120,    39,    40,
      41,    44,    46,    60,    62,    63,    75,   128,    78,   132,
      90,   127,   139,    97,    98,    99,   135,   109,    22,    61,
      77,    76,    80,   102,   161,   145,   147,   148,    37,   149,
      91,     0,     0,     0,     0,     0,   160,     0,     0,     0,
       0,     0,    49,     0,     0,     0,     0,     0,     0,     0,
       0,    69
};

static const yytype_int16 yycheck[] =
{
      44,    44,    68,    98,     6,    38,    23,     6,     6,     4,
       5,     6,     6,    29,    30,    15,     4,     5,     6,    35,
      53,    54,     6,     8,    68,    68,    28,    60,     0,    17,
      18,    19,    27,    27,    36,    23,    80,    36,    36,    27,
      40,    22,    30,    27,     6,   140,    79,    28,     4,     5,
       6,    29,    30,    13,    98,    15,    16,    90,     4,     5,
       6,    17,    18,    19,    22,    21,    26,   133,    24,    21,
      28,    27,    24,     6,    30,   108,    20,    10,    31,    32,
      23,    27,    34,    36,    30,    29,    30,    31,    32,   133,
     133,   124,    21,     9,   127,    23,   140,    26,     4,     5,
       6,    22,   135,    26,   137,    26,    29,    30,   152,   152,
     143,    17,    18,    19,    27,    28,   160,   160,    29,    30,
      25,    27,    29,    30,    30,     3,    33,    38,    39,    40,
      41,    42,    20,    13,    11,    15,    16,    25,    25,    27,
      27,    29,    30,    31,    32,    21,    18,    19,    24,    28,
      29,    30,    20,    85,    86,    87,    83,    84,     6,    22,
       6,    12,    22,    22,    14,    22,    24,     7,    24,     6,
      25,    20,     4,    27,    27,    27,    20,    24,    14,    40,
      47,    46,    27,    72,   160,    28,    28,    28,    22,    28,
      63,    -1,    -1,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      -1,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     8,    44,     6,     0,    23,    45,     9,    46,     6,
      10,    23,    23,    25,    52,     3,    47,    13,    15,    16,
      26,    54,    47,     6,    27,    48,    49,    50,    11,    57,
       6,    50,    55,    56,    55,    55,    53,    57,    20,     6,
      22,     6,    58,    59,    12,    60,    22,    21,    26,    60,
       4,     5,     6,    27,    30,    65,    68,    70,    71,    72,
      22,    49,    14,    22,     6,    17,    18,    19,    23,    61,
      63,    65,    68,    70,    73,    24,    56,    54,    24,    25,
      27,    70,    70,    29,    30,    31,    32,    36,    51,    70,
      25,    59,    29,    30,    31,    32,    64,    27,    27,    27,
      62,    63,    64,    38,    39,    40,    41,    42,    75,    24,
      69,    70,     6,    28,    36,    65,    66,    67,    28,    71,
      71,    72,    72,    72,    22,    28,    70,    20,     7,     6,
      65,    67,     6,    21,    24,    20,    70,    22,    26,     4,
      22,    28,    70,    35,    70,    28,    28,    28,    28,    28,
      63,    70,    33,    70,    65,    67,    70,    61,    74,    26,
      34,    74
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    47,    47,    47,    48,
      48,    49,    49,    49,    50,    51,    51,    52,    52,    53,
      53,    54,    54,    54,    55,    55,    56,    56,    57,    57,
      57,    58,    58,    59,    60,    61,    61,    62,    62,    62,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    64,
      64,    64,    64,    64,    65,    65,    65,    66,    66,    66,
      66,    67,    68,    69,    69,    70,    70,    70,    70,    71,
      71,    71,    71,    72,    72,    72,    72,    72,    72,    73,
      74,    75,    75,    75,    75,    75
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,    12,     2,     0,     8,     2,     1,     0,     3,
       1,     1,     3,     1,     5,     3,     1,     4,     2,     3,
       0,     2,     2,     2,     3,     1,     1,     1,     2,     1,
       0,     3,     1,     7,     2,     3,     1,     0,     3,     1,
       4,     4,     4,     4,     4,     1,     4,     1,     4,     1,
       1,     1,     1,     0,     0,     4,     3,     3,     3,     1,
       1,     2,     4,     3,     1,     3,     3,     2,     1,     3,
       3,     3,     1,     3,     1,     1,     1,     1,     1,     7,
       1,     1,     1,     1,     1,     1
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
#line 218 "corec.y"
    {
        // В конце парсинга: выводим таблицу символов (переменные) в .data
        emitDataSection(output_file);

        // Добавим "служебные" данные (например, для переноса строки)
        fprintf(output_file, "newline: .asciiz \"\\n\"\n");

        // Начинаем секцию кода
        fprintf(output_file, ".text\n.globl main\n");
        fprintf(output_file, "main:\n");
        // ... Можно добавить какой-то "код инициализации" ...
        // Завершаем программу:
        fprintf(output_file, "\n# End of main\n");
        fprintf(output_file, "li $v0, 10   # syscall exit\n");
        fprintf(output_file, "syscall\n");
    }
#line 1450 "corec.tab.c"
    break;

  case 5: /* fonc: DEF ID LBRACE args loc_section dom_section rec_section RBRACE  */
#line 245 "corec.y"
    {
        // addSymbol($2, SYM_FUNC)
    }
#line 1458 "corec.tab.c"
    break;

  case 11: /* decl: ID  */
#line 266 "corec.y"
    {
      addSymbol((yyvsp[0].str), SYM_VAR);
    }
#line 1466 "corec.tab.c"
    break;

  case 12: /* decl: ID EQUALS e  */
#line 270 "corec.y"
    {
      addSymbol((yyvsp[-2].str), SYM_VAR);
      // На самом деле нужно генерировать код для expr.
      // Пусть результат expr = $t0, тогда:
      genAssign((yyvsp[-2].str));
    }
#line 1477 "corec.tab.c"
    break;

  case 13: /* decl: array  */
#line 277 "corec.y"
    {
      // Добавляем массив, если хотите
      // addSymbol(..., SYM_ARR);
    }
#line 1486 "corec.tab.c"
    break;

  case 40: /* i: arrayref op EQUALS e  */
#line 357 "corec.y"
      {
        // tmp[i,j] = expr (упрощаем: не делаем массивы, но можно)
        fprintf(output_file, "# arrayref assign (not fully implemented)\n");
      }
#line 1495 "corec.tab.c"
    break;

  case 41: /* i: ID op EQUALS e  */
#line 362 "corec.y"
      {
        addSymbol((yyvsp[-3].str), SYM_VAR);

        // В реальном коде: нужно сгенерировать вычисление expr => $t0
        // Мы упрощаем, не делаем "codeGen(expr)"
        
        if ((yyvsp[-2].op_type) == OP_NONE) {
          // var = expr
          genAssign((yyvsp[-3].str));
        }
        else if ((yyvsp[-2].op_type) == OP_PLUS) {
          // var += expr
          genPlusAssign((yyvsp[-3].str));
        }
        else if ((yyvsp[-2].op_type) == OP_MINUS) {
          genMinusAssign((yyvsp[-3].str));
        }
        else if ((yyvsp[-2].op_type) == OP_MULT) {
          genMultAssign((yyvsp[-3].str));
        }
        else if ((yyvsp[-2].op_type) == OP_DIV) {
          genDivAssign((yyvsp[-3].str));
        }
      }
#line 1524 "corec.tab.c"
    break;

  case 42: /* i: READ LPAREN ID RPAREN  */
#line 387 "corec.y"
      {
        addSymbol((yyvsp[-1].str), SYM_VAR);
        genRead((yyvsp[-1].str));
      }
#line 1533 "corec.tab.c"
    break;

  case 43: /* i: PRINT LPAREN ID RPAREN  */
#line 392 "corec.y"
      {
        // print(var)
        genPrint((yyvsp[-1].str));
      }
#line 1542 "corec.tab.c"
    break;

  case 44: /* i: PRINTSTR LPAREN CHAINE RPAREN  */
#line 397 "corec.y"
      {
        // print string
        genPrintStr((yyvsp[-1].str));
      }
#line 1551 "corec.tab.c"
    break;

  case 49: /* op: PLUS  */
#line 410 "corec.y"
         { (yyval.op_type) = OP_PLUS; }
#line 1557 "corec.tab.c"
    break;

  case 50: /* op: MINUS  */
#line 411 "corec.y"
          { (yyval.op_type) = OP_MINUS; }
#line 1563 "corec.tab.c"
    break;

  case 51: /* op: MULT  */
#line 412 "corec.y"
         { (yyval.op_type) = OP_MULT; }
#line 1569 "corec.tab.c"
    break;

  case 52: /* op: DIV  */
#line 413 "corec.y"
        { (yyval.op_type) = OP_DIV; }
#line 1575 "corec.tab.c"
    break;

  case 53: /* op: %empty  */
#line 414 "corec.y"
                  { (yyval.op_type) = OP_NONE; }
#line 1581 "corec.tab.c"
    break;


#line 1585 "corec.tab.c"

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

#line 487 "corec.y"


void yyerror(const char* s) {
    fprintf(stderr, "Error: %s at line %d\n", s, yylineno);
}
