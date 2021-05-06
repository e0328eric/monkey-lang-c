#ifndef _MONKEY_LANG_SRC_AST_H_
#define _MONKEY_LANG_SRC_AST_H_

typedef enum
{
    EMPTY_NODE = 0,
    NODE_PROGRAM,
    NODE_STATEMENT,
    NODE_EXPRESSION,
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

Program* mkProgram(void);
void freeProgram(Program*);
void pushStmt(Program*, Stmt*);
Stmt* popStmt(Program*);

/////////////////////////////////////////////
//// Implementation of AST structs //////////
/////////////////////////////////////////////

struct Stmt;
struct Expr;

struct Node
{
    NodeType type;
    union
    {
        Program* program;
        Stmt* stmt;
        Expr* expt;
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
};

struct Stmt
{
    StmtType type;
    union
    {
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
