%{
    #define _GNU_SOURCE

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>

    #include "strlist.h"

    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;

    const char* infile;

    void yyerror(const char* const err);

    char* strformat(const char* const format, ...);

    StrList_t INCLUDE_LIST;
    StrList_t STRUCT_FORWARD_LIST;
    StrList_t STRUCT_LIST;
    StrList_t FUNC_FORWARD_LIST;
    StrList_t FUNC_LIST;

    struct LinkedStr_t {
        const char* const str;
        const struct LinkedStr_t* const next;
    };

    const struct LinkedStr_t* newLinkedStr(const char* const str, const struct LinkedStr_t* const next) {
        const struct LinkedStr_t linkedstr = { .str = str, .next = next };
        return memcpy(malloc(sizeof(struct LinkedStr_t)), &linkedstr, sizeof(struct LinkedStr_t));
    }

    const char* joinLinkedStrBinOp(const struct LinkedStr_t* const ls, const char* const op) {
        return ls->next ? strformat("%s %s %s", ls->str, op, joinLinkedStrBinOp(ls->next, op)) : ls->str;
    }

    const char* joinLinkedStrCompOp(const struct LinkedStr_t* const ls, const char* const op) {
        if (ls->next) {
            const char* const strNext = joinLinkedStrCompOp(ls->next, op);
            return ls->next->next ?
                strformat("(%s %s %s) && %s", ls->str, op, ls->next->str, strNext) :
                strformat("(%s %s %s)", ls->str, op, strNext);
        } else {
            return ls->str;
        }
    }

    const char* joinLinkedStrFuncParam(const struct LinkedStr_t* const ls) {
        return ls ? ls->next ? strformat("%s, %s", ls->str, joinLinkedStrFuncParam(ls->next)) : ls->str : "";
    }

    const char* joinLinkedStrStructAttr(const struct LinkedStr_t* const ls) {
        return ls ? strformat("    %s;\n%s", ls->str, joinLinkedStrStructAttr(ls->next)) : "";
    }
%}

%locations
%define parse.error verbose

%union {
    const char* strval;
    const struct LinkedStr_t* linkedstr;
}

%token<strval> T_ID T_NUM T_STR T_CHAR
%token<strval> OP_UNARY OP_BINARY OP_COMPARE OP_ASSIGN OP_RANGE
%token KW_FUNC KW_VAR KW_STRUCT KW_DO KW_IF KW_WHILE KW_FOR KW_SIZEOF KW_TYPE KW_RETURN KW_BREAK KW_CONTINUE

%type<strval> statement_block statement
%type<strval> elem_or_var_def var_def_stat for_head else_if func_body func_params struct_attr_values var_def type pointers binary_operation
%type<strval> var_assign call_args any_expr elem_expr dollar_expr round_expr arith_expr ternary basic_value

%type<linkedstr> var_def_list operands operands_next

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

top_level_statement: '(' KW_FUNC T_ID '(' func_params ')' type func_body ')'    { StrList_append(&FUNC_LIST, strformat("%s %s(%s) {\n%s}", $7, $3, $5, $8)); StrList_append(&FUNC_FORWARD_LIST, strformat("%s %s(%s);", $7, $3, $5)); }
                   | '(' KW_STRUCT T_ID var_def_list ')'                        { StrList_append(&STRUCT_LIST, strformat("struct %s {\n%s};", $3, joinLinkedStrStructAttr($4))); StrList_append(&STRUCT_FORWARD_LIST, strformat("typedef struct %s %s;", $3, $3)); }
                   ;

statement_block:                                { $$ = ""; }
               | statement statement_block      { $$ = strformat("%s%s", $1, $2); }
               ;

statement: elem_or_var_def                              { $$ = strformat("    %s;\n", $1); }
         | '(' KW_DO statement_block ')'                { $$ = strformat("{\n%s}\n", $3); }
         | '(' KW_IF elem_expr statement else_if ')'    { $$ = strformat("if (%s) {\n%s}\n%s", $3, $4, $5); }
         | '(' KW_WHILE elem_expr statement_block ')'   { $$ = strformat("while (%s) {\n%s}\n", $3, $4); }
         | '(' KW_FOR for_head statement_block ')'      { $$ = strformat("for (%s) {\n%s}\n", $3, $4); }
         | '(' KW_RETURN ')'                            { $$ = "    return;\n"; }
         | '(' KW_RETURN any_expr ')'                   { $$ = strformat("    return %s;\n", $3); }
         | '(' KW_BREAK ')'                             { $$ = "    break;\n"; }
         | '(' KW_CONTINUE ')'                          { $$ = "    continue;\n"; }
         ;

elem_or_var_def: elem_expr              { $$ = $1; }
               | var_def_stat           { $$ = $1; }
               ;

var_def_stat: '(' KW_VAR var_def var_assign ')'     { $$ = strformat("%s%s", $3, $4); }
            ;

for_head: elem_or_var_def elem_expr elem_expr           { $$ = strformat("%s; %s; %s", $1, $2, $3); }
        | T_ID OP_RANGE elem_expr                       { $$ = strformat("int %s = 0; %s < %s; %s++", $1, $1, $3, $1); }
        | T_ID OP_RANGE elem_expr OP_RANGE elem_expr    { $$ = strformat("int %s = %s; %s < %s; %s++", $1, $3, $1, $5, $1); }
        ;

else_if:                                { $$ = ""; }
       | statement                      { $$ = strformat("else {\n%s}\n", $1); }
       | elem_expr statement else_if    { $$ = strformat("else if (%s) {\n%s}\n%s", $1, $2, $3); }
       ;

