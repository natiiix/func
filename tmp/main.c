#include "main.h"

#line 5 "src/main.func"
extern int yylex();

#line 6 "src/main.func"
extern int yyparse();

#line 7 "src/main.func"
extern FILE* yyin;

#line 11 "src/main.func"
char* infile;
#line 13 "src/main.func"
StrList_t INCLUDE_LIST;
#line 14 "src/main.func"
StrList_t C_SNIPPET_LIST;
#line 15 "src/main.func"
StrList_t STRUCT_FORWARD_LIST;
#line 16 "src/main.func"
StrList_t STRUCT_LIST;
#line 17 "src/main.func"
StrList_t GLOBAL_VAR_LIST;
#line 18 "src/main.func"
StrList_t FUNC_FORWARD_LIST;
#line 19 "src/main.func"
StrList_t FUNC_LIST;

#line 21 "src/main.func"
int main(int argc, char** argv) {
#line 24 "src/main.func"
if ((((argc == 1)) || (((argc == 2)) && ((!(strcmp((argv[1]), "-h"))) || (!(strcmp((argv[1]), "--help"))))))) {
{
#line 26 "src/main.func"
    puts("Usage: func <output .c file> <input FunC file(s)...>");
#line 27 "src/main.func"
    return 0;
}
}
#line 31 "src/main.func"
if (((argc < 3))) {
{
#line 33 "src/main.func"
    fprintf(stderr, "Invalid number of command-line arguments (expected at least 2, got %d)\n", (argc - 1));
#line 34 "src/main.func"
    return (-1);
}
}
#line 38 "src/main.func"
if ((!(strsuffix((argv[1]), ".c")))) {
{
#line 40 "src/main.func"
    fputs("First argument must be a file path ending with \".c\"\n", stderr);
#line 41 "src/main.func"
    return (-1);
}
}
#line 46 "src/main.func"
    (INCLUDE_LIST = StrList_ctor());
#line 47 "src/main.func"
    (STRUCT_FORWARD_LIST = StrList_ctor());
#line 48 "src/main.func"
    (STRUCT_LIST = StrList_ctor());
#line 49 "src/main.func"
    (GLOBAL_VAR_LIST = StrList_ctor());
#line 50 "src/main.func"
    (FUNC_FORWARD_LIST = StrList_ctor());
#line 51 "src/main.func"
    (FUNC_LIST = StrList_ctor());
#line 55 "src/main.func"
for (int i = 2; i < argc; i++) {
#line 56 "src/main.func"
if ((!((yyin = fopen((infile = (argv[i])), "r"))))) {
{
#line 58 "src/main.func"
    fprintf(stderr, "Unable to open input file: \"%s\"\n", infile);
#line 59 "src/main.func"
    return (-1);
}
}
#line 63 "src/main.func"
    yyparse();
#line 64 "src/main.func"
    fclose(yyin);
}
#line 70 "src/main.func"
    char* hPath = strdup((argv[1]));
#line 71 "src/main.func"
    ((hPath[(strlen(hPath) - 1)]) = 'h');
#line 73 "src/main.func"
    FILE* fh = fopen(hPath, "w");
#line 74 "src/main.func"
if ((!(fh))) {
{
#line 76 "src/main.func"
    fprintf(stderr, "Unable to open output .h file: \"%s\"\n", hPath);
#line 77 "src/main.func"
    return (-1);
}
}
#line 82 "src/main.func"
    char* headerGuard = sanitizePath(hPath);
#line 83 "src/main.func"
    fprintf(fh, "#ifndef __%s_\n", headerGuard);
#line 84 "src/main.func"
    fprintf(fh, "#define __%s_\n\n", headerGuard);
#line 86 "src/main.func"
    StrList_fjoin((&INCLUDE_LIST), fh, "\n", "\n\n");
#line 87 "src/main.func"
    StrList_fjoin((&STRUCT_FORWARD_LIST), fh, "\n", "\n\n");
#line 88 "src/main.func"
    StrList_fjoin((&STRUCT_LIST), fh, "\n\n", "\n\n");
#line 89 "src/main.func"
    StrList_fjoin((&FUNC_FORWARD_LIST), fh, "\n", "\n\n");
#line 91 "src/main.func"
    fprintf(fh, "#endif\n");
#line 92 "src/main.func"
    fclose(fh);
#line 96 "src/main.func"
    FILE* fc = fopen((argv[1]), "w");
#line 97 "src/main.func"
if ((!(fc))) {
{
#line 99 "src/main.func"
    fprintf(stderr, "Unable to open output .c file: \"%s\"\n", (argv[1]));
#line 100 "src/main.func"
    return (-1);
}
}
#line 105 "src/main.func"
    fprintf(fc, "#include \"%s\"\n\n", getFileName(hPath));
#line 107 "src/main.func"
    StrList_fjoin((&C_SNIPPET_LIST), fc, "\n\n", "\n\n");
#line 108 "src/main.func"
    StrList_fjoin((&GLOBAL_VAR_LIST), fc, "\n", "\n\n");
#line 109 "src/main.func"
    StrList_fjoin((&FUNC_LIST), fc, "\n\n", "\n");
#line 111 "src/main.func"
    fclose(fc);
#line 114 "src/main.func"
    StrList_dtor((&INCLUDE_LIST));
#line 115 "src/main.func"
    StrList_dtor((&C_SNIPPET_LIST));
#line 116 "src/main.func"
    StrList_dtor((&STRUCT_FORWARD_LIST));
#line 117 "src/main.func"
    StrList_dtor((&STRUCT_LIST));
#line 118 "src/main.func"
    StrList_dtor((&GLOBAL_VAR_LIST));
#line 119 "src/main.func"
    StrList_dtor((&FUNC_FORWARD_LIST));
#line 120 "src/main.func"
    StrList_dtor((&FUNC_LIST));
}

#line 123 "src/main.func"
bool strsuffix(char* str, char* suffix) {
#line 124 "src/main.func"
    size_t strLen = strlen(str);
#line 125 "src/main.func"
    size_t suffixLen = strlen(suffix);
#line 127 "src/main.func"
    return (((suffixLen > strLen)) ? false : (!(strcmp((&(str[(strLen - suffixLen)])), suffix))));
}

#line 130 "src/main.func"
char* getFileName(char* path) {
#line 131 "src/main.func"
    size_t startIndex = 0;
#line 133 "src/main.func"
for (size_t i = 0; (path[i]); (++(i))) {
#line 134 "src/main.func"
if (((((path[i]) == '/')) || (((path[i]) == '\\')))) {
    (startIndex = (i + 1));
}
}
#line 139 "src/main.func"
    return (&(path[startIndex]));
}

#line 142 "src/main.func"
char* sanitizePath(char* path) {
#line 143 "src/main.func"
    char* out = strdup(path);
#line 145 "src/main.func"
for (size_t i = 0; (out[i]); (++(i))) {
#line 146 "src/main.func"
    char c = (out[i]);
#line 147 "src/main.func"
if ((!(((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (('0' <= c) && (c <= '9')))))) {
    ((out[i]) = '_');
}
}
#line 152 "src/main.func"
    return out;
}
