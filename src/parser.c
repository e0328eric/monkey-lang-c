#include <stdlib.h>

#define __PRIVATE_PARSER_FUNCTIONS__
#include "parser.h"

#define FALSE 0
#define TRUE  1

struct Parser
{
    Lexer* l;
    Token curToken;
    Token peekToken;
};

Parser* mkParser(Lexer* l)
{
    Parser* output = malloc(sizeof(Parser));
    output->l = l;

    takeToken(output);
    takeToken(output);

    return output;
}

void freeParser(Parser* p)
{
    if (!p)
        return;

    freeToken(&p->curToken);
    freeToken(&p->peekToken);
    freeLexer(p->l);
    free(p);
}

void takeToken(Parser* p)
{
    freeToken(&p->curToken);
    p->curToken = p->peekToken;
    p->peekToken = nextToken(p->l);
}

int curTokenIs(Parser* p, TokenType tokType)
{
    return p->curToken.type == tokType;
}

int peekTokenIs(Parser* p, TokenType tokType)
{
    return p->peekToken.type == tokType;
}

int expectPeek(Parser* p, TokenType tokType)
{
    if (peekTokenIs(p, tokType))
    {
        takeToken(p);
        return TRUE;
    }
    return FALSE;
}

Program* parseProgram(Parser* p)
{
    Program* program = mkProgram();
    Stmt* stmt = NULL;

    while (p->curToken.type != T_EOF)
    {
        stmt = parseStmt(p);
        if (stmt)
            pushStmt(program, &stmt);
        takeToken(p);
    }

    return program;
}

Stmt* parseStmt(Parser* p)
{
    Stmt* stmt = mkStmt();

    switch (p->curToken.type)
    {
    case T_LET:
        stmt->type = STMT_LET;
        stmt->inner.letStmt = parseLetStmt(p);
        break;

    case T_RETURN:
        stmt->type = STMT_RETURN;
        stmt->inner.returnStmt = parseReturnStmt(p);
        break;

    default:
        stmt->type = STMT_EXPRESSION;
        stmt->inner.exprStmt = parseExprStmt(p);
        break;
    }

    return stmt;
}

LetStmt* parseLetStmt(Parser* p)
{
    LetStmt* letStmt = mkLetStmt();

    if (!expectPeek(p, T_IDENT))
        return NULL;

    concatString(letStmt->name->value, p->curToken.literal);

    if (!expectPeek(p, T_ASSIGN))
        return NULL;

    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while (!curTokenIs(p, T_SEMICOLON))
        takeToken(p);

    return letStmt;
}

ReturnStmt* parseReturnStmt(Parser* p) { return NULL; }
ExprStmt* parseExprStmt(Parser* p) { return NULL; }