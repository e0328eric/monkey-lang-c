#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "dynString.h"

#define UNDEFINED_OBJECT_FOUND(type)                           \
    fprintf(stderr, "Undefined" type "found. Exit Program\n"); \
    exit(1)

/* Implementig Constructors */
Program* mkProgram(void)
{
    Program* output = malloc(sizeof(Program));

    output->head = malloc(sizeof(struct ProgNode));
    output->tail = malloc(sizeof(struct ProgNode));

    output->head->before = NULL;
    output->head->value = NULL;
    output->head->next = output->tail;

    output->tail->before = output->head;
    output->tail->value = NULL;
    output->tail->next = NULL;

    output->len = 0;

    return output;
}

Stmt* mkStmt(void)
{
    Stmt* output = malloc(sizeof(Stmt));

    output->type = EMPTY_STMT;
    output->inner.checkIsNull = 0;

    return output;
}

Expr* mkExpr(void)
{
    Expr* output = malloc(sizeof(Expr));

    output->type = EMPTY_EXPR;
    output->inner.checkIsNull = 0;

    return output;
}

LetStmt* mkLetStmt(void)
{
    LetStmt* output = malloc(sizeof(LetStmt));

    output->name = mkIdentExpr();
    output->value = mkExpr();

    return output;
}

ReturnStmt* mkReturnStmt(void)
{
    ReturnStmt* output = malloc(sizeof(ReturnStmt));

    output->returnValue = mkExpr();

    return output;
}

ExprStmt* mkExprStmt(void)
{
    ExprStmt* output = malloc(sizeof(ExprStmt));

    output->expression = mkExpr();

    return output;
}

BlockStmt* mkBlockStmt(void) { return (BlockStmt*)mkProgram(); }

IdentExpr* mkIdentExpr(void)
{
    IdentExpr* output = malloc(sizeof(IdentExpr));

    output->value = NULL;

    return output;
}

IntExpr* mkIntExpr(void)
{
    IntExpr* output = malloc(sizeof(IntExpr));

    output->value = 0;

    return output;
}

BoolExpr* mkBoolExpr(void)
{
    BoolExpr* output = malloc(sizeof(BoolExpr));

    output->value = 0;

    return output;
}

PrefixExpr* mkPrefixExpr(void)
{
    PrefixExpr* output = malloc(sizeof(PrefixExpr));

    output->opt = NULL;
    output->right = mkExpr();

    return output;
}

InfixExpr* mkInfixExpr(void)
{
    InfixExpr* output = malloc(sizeof(InfixExpr));

    output->left = NULL;
    output->opt = NULL;
    output->right = mkExpr();

    return output;
}

IfExpr* mkIfExpr(void)
{
    IfExpr* output = malloc(sizeof(IfExpr));

    output->condition = mkExpr();
    output->consequence = mkBlockStmt();
    output->alternative = NULL;

    return output;
}

/* Implementing Destructors */
void freeProgram(Program* pProg)
{
    if (!pProg)
        return;

    Stmt* tmp = NULL;
    while (pProg->len > 0)
    {
        tmp = popStmt(pProg);
        freeStmt(tmp);
    }

    free(pProg->head);
    free(pProg->tail);
    free(pProg);
}

void freeStmt(Stmt* pStmt)
{
    if (!pStmt)
        return;

    switch (pStmt->type)
    {
    case STMT_LET:
        freeLetStmt(pStmt->inner.letStmt);
        break;

    case STMT_RETURN:
        freeReturnStmt(pStmt->inner.returnStmt);
        break;

    case STMT_EXPRESSION:
        freeExprStmt(pStmt->inner.exprStmt);
        break;

    case STMT_BLOCK:
        freeBlockStmt(pStmt->inner.blockStmt);
        break;

    case EMPTY_STMT:
        break;

    default:
        UNDEFINED_OBJECT_FOUND("Stmt");
        break;
    }

    free(pStmt);
}

