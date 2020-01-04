%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>
    #include <stdbool.h>

    #include "strlist.h"
    #include "main.h"

    extern int yylex();
    extern int yyparse();
    extern FILE* yyin;

    const char* infile;

    extern StrList_t INCLUDE_LIST;
    extern StrList_t C_SNIPPET_LIST;
    extern StrList_t STRUCT_FORWARD_LIST;
    extern StrList_t STRUCT_LIST;
    extern StrList_t GLOBAL_VAR_LIST;
    extern StrList_t FUNC_FORWARD_LIST;
    extern StrList_t FUNC_LIST;

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

    char* cstrip(const char* const str) {
        size_t len = strlen(str);
        char* out = malloc(len - 1);

        size_t index = 0;
        bool escaped = false;

        for (size_t i = 1; i < len - 1; i++) {
            if (escaped) {
                if (str[i] != '"') {
                    out[index++] = '\\';
                }

                escaped = false;
            } else if (str[i] == '\\') {
                escaped = true;
                continue;
            }

            out[index++] = str[i];
        }

        out[index] = '\0';
        return out;
    }

    #define LOC_JOIN(_OUT, _FIRST, _LAST) _OUT.first_line = _FIRST.first_line; _OUT.first_column = _FIRST.first_column; _OUT.last_line = _LAST.last_line; _OUT.last_column = _LAST.last_column;
    #define LOC_COPY(_OUT, _FIRST) _OUT = _FIRST;
    #define LOC_ZERO(_OUT) _OUT.first_line = _OUT.first_column = _OUT.last_line = _OUT.last_column = 0;
    #define LOC_LINE(_IN) strformat("#line %d \"%s\"\n", _IN.first_line, infile)
%}

%locations
%define parse.error verbose

%union {
    const char* strval;
    const struct LinkedStr_t* linkedstr;
}

%token<strval> T_ID T_NUM T_STR T_CHAR
%token<strval> OP_UNARY OP_BINARY OP_COMPARE OP_ASSIGN OP_RANGE
%token KW_FUNC KW_VAR KW_STRUCT KW_DO KW_IF KW_WHILE KW_FOR KW_SIZEOF KW_TYPE KW_CONST KW_RETURN KW_BREAK KW_CONTINUE

%type<strval> statement_block statement
%type<strval> elem_or_var_def var_def_stat for_head else_if func_head func_body func_params struct_attr_values var_def type raw_type minus_operation binary_operation
%type<strval> var_assign call_args any_expr elem_expr dollar_expr round_expr arith_expr ptr_member ternary basic_value

%type<linkedstr> var_def_list operands operands_next

%%

module: include_block top_level_statement_block     { LOC_JOIN(@$, @1, @2); }
      ;

include_block:                                      { LOC_ZERO(@$); }
             | '[' include_list ']' include_block   { LOC_JOIN(@$, @1, @4); }
             ;

include_list:                           { LOC_ZERO(@$); }
            | T_ID include_list         { LOC_JOIN(@$, @1, @2); StrList_append(&INCLUDE_LIST, strformat("%s#include <%s.h>", LOC_LINE(@$), $1)); }
            | T_STR include_list        { LOC_JOIN(@$, @1, @2); StrList_append(&INCLUDE_LIST, strformat("%s#include %s", LOC_LINE(@$), $1)); }
            ;

top_level_statement_block:                                                  { LOC_ZERO(@$); }
                         | top_level_statement top_level_statement_block    { LOC_JOIN(@$, @1, @2); }
                         ;

top_level_statement: '(' KW_FUNC func_head func_body ')'    { LOC_JOIN(@$, @1, @5); StrList_append(&FUNC_LIST, strformat("%s%s {\n%s}", LOC_LINE(@$), $3, $4)); StrList_append(&FUNC_FORWARD_LIST, strformat("%s%s;", LOC_LINE(@$), $3)); }
                   | '(' KW_STRUCT T_ID var_def_list ')'    { LOC_JOIN(@$, @1, @5); StrList_append(&STRUCT_LIST, strformat("%sstruct %s {\n%s};", LOC_LINE(@$), $3, joinLinkedStrStructAttr($4))); StrList_append(&STRUCT_FORWARD_LIST, strformat("%stypedef struct %s %s;", LOC_LINE(@$), $3, $3)); }
                   | var_def_stat                           { LOC_COPY(@$, @1); StrList_append(&GLOBAL_VAR_LIST, strformat("%s%s;", LOC_LINE(@$), $1)); }
                   | T_STR                                  { LOC_COPY(@$, @1); StrList_append(&C_SNIPPET_LIST, strformat("%s%s", LOC_LINE(@$), cstrip($1))); }
                   ;

statement_block:                                { LOC_ZERO(@$); $$ = ""; }
               | statement statement_block      { LOC_JOIN(@$, @1, @2); $$ = strformat("%s%s%s", LOC_LINE(@1), $1, $2); }
               ;

