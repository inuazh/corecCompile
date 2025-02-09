%{
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

%}

%start program

%union {
    int num; 
    float flt;     
    char* str;
    int op_type;  
}

%token LOC
%token <num> NUMBER
%token <flt> FLOTTANT
%token <str> ID
%token <str> CHAINE

%token PROG DEF MAIN DOM REC IN IN_SIMPLE OUT INOUT PRINTSTR PRINT READ
%token EQUALS SEMICOLON COMMA LBRACE RBRACE LBRACKET RBRACKET LPAREN RPAREN
%token PLUS MINUS MULT DIV QUESTION COLON DOTDOT PERCENT EQUAL
%token LT GT LE GE EQ

%left PLUS MINUS
%left MULT DIV
%left LT GT LE GE EQ

%right QUESTION COLON

%type <str> arrayref
%type <op_type> op
%type <str> e t f


%%

program:
    PROG ID LBRACE lfonc DEF MAIN LBRACE loc_section dom_section rec_section RBRACE RBRACE
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
;

/* 
   Набор функций (fonc). 
   Для каждой 'def ID { ... }' в конце 
   напечатаем метку ID: и jr $ra 
*/
lfonc:
    lfonc fonc
  | /* epsilon */
;

fonc:
    DEF ID LBRACE args loc_section dom_section rec_section RBRACE
    {
      // В конце описания функции выведем метку <ID> и "jr $ra"
      fprintf(output_file, "\n# function %s\n", $2);
      fprintf(output_file, "%s:\n", $2);
      fprintf(output_file, "jr $ra\n");
    }
;

loc_section:
    LOC ldecl
  | LOC 
  | /* empty */
;

ldecl:
    ldecl COMMA decl
  | decl
;

decl:
    ID
    {
      addSymbol($1, SYM_VAR);
    }
  | ID EQUALS e
    {
      addSymbol($1, SYM_VAR);
      fprintf(output_file, "%s", $3);
      genAssign($1);
      free($3);
    }
  | array
    {
      // addSymbol(..., SYM_ARR);
    }
;

array:
    LPAREN ID COMMA dlist RPAREN
;

dlist:
    dlist COMMA e
  | e
;

/* Аргументы [in: N; out: X], разрешаем любую комбинацию in/out/inout */
args:
    LBRACKET RBRACKET
  | LBRACKET io_sections RBRACKET
;

io_sections:
    sectionlist
;

sectionlist:
    sectionlist SEMICOLON section
  | section
;

section:
    in_section
  | out_section
  | inout_section
;

in_section:
    IN param_list
;

out_section:
    OUT param_list
;

inout_section:
    INOUT param_list
;

param_list:
    param_list COMMA param
  | param
;

param:
    ID
  | LPAREN id_list RPAREN
;

id_list:
    id_list COMMA ID
  | ID
;

dom_section:
    DOM domlist 
  | DOM 
  | /* epsilon */
;

domlist:
    domlist COMMA d
  | d
;

d:
    ID IN_SIMPLE LBRACKET e DOTDOT e RBRACKET
;

rec_section:
    REC blockinst
;

blockinst:
    LBRACE listei RBRACE
  | i
;

listei:
    | listei SEMICOLON i
    | i
;

/* Инструкции */
i:
    arrayref op EQUALS e
      {
        /* Печатаем код expr, затем arrayref assign (индекс = 0, заглушка) */
        fprintf(output_file, "%s", $4);
        fprintf(output_file, "# arrayref assign\n");
        fprintf(output_file, "la $t2, %s\n", $1);
        fprintf(output_file, "li $t1, 0\n");
        fprintf(output_file, "add $t2, $t2, $t1\n");
        fprintf(output_file, "sw $t0, 0($t2)\n");
        free($1);
        free($4);
      }
    | ID op EQUALS e
      {
        addSymbol($1, SYM_VAR);
        fprintf(output_file, "%s", $4);
        if ($2 == OP_NONE) {
          genAssign($1);
        } else if ($2 == OP_PLUS) {
          genPlusAssign($1);
        } else if ($2 == OP_MINUS) {
          genMinusAssign($1);
        } else if ($2 == OP_MULT) {
          genMultAssign($1);
        } else if ($2 == OP_DIV) {
          genDivAssign($1);
        }
        free($4);
      }
    | READ LPAREN ID RPAREN
      {
        addSymbol($3, SYM_VAR);
        genRead($3);
      }
    | PRINT LPAREN ID RPAREN
      {
        genPrint($3);
      }
    | PRINTSTR LPAREN CHAINE RPAREN
      {
        genPrintStr($3);
      }
    | call
    | PRINT LPAREN precinst RPAREN
    | cond
    | PRINT LPAREN call RPAREN
