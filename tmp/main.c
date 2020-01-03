#include "main.h"

#line 7 "src/main.func"
extern int yylex();

#line 8 "src/main.func"
extern int yyparse();

#line 9 "src/main.func"
extern FILE* yyin;

#line 13 "src/main.func"
char* infile;
#line 15 "src/main.func"
StrList_t INCLUDE_LIST;
#line 16 "src/main.func"
StrList_t C_SNIPPET_LIST;
#line 17 "src/main.func"
StrList_t STRUCT_FORWARD_LIST;
#line 18 "src/main.func"
StrList_t STRUCT_LIST;
#line 19 "src/main.func"
StrList_t GLOBAL_VAR_LIST;
#line 20 "src/main.func"
StrList_t FUNC_FORWARD_LIST;
#line 21 "src/main.func"
StrList_t FUNC_LIST;

#line 23 "src/main.func"
int main(int argc, char** argv) {
#line 26 "src/main.func"
if ((((argc == 1)) || (((argc == 2)) && ((!(strcmp((argv[1]), "-h"))) || (!(strcmp((argv[1]), "--help"))))))) {
{
#line 28 "src/main.func"
    puts("Usage: func <output .c file> <input FunC file(s)...>");
#line 29 "src/main.func"
    return 0;
}
}
#line 33 "src/main.func"
if (((argc < 3))) {
{
#line 35 "src/main.func"
    fprintf(stderr, "Invalid number of command-line arguments (expected at least 2, got %d)\n", (argc - 1));
#line 36 "src/main.func"
    return (-1);
}
}
#line 40 "src/main.func"
if ((!(strsuffix((argv[1]), ".c")))) {
{
#line 42 "src/main.func"
    fputs("First argument must be a file path ending with \".c\"\n", stderr);
#line 43 "src/main.func"
    return (-1);
}
}
#line 48 "src/main.func"
    (INCLUDE_LIST = StrList_ctor());
#line 49 "src/main.func"
    (STRUCT_FORWARD_LIST = StrList_ctor());
#line 50 "src/main.func"
    (STRUCT_LIST = StrList_ctor());
#line 51 "src/main.func"
    (GLOBAL_VAR_LIST = StrList_ctor());
#line 52 "src/main.func"
    (FUNC_FORWARD_LIST = StrList_ctor());
#line 53 "src/main.func"
    (FUNC_LIST = StrList_ctor());
#line 57 "src/main.func"
for (int i = 2; i < argc; i++) {
#line 58 "src/main.func"
if ((!((yyin = fopen((infile = (argv[i])), "r"))))) {
{
#line 60 "src/main.func"
    fprintf(stderr, "Unable to open input file: \"%s\"\n", infile);
#line 61 "src/main.func"
    return (-1);
}
}
#line 65 "src/main.func"
    yyparse();
#line 66 "src/main.func"
    fclose(yyin);
}
#line 72 "src/main.func"
    char* hPath = strdup((argv[1]));
#line 73 "src/main.func"
    ((hPath[(strlen(hPath) - 1)]) = 'h');
#line 75 "src/main.func"
    FILE* fh = fopen(hPath, "w");
#line 76 "src/main.func"
if ((!(fh))) {
{
#line 78 "src/main.func"
    fprintf(stderr, "Unable to open output .h file: \"%s\"\n", hPath);
#line 79 "src/main.func"
    return (-1);
}
}
#line 84 "src/main.func"
    char* headerGuard = sanitizePath(hPath);
#line 85 "src/main.func"
    fprintf(fh, "#ifndef __%s_\n", headerGuard);
#line 86 "src/main.func"
    fprintf(fh, "#define __%s_\n\n", headerGuard);
#line 88 "src/main.func"
    StrList_fjoin((&INCLUDE_LIST), fh, "\n", "\n\n");
#line 89 "src/main.func"
    StrList_fjoin((&STRUCT_FORWARD_LIST), fh, "\n", "\n\n");
#line 90 "src/main.func"
    StrList_fjoin((&STRUCT_LIST), fh, "\n\n", "\n\n");
#line 91 "src/main.func"
    StrList_fjoin((&FUNC_FORWARD_LIST), fh, "\n", "\n\n");
#line 93 "src/main.func"
    fprintf(fh, "#endif\n");
#line 94 "src/main.func"
    fclose(fh);
#line 98 "src/main.func"
    FILE* fc = fopen((argv[1]), "w");
#line 99 "src/main.func"
if ((!(fc))) {
{
#line 101 "src/main.func"
    fprintf(stderr, "Unable to open output .c file: \"%s\"\n", (argv[1]));
#line 102 "src/main.func"
    return (-1);
}
}
#line 107 "src/main.func"
    fprintf(fc, "#include \"%s\"\n\n", getFileName(hPath));
#line 109 "src/main.func"
    StrList_fjoin((&C_SNIPPET_LIST), fc, "\n\n", "\n\n");
#line 110 "src/main.func"
    StrList_fjoin((&GLOBAL_VAR_LIST), fc, "\n", "\n\n");
#line 111 "src/main.func"
    StrList_fjoin((&FUNC_LIST), fc, "\n\n", "\n");
#line 113 "src/main.func"
    fclose(fc);
#line 116 "src/main.func"
    StrList_dtor((&INCLUDE_LIST));
#line 117 "src/main.func"
    StrList_dtor((&C_SNIPPET_LIST));
#line 118 "src/main.func"
    StrList_dtor((&STRUCT_FORWARD_LIST));
#line 119 "src/main.func"
    StrList_dtor((&STRUCT_LIST));
#line 120 "src/main.func"
    StrList_dtor((&GLOBAL_VAR_LIST));
#line 121 "src/main.func"
    StrList_dtor((&FUNC_FORWARD_LIST));
#line 122 "src/main.func"
    StrList_dtor((&FUNC_LIST));
}

