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
int getErrLen(Parser*);
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

typedef void (*PrefixParseFn)(Parser*, Expr** output);
typedef void (*InfixParseFn)(Parser*, Expr* output, Expr* left);

void peekError(Parser*, TokenType);
void illegalTokenFoundError(Parser* p);
void noPrefixParseFnError(Parser*, TokenType);
int curTokenIs(Parser*, TokenType);
int peekTokenIs(Parser*, TokenType);
int expectPeek(Parser*, TokenType);
Precedence curPrecedence(Parser*);
Precedence peekPrecedence(Parser*);

Stmt* parseStmt(Parser*);
void parseLetStmt(Parser*, Stmt*);
void parseReturnStmt(Parser*, Stmt*);
void parseExprStmt(Parser*, Stmt*);
void parseBlockStmt(Parser*, BlockStmt*);

void parseExpr(Parser*, Expr**, Precedence);
void parseFntParams(Parser*, Parameters**);
void parseCallArguments(Parser*, Arguments**);

// PrefixParseFn functions
void parseIdentExpr(Parser*, Expr**);
void parseIntExpr(Parser*, Expr**);
void parseBoolExpr(Parser*, Expr**);
void parsePrefixExpr(Parser*, Expr**);
void parseGroupedExpr(Parser*, Expr**);
void parseIfExpr(Parser*, Expr**);
void parseFntExpr(Parser*, Expr**);

// InfixParseFn functions
void parseInfixExpr(Parser*, Expr*, Expr*);
void parseCallExpr(Parser*, Expr*, Expr*);

#endif // __PRIVATE_PARSER_OBJECTS__

#endif // _MONKEY_LANG_SRC_PARSER_H_