;

/* Операторы +=, -=, ... */
op:
    PLUS { $$ = OP_PLUS; }
  | MINUS { $$ = OP_MINUS; }
  | MULT { $$ = OP_MULT; }
  | DIV { $$ = OP_DIV; }
  | /* epsilon */ { $$ = OP_NONE; }
;

/* Вызовы функций (упрощённо) */
call:
    | ID LPAREN lcall RPAREN
    | ID LPAREN RPAREN
;

lcall:
    lcall COMMA call
  | lcall COMMA precinst
  | call
  | precinst
;

precinst:
    PERCENT NUMBER
;

/* arrayref */
arrayref:
  ID LBRACKET elist RBRACKET
  {
    $$ = $1;
  }
;

elist:
    elist COMMA e
  | e
;

/* Правила для e/t/f с «генерацией на лету» */
e:
    e PLUS t
    {
      char* part1 = concat2($1, "  # e PLUS t\n  move $t1, $t0\n");
      char* part2 = concat2(part1, $3);
      free(part1);
      char* final = concat2(part2, "  add $t0, $t1, $t0\n");
      free(part2);
      $$ = final;
    }
  | e MINUS t
    {
      char* p1 = concat2($1, "  # e MINUS t\n  move $t1, $t0\n");
      char* p2 = concat2(p1, $3);
      free(p1);
      char* fin = concat2(p2, "  sub $t0, $t1, $t0\n");
      free(p2);
      $$ = fin;
    }
  | MINUS e
    {
      char* fin = concat2($2, "  # unary minus\n  neg $t0, $t0\n");
      $$ = fin;
    }
  | t
    {
      $$ = $1;
    }
;

t:
    t MULT f
    {
      char* p1 = concat2($1, "  # t MULT f\n  move $t1, $t0\n");
      char* p2 = concat2(p1, $3);
      free(p1);
      char* fin = concat2(p2, "  mul $t0, $t1, $t0\n");
      free(p2);
      $$ = fin;
    }
  | t DIV f
    {
      char* p1 = concat2($1, "  # t DIV f\n  move $t1, $t0\n");
      char* p2 = concat2(p1, $3);
      free(p1);
      char* fin = concat2(p2, "  div $t1, $t0\n  mflo $t0\n");
      free(p2);
      $$ = fin;
    }
  | t PERCENT f
    {
      char* p1 = concat2($1, "  # t %% f\n  move $t1, $t0\n");
      char* p2 = concat2(p1, $3);
      free(p1);
      char* fin = concat2(p2, "  div $t1, $t0\n  mfhi $t0\n");
      free(p2);
      $$ = fin;
    }
  | f
    {
      $$ = $1;
    }
;

f:
    LPAREN e RPAREN
    {
      $$ = $2; 
    }
  | NUMBER
    {
      char buf[64];
      sprintf(buf, "  li $t0, %d\n", $1);
      $$ = strdup(buf);
    }
  | ID
    {
      char buf[64];
      sprintf(buf, "  lw $t0, %s\n", $1);
      $$ = strdup(buf);
    }
  | call
    {
      $$ = strdup("# call - not implemented\n");
    }
  | arrayref
    {
      $$ = strdup("# f: arrayref - not implemented\n");
    }
  | FLOTTANT
    {
      $$ = strdup("# float not fully handled\n");
    }
;

cond:
    e oprel e QUESTION b COLON b
;

b:
    blockinst
;

oprel:
    LT  
  | GT  
  | LE  
  | GE  
  | EQ  
;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s at line %d\n", s, yylineno);
}
