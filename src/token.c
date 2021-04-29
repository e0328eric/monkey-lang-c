#include <stdlib.h>
#include <string.h>

#include "token.h"

Token mkToken(TokenType type, const char* str)
{
    String literal = mkString(str);
    return (Token){type, literal};
}

void freeToken(Token* pTok) { free(&pTok->literal); }

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
