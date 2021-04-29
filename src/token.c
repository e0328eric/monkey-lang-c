#include <stdlib.h>
#include <string.h>

#include "token.h"

Token mkToken(TokenType type, const char* str)
{
    String literal = mkString(str);
    return (Token){type, literal};
}

void freeToken(Token* pTok) { freeString(&pTok->literal); }

TokenType lookupIdent(const char* str)
{
#define MAKE_KEYWORDS(expected, tokType) \
    if (strcmp(expected, str) == 0)      \
        return tokType;

    MAKE_KEYWORDS("let", T_LET);
    MAKE_KEYWORDS("fn", T_FUNCTION);
    MAKE_KEYWORDS("if", T_IF);
    MAKE_KEYWORDS("else", T_ELSE);
    MAKE_KEYWORDS("true", T_TRUE);
    MAKE_KEYWORDS("false", T_FALSE);
    MAKE_KEYWORDS("return", T_RETURN);

    return T_IDENT;

#undef MAKE_KEYWORDS
}

const char* printTokType(TokenType tokType)
{
    switch (tokType)
    {
    case T_ILLEGAL:
        return "ILLEGAL";
    case T_EOF:
        return "EOF";
    case T_IDENT:
        return "IDENT";
    case T_INT:
        return "INT";
    case T_ASSIGN:
        return "ASSIGN";
    case T_PLUS:
        return "PLUS";
    case T_MINUS:
        return "MINUS";
    case T_BANG:
        return "BANG";
    case T_ASTERISK:
        return "ASTERISK";
    case T_LT:
        return "LT";
    case T_GT:
        return "GT";
    case T_EQ:
        return "EQ";
    case T_NOT_EQ:
        return "NOT_EQ";
    case T_SLASH:
        return "SLASH";
    case T_COMMA:
        return "COMMA";
    case T_SEMICOLON:
        return "SEMICOLON";
    case T_LPAREN:
        return "LPAREN";
    case T_RPAREN:
        return "RPAREN";
    case T_LBRACE:
        return "LBRACE";
    case T_RBRACE:
        return "RBRACE";
    case T_LET:
        return "LET";
    case T_FUNCTION:
        return "FUNCTION";
    case T_IF:
        return "IF";
    case T_ELSE:
        return "ELSE";
    case T_TRUE:
        return "TRUE";
    case T_FALSE:
        return "FALSE";
    case T_RETURN:
        return "RETURN";
    default:
        return "????";
    }
}
