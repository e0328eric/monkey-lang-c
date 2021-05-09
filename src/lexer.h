#ifndef _MONKEY_LANG_SRC_LEXER_H_
#define _MONKEY_LANG_SRC_LEXER_H_

#include "token.h"

typedef struct Lexer Lexer;

Lexer* mkLexer(const char*);
void freeLexer(Lexer*);

Token nextToken(Lexer*);

#endif //_MONKEY_LANG_SRC_LEXER_H_
