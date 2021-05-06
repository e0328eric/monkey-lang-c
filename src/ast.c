#include <stdlib.h>

#include "ast.h"

Program* mkProgram(void)
{
    Program* pProg = malloc(sizeof(Program));

    pProg->head = malloc(sizeof(struct ProgNode));
    pProg->tail = malloc(sizeof(struct ProgNode));

    pProg->head->before = NULL;
    pProg->head->value = NULL;
    pProg->head->next = pProg->tail;

    pProg->tail->before = pProg->head;
    pProg->tail->value = NULL;
    pProg->tail->next = NULL;

    pProg->len = 0;

    return pProg;
}

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
