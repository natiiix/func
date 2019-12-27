#include "strlist.h"
#include <stdio.h>
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

char* StrList_join(StrList_t* const list, const char* const separator)
{
    const size_t sepLen = separator ? strlen(separator) : 0;

    size_t totalLen = 0;
    for (size_t i = 0; i < list->size; i++)
    {
        totalLen += strlen(list->ptr[i]);

        if (i)
        {
            totalLen += sepLen;
        }
    }

    char* const result = malloc(totalLen + 1);

    size_t index = 0;
    for (size_t i = 0; i < list->size; i++)
    {
        strcpy(&result[index], list->ptr[i]);
        index += strlen(list->ptr[i]);

        if (i)
        {
            strcpy(&result[index], separator);
            index += sepLen;
        }
    }

    if (index != totalLen)
    {
        free(result);
        return NULL;
    }

    return result;
}

void StrList_printf(StrList_t* const list, const char* const format)
{
    for (size_t i = 0; i < list->size; i++)
    {
        printf(format, list->ptr[i]);
    }
}