func_body: statement_block              { $$ = $1; }
         | '$' ternary                  { $$ = strformat("    return %s;\n", $2); }
         ;

func_params: var_def_list               { $$ = $1 ? joinLinkedStrFuncParam($1) : "void"; }
           | var_def                    { $$ = $1; }
           ;

struct_attr_values:                                             { $$ = ""; }
                  | '(' T_ID any_expr ')' struct_attr_values    { $$ = strformat(".%s = %s, %s", $2, $3, $5); }
                  ;

var_def_list:                               { $$ = NULL; }
            | '(' var_def ')' var_def_list  { $$ = newLinkedStr($2, $4); }
            ;

var_def: T_ID type                      { $$ = strformat("%s %s", $2, $1); }
       ;

type: T_ID pointers                     { $$ = strformat("%s%s", $1, $2); }
    ;

pointers:                               { $$ = ""; }
        | '*' pointers                  { $$ = strformat("*%s", $2); }
        ;

var_assign:                             { $$ = ""; }
          | any_expr                    { $$ = strformat(" = %s", $1); }
          ;

call_args:                              { $$ = ""; }
         | dollar_expr                  { $$ = $1; }
         | elem_expr call_args          { $$ = $2[0] ? strformat("%s, %s", $1, $2) : $1; }
         ;

any_expr: elem_expr                     { $$ = $1; }
        | dollar_expr                   { $$ = $1; }
        ;

elem_expr: '(' round_expr ')'               { $$ = $2; }
         | '[' any_expr ']'                 { $$ = strformat("(*%s)", $2); }
         | '[' elem_expr any_expr ']'       { $$ = strformat("(%s[%s])", $2, $3); }
         | '[' '*' any_expr ']'             { $$ = strformat("(&%s)", $3); }
         | '[' '*' elem_expr any_expr ']'   { $$ = strformat("(&(%s[%s]))", $3, $4); }
         | '{' struct_attr_values '}'       { $$ = strformat("{%s}", $2); }
         | elem_expr '.' T_ID               { $$ = strformat("(%s.%s)", $1, $3); }
         | basic_value                      { $$ = $1; }
         ;

dollar_expr: '$' round_expr             { $$ = $2; }
           ;

round_expr: T_ID call_args              { $$ = strformat("%s(%s)", $1, $2); }
          | arith_expr                  { $$ = strformat("(%s)", $1); }
          | KW_SIZEOF type              { $$ = strformat("sizeof(%s)", $2); }
          | KW_TYPE type any_expr       { $$ = strformat("((%s)%s)", $2, $3); }
          | '$' ternary                 { $$ = $2; }
          ;

arith_expr: OP_UNARY any_expr               { $$ = strformat("%s(%s)", $1, $2); }
          | OP_ASSIGN elem_expr any_expr    { $$ = strformat("%s %s %s", $2, $1, $3); }
          | binary_operation operands       { $$ = joinLinkedStrBinOp($2, $1); }
          | OP_COMPARE operands             { $$ = joinLinkedStrCompOp($2, $1); }
          ;

ternary: any_expr                       { $$ = $1; }
       | elem_expr elem_expr ternary    { $$ = strformat("(%s ? %s : %s)", $1, $2, $3); }
       ;

operands: elem_expr operands_next       { $$ = newLinkedStr($1, $2); }
        ;

operands_next: any_expr                 { $$ = newLinkedStr($1, NULL); }
             | operands                 { $$ = $1; }
             ;

basic_value: T_ID                       { $$ = $1; }
           | T_NUM                      { $$ = $1; }
           | T_STR                      { $$ = $1; }
           | T_CHAR                     { $$ = $1; }
           ;

binary_operation: OP_BINARY             { $$ = $1; }
                | '*'                   { $$ = "*"; }
                ;

%%

void yyerror(const char* const err) {
    fprintf(stderr, "%s:%d:%d -- Parser Error: %s\n", infile, yylloc.first_line, yylloc.first_column, err);
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
    if (argc == 1 || (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
        puts("Usage: func <output .c file> <input FunC file(s)...>");
        return 0;
    }

    if (argc < 3) {
        fprintf(stderr, "Invalid number of command-line arguments (expected at least 2, got %d)\n", argc - 1);
        return -1;
    }

    INCLUDE_LIST = StrList_ctor();
    STRUCT_LIST = StrList_ctor();
    FUNC_FORWARD_LIST = StrList_ctor();
    FUNC_LIST = StrList_ctor();

    for (int i = 2; i < argc; i++) {
        if (!(yyin = fopen(infile = argv[i], "r"))) {
            fprintf(stderr, "Unable to open input file: \"%s\"\n", infile);
            return -1;
        }

        yyparse();
        fclose(yyin);
    }

    FILE* const fc = fopen(argv[1], "w");
    if (!fc) {
        fprintf(stderr, "Unable to open output .c file: \"%s\"\n", argv[1]);
        return -1;
    }

    StrList_fjoin(&INCLUDE_LIST, fc, "\n");
    fputs("\n\n", fc);
    StrList_fjoin(&STRUCT_FORWARD_LIST, fc, "\n");
    fputs("\n\n", fc);
    StrList_fjoin(&STRUCT_LIST, fc, "\n\n");
    fputs("\n\n", fc);
    StrList_fjoin(&FUNC_FORWARD_LIST, fc, "\n");
    fputs("\n\n", fc);
    StrList_fjoin(&FUNC_LIST, fc, "\n\n");
    fputs("\n", fc);

    fclose(fc);

    StrList_dtor(&INCLUDE_LIST);
    StrList_dtor(&STRUCT_LIST);
    StrList_dtor(&FUNC_FORWARD_LIST);
    StrList_dtor(&FUNC_LIST);
}
