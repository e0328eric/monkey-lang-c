#ifndef _MONKEY_LANG_SRC_TOKEN_H_
#define _MONKEY_LANG_SRC_TOKEN_H_

#include "dynString.h"

typedef enum
{
    T_ILLEGAL = -1,
    T_EOF = 0,
    T_IDENT,
    T_INT,
    T_PLUS,
    T_MINUS,
    T_BANG,
    T_ASTERISK,
    T_LT,
    T_GT,
    T_EQ,
    T_NOT_EQ,
    T_SLASH,
    T_COMMA,
    T_SEMICOLON,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_LET,
    T_FUNCTION,
    T_IF,
    T_ELSE,
    T_TRUE,
    T_FALSE,
    T_RETURN,
} TokenType;

typedef struct
{
    TokenType type;
    String literal;
} Token;

Token mkToken(TokenType, const char*);
void freeToken(Token*);

TokenType lookupIdent(const char*);

#endif //_MONKEY_LANG_SRC_TOKEN_H_
