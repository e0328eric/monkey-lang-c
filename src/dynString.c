#include <stdlib.h>
#include <string.h>

#include "dynString.h"

String mkString(const char* str)
{
    String string = EMPTY_STRING;
    size_t strLen = strlen(str);

    string.capacity = (strLen + 1) << 1;
    string.len = strLen;
    string.inner = malloc(string.capacity);
    memcpy(string.inner, str, strLen);
    string.inner[strLen] = '\0';

    return string;
}

String mkNString(const char* str, size_t strLen)
{
    String string = (String){0, 0, NULL};

    string.capacity = (strLen + 1) << 1;
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
        pString->capacity = (pString->capacity + 1) << 1;
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
        pString->capacity = (pString->capacity + strLen + 1) << 1;
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
        pString->capacity = (pString->capacity + strLen + 1) << 1;
        pString->inner = realloc(pString->inner, pString->capacity);
    }
    memcpy(pString->inner + pString->len, str, strLen);
    pString->inner[pString->len + strLen] = '\0';
    pString->len += strLen;
}

String concatString(const String* pString1, const String* pString2)
{
    if (!pString1 || !pString2)
        return EMPTY_STRING;

    String string = EMPTY_STRING;
    string.capacity = pString1->capacity;
    string.len = pString1->len;
    string.inner = malloc(string.capacity);
    memcpy(string.inner, pString1->inner, string.len);

    if (string.len + pString2->len + 1 >= string.capacity)
    {
        string.capacity = (string.capacity + pString2->len + 1) << 1;
        string.inner = realloc(string.inner, string.capacity);
    }
    memcpy(string.inner + string.len, pString2->inner, pString2->len);
    string.inner[string.len + pString2->len] = '\0';
    string.len += pString2->len;

    return string;
}

const char* getStr(const String* pString) { return pString->inner; }
size_t getLen(const String* pString) { return pString->len; }
