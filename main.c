#include <stdio.h>
#include "corec.tab.h"
#include <stdlib.h> 


extern int yyparse();

extern int yylex();
extern char* yytext;

extern FILE *output_file; 

extern int yydebug;


int main() {
    yydebug = 1;
    output_file = fopen("output.s", "w");
    if (!output_file) {
        fprintf(stderr, "Error: Cannot open output.s\n");
        exit(1);
    }
    yyparse();
    fclose(output_file);
    return 0;
}
