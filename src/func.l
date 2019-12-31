%{
    #include "func.tab.h"

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdint.h>

    #define STR_TOKEN(_TOKEN_TYPE) { yylval.strval = strdup(yytext); return _TOKEN_TYPE; }
    #define TOKEN(_TOKEN_TYPE) { return _TOKEN_TYPE; }
    #define IGNORE_TOKEN { }
    #define CHAR_TOKEN { return yytext[0]; }

    extern const char* infile;

    void count(void);

    #define YY_USER_ACTION count();
%}

%option noyywrap
%option nounput
%option noinput

%%

"//"[^\n]*                      IGNORE_TOKEN

"func"                          TOKEN(KW_FUNC)
"var"                           TOKEN(KW_VAR)
"struct"                        TOKEN(KW_STRUCT)
"do"                            TOKEN(KW_DO)
"if"                            TOKEN(KW_IF)
"while"                         TOKEN(KW_WHILE)
"for"                           TOKEN(KW_FOR)
"sizeof"                        TOKEN(KW_SIZEOF)
"type"                          TOKEN(KW_TYPE)
"return"                        TOKEN(KW_RETURN)
"break"                         TOKEN(KW_BREAK)
"continue"                      TOKEN(KW_CONTINUE)

[_a-zA-Z][_a-zA-Z0-9]*                      STR_TOKEN(T_ID)
[0-9]+(\.[0-9]+)?|0x[0-9a-fA-F]+|0b[01]+    STR_TOKEN(T_NUM)
\"([^\\\"]|\\.)*\"                          STR_TOKEN(T_STR)
'([^\\']|\\.)'                              STR_TOKEN(T_CHAR)

".."                            TOKEN(OP_RANGE)

("++"|"--"|"!")                                             STR_TOKEN(OP_UNARY)
("=="|"!="|"<="|">="|"<"|">")                               STR_TOKEN(OP_COMPARE)
("="|"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=")   STR_TOKEN(OP_ASSIGN)
("+"|"/"|"%"|"&&"|"||"|"&"|"|"|"^"|"<<"|">>")               STR_TOKEN(OP_BINARY)

[\(\)\[\]{}\$\.\*\-]            CHAR_TOKEN

[ \t\r\n]+                      IGNORE_TOKEN

.|\n                            { fprintf(stderr, "%s:%d:%d -- Lexer Error: unexpected character '%c'\n", infile, yylloc.first_line, yylloc.first_column, yytext[0]); exit(-1); }

%%

void count(void) {
    static const char* prev_infile = NULL;
    static int line = 0;
    static int column = 0;

    // It is necessary to reset the location counters
    // when the input file changes to a new file.
    // NOTE: I know this is pointer comparison, it's fine.
    if (infile != prev_infile) {
        prev_infile = infile;
        line = 1;
        column = 1;
    }

    yylloc.first_line = line;
    yylloc.first_column = column;

    for (size_t i = 0; yytext[i]; i++) {
        if (yytext[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }

    yylloc.last_line = line;
    yylloc.last_column = column - 1;
}
