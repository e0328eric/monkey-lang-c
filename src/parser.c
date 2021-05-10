#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

#define __PRIVATE_PARSER_OBJECTS__
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
        stmt->inner.checkIsNull = 0;               \
        return;                                    \
    }

const PrefixParseFn prefixParseFns[] = {
    NULL,            // T_EOF = 0
    parseIdentExpr,  // T_IDENT
    parseIntExpr,    // T_INT
    NULL,            // T_ASSIGN
    NULL,            // T_PLUS
    parsePrefixExpr, // T_MINUS
    parsePrefixExpr, // T_BANG
    NULL,            // T_ASTERISK
    NULL,            // T_LT
    NULL,            // T_GT
    NULL,            // T_EQ
    NULL,            // T_NOT_EQ
    NULL,            // T_SLASH
    NULL,            // T_COMMA
    NULL,            // T_SEMICOLON
    NULL,            // T_LPAREN
    NULL,            // T_RPAREN
    NULL,            // T_LBRACE
    NULL,            // T_RBRACE
    NULL,            // T_LET
    NULL,            // T_FUNCTION
    NULL,            // T_IF
    NULL,            // T_ELSE
    NULL,            // T_TRUE
    NULL,            // T_FALSE
    NULL,            // T_RETURN
    NULL,            // T_ZERO
    NULL,            // T_ILLEGAL
};

const InfixParseFn infixParseFns[] = {
    NULL,           // T_EOF = 0
    NULL,           // T_IDENT
    NULL,           // T_INT
    NULL,           // T_ASSIGN
    parseInfixExpr, // T_PLUS
    parseInfixExpr, // T_MINUS
    NULL,           // T_BANG
    parseInfixExpr, // T_ASTERISK
    parseInfixExpr, // T_LT
    parseInfixExpr, // T_GT
    parseInfixExpr, // T_EQ
    parseInfixExpr, // T_NOT_EQ
    parseInfixExpr, // T_SLASH
    NULL,           // T_COMMA
    NULL,           // T_SEMICOLON
    NULL,           // T_LPAREN
    NULL,           // T_RPAREN
    NULL,           // T_LBRACE
    NULL,           // T_RBRACE
    NULL,           // T_LET
    NULL,           // T_FUNCTION
    NULL,           // T_IF
    NULL,           // T_ELSE
    NULL,           // T_TRUE
    NULL,           // T_FALSE
    NULL,           // T_RETURN
    NULL,           // T_ZERO
    NULL,           // T_ILLEGAL
};

inline Precedence takePrec(TokenType tokType)
{
    switch (tokType)
    {
    case T_EQ:
    case T_NOT_EQ:
        return EQ_PREC;

    case T_LT:
    case T_GT:
        return CMP_PREC;

    case T_PLUS:
    case T_MINUS:
        return SUM_PREC;

    case T_SLASH:
    case T_ASTERISK:
        return PRODUCT_PREC;

    default:
        return LOWEST_PREC;
    }
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
    output->curToken = INIT_TOKEN;
    output->peekToken = INIT_TOKEN;
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

// Main Part of the Parser of Monkey language
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

void parseExprStmt(Parser* p, Stmt* stmt)
{
    ASSERT(stmt);

    ExprStmt* exprStmt = mkExprStmt();
    parseExpr(p, &exprStmt->expression, LOWEST_PREC);

    if (peekTokenIs(p, T_SEMICOLON))
        takeToken(p);

    stmt->inner.exprStmt = exprStmt;
}

void parseExpr(Parser* p, Expr** pExpr, Precedence prec)
{
    ASSERT(pExpr && *pExpr);

    Expr* leftExpr = NULL;

    PrefixParseFn prefix = prefixParseFns[p->curToken.type];
    if (!prefix)
    {
        noPrefixParseFnError(p, p->curToken.type);
        (*pExpr)->inner.checkIsNull = 0;
        return;
    }
    prefix(p, *pExpr);

    while (!peekTokenIs(p, T_SEMICOLON) && prec < peekPrecedence(p))
    {
        InfixParseFn infix = infixParseFns[p->peekToken.type];
        if (!infix)
            return;

        takeToken(p);

        leftExpr = *pExpr;
        *pExpr = mkExpr();
        infix(p, *pExpr, leftExpr);
    }
}

void parseIdentExpr(Parser* p, Expr* expr)
{
    ASSERT(expr);

    expr->type = EXPR_IDENT;
    expr->inner.identExpr = mkIdentExpr();
    expr->inner.identExpr->value = mkString(getStr(p->curToken.literal));
}

void parseIntExpr(Parser* p, Expr* expr)
{
    ASSERT(expr);

    expr->type = EXPR_INTEGER;
    expr->inner.intExpr = mkIntExpr();
    expr->inner.intExpr->value = atoll(getStr(p->curToken.literal));
}

void parsePrefixExpr(Parser* p, Expr* expr)
{
    ASSERT(expr);

    expr->type = EXPR_PREFIX;
    expr->inner.prefixExpr = mkPrefixExpr();
    expr->inner.prefixExpr->operator= mkString(getStr(p->curToken.literal));

    takeToken(p);

    parseExpr(p, &expr->inner.prefixExpr->right, PREFIX_PREC);
}

void parseInfixExpr(Parser* p, Expr* output, Expr* left)
{
    ASSERT(output && left);

    output->type = EXPR_INFIX;
    output->inner.infixExpr = mkInfixExpr();
    output->inner.infixExpr->left = left;
    output->inner.infixExpr->operator= mkString(getStr(p->curToken.literal));

    Precedence prec = curPrecedence(p);
    takeToken(p);
    parseExpr(p, &output->inner.infixExpr->right, prec);
}

////////////////////////////////////////// // END OF MAIN PARSER IMPLEMENTATION

// Helper functions to implementing the parser such as emitting error messages
// and peeking tokens from the lexer etc.
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

void noPrefixParseFnError(Parser* p, TokenType tokType)
{
    char msg[60];

    if (p->errLen >= MAXIMUM_ERR_MSGS)
    {
        fprintf(stderr, "too many parse error occurs.");
        return;
    }

    sprintf(msg, "no prefix parser function for %s found",
            printTokType(tokType));

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

Precedence curPrecedence(Parser* p) { return takePrec(p->curToken.type); }

Precedence peekPrecedence(Parser* p) { return takePrec(p->peekToken.type); }
