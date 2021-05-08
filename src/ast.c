#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

#define UNDEFINED_OBJECT_FOUND(type)                           \
    fprintf(stderr, "Undefined" type "found. Exit Program\n"); \
    exit(1)

/* Implementig Constructors */
Node* mkNode(void)
{
    Node* output = malloc(sizeof(Node));

    output->type = EMPTY_NODE;
    output->inner.ignore = 0;

    return output;
}

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
    output->inner.ignore = 0;

    return output;
}

Expr* mkExpr(void)
{
    Expr* output = malloc(sizeof(Expr));

    output->type = EMPTY_EXPR;
    output->inner.ignore = 0;

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

IdentExpr* mkIdentExpr(void)
{
    IdentExpr* output = malloc(sizeof(IdentExpr));

    output->value = NULL;

    return output;
}

/* Implementing Destructors */
void freeNode(Node* pNode)
{
    if (!pNode)
        return;

    switch (pNode->type)
    {
    case NODE_PROGRAM:
        freeProgram(pNode->inner.program);
        break;
    case NODE_STMT:
        freeStmt(pNode->inner.stmt);
        break;
    case NODE_EXPR:
        freeExpr(pNode->inner.expr);
        break;
    case EMPTY_NODE:
        break;
    default:
        UNDEFINED_OBJECT_FOUND("Node");
        break;
    }

    free(pNode);
}

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
    case EMPTY_STMT:
        break;
    default:
        UNDEFINED_OBJECT_FOUND("Stmt");
        break;
    }

    free(pStmt);
}

void freeExpr(Expr* pExpr)
{
    if (!pExpr)
        return;

    switch (pExpr->type)
    {
    case EXPR_IDENT:
        freeIdentExpr(pExpr->inner.identExpr);
        break;
    case EMPTY_EXPR:
        break;
    default:
        UNDEFINED_OBJECT_FOUND("Expr");
        break;
    }

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

void freeIdentExpr(IdentExpr* pIdentExpr)
{
    if (!pIdentExpr || !pIdentExpr->value)
        return;

    free((void*)pIdentExpr->value);

    free(pIdentExpr);
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