void freeExprWithoutSelf(Expr* pExpr)
{
    if (!pExpr)
        return;

    switch (pExpr->type)
    {
    case EXPR_IDENT:
        freeIdentExpr(pExpr->inner.identExpr);
        break;

    case EXPR_INTEGER:
        freeIntExpr(pExpr->inner.intExpr);
        break;

    case EXPR_BOOL:
        freeBoolExpr(pExpr->inner.boolExpr);
        break;

    case EXPR_PREFIX:
        freePrefixExpr(pExpr->inner.prefixExpr);
        break;

    case EXPR_INFIX:
        freeInfixExpr(pExpr->inner.infixExpr);
        break;

    case EXPR_IF:
        freeIfExpr(pExpr->inner.ifExpr);

    case EMPTY_EXPR:
        break;

    default:
        UNDEFINED_OBJECT_FOUND("Expr");
        break;
    }
}

void freeExpr(Expr* pExpr)
{
    if (!pExpr)
        return;

    freeExprWithoutSelf(pExpr);
    free(pExpr);
}

void freeLetStmt(LetStmt* pLetStmt)
{
    if (!pLetStmt)
        return;

    freeIdentExpr(pLetStmt->name);
    freeExpr(pLetStmt->value);

    free(pLetStmt);
}

void freeReturnStmt(ReturnStmt* pReturnStmt)
{
    if (!pReturnStmt)
        return;

    freeExpr(pReturnStmt->returnValue);

    free(pReturnStmt);
}

void freeExprStmt(ExprStmt* pExprStmt)
{
    if (!pExprStmt)
        return;

    freeExpr(pExprStmt->expression);

    free(pExprStmt);
}

void freeBlockStmt(BlockStmt* pBlockStmt) { freeProgram((Program*)pBlockStmt); }

void freeIdentExpr(IdentExpr* pIdentExpr)
{
    if (!pIdentExpr)
        return;

    freeString(pIdentExpr->value);

    free(pIdentExpr);
}

void freeIntExpr(IntExpr* pIntExpr)
{
    if (!pIntExpr)
        return;

    free(pIntExpr);
}

void freeBoolExpr(BoolExpr* pBoolExpr)
{
    if (!pBoolExpr)
        return;

    free(pBoolExpr);
}

void freePrefixExpr(PrefixExpr* pPrefixExpr)
{
    if (!pPrefixExpr)
        return;

    freeString(pPrefixExpr->opt);
    freeExpr(pPrefixExpr->right);
    free(pPrefixExpr);
}

void freeInfixExpr(InfixExpr* pInfixExpr)
{
    if (!pInfixExpr)
        return;

    freeExpr(pInfixExpr->left);
    freeString(pInfixExpr->opt);
    freeExpr(pInfixExpr->right);
    free(pInfixExpr);
}

void freeIfExpr(IfExpr* pIfExpr)
{
    if (!pIfExpr)
        return;

    freeExpr(pIfExpr->condition);
    freeBlockStmt(pIfExpr->consequence);
    freeBlockStmt(pIfExpr->alternative);
    free(pIfExpr);
}

/* Implementing Stringify */
String* stringifyProgram(Program* pProg)
{
    if (!pProg)
        return NULL;

    String* output = mkString("");
    struct ProgNode* tmp = pProg->tail->before;
    while (tmp != pProg->head)
    {
        concatFreeString(output, stringifyStmt(tmp->value));
        tmp = tmp->before;
    }

    return output;
}

String* stringifyStmt(Stmt* pStmt)
{
    if (!pStmt)
        return NULL;

    String* output = NULL;

    switch (pStmt->type)
    {
    case STMT_LET:
        output = stringifyLetStmt(pStmt->inner.letStmt);
        break;

    case STMT_RETURN:
        output = stringifyReturnStmt(pStmt->inner.returnStmt);
        break;

    case STMT_EXPRESSION:
        output = stringifyExprStmt(pStmt->inner.exprStmt);
        break;

    case STMT_BLOCK:
        output = stringifyBlockStmt(pStmt->inner.blockStmt);
        break;

    default:
        break;
    }

    return output;
}

String* stringifyExpr(Expr* pExpr)
{
    if (!pExpr)
        return NULL;

    String* output = NULL;

    switch (pExpr->type)
    {
    case EXPR_IDENT:
        output = stringifyIdentExpr(pExpr->inner.identExpr);
        break;

    case EXPR_INTEGER:
        output = stringifyIntExpr(pExpr->inner.intExpr);
        break;

    case EXPR_BOOL:
        output = stringifyBoolExpr(pExpr->inner.boolExpr);
        break;

    case EXPR_PREFIX:
        output = stringifyPrefixExpr(pExpr->inner.prefixExpr);
        break;

    case EXPR_INFIX:
        output = stringifyInfixExpr(pExpr->inner.infixExpr);
        break;

    case EXPR_IF:
        output = stringifyIfExpr(pExpr->inner.ifExpr);

    default:
        break;
    }

    return output;
}

