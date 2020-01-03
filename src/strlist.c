#include "strlist.h"
#include <string.h>

const size_t INIT_CAP = 64;
const size_t ITEM_SIZE = sizeof(char*);

StrList_t StrList_ctor(void)
{
    return (StrList_t){
        .ptr = malloc(INIT_CAP * ITEM_SIZE),
        .capacity = INIT_CAP,
        .size = 0};
}

void StrList_dtor(const StrList_t* const list)
{
    if (list && list->ptr)
    {
        free(list->ptr);
    }
}

void StrList_append(StrList_t* const list, const char* const str)
{
    if (list->size == list->capacity)
    {
        list->capacity <<= 1;
        list->ptr = realloc(list->ptr, list->capacity * ITEM_SIZE);
    }

    list->ptr[list->size++] = str;
}

void StrList_fjoin(StrList_t* const list, FILE* const f, const char* const separator, const char* const suffix)
{
    for (size_t i = 0; i < list->size; i++)
    {
        if (i)
        {
            fputs(separator, f);
        }

        fputs(list->ptr[i], f);
    }

    if (list->size)
    {
        fputs(suffix, f);
    }
}
