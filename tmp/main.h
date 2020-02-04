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
#line 8 "src/main.func"
typedef char const* ConstString;

#line 1 "src/LinkedStr.func"
typedef struct LinkedStr_t LinkedStr_t;

#line 1 "src/LinkedStr.func"
struct LinkedStr_t {
    ConstString str;
    LinkedStr_t const* next;
};

#line 28 "src/main.func"
int main(int argc, String* argv);
#line 134 "src/main.func"
bool strsuffix(String str, String suffix);
#line 141 "src/main.func"
String getFileName(String path);
#line 153 "src/main.func"
String sanitizePath(String path);
#line 166 "src/main.func"
String strformat(String format, ...);
#line 177 "src/main.func"
void yyerror(char const* err);
#line 6 "src/LinkedStr.func"
LinkedStr_t* newLinkedStr(ConstString str, LinkedStr_t const* next);
#line 11 "src/LinkedStr.func"
ConstString joinLinkedStrBinOp(LinkedStr_t const* ls, ConstString op);
#line 17 "src/LinkedStr.func"
ConstString joinLinkedStrCompOp(LinkedStr_t const* ls, ConstString op);
#line 31 "src/LinkedStr.func"
ConstString joinLinkedStrFuncParam(LinkedStr_t const* ls);
#line 40 "src/LinkedStr.func"
ConstString joinLinkedStrStructAttr(LinkedStr_t const* ls);

#endif
