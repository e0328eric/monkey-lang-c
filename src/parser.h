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

#ifdef __PRIVATE_PARSER_FUNCTIONS__
#include "token.h"

void peekError(Parser*, TokenType);
int curTokenIs(Parser*, TokenType);
int peekTokenIs(Parser*, TokenType);
int expectPeek(Parser*, TokenType);

Stmt* parseStmt(Parser*);
void parseLetStmt(Parser*, Stmt*);
void parseReturnStmt(Parser*, Stmt*);
void parseExprStmt(Parser*, Stmt*);

#endif

#endif // _MONKEY_LANG_SRC_PARSER_H_
