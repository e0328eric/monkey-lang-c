#ifndef _MONKEY_LANG_SRC_AST_H_
#define _MONKEY_LANG_SRC_AST_H_

#include <ctype.h>

typedef enum
{
    EMPTY_NODE = 0,
    NODE_PROGRAM,
    NODE_STMT,
    NODE_EXPR,
} NodeType;

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
} ExprType;

typedef struct Node Node;
typedef struct Program Program;
typedef struct Stmt Stmt;
typedef struct Expr Expr;

typedef struct LetStmt LetStmt;
typedef struct ReturnStmt ReturnStmt;
typedef struct ExprStmt ExprStmt;

typedef struct IdentExpr IdentExpr;

Node* mkNode(void);
Program* mkProgram(void);
Stmt* mkStmt(void);
Expr* mkExpr(void);
LetStmt* mkLetStmt(void);
ReturnStmt* mkReturnStmt(void);
ExprStmt* mkExprStmt(void);
IdentExpr* mkIdentExpr(void);

void freeNode(Node*);
void freeProgram(Program*);
void freeStmt(Stmt*);
void freeExpr(Expr*);
void freeLetStmt(LetStmt*);
void freeReturnStmt(ReturnStmt*);
void freeExprStmt(ExprStmt*);
void freeIdentExpr(IdentExpr*);

void pushStmt(Program*, Stmt**);
Stmt* popStmt(Program*);

/* Implementation of AST structs */

struct Stmt;
struct Expr;

struct Node
{
    NodeType type;
    union
    {
        int ignore; // To make it zero initizlize
        Program* program;
        Stmt* stmt;
        Expr* expr;
    } inner;
};

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
        int ignore; // To make it zero initizlize
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
        int ignore; // To make it zero initizlize
        IdentExpr* identExpr;
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
    const char* value;
};

#endif //_MONKEY_LANG_SRC_AST_H_
