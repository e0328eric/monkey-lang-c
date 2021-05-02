#ifndef _MONKEY_LANG_SRC_AST_H_
#define _MONKEY_LANG_SRC_AST_H_

typedef enum
{
    NODE_STATEMENT,
    NODE_EXPRESSION,
    NODE_PROGRAM,
} NodeType;

typedef enum
{
    STMT_LET,
    STMT_RETURN,
    STMT_EXPRESSION,
} StmtType;

typedef enum
{
    EXPR_IDENT,
} ExprType;

typedef struct Node Node;
typedef struct Stmt Stmt;
typedef struct Expr Expr;

typedef struct LetStmt LetStmt;
typedef struct ReturnStmt ReturnStmt;
typedef struct ExpressionStmt ExpressionStmt;

typedef struct IdentExpr IdentExpr;

#endif //_MONKEY_LANG_SRC_AST_H_
