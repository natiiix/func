#include "main.h"

#line 9 "src/main.func"
extern FILE* yyin;

#line 11 "src/main.func"
int yylex();

#line 12 "src/main.func"
int yyparse();

#line 16 "src/main.func"
String infile;
#line 18 "src/main.func"
StrList_t INCLUDE_LIST;
#line 19 "src/main.func"
StrList_t TYPEDEF_LIST;
#line 20 "src/main.func"
StrList_t C_SNIPPET_LIST;
#line 21 "src/main.func"
StrList_t STRUCT_FORWARD_LIST;
#line 22 "src/main.func"
StrList_t STRUCT_LIST;
#line 23 "src/main.func"
StrList_t GLOBAL_VAR_LIST;
#line 24 "src/main.func"
StrList_t FUNC_FORWARD_LIST;
#line 25 "src/main.func"
StrList_t FUNC_LIST;

#line 27 "src/main.func"
int main(int argc, String* argv) {
#line 30 "src/main.func"
if ((((argc == 1)) || (((argc == 2)) && ((!(strcmp((argv[1]), "-h"))) || (!(strcmp((argv[1]), "--help"))))))) {
#line 31 "src/main.func"
{
#line 32 "src/main.func"
    puts("Usage: func <output .c file> <input FunC file(s)...>");
#line 33 "src/main.func"
    return 0;
}
}
#line 37 "src/main.func"
if (((argc < 3))) {
#line 38 "src/main.func"
{
#line 39 "src/main.func"
    fprintf(stderr, "Invalid number of command-line arguments (expected at least 2, got %d)\n", (argc - 1));
#line 40 "src/main.func"
    return (-1);
}
}
#line 44 "src/main.func"
if ((!(strsuffix((argv[1]), ".c")))) {
#line 45 "src/main.func"
{
#line 46 "src/main.func"
    fputs("First argument must be a file path ending with \".c\"\n", stderr);
#line 47 "src/main.func"
    return (-1);
}
}
#line 52 "src/main.func"
    (INCLUDE_LIST = StrList_ctor());
#line 53 "src/main.func"
    (TYPEDEF_LIST = StrList_ctor());
#line 54 "src/main.func"
    (C_SNIPPET_LIST = StrList_ctor());
#line 55 "src/main.func"
    (STRUCT_FORWARD_LIST = StrList_ctor());
#line 56 "src/main.func"
    (STRUCT_LIST = StrList_ctor());
#line 57 "src/main.func"
    (GLOBAL_VAR_LIST = StrList_ctor());
#line 58 "src/main.func"
    (FUNC_FORWARD_LIST = StrList_ctor());
#line 59 "src/main.func"
    (FUNC_LIST = StrList_ctor());
#line 63 "src/main.func"
for (int i = 2; i < argc; i++) {
#line 64 "src/main.func"
if ((!((yyin = fopen((infile = (argv[i])), "r"))))) {
#line 65 "src/main.func"
{
#line 66 "src/main.func"
    fprintf(stderr, "Unable to open input file: \"%s\"\n", infile);
#line 67 "src/main.func"
    return (-1);
}
}
#line 71 "src/main.func"
    yyparse();
#line 72 "src/main.func"
    fclose(yyin);
}
#line 78 "src/main.func"
    String hPath = strdup((argv[1]));
#line 79 "src/main.func"
    ((hPath[(strlen(hPath) - 1)]) = 'h');
#line 81 "src/main.func"
    FILE* fh = fopen(hPath, "w");
#line 82 "src/main.func"
if ((!(fh))) {
#line 83 "src/main.func"
{
#line 84 "src/main.func"
    fprintf(stderr, "Unable to open output .h file: \"%s\"\n", hPath);
#line 85 "src/main.func"
    return (-1);
}
}
#line 90 "src/main.func"
    String headerGuard = sanitizePath(hPath);
#line 91 "src/main.func"
    fprintf(fh, "#ifndef __%s_\n", headerGuard);
#line 92 "src/main.func"
    fprintf(fh, "#define __%s_\n\n", headerGuard);
#line 94 "src/main.func"
    StrList_fjoin((&INCLUDE_LIST), fh, "\n", "\n\n");
#line 95 "src/main.func"
    StrList_fjoin((&TYPEDEF_LIST), fh, "\n", "\n\n");
#line 96 "src/main.func"
    StrList_fjoin((&STRUCT_FORWARD_LIST), fh, "\n", "\n\n");
#line 97 "src/main.func"
    StrList_fjoin((&STRUCT_LIST), fh, "\n\n", "\n\n");
#line 98 "src/main.func"
    StrList_fjoin((&FUNC_FORWARD_LIST), fh, "\n", "\n\n");
#line 100 "src/main.func"
    fprintf(fh, "#endif\n");
#line 101 "src/main.func"
    fclose(fh);
#line 105 "src/main.func"
    FILE* fc = fopen((argv[1]), "w");
#line 106 "src/main.func"
if ((!(fc))) {
#line 107 "src/main.func"
{
#line 108 "src/main.func"
    fprintf(stderr, "Unable to open output .c file: \"%s\"\n", (argv[1]));
#line 109 "src/main.func"
    return (-1);
}
}
#line 114 "src/main.func"
    fprintf(fc, "#include \"%s\"\n\n", getFileName(hPath));
#line 116 "src/main.func"
    StrList_fjoin((&C_SNIPPET_LIST), fc, "\n\n", "\n\n");
#line 117 "src/main.func"
    StrList_fjoin((&GLOBAL_VAR_LIST), fc, "\n", "\n\n");
#line 118 "src/main.func"
    StrList_fjoin((&FUNC_LIST), fc, "\n\n", "\n");
#line 120 "src/main.func"
    fclose(fc);
#line 123 "src/main.func"
    StrList_dtor((&INCLUDE_LIST));
#line 124 "src/main.func"
    StrList_dtor((&TYPEDEF_LIST));
#line 125 "src/main.func"
    StrList_dtor((&C_SNIPPET_LIST));
#line 126 "src/main.func"
    StrList_dtor((&STRUCT_FORWARD_LIST));
#line 127 "src/main.func"
    StrList_dtor((&STRUCT_LIST));
#line 128 "src/main.func"
    StrList_dtor((&GLOBAL_VAR_LIST));
#line 129 "src/main.func"
    StrList_dtor((&FUNC_FORWARD_LIST));
#line 130 "src/main.func"
    StrList_dtor((&FUNC_LIST));
}

