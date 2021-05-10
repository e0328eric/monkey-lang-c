#include <stdint.h>

#include "ast.h"
#define MAIN_TEST_NAME TestParser

#include "parser.h"
#include "testing.h"

#define FALSE 0
#define TRUE  1

#define MAKE_PARSER2(input, expectedLen)                               \
    Lexer* l = mkLexer(input);                                         \
    Parser* p = mkParser(l);                                           \
    Program* program = parseProgram(p);                                \
    String** errors = getErrors(p);                                    \
    if (errors)                                                        \
    {                                                                  \
        for (int i = 0; errors[i]; ++i)                                \
        {                                                              \
            PRINT_ERR("%s", getStr(errors[i]));                        \
            freeString(errors[i]);                                     \
        }                                                              \
        free(errors);                                                  \
        testStatus = TEST_FAILED;                                      \
        goto EXIT_IF_FAILED;                                           \
    }                                                                  \
    if (!program)                                                      \
    {                                                                  \
        PRINT_ERR("parseProgram(p) returns NULL", NULL);               \
        testStatus = TEST_FAILED;                                      \
        goto EXIT_IF_FAILED;                                           \
    }                                                                  \
    if (program->len != (size_t)expectedLen)                           \
    {                                                                  \
        PRINT_ERR("program does not contain %d statements. got = %zu", \
                  expectedLen, program->len);                          \
        testStatus = TEST_FAILED;                                      \
        goto EXIT_IF_FAILED;                                           \
    }
#define MAKE_PARSER(expectedLen) MAKE_PARSER2(input, expectedLen)

/* Function Signatures */
int testLetStatement(Stmt*, const char*);
int testIntegerLiteral(Expr*, int64_t);

///////////////////////////////////////////////////////////////////////
/////////    Main Parser Test Case Impls    ///////////////////////////
///////////////////////////////////////////////////////////////////////

TEST(TestLetStmts)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    struct
    {
        const char* expectedIdentifier;
    } tests[] = {
        {"x"},
        {"y"},
        {"foobar"},
    };
    const char* input = "let x = 5;\n"
                        "let y = 10;\n"
                        "let foobar = 838383;";
    MAKE_PARSER(3);

#define tt tests[i]
    for (int i = 0; i < 3; ++i)
    {
        stmt = popStmt(program);
        if (!testLetStatement(stmt, tt.expectedIdentifier))
        {
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }
        freeStmt(stmt);
    }
#undef tt

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

TEST(TestReturnStmts)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    const char* input = "return 5;\n"
                        "return 10;\n"
                        "return 993322;";
    MAKE_PARSER(3);

    for (int i = 0; i < 3; ++i)
    {
        stmt = popStmt(program);
        if (!stmt)
        {
            PRINT_ERR("stmt is NULL", NULL);
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }

        if (stmt->type != STMT_RETURN)
        {
            PRINT_ERR("stmt->type is not %d. got = %d", STMT_RETURN,
                      stmt->type);
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }
        freeStmt(stmt);
    }

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

TEST(TestIdentExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Expr* expr = NULL;
    IdentExpr* identExpr = NULL;
    const char* input = "foobar;";
    MAKE_PARSER(1);

    stmt = popStmt(program);
    expr = stmt->inner.exprStmt->expression;

    if (!expr)
    {
        PRINT_ERR("expr is NULL", NULL);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }
    if (expr->type != EXPR_IDENT)
    {
        PRINT_ERR("expr->inner is not EXPR_IDENT. got = %d", expr->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }
    identExpr = expr->inner.identExpr;

    if (cmpStringStr(identExpr->value, "foobar") != 0)
    {
        PRINT_ERR("identExpr->value is not `%s`. got = `%s`", "foobar",
                  getStr(identExpr->value));
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    freeStmt(stmt);

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

TEST(TestIntegerLiteralExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Expr* expr = NULL;
    IntExpr* intExpr = NULL;
    const char* input = "5;";
    MAKE_PARSER(1);

    stmt = popStmt(program);
    expr = stmt->inner.exprStmt->expression;

    if (!expr)
    {
        PRINT_ERR("expr is NULL", NULL);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }
    if (expr->type != EXPR_INTEGER)
    {
        PRINT_ERR("expr->inner is not EXPR_INTEGER. got = %d", expr->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }
    intExpr = expr->inner.intExpr;

    if (intExpr->value != 5)
    {
        PRINT_ERR("intExpr->value is not %d. got = %lld", 5, intExpr->value);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    freeStmt(stmt);

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

TEST(TestPrefixExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Expr* expr = NULL;
    PrefixExpr* prefixExpr = NULL;
    struct
    {
        const char* input;
        const char* operator;
        int64_t intValue;
    } prefixTests[] = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
    };

#define tt prefixTests[i]
    for (int i = 0; i < 2; ++i)
    {
        MAKE_PARSER2(tt.input, 1);

        stmt = popStmt(program);
        expr = stmt->inner.exprStmt->expression;

        if (!expr)
        {
            PRINT_ERR("expr is NULL", NULL);
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }
        if (expr->type != EXPR_PREFIX)
        {
            PRINT_ERR("expr->inner is not EXPR_PREFIX. got = %d", expr->type);
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }
        prefixExpr = expr->inner.prefixExpr;

        if (cmpStringStr(prefixExpr->operator, tt.operator) != 0)
        {
            PRINT_ERR("prefixExpr->operator is not `%s`. got = `%s`",
                      tt.operator, getStr(prefixExpr->operator));
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }

        if (!testIntegerLiteral(prefixExpr->right, tt.intValue))
        {
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }

        freeStmt(stmt);
        freeProgram(program);
        freeParser(p);
        continue;

    EXIT_IF_FAILED:
        freeProgram(program);
        freeParser(p);
        break;
    }
#undef tt

    return testStatus;
}

#ifndef _MONKEY_PARSER_TEST_H_
#define _MONKEY_PARSER_TEST_H_

int testLetStatement(Stmt* stmt, const char* name)
{
    if (!stmt)
    {
        PRINT_ERR("stmt is NULL", NULL);
        return FALSE;
    }

    if (stmt->type != STMT_LET)
    {
        PRINT_ERR("stmt->type is not %d. got = %d", STMT_LET, stmt->type);
        return FALSE;
    }

    LetStmt* letStmt = stmt->inner.letStmt;
    if (cmpStringStr(letStmt->name->value, name) != 0)
    {
        PRINT_ERR("letStmt->name->value is not `%s`. got `%s`.", name,
                  getStr(letStmt->name->value));
        return FALSE;
    }

    return TRUE;
}

int testIntegerLiteral(Expr* expr, int64_t value)
{
    if (!expr)
    {
        PRINT_ERR("expr is NULL", NULL);
        return FALSE;
    }

    if (expr->type != EXPR_INTEGER)
    {
        PRINT_ERR(
            "expr->type in testIntegerLiteral is not EXPR_INTEGER. got = %d",
            expr->type);
        return FALSE;
    }

    if (expr->inner.intExpr->value != value)
    {
        PRINT_ERR("given IntExpr not %lld. got = %lld", value,
                  expr->inner.intExpr->value);
        return FALSE;
    }

    return TRUE;
}

#endif // _MONKEY_PARSER_TEST_H_

MAIN_TEST(
    {
        RUN_TEST(TestLetStmts);
        RUN_TEST(TestReturnStmts);
        RUN_TEST(TestIdentExprs);
        RUN_TEST(TestIntegerLiteralExprs);
        RUN_TEST(TestPrefixExprs);
    })

#undef MAIN_TEST_NAME // End TestParser
