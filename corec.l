%{
#include "corec.tab.h" /* Заголовочный файл для Bison */
#include <stdlib.h>    /* Для atoi */
#include <string.h>    /* Для strdup */

extern YYSTYPE yylval; // Подключение yylval из Bison
%}


%%


"prog"          { return PROG; }
"def"           { return DEF; }
"Main"          { return MAIN; }
"Dom"           { return DOM; }
"Rec"           { return REC; }
"in:"           { return IN; }
"out:"          { return OUT; }
"inout:"        { return INOUT; }
"printstr"      { return PRINTSTR; }
"print"         { return PRINT; }
"read"          { return READ; }


[0-9]+                  { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return IDENTIFIER; }
"="                     { return EQUALS; }
";"                     { return SEMICOLON; }
","                     { return COMMA; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return STAR; }
"/"                     { return SLASH; }
"?"                     { return QUESTION; }
":"                     { return COLON; }


[ \t\n]+                ;  
.                       { return yytext[0]; } 

%%


int yywrap(void) {
    return 1;
}
