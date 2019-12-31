#include <string.h>
#include <stdio.h>
#include "strlist.h"

extern int yylex();

extern int yyparse();

extern FILE* yyin;





char* infile;
StrList_t INCLUDE_LIST;
StrList_t C_SNIPPET_LIST;
StrList_t STRUCT_FORWARD_LIST;
StrList_t STRUCT_LIST;
StrList_t GLOBAL_VAR_LIST;
StrList_t FUNC_FORWARD_LIST;
StrList_t FUNC_LIST;

int main(int argc, char** argv);

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
    return 0xFF;
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
    return 0xFF;
}
}
    yyparse();
    fclose(yyin);
}
    FILE* fc = fopen((argv[1]), "w");
if ((!(fc))) {
{
    fprintf(stderr, "Unable to open output .c file: \"%s\"\n", (argv[1]));
    return 0xFF;
}
}
    StrList_fjoin((&INCLUDE_LIST), fc, "\n");
    fputs("\n\n", fc);
    StrList_fjoin((&C_SNIPPET_LIST), fc, "\n\n");
    fputs("\n\n", fc);
    StrList_fjoin((&STRUCT_FORWARD_LIST), fc, "\n");
    fputs("\n\n", fc);
    StrList_fjoin((&STRUCT_LIST), fc, "\n\n");
    fputs("\n\n", fc);
    StrList_fjoin((&GLOBAL_VAR_LIST), fc, "\n");
    fputs("\n\n", fc);
    StrList_fjoin((&FUNC_FORWARD_LIST), fc, "\n");
    fputs("\n\n", fc);
    StrList_fjoin((&FUNC_LIST), fc, "\n\n");
    fputs("\n", fc);
    fclose(fc);
    StrList_dtor((&INCLUDE_LIST));
    StrList_dtor((&C_SNIPPET_LIST));
    StrList_dtor((&STRUCT_FORWARD_LIST));
    StrList_dtor((&STRUCT_LIST));
    StrList_dtor((&GLOBAL_VAR_LIST));
    StrList_dtor((&FUNC_FORWARD_LIST));
    StrList_dtor((&FUNC_LIST));
}
