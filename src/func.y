%{
    #define _GNU_SOURCE

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>

    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;

    void yyerror(const char* const err);

    char* strformat(const char* const format, ...);
%}

%define parse.error verbose

%union {
    const char* strval;
}

%token<strval> T_NUM T_ID T_STR T_OP

%token KW_INCLUDE KW_FUNC KW_IF

%type<strval> module top_level_statement statement_block statement
%type<strval> func_params param_list param_list_next pointers operator
%type<strval> call_args call_args_next expression arith_expr basic_value

%%

translation_unit: module                { puts($1); }
                ;

module:                                 { $$ = ""; }
      | top_level_statement module      { $$ = strformat("%s\n%s", $1, $2); }
      ;


top_level_statement: '(' KW_INCLUDE T_ID ')'                                        { $$ = strformat("#include <%s.h>", $3); }
                   | '(' KW_FUNC T_ID T_ID '(' func_params ')' statement_block ')'  { $$ = strformat("%s %s(%s) {\n%s}", $3, $4, $6, $8); }
                   ;

statement_block:                                { $$ = ""; }
               | statement statement_block      { $$ = strformat("%s%s", $1, $2); }
               ;

statement: expression                                       { $$ = strformat("    %s;\n", $1); }
         | '{' statement_block '}'                          { $$ = strformat("{\n%s}\n", $2); }
         | '(' KW_IF expression statement statement ')'     { $$ = strformat("if (%s) {\n%s}\nelse {\n%s}\n", $3, $4, $5); }
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

expression: expression '[' expression ']'   { $$ = strformat("%s[%s]", $1, $3); }
          | '(' T_ID call_args ')'          { $$ = strformat("%s(%s)", $2, $3); }
          | '(' arith_expr ')'              { $$ = strformat("(%s)", $2); }
          | basic_value                     { $$ = $1; }
          ;

arith_expr: operator expression expression   { $$ = strformat("%s %s %s", $2, $1, $3); }
          ;

basic_value: T_NUM                      { $$ = $1; }
           | T_STR                      { $$ = $1; }
           | T_ID                       { $$ = $1; }
           ;

operator: T_OP                          { $$ = $1; }
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
    yyparse();
}
