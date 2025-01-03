#include <stdio.h>
#include "corec.tab.h" // Заголовок Bison

// Объявление функций, предоставляемых Flex
extern int yylex();
extern char* yytext;

int main() {
    int token;
    while ((token = yylex())) {
        printf("Token: %d, Text: %s\n", token, yytext);
    }
    return 0;
}