statement: elem_or_var_def                              { LOC_COPY(@$, @1); $$ = strformat("    %s;\n", $1); }
         | '(' KW_DO statement_block ')'                { LOC_JOIN(@$, @1, @4); $$ = strformat("{\n%s}\n", $3); }
         | '(' KW_IF elem_expr statement else_if ')'    { LOC_JOIN(@$, @1, @6); $$ = strformat("if (%s) {\n%s}\n%s", $3, $4, $5); }
         | '(' KW_WHILE elem_expr statement_block ')'   { LOC_JOIN(@$, @1, @5); $$ = strformat("while (%s) {\n%s}\n", $3, $4); }
         | '(' KW_FOR for_head statement_block ')'      { LOC_JOIN(@$, @1, @5); $$ = strformat("for (%s) {\n%s}\n", $3, $4); }
         | '(' KW_RETURN ')'                            { LOC_JOIN(@$, @1, @3); $$ = "    return;\n"; }
         | '(' KW_RETURN any_expr ')'                   { LOC_JOIN(@$, @1, @4); $$ = strformat("    return %s;\n", $3); }
         | '(' KW_BREAK ')'                             { LOC_JOIN(@$, @1, @3); $$ = "    break;\n"; }
         | '(' KW_CONTINUE ')'                          { LOC_JOIN(@$, @1, @3); $$ = "    continue;\n"; }
         ;

elem_or_var_def: elem_expr              { LOC_COPY(@$, @1); $$ = $1; }
               | var_def_stat           { LOC_COPY(@$, @1); $$ = $1; }
               ;

var_def_stat: '(' KW_VAR var_def var_assign ')'     { LOC_JOIN(@$, @1, @5); $$ = strformat("%s%s", $3, $4); }
            ;

for_head: elem_or_var_def elem_expr elem_expr           { LOC_JOIN(@$, @1, @3); $$ = strformat("%s; %s; %s", $1, $2, $3); }
        | T_ID OP_RANGE elem_expr                       { LOC_JOIN(@$, @1, @2); $$ = strformat("int %s = 0; %s < %s; %s++", $1, $1, $3, $1); }
        | T_ID OP_RANGE elem_expr OP_RANGE elem_expr    { LOC_JOIN(@$, @1, @4); $$ = strformat("int %s = %s; %s < %s; %s++", $1, $3, $1, $5, $1); }
        ;

else_if:                                { LOC_ZERO(@$); $$ = ""; }
       | statement                      { LOC_COPY(@$, @1); $$ = strformat("else {\n%s}\n", $1); }
       | elem_expr statement else_if    { LOC_JOIN(@$, @1, @3); $$ = strformat("else if (%s) {\n%s}\n%s", $1, $2, $3); }
       ;

func_head: T_ID '(' func_params ')' type    { LOC_JOIN(@$, @1, @5); $$ = strformat("%s %s(%s)", $5, $1, $3); }
         | T_ID type                        { LOC_JOIN(@$, @1, @2); $$ = strformat("%s %s()", $2, $1); }
         ;

func_body: statement_block              { LOC_COPY(@$, @1); $$ = $1; }
         | '$' ternary                  { LOC_JOIN(@$, @1, @2); $$ = strformat("    return %s;\n", $2); }
         ;

func_params: var_def_list               { LOC_COPY(@$, @1); $$ = $1 ? joinLinkedStrFuncParam($1) : "void"; }
           | var_def                    { LOC_COPY(@$, @1); $$ = $1; }
           | var_def_list OP_RANGE      { LOC_JOIN(@$, @1, @2); $$ = $1 ? joinLinkedStrFuncParam(newLinkedStr("...", $1)) : "..."; }
           | var_def OP_RANGE           { LOC_JOIN(@$, @1, @2); $$ = strformat("%s, ...", $1); }
           ;

struct_attr_values:                                             { LOC_ZERO(@$); $$ = ""; }
                  | '(' T_ID any_expr ')' struct_attr_values    { LOC_JOIN(@$, @1, @4); $$ = strformat(".%s = %s, %s", $2, $3, $5); }
                  ;

var_def_list:                               { LOC_ZERO(@$); $$ = NULL; }
            | '(' var_def ')' var_def_list  { LOC_JOIN(@$, @1, @4); $$ = newLinkedStr($2, $4); }
            ;

var_def: T_ID type                      { LOC_JOIN(@$, @1, @2); $$ = strformat("%s %s", $2, $1); }
       ;

type: raw_type                          { LOC_COPY(@$, @1); $$ = $1; }
    | KW_CONST raw_type                 { LOC_JOIN(@$, @1, @2); $$ = strformat("%s const", $2); }
    ;

raw_type: T_ID                          { LOC_COPY(@$, @1); $$ = $1; }
        | '*' type                      { LOC_JOIN(@$, @1, @2); $$ = strformat("%s*", $2); }
        ;

var_assign:                             { LOC_ZERO(@$); $$ = ""; }
          | any_expr                    { LOC_COPY(@$, @1); $$ = strformat(" = %s", $1); }
          ;

