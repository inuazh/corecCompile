%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();           // Объявление функции yylex
void yyerror(const char* s); // Объявление функции yyerror
%}

%union {
    int num;      // Для чисел
    char* str;    // Для строк и идентификаторов
}

%token <num> NUMBER
%token <str> IDENTIFIER
%token PROG DEF MAIN DOM REC IN OUT INOUT PRINTSTR PRINT READ
%token EQUALS SEMICOLON COMMA LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN
%token PLUS MINUS STAR SLASH QUESTION COLON



%%

// Грамматика языка (начальная версия)
program:
    PROG IDENTIFIER LBRACE functions RBRACE
;

functions:
    functions function
    | function
;

function:
    DEF IDENTIFIER LBRACE RBRACE
;

%%

// Обработчик ошибок
void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}
