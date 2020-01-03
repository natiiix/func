#ifndef _STRLIST_H
#define _STRLIST_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    const char** ptr;
    size_t capacity;
    size_t size;
} StrList_t;

StrList_t StrList_ctor(void);
void StrList_dtor(const StrList_t* const list);

void StrList_append(StrList_t* const list, const char* const str);
void StrList_fjoin(StrList_t* const list, FILE* const f, const char* const separator, const char* const suffix);

#endif