call_args:                              { LOC_ZERO(@$); $$ = ""; }
         | dollar_expr                  { LOC_COPY(@$, @1); $$ = $1; }
         | elem_expr call_args          { LOC_JOIN(@$, @1, @2); $$ = $2[0] ? strformat("%s, %s", $1, $2) : $1; }
         ;

any_expr: elem_expr                     { LOC_COPY(@$, @1); $$ = $1; }
        | dollar_expr                   { LOC_COPY(@$, @1); $$ = $1; }
        ;

elem_expr: '(' round_expr ')'               { LOC_JOIN(@$, @1, @3); $$ = $2; }
         | '[' any_expr ']'                 { LOC_JOIN(@$, @1, @3); $$ = strformat("(*%s)", $2); }
         | '[' elem_expr any_expr ']'       { LOC_JOIN(@$, @1, @4); $$ = strformat("(%s[%s])", $2, $3); }
         | '[' '*' any_expr ']'             { LOC_JOIN(@$, @1, @4); $$ = strformat("(&%s)", $3); }
         | '[' '*' elem_expr any_expr ']'   { LOC_JOIN(@$, @1, @5); $$ = strformat("(&(%s[%s]))", $3, $4); }
         | '{' struct_attr_values '}'       { LOC_JOIN(@$, @1, @3); $$ = strformat("{%s}", $2); }
         | '{' T_ID struct_attr_values '}'  { LOC_JOIN(@$, @1, @4); $$ = strformat("((%s){%s})", $2, $3); }
         | elem_expr '.' T_ID               { LOC_JOIN(@$, @1, @3); $$ = strformat("(%s.%s)", $1, $3); }
         | elem_expr '.' '[' ptr_member ']' { LOC_JOIN(@$, @1, @3); $$ = strformat("(%s%s)", $1, $4); }
         | basic_value                      { LOC_COPY(@$, @1); $$ = $1; }
         ;

dollar_expr: '$' round_expr             { LOC_JOIN(@$, @1, @2); $$ = $2; }
           ;

round_expr: T_ID call_args              { LOC_JOIN(@$, @1, @2); $$ = strformat("%s(%s)", $1, $2); }
          | arith_expr                  { LOC_COPY(@$, @1); $$ = strformat("(%s)", $1); }
          | KW_SIZEOF type              { LOC_JOIN(@$, @1, @2); $$ = strformat("sizeof(%s)", $2); }
          | KW_TYPE type any_expr       { LOC_JOIN(@$, @1, @3); $$ = strformat("((%s)%s)", $2, $3); }
          | '$' ternary                 { LOC_JOIN(@$, @1, @2); $$ = $2; }
          ;

arith_expr: binary_operation operands       { LOC_JOIN(@$, @1, @2); $$ = joinLinkedStrBinOp($2, $1); }
          | OP_UNARY any_expr               { LOC_JOIN(@$, @1, @2); $$ = strformat("%s(%s)", $1, $2); }
          | OP_ASSIGN elem_expr any_expr    { LOC_JOIN(@$, @1, @3); $$ = strformat("%s %s %s", $2, $1, $3); }
          | OP_COMPARE operands             { LOC_JOIN(@$, @1, @2); $$ = joinLinkedStrCompOp($2, $1); }
          | minus_operation                 { LOC_COPY(@$, @1); $$ = $1; }
          ;

ptr_member:                             { LOC_ZERO(@$); $$ = ""; }
          | T_ID ptr_member             { LOC_JOIN(@$, @1, @2); $$ = strformat("->%s%s", $1, $2); }
          ;

ternary: any_expr                       { LOC_COPY(@$, @1); $$ = $1; }
       | elem_expr elem_expr ternary    { LOC_JOIN(@$, @1, @3); $$ = strformat("(%s ? %s : %s)", $1, $2, $3); }
       ;

operands: elem_expr operands_next       { LOC_JOIN(@$, @1, @2); $$ = newLinkedStr($1, $2); }
        ;

operands_next: any_expr                 { LOC_COPY(@$, @1); $$ = newLinkedStr($1, NULL); }
             | operands                 { LOC_COPY(@$, @1); $$ = $1; }
             ;

basic_value: T_ID                       { LOC_COPY(@$, @1); $$ = $1; }
           | T_NUM                      { LOC_COPY(@$, @1); $$ = $1; }
           | T_STR                      { LOC_COPY(@$, @1); $$ = $1; }
           | T_CHAR                     { LOC_COPY(@$, @1); $$ = $1; }
           ;

minus_operation: '-' any_expr           { LOC_JOIN(@$, @1, @2); $$ = strformat("-%s", $2); }
               | '-' operands           { LOC_JOIN(@$, @1, @2); $$ = joinLinkedStrBinOp($2, "-"); }
               ;

binary_operation: OP_BINARY             { LOC_COPY(@$, @1); $$ = $1; }
                | '*'                   { LOC_COPY(@$, @1); $$ = "*"; }
                ;

%%
