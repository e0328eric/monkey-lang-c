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
int testLiteral(Expr*, int64_t);

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
        PRINT_ERR("expr->inner is not %d. got = %d", EXPR_IDENT, expr->type);
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
        PRINT_ERR("expr->inner is not %d. got = %d", EXPR_INTEGER, expr->type);
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
        const char* opt;
        int64_t intValue;
    } prefixTests[] = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
        {"!true;", "!", TRUE},
        {"!false;", "!", FALSE},
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
            PRINT_ERR("expr->inner is not %d. got = %d", EXPR_PREFIX,
                      expr->type);
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }
        prefixExpr = expr->inner.prefixExpr;

        if (cmpStringStr(prefixExpr->opt, tt.opt) != 0)
        {
            PRINT_ERR("prefixExpr->opt is not `%s`. got = `%s`", tt.opt,
                      getStr(prefixExpr->opt));
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }

        if (!testLiteral(prefixExpr->right, tt.intValue))
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

TEST(TestInfixExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Expr* expr = NULL;
    InfixExpr* infixExpr = NULL;
    struct
    {
        const char* input;
        int64_t leftValue;
        const char* opt;
        int64_t rightValue;
    } infixTests[] = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
        {"true == true", TRUE, "==", TRUE},
        {"true != false", TRUE, "!=", FALSE},
        {"false == false", FALSE, "==", FALSE},
        {NULL, 0, NULL, 0},
    };

#define tt infixTests[i]
    for (int i = 0; tt.input; ++i)
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
        if (expr->type != EXPR_INFIX)
        {
            PRINT_ERR("expr->inner is not %d. got = %d", EXPR_INFIX,
                      expr->type);
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }
        infixExpr = expr->inner.infixExpr;

        if (!testLiteral(infixExpr->left, tt.leftValue))
        {
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }

        if (cmpStringStr(infixExpr->opt, tt.opt) != 0)
        {
            PRINT_ERR("infixExpr->opt is not `%s`. got = `%s`", tt.opt,
                      getStr(infixExpr->opt));
            freeStmt(stmt);
            testStatus = TEST_FAILED;
            goto EXIT_IF_FAILED;
        }

        if (!testLiteral(infixExpr->right, tt.rightValue))
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

TEST(TestOperatorPrecedenceParsing)
{
    static int testStatus = TEST_SUCESSED;

    Lexer* l = NULL;
    Parser* p = NULL;
    Program* program = NULL;
    String** errors = NULL;
    String* actual = NULL;

    struct
    {
        const char* input;
        const char* expected;
    } tests[] = {
        {
            "-a * b",
            "((-a) * b)",
        },
        {
            "!-a",
            "(!(-a))",
        },
        {
            "a + b + c",
            "((a + b) + c)",
        },
        {
            "a + b - c",
            "((a + b) - c)",
        },
        {
            "a * b * c",
            "((a * b) * c)",
        },
        {
            "a * b / c",
            "((a * b) / c)",
        },
        {
            "a + b / c",
            "(a + (b / c))",
        },
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {"true", "true"},
        {
            "false",
            "false",
        },
        {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },
        {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        {
            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        {
            "!(true == true)",
            "(!(true == true))",
        },
        {NULL, NULL},
    };

#define tt tests[i]
    for (int i = 0; tt.input; ++i)
    {
        l = mkLexer(tt.input);
        p = mkParser(l);
        program = parseProgram(p);
        errors = getErrors(p);
        if (errors)
        {
            for (int i = 0; errors[i]; ++i)
            {
                PRINT_ERR("%s", getStr(errors[i]));
                freeString(errors[i]);
            }
            free(errors);
            testStatus = TEST_FAILED;
            freeProgram(program);
            freeParser(p);
            break;
        }

        actual = stringifyProgram(program);
        if (cmpStringStr(actual, tt.expected) != 0)
        {
            PRINT_ERR("expected = %s\n got = %s", tt.expected, getStr(actual));
            testStatus = TEST_FAILED;
            freeString(actual);
            freeProgram(program);
            freeParser(p);
            break;
        }

        freeString(actual);
        freeProgram(program);
        freeParser(p);
    }
#undef tt

    return testStatus;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

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

int testLiteral(Expr* expr, int64_t value)
{
    if (!expr)
    {
        PRINT_ERR("expr is NULL", NULL);
        return FALSE;
    }

    switch (expr->type)
    {
    case EXPR_INTEGER:
        if (expr->inner.intExpr->value != (int)value)
        {
            PRINT_ERR("given IntExpr is not %lld. got = %lld", value,
                      expr->inner.intExpr->value);
            return FALSE;
        }
        break;

    case EXPR_BOOL:
        if (expr->inner.boolExpr->value != value)
        {
            PRINT_ERR("given BoolExpr is not `%s`. got = %s",
                      value ? "true" : "false",
                      expr->inner.boolExpr->value ? "true" : "false");
            return FALSE;
        }
        break;

    default:
        PRINT_ERR("expr->type in testLiteral is neither EXPR_INTEGER nor "
                  "EXPR_BOOL. got = %d",
                  expr->type);
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
        RUN_TEST(TestInfixExprs);
        RUN_TEST(TestOperatorPrecedenceParsing);
    })

#undef MAIN_TEST_NAME // End TestParser
