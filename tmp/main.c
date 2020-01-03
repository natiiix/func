#line 1 "src/main.func"
#include <string.h>
#line 1 "src/main.func"
#include <stdio.h>
#line 2 "src/main.func"
#include "strlist.h"

#line 4 "src/main.func"
extern int yylex();

#line 5 "src/main.func"
extern int yyparse();

#line 6 "src/main.func"
extern FILE* yyin;

#line 8 "src/main.func"
char* infile;
#line 10 "src/main.func"
StrList_t INCLUDE_LIST;
#line 11 "src/main.func"
StrList_t C_SNIPPET_LIST;
#line 12 "src/main.func"
StrList_t STRUCT_FORWARD_LIST;
#line 13 "src/main.func"
StrList_t STRUCT_LIST;
#line 14 "src/main.func"
StrList_t GLOBAL_VAR_LIST;
#line 15 "src/main.func"
StrList_t FUNC_FORWARD_LIST;
#line 16 "src/main.func"
StrList_t FUNC_LIST;

#line 18 "src/main.func"
int main(int argc, char** argv);

#line 18 "src/main.func"
int main(int argc, char** argv) {
if ((((argc == 1)) || (((argc == 2)) && ((!(strcmp((argv[1]), "-h"))) || (!(strcmp((argv[1]), "--help"))))))) {
{
    puts("Usage: func <output .c file> <input FunC file(s)...>");
    return 0;
}
}
if (((argc < 3))) {
{
    fprintf(stderr, "Invalid number of command-line arguments (expected at least 2, got %d)\n", (argc - 1));
    return (-1);
}
}
    (INCLUDE_LIST = StrList_ctor());
    (STRUCT_FORWARD_LIST = StrList_ctor());
    (STRUCT_LIST = StrList_ctor());
    (GLOBAL_VAR_LIST = StrList_ctor());
    (FUNC_FORWARD_LIST = StrList_ctor());
    (FUNC_LIST = StrList_ctor());
for (int i = 2; i < argc; i++) {
if ((!((yyin = fopen((infile = (argv[i])), "r"))))) {
{
    fprintf(stderr, "Unable to open input file: \"%s\"\n", infile);
    return (-1);
}
}
    yyparse();
    fclose(yyin);
}
    FILE* fc = fopen((argv[1]), "w");
if ((!(fc))) {
{
    fprintf(stderr, "Unable to open output .c file: \"%s\"\n", (argv[1]));
    return (-1);
}
}
    StrList_fjoin((&INCLUDE_LIST), fc, "\n", "\n\n");
    StrList_fjoin((&C_SNIPPET_LIST), fc, "\n\n", "\n\n");
    StrList_fjoin((&STRUCT_FORWARD_LIST), fc, "\n", "\n\n");
    StrList_fjoin((&STRUCT_LIST), fc, "\n\n", "\n\n");
    StrList_fjoin((&GLOBAL_VAR_LIST), fc, "\n", "\n\n");
    StrList_fjoin((&FUNC_FORWARD_LIST), fc, "\n", "\n\n");
    StrList_fjoin((&FUNC_LIST), fc, "\n\n", "\n");
    fclose(fc);
    StrList_dtor((&INCLUDE_LIST));
    StrList_dtor((&C_SNIPPET_LIST));
    StrList_dtor((&STRUCT_FORWARD_LIST));
    StrList_dtor((&STRUCT_LIST));
    StrList_dtor((&GLOBAL_VAR_LIST));
    StrList_dtor((&FUNC_FORWARD_LIST));
    StrList_dtor((&FUNC_LIST));
}