String* stringifyLetStmt(LetStmt* pLetStmt)
{
    if (!pLetStmt)
        return NULL;

    String* output = mkString("let ");
    concatFreeString(output, stringifyIdentExpr(pLetStmt->name));
    appendStr(output, " = ");
    if (pLetStmt->value)
        concatFreeString(output, stringifyExpr(pLetStmt->value));
    appendStr(output, ";");

    return output;
}

String* stringifyReturnStmt(ReturnStmt* pReturnStmt)
{
    if (!pReturnStmt)
        return NULL;

    String* output = mkString("return ");
    if (!pReturnStmt->returnValue)
        concatFreeString(output, stringifyExpr(pReturnStmt->returnValue));
    appendStr(output, ";");

    return output;
}

String* stringifyExprStmt(ExprStmt* pExprStmt)
{
    if (!pExprStmt || !pExprStmt->expression)
        return NULL;

    return stringifyExpr(pExprStmt->expression);
}

String* stringifyBlockStmt(BlockStmt* pBlockStmt)
{
    return stringifyProgram((Program*)pBlockStmt);
}

String* stringifyIdentExpr(IdentExpr* pIdentExpr)
{
    if (!pIdentExpr)
        return NULL;

    return mkString(getStr(pIdentExpr->value));
}

String* stringifyIntExpr(IntExpr* pIntExpr)
{
    if (!pIntExpr)
        return NULL;

    char buffer[22];
    sprintf(buffer, "%lld", pIntExpr->value);

    return mkString(buffer);
}

String* stringifyBoolExpr(BoolExpr* pBoolExpr)
{
    if (!pBoolExpr)
        return NULL;

    return mkString(pBoolExpr->value != 0 ? "true" : "false");
}

String* stringifyPrefixExpr(PrefixExpr* pPrefixExpr)
{
    if (!pPrefixExpr)
        return NULL;

    String* output = mkString("(");
    concatString(output, pPrefixExpr->opt);
    concatFreeString(output, stringifyExpr(pPrefixExpr->right));
    appendChar(output, ')');

    return output;
}

String* stringifyInfixExpr(InfixExpr* pInfixExpr)
{
    if (!pInfixExpr)
        return NULL;

    String* output = mkString("(");
    concatFreeString(output, stringifyExpr(pInfixExpr->left));
    appendChar(output, ' ');
    concatString(output, pInfixExpr->opt);
    appendChar(output, ' ');
    concatFreeString(output, stringifyExpr(pInfixExpr->right));
    appendChar(output, ')');

    return output;
}

String* stringifyIfExpr(IfExpr* pIfExpr)
{
    if (!pIfExpr)
        return NULL;

    String* output = mkString("if ");
    concatFreeString(output, stringifyExpr(pIfExpr->condition));
    appendChar(output, ' ');
    concatFreeString(output, stringifyBlockStmt(pIfExpr->consequence));

    if (pIfExpr->alternative)
    {
        appendStr(output, "else ");
        concatFreeString(output, stringifyBlockStmt(pIfExpr->alternative));
    }

    return output;
}

/* Implementing push and pop */
void pushStmt(Program* pProg, Stmt** pStmt)
{
    if (!pProg || !pStmt)
        return;

    struct ProgNode* tmp = malloc(sizeof(struct ProgNode));

    tmp->value = *pStmt;
    *pStmt = NULL;

    tmp->before = pProg->head;
    tmp->next = pProg->head->next;
    pProg->head->next = tmp;
    tmp->next->before = tmp;

    ++pProg->len;
}

Stmt* popStmt(Program* pProg)
{
    if (!pProg || pProg->len == 0)
        return NULL;

    struct ProgNode* tmp = pProg->tail->before;
    if (tmp == pProg->head)
        return NULL;

    Stmt* output = tmp->value;
    --pProg->len;

    pProg->tail->before = tmp->before;
    pProg->tail->before->next = pProg->tail;

    free(tmp);

    return output;
}
