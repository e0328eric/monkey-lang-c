#ifndef _MONKEY_LANG_SRC_DYNSTRING_H_
#define _MONKEY_LANG_SRC_DYNSTRING_H_

#include <ctype.h>

typedef struct
{
    size_t capacity;
    size_t len;
    char* inner;
} String;

#define EMPTY_STRING ((String){0, 0, NULL})

String mkString(const char*);
String mkNString(const char*, size_t);

void freeString(String*);

void appendChar(String*, char);
void appendStr(String*, const char*);
void appendNStr(String*, const char*, size_t);

String concatString(String*, String*);

const char* getStr(String*);
size_t getLen(String*);

#endif //_MONKEY_LANG_SRC_DYNSTRING_H_
