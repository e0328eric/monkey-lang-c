#ifndef _MONKEY_LANG_SRC_AST_H_
#define _MONKEY_LANG_SRC_AST_H_

#include <ctype.h>
#include <stdint.h>

#include "dynString.h"

typedef enum
{
    EMPTY_STMT = 0,
    STMT_LET,
    STMT_RETURN,
    STMT_EXPRESSION,
} StmtType;

typedef enum
{
    EMPTY_EXPR = 0,
    EXPR_IDENT,
    EXPR_INTEGER,
    EXPR_PREFIX,
} ExprType;

typedef struct Program Program;
typedef struct Stmt Stmt;
typedef struct Expr Expr;

typedef struct LetStmt LetStmt;
typedef struct ReturnStmt ReturnStmt;
typedef struct ExprStmt ExprStmt;

typedef struct IdentExpr IdentExpr;
typedef struct IntExpr IntExpr;
typedef struct PrefixExpr PrefixExpr;

Program* mkProgram(void);
Stmt* mkStmt(void);
Expr* mkExpr(void);
LetStmt* mkLetStmt(void);
ReturnStmt* mkReturnStmt(void);
ExprStmt* mkExprStmt(void);
IdentExpr* mkIdentExpr(void);
IntExpr* mkIntExpr(void);
PrefixExpr* mkPrefixExpr(void);

void freeProgram(Program*);
void freeStmt(Stmt*);
void freeExpr(Expr*);
void freeLetStmt(LetStmt*);
void freeReturnStmt(ReturnStmt*);
void freeExprStmt(ExprStmt*);
void freeIdentExpr(IdentExpr*);
void freeIntExpr(IntExpr*);
void freePrefixExpr(PrefixExpr*);

String* stringifyProgram(Program*);
String* stringifyStmt(Stmt*);
String* stringifyExpr(Expr*);
String* stringifyLetStmt(LetStmt*);
String* stringifyReturnStmt(ReturnStmt*);
String* stringifyExprStmt(ExprStmt*);
String* stringifyIdentExpr(IdentExpr*);
String* stringifyIntExpr(IntExpr*);
String* stringifyPrefixExpr(PrefixExpr*);

void pushStmt(Program*, Stmt**);
Stmt* popStmt(Program*);

/* Implementation of AST structs */

struct Stmt;
struct Expr;

struct ProgNode
{
    struct ProgNode* before;
    Stmt* value;
    struct ProgNode* next;
};

struct Program
{
    struct ProgNode* head;
    struct ProgNode* tail;
    size_t len;
};

struct Stmt
{
    StmtType type;
    union
    {
        uintptr_t checkIsNull; // To make it zero initizlize
        LetStmt* letStmt;
        ReturnStmt* returnStmt;
        ExprStmt* exprStmt;
    } inner;
};

struct Expr
{
    ExprType type;
    union
    {
        uintptr_t checkIsNull; // To make it zero initizlize
        IdentExpr* identExpr;
        IntExpr* intExpr;
        PrefixExpr* prefixExpr;
    } inner;
};

/* Statements */
struct LetStmt
{
    IdentExpr* name;
    Expr* value;
};

struct ReturnStmt
{
    Expr* returnValue;
};

struct ExprStmt
{
    Expr* expression;
};

/* Expressions */
struct IdentExpr
{
    String* value;
};

struct IntExpr
{
    int64_t value;
};

struct PrefixExpr
{
    String* operator;
    Expr* right;
};

#endif //_MONKEY_LANG_SRC_AST_H_
