#ifndef __tmp_main_h_
#define __tmp_main_h_

#line 1 "src/main.func"
#include <string.h>
#line 1 "src/main.func"
#include <stdio.h>
#line 2 "src/main.func"
#include <stdint.h>
#line 2 "src/main.func"
#include <stdbool.h>
#line 3 "src/main.func"
#include <stdarg.h>
#line 4 "src/main.func"
#include "func.tab.h"
#line 5 "src/main.func"
#include "strlist.h"

#line 24 "src/main.func"
int main(int argc, char** argv);
#line 126 "src/main.func"
bool strsuffix(char* str, char* suffix);
#line 133 "src/main.func"
char* getFileName(char* path);
#line 145 "src/main.func"
char* sanitizePath(char* path);
#line 158 "src/main.func"
char* strformat(char* format, ...);
#line 169 "src/main.func"
void yyerror(char const* err);

#endif
