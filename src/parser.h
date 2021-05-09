#ifndef _MONKEY_LANG_SRC_PARSER_H_
#define _MONKEY_LANG_SRC_PARSER_H_

#include "ast.h"
#include "lexer.h"

typedef struct Parser Parser;

Parser* mkParser(Lexer*);
void freeParser(Parser*);
void takeToken(Parser*);

Program* parseProgram(Parser*);

#ifdef __PRIVATE_PARSER_FUNCTIONS__
#include "token.h"

int curTokenIs(Parser*, TokenType);
int peekTokenIs(Parser*, TokenType);
int expectPeek(Parser*, TokenType);

Stmt* parseStmt(Parser*);
LetStmt* parseLetStmt(Parser*);
ReturnStmt* parseReturnStmt(Parser*);
ExprStmt* parseExprStmt(Parser*);

#endif

#endif // _MONKEY_LANG_SRC_PARSER_H_
