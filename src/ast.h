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
    STMT_BLOCK,
} StmtType;

typedef enum
{
    EMPTY_EXPR = 0,
    EXPR_IDENT,
    EXPR_INTEGER,
    EXPR_BOOL,
    EXPR_PREFIX,
    EXPR_INFIX,
    EXPR_IF,
    EXPR_FUNCTION,
    EXPR_CALL,
} ExprType;

typedef struct Program Program;
typedef struct Stmt Stmt;
typedef struct Expr Expr;

typedef struct LetStmt LetStmt;
typedef struct ReturnStmt ReturnStmt;
typedef struct ExprStmt ExprStmt;
typedef struct Program BlockStmt;

typedef struct IdentExpr IdentExpr;
typedef struct IntExpr IntExpr;
typedef struct BoolExpr BoolExpr;
typedef struct PrefixExpr PrefixExpr;
typedef struct InfixExpr InfixExpr;
typedef struct IfExpr IfExpr;
typedef struct Parameters Parameters;
typedef struct Arguments Arguments;
typedef struct FntExpr FntExpr;
typedef struct CallExpr CallExpr;

Program* mkProgram(void);
Stmt* mkStmt(void);
Expr* mkExpr(void);
LetStmt* mkLetStmt(void);
ReturnStmt* mkReturnStmt(void);
ExprStmt* mkExprStmt(void);
BlockStmt* mkBlockStmt(void);
IdentExpr* mkIdentExpr(void);
IntExpr* mkIntExpr(void);
BoolExpr* mkBoolExpr(void);
PrefixExpr* mkPrefixExpr(void);
InfixExpr* mkInfixExpr(void);
IfExpr* mkIfExpr(void);
FntExpr* mkFntExpr(void);
CallExpr* mkCallExpr(void);

void freeProgram(Program*);
void freeStmt(Stmt*);
void freeExpr(Expr*);
void freeExprWithoutSelf(Expr*);
void freeLetStmt(LetStmt*);
void freeReturnStmt(ReturnStmt*);
void freeExprStmt(ExprStmt*);
void freeBlockStmt(BlockStmt*);
void freeIdentExpr(IdentExpr*);
void freeIntExpr(IntExpr*);
void freeBoolExpr(BoolExpr*);
void freePrefixExpr(PrefixExpr*);
void freeInfixExpr(InfixExpr*);
void freeIfExpr(IfExpr*);
void freeFntExpr(FntExpr*);
void freeCallExpr(CallExpr*);
void freeParameters(Parameters*);
void freeArguments(Arguments*);

String* stringifyProgram(Program*);
String* stringifyStmt(Stmt*);
String* stringifyExpr(Expr*);
String* stringifyLetStmt(LetStmt*);
String* stringifyReturnStmt(ReturnStmt*);
String* stringifyExprStmt(ExprStmt*);
String* stringifyBlockStmt(BlockStmt*);
String* stringifyIdentExpr(IdentExpr*);
String* stringifyIntExpr(IntExpr*);
String* stringifyBoolExpr(BoolExpr*);
String* stringifyPrefixExpr(PrefixExpr*);
String* stringifyInfixExpr(InfixExpr*);
String* stringifyIfExpr(IfExpr*);
String* stringifyFntExpr(FntExpr*);
String* stringifyCallExpr(CallExpr*);

void pushStmt(Program*, Stmt**);
Stmt* popStmt(Program*);
void pushParam(Parameters*, IdentExpr**);
void pushArgs(Arguments*, Expr**);

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
        BlockStmt* blockStmt;
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
        BoolExpr* boolExpr;
        PrefixExpr* prefixExpr;
        InfixExpr* infixExpr;
        IfExpr* ifExpr;
        FntExpr* fntExpr;
        CallExpr* callExpr;
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

struct BoolExpr
{
    int value;
};

struct PrefixExpr
{
    String* opt;
    Expr* right;
};

struct InfixExpr
{
    Expr* left;
    String* opt;
    Expr* right;
};

struct IfExpr
{
    Expr* condition;
    BlockStmt* consequence;
    BlockStmt* alternative;
};

struct ParamNode
{
    struct ParamNode* before;
    IdentExpr* value;
    struct ParamNode* next;
};

struct Parameters
{
    struct ParamNode* head;
    struct ParamNode* tail;
    size_t len;
};

struct FntExpr
{
    Parameters* parameters;
    BlockStmt* body;
};

struct ArgNode
{
    struct ArgNode* before;
    Expr* value;
    struct ArgNode* next;
};

struct Arguments
{
    struct ArgNode* head;
    struct ArgNode* tail;
    size_t len;
};

struct CallExpr
{
    Expr* function;
    Arguments* arguments;
};

#endif //_MONKEY_LANG_SRC_AST_H_
