%{
#include "corec.tab.h" 
#include <stdlib.h>    
#include <string.h>    

extern YYSTYPE yylval; 

%}


%%

"prog"          { printf("Token: PROG\n"); return PROG; }
"def"           { printf("Token: DEF\n"); return DEF; }
"Main"          { printf("Token: MAIN\n"); return MAIN; }
"Dom:"          { printf("Token: DOM\n"); return DOM; }
"Rec:"          { printf("Token: REC\n"); return REC; }
"in:"           { printf("Token: IN\n"); return IN; }
"in"            { printf("Token: IN_SIMPLE\n"); return IN_SIMPLE; }
"out:"          { printf("Token: OUT\n"); return OUT; }
"inout:"        { printf("Token: INOUT\n"); return INOUT; }
"printstr"      { printf("Token: PRINTSTR\n"); return PRINTSTR; }
"print"         { printf("Token: PRINT\n"); return PRINT; }
"read"          { printf("Token: READ\n"); return READ; }
".."            { printf("Token: DOTDOT\n"); return DOTDOT; }
"Loc:"          { printf("Token: LOC\n"); return LOC; }


[0-9]+                  { printf("Token: NUMBER, Value: %s\n", yytext); yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { printf("Token: ID, Value: %s\n", yytext); yylval.str = strdup(yytext); return ID; }
[0-9]+[a-zA-Z_][a-zA-Z0-9_]* { printf("Token: ID, Value: %s\n", yytext); yylval.str = strdup(yytext); return ID; } // Id qui commence par un chiffre
[0-9]+\.[0-9]+          { printf("Token: FLOTTANT, Value: %s\n", yytext); yylval.num = atof(yytext); }

\"(.*?)\"               { printf("Token: CHAINE, Value: %s\n", yytext); yylval.str = strdup(yytext + 1); // Скопировать строку без первой кавычки
                                if (yylval.str[strlen(yylval.str) - 1] == '"') // Убедиться, что последняя кавычка существует
                                    { yylval.str[strlen(yylval.str) - 1] = '\0'; } // Удалить последнюю кавычку
                                                    return CHAINE; }



"="                     { printf("Token: EQUALS\n"); return EQUALS; }
";"                     { printf("Token: SEMICOLON\n"); return SEMICOLON; }
","                     { printf("Token: COMMA\n"); return COMMA; }
"{"                     { printf("Token: LBRACE\n"); return LBRACE; }
"}"                     { printf("Token: RBRACE\n"); return RBRACE; }
"["                     { printf("Token: LBRACKET\n"); return LBRACKET; }
"]"                     { printf("Token: RBRACKET\n"); return RBRACKET; }
"("                     { printf("Token: LPAREN\n"); return LPAREN; }
")"                     { printf("Token: RPAREN\n"); return RPAREN; }
"+"                     { printf("Token: PLUS\n"); return PLUS; }
"-"                     { printf("Token: MINUS\n"); return MINUS; }
"*"                     { printf("Token: MULT\n"); return MULT; }
"/"                     { printf("Token: DIV\n"); return DIV; }
"?"                     { printf("Token: QUESTION\n"); return QUESTION; }
"%"                     { printf("Token: PERCENT\n"); return PERCENT; }
":"                     { printf("Token: COLON\n"); return COLON; }
"<="                    { printf("Token: LE\n"); return LE; }
">="                    { printf("Token: GE\n"); return GE; }
"=="                    { printf("Token: EQ\n"); return EQ; }
"<"                     { printf("Token: LT\n"); return LT; }
">"                     { printf("Token: GT\n"); return GT; }

"/*"([^*]|\*+[^*/])*\*+"/" ; // eviter des commentaires
[ \t]+          ;   
\n              ;   
.               { printf("Unrecognized token: %s\n", yytext); return yytext[0]; }

%%

int yywrap(void) {
    return 1;
}
