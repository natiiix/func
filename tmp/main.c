#include "main.h"

#line 10 "src/main.func"
extern FILE* yyin;

#line 12 "src/main.func"
int yylex();

#line 13 "src/main.func"
int yyparse();

#line 17 "src/main.func"
String infile;
#line 19 "src/main.func"
StrList_t INCLUDE_LIST;
#line 20 "src/main.func"
StrList_t TYPEDEF_LIST;
#line 21 "src/main.func"
StrList_t C_SNIPPET_LIST;
#line 22 "src/main.func"
StrList_t STRUCT_FORWARD_LIST;
#line 23 "src/main.func"
StrList_t STRUCT_LIST;
#line 24 "src/main.func"
StrList_t GLOBAL_VAR_LIST;
#line 25 "src/main.func"
StrList_t FUNC_FORWARD_LIST;
#line 26 "src/main.func"
StrList_t FUNC_LIST;

#line 28 "src/main.func"
int main(int argc, String* argv) {
#line 31 "src/main.func"
if ((((argc == 1)) || (((argc == 2)) && ((!(strcmp((argv[1]), "-h"))) || (!(strcmp((argv[1]), "--help"))))))) {
#line 32 "src/main.func"
{
#line 33 "src/main.func"
    puts("Usage: func <output .c file> <input FunC file(s)...>");
#line 34 "src/main.func"
    return 0;
}
}
#line 38 "src/main.func"
if (((argc < 3))) {
#line 39 "src/main.func"
{
#line 40 "src/main.func"
    fprintf(stderr, "Invalid number of command-line arguments (expected at least 2, got %d)\n", (argc - 1));
#line 41 "src/main.func"
    return (-1);
}
}
#line 45 "src/main.func"
if ((!(strsuffix((argv[1]), ".c")))) {
#line 46 "src/main.func"
{
#line 47 "src/main.func"
    fputs("First argument must be a file path ending with \".c\"\n", stderr);
#line 48 "src/main.func"
    return (-1);
}
}
#line 53 "src/main.func"
    (INCLUDE_LIST = StrList_ctor());
#line 54 "src/main.func"
    (TYPEDEF_LIST = StrList_ctor());
#line 55 "src/main.func"
    (C_SNIPPET_LIST = StrList_ctor());
#line 56 "src/main.func"
    (STRUCT_FORWARD_LIST = StrList_ctor());
#line 57 "src/main.func"
    (STRUCT_LIST = StrList_ctor());
#line 58 "src/main.func"
    (GLOBAL_VAR_LIST = StrList_ctor());
#line 59 "src/main.func"
    (FUNC_FORWARD_LIST = StrList_ctor());
#line 60 "src/main.func"
    (FUNC_LIST = StrList_ctor());
#line 64 "src/main.func"
for (int i = 2; i < argc; i++) {
#line 65 "src/main.func"
if ((!((yyin = fopen((infile = (argv[i])), "r"))))) {
#line 66 "src/main.func"
{
#line 67 "src/main.func"
    fprintf(stderr, "Unable to open input file: \"%s\"\n", infile);
#line 68 "src/main.func"
    return (-1);
}
}
#line 72 "src/main.func"
    yyparse();
#line 73 "src/main.func"
    fclose(yyin);
}
#line 79 "src/main.func"
    String hPath = strdup((argv[1]));
#line 80 "src/main.func"
    ((hPath[(strlen(hPath) - 1)]) = 'h');
#line 82 "src/main.func"
    FILE* fh = fopen(hPath, "w");
#line 83 "src/main.func"
if ((!(fh))) {
#line 84 "src/main.func"
{
#line 85 "src/main.func"
    fprintf(stderr, "Unable to open output .h file: \"%s\"\n", hPath);
#line 86 "src/main.func"
    return (-1);
}
}
#line 91 "src/main.func"
    String headerGuard = sanitizePath(hPath);
#line 92 "src/main.func"
    fprintf(fh, "#ifndef __%s_\n", headerGuard);
#line 93 "src/main.func"
    fprintf(fh, "#define __%s_\n\n", headerGuard);
#line 95 "src/main.func"
    StrList_fjoin((&INCLUDE_LIST), fh, "\n", "\n\n");
#line 96 "src/main.func"
    StrList_fjoin((&TYPEDEF_LIST), fh, "\n", "\n\n");
#line 97 "src/main.func"
    StrList_fjoin((&STRUCT_FORWARD_LIST), fh, "\n", "\n\n");
#line 98 "src/main.func"
    StrList_fjoin((&STRUCT_LIST), fh, "\n\n", "\n\n");
#line 99 "src/main.func"
    StrList_fjoin((&FUNC_FORWARD_LIST), fh, "\n", "\n\n");
#line 101 "src/main.func"
    fprintf(fh, "#endif\n");
#line 102 "src/main.func"
    fclose(fh);
#line 106 "src/main.func"
    FILE* fc = fopen((argv[1]), "w");
#line 107 "src/main.func"
if ((!(fc))) {
#line 108 "src/main.func"
{
#line 109 "src/main.func"
    fprintf(stderr, "Unable to open output .c file: \"%s\"\n", (argv[1]));
#line 110 "src/main.func"
    return (-1);
}
}
#line 115 "src/main.func"
    fprintf(fc, "#include \"%s\"\n\n", getFileName(hPath));
#line 117 "src/main.func"
    StrList_fjoin((&C_SNIPPET_LIST), fc, "\n\n", "\n\n");
#line 118 "src/main.func"
    StrList_fjoin((&GLOBAL_VAR_LIST), fc, "\n", "\n\n");
#line 119 "src/main.func"
    StrList_fjoin((&FUNC_LIST), fc, "\n\n", "\n");
#line 121 "src/main.func"
    fclose(fc);
#line 124 "src/main.func"
    StrList_dtor((&INCLUDE_LIST));
#line 125 "src/main.func"
    StrList_dtor((&TYPEDEF_LIST));
#line 126 "src/main.func"
    StrList_dtor((&C_SNIPPET_LIST));
#line 127 "src/main.func"
    StrList_dtor((&STRUCT_FORWARD_LIST));
#line 128 "src/main.func"
    StrList_dtor((&STRUCT_LIST));
#line 129 "src/main.func"
    StrList_dtor((&GLOBAL_VAR_LIST));
#line 130 "src/main.func"
    StrList_dtor((&FUNC_FORWARD_LIST));
#line 131 "src/main.func"
    StrList_dtor((&FUNC_LIST));
}

