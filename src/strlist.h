#ifndef _STRLIST_H
#define _STRLIST_H

#include <stdint.h>
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
char* StrList_join(StrList_t* const list, const char* const delimiter);

#endif
