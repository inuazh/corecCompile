/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_COREC_TAB_H_INCLUDED
# define YY_YY_COREC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    LOC = 258,                     /* LOC  */
    NUMBER = 259,                  /* NUMBER  */
    FLOTTANT = 260,                /* FLOTTANT  */
    ID = 261,                      /* ID  */
    CHAINE = 262,                  /* CHAINE  */
    PROG = 263,                    /* PROG  */
    DEF = 264,                     /* DEF  */
    MAIN = 265,                    /* MAIN  */
    DOM = 266,                     /* DOM  */
    REC = 267,                     /* REC  */
    IN = 268,                      /* IN  */
    IN_SIMPLE = 269,               /* IN_SIMPLE  */
    OUT = 270,                     /* OUT  */
    INOUT = 271,                   /* INOUT  */
    PRINTSTR = 272,                /* PRINTSTR  */
    PRINT = 273,                   /* PRINT  */
    READ = 274,                    /* READ  */
    EQUALS = 275,                  /* EQUALS  */
    SEMICOLON = 276,               /* SEMICOLON  */
    COMMA = 277,                   /* COMMA  */
    LBRACE = 278,                  /* LBRACE  */
    RBRACE = 279,                  /* RBRACE  */
    LBRACKET = 280,                /* LBRACKET  */
    RBRACKET = 281,                /* RBRACKET  */
    LPAREN = 282,                  /* LPAREN  */
    RPAREN = 283,                  /* RPAREN  */
    PLUS = 284,                    /* PLUS  */
    MINUS = 285,                   /* MINUS  */
    MULT = 286,                    /* MULT  */
    DIV = 287,                     /* DIV  */
    QUESTION = 288,                /* QUESTION  */
    COLON = 289,                   /* COLON  */
    DOTDOT = 290,                  /* DOTDOT  */
    PERCENT = 291,                 /* PERCENT  */
    EQUAL = 292,                   /* EQUAL  */
    LT = 293,                      /* LT  */
    GT = 294,                      /* GT  */
    LE = 295,                      /* LE  */
    GE = 296,                      /* GE  */
    EQ = 297                       /* EQ  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 186 "corec.y"

    int num; 
    float flt;     
    char* str;
    int op_type;  // для хранения типа оператора

#line 113 "corec.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_COREC_TAB_H_INCLUDED  */
