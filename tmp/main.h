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

#line 7 "src/main.func"
typedef char* String;

#line 27 "src/main.func"
int main(int argc, String* argv);
#line 133 "src/main.func"
bool strsuffix(String str, String suffix);
#line 140 "src/main.func"
String getFileName(String path);
#line 152 "src/main.func"
String sanitizePath(String path);
#line 165 "src/main.func"
String strformat(String format, ...);
#line 176 "src/main.func"
void yyerror(char const* err);

#endif
