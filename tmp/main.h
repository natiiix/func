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
#include "strlist.h"

#line 21 "src/main.func"
int main(int argc, char** argv);
#line 123 "src/main.func"
bool strsuffix(char* str, char* suffix);
#line 130 "src/main.func"
char* getFileName(char* path);
#line 142 "src/main.func"
char* sanitizePath(char* path);

#endif
