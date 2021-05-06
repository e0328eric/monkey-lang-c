#include <stdlib.h>
#include <string.h>

#include "dynString.h"

struct String
{
    size_t capacity;
    size_t len;
    char* inner;
};

struct String* mkString(const char* str)
{
    struct String* string = malloc(sizeof(struct String));
    size_t strLen = strlen(str);

    string->capacity = (strLen + 1) << 1;
    string->len = strLen;
    string->inner = malloc(string->capacity);
    memcpy(string->inner, str, strLen);
    string->inner[strLen] = '\0';

    return string;
}

struct String* mkNString(const char* str, size_t strLen)
{
    struct String* string = malloc(sizeof(struct String));

    string->capacity = (strLen + 1) << 1;
    string->len = strLen;
    string->inner = malloc(string->capacity);
    memcpy(string->inner, str, strLen);
    string->inner[strLen] = '\0';

    return string;
}

void freeString(struct String* pString)
{
    free(pString->inner);
    free(pString);
}

void appendChar(struct String* pString, char chr)
{
    if (pString->len + 1 >= pString->capacity)
    {
        pString->capacity = (pString->capacity + 1) << 1;
        pString->inner = realloc(pString->inner, pString->capacity);
    }
    pString->inner[pString->len++] = chr;
    pString->inner[pString->len] = '\0';
}

void appendStr(struct String* pString, const char* str)
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

void appendNStr(struct String* pString, const char* str, size_t strLen)
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

void concatString(struct String* pString1, struct String* pString2)
{
    if (!pString1 || !pString2)
        return;

    if (pString1->len + pString2->len + 1 >= pString1->capacity)
    {
        pString1->capacity = (pString1->capacity + pString2->len + 1) << 1;
        pString1->inner = realloc(pString1->inner, pString1->capacity);
    }
    memcpy(pString1->inner + pString1->len, pString2->inner, pString2->len);
    pString1->inner[pString1->len + pString2->len] = '\0';
    pString1->len += pString2->len;
}

void concatFreeString(struct String* pString1, struct String* pString2)
{
    concatString(pString1, pString2);
    freeString(pString2);
}

int cmpString(const struct String* pString1, const struct String* pString2)
{
    return strcmp(pString1->inner, pString2->inner);
}

const char* getStr(const struct String* pString) { return pString->inner; }
size_t getLen(const struct String* pString) { return pString->len; }
