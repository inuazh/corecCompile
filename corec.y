%{
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







%}

%start program

%union {
    int num; 
    float flt;     
    char* str;
    int op_type;  // для хранения типа оператора
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

%type <op_type> op

%debug

%%

program:
    PROG ID LBRACE lfonc DEF MAIN LBRACE loc_section dom_section rec_section RBRACE RBRACE
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
;


lfonc:
    lfonc fonc
    | /* epsilon */
;


fonc:
    DEF ID LBRACE args loc_section dom_section rec_section RBRACE
    {
        // addSymbol($2, SYM_FUNC)
    }
;



loc_section:
    LOC ldecl
    | LOC 
    | /* epsilon */
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
      // На самом деле нужно генерировать код для expr.
      // Пусть результат expr = $t0, тогда:
      genAssign($1);
    }
  | array
    {
      // Добавляем массив, если хотите
      // addSymbol(..., SYM_ARR);
    }
;

array:
    LPAREN ID COMMA dlist RPAREN
;

dlist:
    dlist COMMA e
    |
    e
;

args:
    LBRACKET sectionargs largs RBRACKET
    | LBRACKET RBRACKET
;


largs:
    largs SEMICOLON sectionargs
    | /* epsilon */
;



sectionargs:
    IN arglist
    | OUT arglist
    | INOUT arglist
;

arglist:
    arglist COMMA eltarg
    | eltarg
;

eltarg:
    array
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
    |i
;

listei:
    | listei SEMICOLON i
    | i
;


i:
    arrayref op EQUALS e
      {
        // tmp[i,j] = expr (упрощаем: не делаем массивы, но можно)
        fprintf(output_file, "# arrayref assign (not fully implemented)\n");
      }
    | ID op EQUALS e
      {
        addSymbol($1, SYM_VAR);

        // В реальном коде: нужно сгенерировать вычисление expr => $t0
        // Мы упрощаем, не делаем "codeGen(expr)"
        
        if ($2 == OP_NONE) {
          // var = expr
          genAssign($1);
        }
        else if ($2 == OP_PLUS) {
          // var += expr
          genPlusAssign($1);
        }
        else if ($2 == OP_MINUS) {
          genMinusAssign($1);
        }
        else if ($2 == OP_MULT) {
          genMultAssign($1);
        }
        else if ($2 == OP_DIV) {
          genDivAssign($1);
        }
      }
    | READ LPAREN ID RPAREN
      {
        addSymbol($3, SYM_VAR);
        genRead($3);
      }
    | PRINT LPAREN ID RPAREN
      {
        // print(var)
        genPrint($3);
      }
    | PRINTSTR LPAREN CHAINE RPAREN
      {
        // print string
        genPrintStr($3);
      }
    | call
    | PRINT LPAREN precinst RPAREN
    | cond   /* если у вас есть */
    | PRINT LPAREN call RPAREN
;


/* Упрощенный способ различать +=, -=, *=, /= vs. = */
op:
    PLUS { $$ = OP_PLUS; }
  | MINUS { $$ = OP_MINUS; }
  | MULT { $$ = OP_MULT; }
  | DIV { $$ = OP_DIV; }
  | /* epsilon */ { $$ = OP_NONE; }
;


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

arrayref:
    ID LBRACKET elist RBRACKET
;

elist:
    elist COMMA e
    | e
;

e:
    e PLUS t
    | e MINUS t
    | MINUS e
    | t
;

t:
    t MULT f
    | t DIV f
    | t PERCENT f
    | f
;

f:
    LPAREN e RPAREN
    | ID
    | call
    | arrayref
    | NUMBER
    | FLOTTANT
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
