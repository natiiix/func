%{
    #define _GNU_SOURCE

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>

    #include "src/strlist.h"

    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;

    void yyerror(const char* const err);

    char* strformat(const char* const format, ...);

    StrList_t INCLUDE_LIST;
    StrList_t FUNC_PREDEF_LIST;
    StrList_t FUNC_LIST;
%}

%define parse.error verbose

%union {
    const char* strval;
}

%token<strval> T_ID T_NUM T_STR T_CHAR T_BIN_OP T_ASSIGN_OP

%token KW_INCLUDE KW_FUNC KW_IF KW_WHILE

%type<strval> statement_block statement
%type<strval> func_params param_list param_list_next pointers operator
%type<strval> call_args call_args_next expression arith_expr basic_value

%%

module: include_block top_level_statement_block
      ;

include_block:
             | '[' include_list ']' include_block
             ;

include_list:
            | T_ID include_list         { StrList_append(&INCLUDE_LIST, strformat("#include <%s.h>", $1)); }
            | T_STR include_list        { StrList_append(&INCLUDE_LIST, strformat("#include \"%s\"", $1)); }
            ;

top_level_statement_block:
                         | top_level_statement top_level_statement_block
                         ;

top_level_statement: '(' KW_FUNC T_ID '(' func_params ')' T_ID statement_block ')'  { StrList_append(&FUNC_LIST, strformat("%s %s(%s) {\n%s}", $7, $3, $5, $8)); StrList_append(&FUNC_PREDEF_LIST, strformat("%s %s(%s);", $7, $3, $5)); }
                   ;

statement_block:                                { $$ = ""; }
               | statement statement_block      { $$ = strformat("%s%s", $1, $2); }
               ;

statement: expression                                       { $$ = strformat("    %s;\n", $1); }
         | '{' statement_block '}'                          { $$ = strformat("{\n%s}\n", $2); }
         | '(' KW_IF expression statement statement ')'     { $$ = strformat("if (%s) {\n%s}\nelse {\n%s}\n", $3, $4, $5); }
         | '(' KW_WHILE expression statement_block ')'       { $$ = strformat("while (%s) {\n%s}\n", $3, $4); }
         ;

func_params:                            { $$ = "void"; }
           | param_list                 { $$ = $1; }
           ;

param_list: T_ID pointers T_ID param_list_next  { $$ = strformat("%s%s %s%s", $1, $2, $3, $4); }
          ;

param_list_next:                        { $$ = ""; }
               | ',' param_list         { $$ = strformat(", %s", $2); }
               ;

pointers:                               { $$ = ""; }
        | '*' pointers                  { $$ = strformat("*%s", $2); }
        ;

call_args:                              { $$ = ""; }
         | expression call_args_next    { $$ = strformat("%s%s", $1, $2); }
         ;

call_args_next:                             { $$ = ""; }
              | expression call_args_next   { $$ = strformat(", %s%s", $1, $2); }
              ;

expression: '(' T_ID call_args ')'          { $$ = strformat("%s(%s)", $2, $3); }
          | '(' arith_expr ')'              { $$ = strformat("(%s)", $2); }
          | '[' expression ']'              { $$ = strformat("(*%s)", $2); }
          | '[' expression expression ']'   { $$ = strformat("(%s[%s])", $2, $3); }
          | basic_value                     { $$ = $1; }
          ;

arith_expr: operator expression expression   { $$ = strformat("%s %s %s", $2, $1, $3); }
          ;

basic_value: T_ID                       { $$ = $1; }
           | T_NUM                      { $$ = $1; }
           | T_STR                      { $$ = $1; }
           | T_CHAR                     { $$ = $1; }
           ;

operator: T_BIN_OP                      { $$ = $1; }
        | T_ASSIGN_OP                   { $$ = $1; }
        | '*'                           { $$ = "*"; }
        ;

%%

void yyerror(const char* const err) {
    fprintf(stderr, "Parser error: %s\n", err);
    exit(-1);
}

char* strformat(const char* const format, ...) {
    va_list vargs;
    va_start(vargs, format);

    char* s = NULL;
    vasprintf(&s, format, vargs);

    va_end(vargs);

    return s;
}

int main(const int argc, const char* const* const argv) {
    yyin = stdin;

    INCLUDE_LIST = StrList_ctor();
    FUNC_PREDEF_LIST = StrList_ctor();

    yyparse();

    StrList_printf(&INCLUDE_LIST, "%s\n");
    puts("");
    StrList_printf(&FUNC_PREDEF_LIST, "%s\n");
    puts("");
    StrList_printf(&FUNC_LIST, "%s\n\n");

    StrList_dtor(&INCLUDE_LIST);
    StrList_dtor(&FUNC_PREDEF_LIST);
}
