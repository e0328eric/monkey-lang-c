#ifndef _MONKEY_LANG_SRC_DYNSTRING_H_
#define _MONKEY_LANG_SRC_DYNSTRING_H_

#include <ctype.h>

typedef struct String String;

String* mkString(const char*);
String* mkNString(const char*, size_t);

void freeString(String*);

void appendChar(String*, char);
void appendStr(String*, const char*);
void appendNStr(String*, const char*, size_t);

void concatString(String*, String*);
void concatFreeString(String*, String*);
int cmpString(const String*, const String*);
int cmpStringStr(const String*, const char*);
int cmpStrString(const char*, const String*);

const char* getStr(const String*);
size_t getLen(const String*);

#endif //_MONKEY_LANG_SRC_DYNSTRING_H_
