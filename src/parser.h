#ifndef _MONKEY_LANG_SRC_PARSER_H_
#define _MONKEY_LANG_SRC_PARSER_H_

#include "ast.h"
#include "lexer.h"

#define MAXIMUM_ERR_MSGS 20

typedef struct Parser Parser;

Parser* mkParser(Lexer*);
void freeParser(Parser*);
void takeToken(Parser*);

String** getErrors(Parser*);
Program* parseProgram(Parser*);

#ifdef __PRIVATE_PARSER_OBJECTS__
#include "token.h"

typedef enum
{
    ZERO_PREC = 0,
    LOWEST_PREC,
    EQ_PREC,
    CMP_PREC,
    SUM_PREC,
    PRODUCT_PREC,
    PREFIX_PREC,
    CALL_PREC,
} Precedence;

typedef void (*PrefixParseFn)(Parser*, Expr* output);
typedef void (*InfixParseFn)(Parser*, Expr* output, Expr* left);

void peekError(Parser*, TokenType);
void noPrefixParseFnError(Parser*, TokenType);
int curTokenIs(Parser*, TokenType);
int peekTokenIs(Parser*, TokenType);
int expectPeek(Parser*, TokenType);

Stmt* parseStmt(Parser*);
void parseLetStmt(Parser*, Stmt*);
void parseReturnStmt(Parser*, Stmt*);
void parseExprStmt(Parser*, Stmt*);

void parseExpr(Parser*, Expr*, Precedence);

// PrefixParseFn functions
void parseIdentExpr(Parser*, Expr*);
void parseIntExpr(Parser*, Expr*);
void parsePrefixExpr(Parser*, Expr*);

#endif // __PRIVATE_PARSER_OBJECTS__

#endif // _MONKEY_LANG_SRC_PARSER_H_
