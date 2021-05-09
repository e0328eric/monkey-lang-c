#include <stdio.h>
#include <stdlib.h>

#define __PRIVATE_PARSER_FUNCTIONS__
#include "parser.h"

#define FALSE 0
#define TRUE  1

#define EXPECT_PEEK(nodeName, tokType, destructor) \
    if (!expectPeek(p, tokType))                   \
    {                                              \
        destructor(nodeName);                      \
        return NULL;                               \
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
    p->errors = p->errors ? realloc(p->errors,
                                    sizeof(String*) * (size_t)(p->errLen + 1))
                          : malloc(sizeof(String*));
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

    EXPECT_PEEK(letStmt, T_IDENT, freeLetStmt);
    concatString(letStmt->name->value, p->curToken.literal);
    EXPECT_PEEK(letStmt, T_ASSIGN, freeLetStmt);

    // TODO: We're skipping the expressions until we
    // encounter a semicolon
    while (!curTokenIs(p, T_SEMICOLON))
        takeToken(p);

    return letStmt;
}

ReturnStmt* parseReturnStmt(Parser* p) { return NULL; }
ExprStmt* parseExprStmt(Parser* p) { return NULL; }