#line 133 "src/main.func"
bool strsuffix(String str, String suffix) {
#line 134 "src/main.func"
    size_t strLen = strlen(str);
#line 135 "src/main.func"
    size_t suffixLen = strlen(suffix);
#line 137 "src/main.func"
    return (((suffixLen > strLen)) ? false : (!(strcmp((&(str[(strLen - suffixLen)])), suffix))));
}

#line 140 "src/main.func"
String getFileName(String path) {
#line 141 "src/main.func"
    size_t startIndex = 0;
#line 143 "src/main.func"
for (size_t i = 0; (path[i]); (++(i))) {
#line 144 "src/main.func"
if (((((path[i]) == '/')) || (((path[i]) == '\\')))) {
#line 145 "src/main.func"
    (startIndex = (i + 1));
}
}
#line 149 "src/main.func"
    return (&(path[startIndex]));
}

#line 152 "src/main.func"
String sanitizePath(String path) {
#line 153 "src/main.func"
    String out = strdup(path);
#line 155 "src/main.func"
for (size_t i = 0; (out[i]); (++(i))) {
#line 156 "src/main.func"
    char c = (out[i]);
#line 157 "src/main.func"
if ((!(((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (('0' <= c) && (c <= '9')))))) {
#line 158 "src/main.func"
    ((out[i]) = '_');
}
}
#line 162 "src/main.func"
    return out;
}

#line 165 "src/main.func"
String strformat(String format, ...) {
#line 166 "src/main.func"
    va_list vargs;
#line 167 "src/main.func"
    va_start(vargs, format);
#line 169 "src/main.func"
    String s = NULL;
#line 170 "src/main.func"
    vasprintf((&s), format, vargs);
#line 172 "src/main.func"
    va_end(vargs);
#line 173 "src/main.func"
    return s;
}

#line 176 "src/main.func"
void yyerror(char const* err) {
#line 177 "src/main.func"
    fprintf(stderr, "%s:%d:%d -- Parser Error: %s\n", infile, (yylloc.first_line), (yylloc.first_column), err);
#line 178 "src/main.func"
    exit((-1));
}
