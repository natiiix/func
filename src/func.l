%{
    #include "func.tab.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdint.h>

    #define STR_TOKEN(_TOKEN_TYPE) { count(); yylval.strval = strdup(yytext); return _TOKEN_TYPE; }
    #define TOKEN(_TOKEN_TYPE) { count(); return _TOKEN_TYPE; }
    #define IGNORE_TOKEN { count(); }
    #define CHAR_TOKEN { count(); return yytext[0]; }

    size_t line = 1;
    size_t column = 1;

    void count(void);
%}

%option noyywrap

%%

"//"[^\n]*                      IGNORE_TOKEN

[\(\)]                          CHAR_TOKEN

"func"                          TOKEN(KW_FUNC)
"if"                            TOKEN(KW_IF)
"while"                         TOKEN(KW_WHILE)

[_a-zA-Z][_a-zA-Z0-9]*          STR_TOKEN(T_ID)
[0-9]+(\.[0-9]+)?               STR_TOKEN(T_NUM)
\"([^\\\"]|\\.)*\"              STR_TOKEN(T_STR)
'([^\\']|\\.)'                  STR_TOKEN(T_CHAR)

(==|!=|<|>|<=|>=|\+|-|\/|%)     STR_TOKEN(T_BIN_OP)
(=|\+=|-=|\*=|\/=|%=|&=|\|=)    STR_TOKEN(T_ASSIGN_OP)

[{}\*,\[\]]                     CHAR_TOKEN

[ \t\r\n]+                      IGNORE_TOKEN

[.\n]+                          { fprintf(stderr, "Lexer error at line %zu column %zu: unexpected token \"%.16s...\"\n", line, column, yytext); exit(-1); }

%%

void count(void) {
    for (size_t i = 0; yytext[i]; i++) {
        if (yytext[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
}
