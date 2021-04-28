#include <stdlib.h>
#include <string.h>

#include "dynString.h"

String mkString(const char* str)
{
    String string = EMPTY_STRING;
    size_t strLen = strlen(str);

    string.capacity = (size_t)((double)(strLen + 1) * 1.1);
    string.len = strLen;
    string.inner = malloc(string.capacity);
    memcpy(string.inner, str, strLen);
    string.inner[strLen] = '\0';

    return string;
}

String mkNString(const char* str, size_t strLen)
{
    String string = (String){0, 0, NULL};

    string.capacity = (size_t)((double)(strLen + 1) * 1.1);
    string.len = strLen;
    string.inner = malloc(string.capacity);
    memcpy(string.inner, str, strLen);
    string.inner[strLen] = '\0';

    return string;
}

void freeString(String* pString) { free(pString->inner); }

void appendChar(String* pString, char chr)
{
    if (pString->len + 1 >= pString->capacity)
    {
        pString->capacity = (size_t)((double)(pString->capacity + 1) * 1.1);
        pString->inner = realloc(pString->inner, pString->capacity);
    }
    pString->inner[pString->len++] = chr;
    pString->inner[pString->len] = '\0';
}

void appendStr(String* pString, const char* str)
{
    size_t strLen = strlen(str);
    if (pString->len + strLen + 1 >= pString->capacity)
    {
        pString->capacity =
            (size_t)((double)(pString->capacity + strLen + 1) * 1.1);
        pString->inner = realloc(pString->inner, pString->capacity);
    }
    memcpy(pString->inner + pString->len, str, strLen);
    pString->inner[pString->len + strLen] = '\0';
    pString->len += strLen;
}

void appendNStr(String* pString, const char* str, size_t strLen)
{
    if (pString->len + strLen + 1 >= pString->capacity)
    {
        pString->capacity =
            (size_t)((double)(pString->capacity + strLen + 1) * 1.1);
        pString->inner = realloc(pString->inner, pString->capacity);
    }
    memcpy(pString->inner + pString->len, str, strLen);
    pString->inner[pString->len + strLen] = '\0';
    pString->len += strLen;
}

String concatString(String* pString1, String* pString2)
{
    if (pString1->len + pString2->len + 1 >= pString1->capacity)
    {
        pString1->capacity =
            (size_t)((double)(pString1->capacity + pString2->len + 1) * 1.1);
        pString1->inner = realloc(pString1->inner, pString1->capacity);
    }
    memcpy(pString1->inner + pString1->len, pString2->inner, pString2->len);
    pString1->inner[pString1->len + pString2->len] = '\0';
    pString1->len += pString2->len;
}

const char* getStr(String* pString) { return pString->inner; }
size_t getLen(String* pString) { return pString->len; }