#line 134 "src/main.func"
bool strsuffix(String str, String suffix) {
#line 135 "src/main.func"
    size_t strLen = strlen(str);
#line 136 "src/main.func"
    size_t suffixLen = strlen(suffix);
#line 138 "src/main.func"
    return (((suffixLen > strLen)) ? false : (!(strcmp((&(str[(strLen - suffixLen)])), suffix))));
}

#line 141 "src/main.func"
String getFileName(String path) {
#line 142 "src/main.func"
    size_t startIndex = 0;
#line 144 "src/main.func"
for (size_t i = 0; (path[i]); (++(i))) {
#line 145 "src/main.func"
if (((((path[i]) == '/')) || (((path[i]) == '\\')))) {
#line 146 "src/main.func"
    (startIndex = (i + 1));
}
}
#line 150 "src/main.func"
    return (&(path[startIndex]));
}

#line 153 "src/main.func"
String sanitizePath(String path) {
#line 154 "src/main.func"
    String out = strdup(path);
#line 156 "src/main.func"
for (size_t i = 0; (out[i]); (++(i))) {
#line 157 "src/main.func"
    char c = (out[i]);
#line 158 "src/main.func"
if ((!(((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (('0' <= c) && (c <= '9')))))) {
#line 159 "src/main.func"
    ((out[i]) = '_');
}
}
#line 163 "src/main.func"
    return out;
}

#line 166 "src/main.func"
String strformat(String format, ...) {
#line 167 "src/main.func"
    va_list vargs;
#line 168 "src/main.func"
    va_start(vargs, format);
#line 170 "src/main.func"
    String s = NULL;
#line 171 "src/main.func"
    vasprintf((&s), format, vargs);
#line 173 "src/main.func"
    va_end(vargs);
#line 174 "src/main.func"
    return s;
}

#line 177 "src/main.func"
void yyerror(char const* err) {
#line 178 "src/main.func"
    fprintf(stderr, "%s:%d:%d -- Parser Error: %s\n", infile, (yylloc.first_line), (yylloc.first_column), err);
#line 179 "src/main.func"
    exit((-1));
}

#line 6 "src/LinkedStr.func"
LinkedStr_t* newLinkedStr(ConstString str, LinkedStr_t const* next) {
#line 7 "src/LinkedStr.func"
    LinkedStr_t linkedstr = {.str = str, .next = next, };
#line 8 "src/LinkedStr.func"
    return memcpy(malloc(sizeof(LinkedStr_t)), (&linkedstr), sizeof(LinkedStr_t));
}

#line 11 "src/LinkedStr.func"
ConstString joinLinkedStrBinOp(LinkedStr_t const* ls, ConstString op) {
    return ((ls->next) ? strformat("%s %s %s", (ls->str), op, joinLinkedStrBinOp((ls->next), op)) : (ls->str));
}

#line 17 "src/LinkedStr.func"
ConstString joinLinkedStrCompOp(LinkedStr_t const* ls, ConstString op) {
#line 18 "src/LinkedStr.func"
if ((ls->next)) {
#line 19 "src/LinkedStr.func"
{
#line 20 "src/LinkedStr.func"
    ConstString strNext = joinLinkedStrCompOp((ls->next), op);
#line 21 "src/LinkedStr.func"
    return ((ls->next->next) ? strformat("(%s %s %s) && %s", (ls->str), op, (ls->next->str), strNext) : strformat("(%s %s %s)", (ls->str), op, strNext));
}
}
else {
#line 27 "src/LinkedStr.func"
    return (ls->str);
}
}

#line 31 "src/LinkedStr.func"
ConstString joinLinkedStrFuncParam(LinkedStr_t const* ls) {
    return (ls ? ((ls->next) ? strformat("%s, %s", (ls->str), joinLinkedStrFuncParam((ls->next))) : (ls->str)) : "");
}

#line 40 "src/LinkedStr.func"
ConstString joinLinkedStrStructAttr(LinkedStr_t const* ls) {
    return (ls ? strformat("    %s;\n%s", (ls->str), joinLinkedStrStructAttr((ls->next))) : "");
}
