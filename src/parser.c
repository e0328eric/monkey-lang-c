#include <stdio.h>
#include <stdlib.h>

#define __PRIVATE_PARSER_FUNCTIONS__
#include "parser.h"

#define FALSE 0
#define TRUE  1

#define ASSERT(expected)                                 \
    if (!(expected))                                     \
    {                                                    \
        fprintf(stderr, "Assert Failed for " #expected); \
        __asm("int $3");                                 \
    }

#define EXPECT_PEEK(nodeName, tokType, destructor) \
    if (!expectPeek(p, tokType))                   \
    {                                              \
        destructor(nodeName);                      \
        stmt->inner.nodeName = NULL;               \
        return;                                    \
    }

struct Parser
{
    Lexer* l;
    Token curToken;
    Token peekToken;
    String** errors;
    int errLen;
};

Parser* mkParser(Lexer* l)
{
    Parser* output = malloc(sizeof(Parser));
    output->l = l;
    output->errors = NULL;
    output->errLen = 0;

    takeToken(output);
    takeToken(output);

    return output;
}

void freeParser(Parser* p)
{
    if (!p)
        return;

    if (p->errors)
    {
        for (int i = 0; i < MAXIMUM_ERR_MSGS; ++i)
            freeString(p->errors[i]);
        free(p->errors);
    }

    freeToken(&p->curToken);
    freeToken(&p->peekToken);
    freeLexer(p->l);
    free(p);
}

Program* parseProgram(Parser* p)
{
    Program* program = mkProgram();
    Stmt* stmt = NULL;

    while (p->curToken.type != T_EOF)
    {
        stmt = parseStmt(p);
        if (stmt->inner.checkIsNull != 0)
            pushStmt(program, &stmt);
        else
            freeStmt(stmt);
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
        parseLetStmt(p, stmt);
        break;

    case T_RETURN:
        stmt->type = STMT_RETURN;
        parseReturnStmt(p, stmt);
        break;

    default:
        stmt->type = STMT_EXPRESSION;
        parseExprStmt(p, stmt);
        break;
    }

    return stmt;
}

void parseLetStmt(Parser* p, Stmt* stmt)
{
    ASSERT(stmt);

    LetStmt* letStmt = mkLetStmt();

    EXPECT_PEEK(letStmt, T_IDENT, freeLetStmt);
    letStmt->name->value = mkString(getStr(p->curToken.literal));
    EXPECT_PEEK(letStmt, T_ASSIGN, freeLetStmt);

    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while (!curTokenIs(p, T_SEMICOLON))
        takeToken(p);

    stmt->inner.letStmt = letStmt;
}

void parseReturnStmt(Parser* p, Stmt* stmt)
{
    ASSERT(stmt);

    ReturnStmt* returnStmt = mkReturnStmt();
    takeToken(p);

    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while (!curTokenIs(p, T_SEMICOLON))
        takeToken(p);

    stmt->inner.returnStmt = returnStmt;
}

void parseExprStmt(Parser* p, Stmt* stmt) {}

String** getErrors(Parser* p)
{
    String** output = p->errors;
    p->errors = NULL;
    p->errLen = 0;
    return output;
}

void peekError(Parser* p, TokenType tokType)
{
    char msg[60];

    if (p->errLen >= MAXIMUM_ERR_MSGS)
    {
        fprintf(stderr, "too many parse error occurs.");
        return;
    }

    sprintf(msg, "expected next token to be %s, got %s instead",
            printTokType(tokType), printTokType(p->peekToken.type));

    if (!p->errors)
        p->errors = malloc(sizeof(String*) * (MAXIMUM_ERR_MSGS + 1));
    p->errors[p->errLen++] = mkString(msg);
    p->errors[p->errLen] = NULL;
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
    peekError(p, tokType);
    return FALSE;
}