#line 125 "src/main.func"
bool strsuffix(char* str, char* suffix) {
#line 126 "src/main.func"
    size_t strLen = strlen(str);
#line 127 "src/main.func"
    size_t suffixLen = strlen(suffix);
#line 129 "src/main.func"
    return (((suffixLen > strLen)) ? false : (!(strcmp((&(str[(strLen - suffixLen)])), suffix))));
}

#line 132 "src/main.func"
char* getFileName(char* path) {
#line 133 "src/main.func"
    size_t startIndex = 0;
#line 135 "src/main.func"
for (size_t i = 0; (path[i]); (++(i))) {
#line 136 "src/main.func"
if (((((path[i]) == '/')) || (((path[i]) == '\\')))) {
    (startIndex = (i + 1));
}
}
#line 141 "src/main.func"
    return (&(path[startIndex]));
}

#line 144 "src/main.func"
char* sanitizePath(char* path) {
#line 145 "src/main.func"
    char* out = strdup(path);
#line 147 "src/main.func"
for (size_t i = 0; (out[i]); (++(i))) {
#line 148 "src/main.func"
    char c = (out[i]);
#line 149 "src/main.func"
if ((!(((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (('0' <= c) && (c <= '9')))))) {
    ((out[i]) = '_');
}
}
#line 154 "src/main.func"
    return out;
}

#line 157 "src/main.func"
char* strformat(char* format, ...) {
#line 158 "src/main.func"
    va_list vargs;
#line 159 "src/main.func"
    va_start(vargs, format);
#line 161 "src/main.func"
    char* s = NULL;
#line 162 "src/main.func"
    vasprintf((&s), format, vargs);
#line 164 "src/main.func"
    va_end(vargs);
#line 165 "src/main.func"
    return s;
}

#line 168 "src/main.func"
void yyerror(const char* err) {
#line 169 "src/main.func"
    fprintf(stderr, "%s:%d:%d -- Parser Error: %s\n", infile, (yylloc.first_line), (yylloc.first_column), err);
#line 170 "src/main.func"
    exit((-1));
}
