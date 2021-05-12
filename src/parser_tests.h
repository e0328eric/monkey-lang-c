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
int testLetStatement(Stmt*, const char*, int64_t);
int testIdentExpr(Expr*, const char*);
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
        int64_t expectedValue;
    } tests[] = {
        {"x", 5},
        {"y", 10},
        {"foobar", 838383},
    };
    const char* input = "let x = 5;\n"
                        "let y = 10;\n"
                        "let foobar = 838383;";
    MAKE_PARSER(3);

#define tt tests[i]
    for (int i = 0; i < 3; ++i)
    {
        stmt = popStmt(program);
        if (!testLetStatement(stmt, tt.expectedIdentifier, tt.expectedValue))
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
    struct
    {
        int64_t expectedValue;
    } tests[] = {{5}, {10}, {993322}};
    const char* input = "return 5;\n"
                        "return 10;\n"
                        "return 993322;";
    MAKE_PARSER(3);

#define tt tests[i]
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

        if (!testLiteral(stmt->inner.returnStmt->returnValue, tt.expectedValue))
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
        {
            "a + add(b * c) + d",
            "((a + add((b * c))) + d)",
        },
        {
            "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
            "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
        },
        {
            "add(a + b + c * d / f + g)",
            "add((((a + b) + ((c * d) / f)) + g))",
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

TEST(TestIfExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Stmt* consequenceStmt = NULL;
    Expr* expr = NULL;
    Expr* consequence = NULL;
    IfExpr* ifExpr = NULL;
    String* gotString = NULL;

    const char* input = "if (x < y) { x };";
    MAKE_PARSER(1);

    stmt = popStmt(program);

    if (stmt->type != STMT_EXPRESSION)
    {
        PRINT_ERR("stmt->type is not %d. got = %d", STMT_EXPRESSION,
                  stmt->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    expr = stmt->inner.exprStmt->expression;
    if (expr->type != EXPR_IF)
    {
        PRINT_ERR("expr->type is not %d. got = %d", EXPR_IF, expr->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    ifExpr = expr->inner.ifExpr;
    gotString = stringifyExpr(ifExpr->condition);
    if (cmpStringStr(gotString, "(x < y)"))
    {
        PRINT_ERR("ifExpr->condition was expected `(x < y)`, instead of `%s`",
                  getStr(gotString));
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    if (ifExpr->consequence->len != 1)
    {
        PRINT_ERR("consequence is not 1 statement. got = %zu",
                  ifExpr->consequence->len);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    consequenceStmt = popStmt((Program*)ifExpr->consequence);
    if (consequenceStmt->type != STMT_EXPRESSION)
    {
        PRINT_ERR("consequenceStmt>type is not %d. got = %d", STMT_EXPRESSION,
                  stmt->type);
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    consequence = consequenceStmt->inner.exprStmt->expression;
    if (!testIdentExpr(consequence, "x"))
    {
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    if (ifExpr->alternative)
    {
        PRINT_ERR("if->alternative is not NULL.", NULL);
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    freeStmt(consequenceStmt);
    freeString(gotString);
    freeStmt(stmt);

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

TEST(TestIfElseExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Stmt* consequenceStmt = NULL;
    Stmt* alternativeStmt = NULL;
    Expr* expr = NULL;
    Expr* consequence = NULL;
    Expr* alternative = NULL;
    IfExpr* ifExpr = NULL;
    String* gotString = NULL;

    const char* input = "if (x < y) { x } else { y };";
    MAKE_PARSER(1);

    stmt = popStmt(program);

    if (stmt->type != STMT_EXPRESSION)
    {
        PRINT_ERR("stmt->type is not %d. got = %d", STMT_EXPRESSION,
                  stmt->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    expr = stmt->inner.exprStmt->expression;
    if (expr->type != EXPR_IF)
    {
        PRINT_ERR("expr->type is not %d. got = %d", EXPR_IF, expr->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    ifExpr = expr->inner.ifExpr;
    gotString = stringifyExpr(ifExpr->condition);
    if (cmpStringStr(gotString, "(x < y)"))
    {
        PRINT_ERR("ifExpr->condition was expected `(x < y)`, instead of `%s`",
                  getStr(gotString));
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    if (ifExpr->consequence->len != 1)
    {
        PRINT_ERR("consequence is not 1 statement. got = %zu",
                  ifExpr->consequence->len);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    consequenceStmt = popStmt((Program*)ifExpr->consequence);
    if (consequenceStmt->type != STMT_EXPRESSION)
    {
        PRINT_ERR("consequenceStmt>type is not %d. got = %d", STMT_EXPRESSION,
                  stmt->type);
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    consequence = consequenceStmt->inner.exprStmt->expression;
    if (!testIdentExpr(consequence, "x"))
    {
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    if (ifExpr->alternative->len != 1)
    {
        PRINT_ERR("alternative is not 1 statement. got = %zu",
                  ifExpr->alternative->len);
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    alternativeStmt = popStmt((Program*)ifExpr->alternative);
    if (alternativeStmt->type != STMT_EXPRESSION)
    {
        PRINT_ERR("alternativeStmt>type is not %d. got = %d", STMT_EXPRESSION,
                  stmt->type);
        freeStmt(alternativeStmt);
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    alternative = alternativeStmt->inner.exprStmt->expression;
    if (!testIdentExpr(alternative, "y"))
    {
        freeStmt(alternativeStmt);
        freeStmt(consequenceStmt);
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    freeStmt(alternativeStmt);
    freeStmt(consequenceStmt);
    freeString(gotString);
    freeStmt(stmt);

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

TEST(TestFunctionLiteralExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Expr* expr = NULL;
    FntExpr* fntExpr = NULL;
    struct ParamNode* tmp = NULL;
    String* gotString = NULL;

    const char* input = "fn(x, y) { x + y; }";
    MAKE_PARSER(1);

    stmt = popStmt(program);

    if (stmt->type != STMT_EXPRESSION)
    {
        PRINT_ERR("stmt->type is not %d. got = %d", STMT_EXPRESSION,
                  stmt->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    expr = stmt->inner.exprStmt->expression;
    if (expr->type != EXPR_FUNCTION)
    {
        PRINT_ERR("expr->type is not %d. got = %d", EXPR_FUNCTION, expr->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    fntExpr = expr->inner.fntExpr;
    if (fntExpr->parameters->len != 2)
    {
        PRINT_ERR("The length of fntExpr->parameters is not %d. got = %zu", 2,
                  fntExpr->parameters->len);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    tmp = fntExpr->parameters->tail->before;
    if (cmpStringStr(tmp->value->value, "x") != 0)
    {
        PRINT_ERR("Expected `%s`, got `%s`", "x",
                  getStr(stringifyIdentExpr(tmp->value)));
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }
    tmp = tmp->before;
    if (cmpStringStr(tmp->value->value, "y") != 0)
    {
        PRINT_ERR("Expected `%s`, got `%s`", "y",
                  getStr(stringifyIdentExpr(tmp->value)));
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    if (fntExpr->body->len != 1)
    {
        PRINT_ERR("The length of fntExpr->body is not %d. got = %zu", 2,
                  fntExpr->body->len);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    gotString = stringifyBlockStmt(fntExpr->body);
    if (cmpStringStr(gotString, "(x + y)") != 0)
    {
        PRINT_ERR("Expected `%s`, got = `%s`", "(x + y)", getStr(gotString));
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    freeString(gotString);
    freeStmt(stmt);

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

TEST(TestCallExprs)
{
    static int testStatus = TEST_SUCESSED;

    Stmt* stmt = NULL;
    Expr* expr = NULL;
    CallExpr* callExpr = NULL;
    struct ArgNode* tmp = NULL;
    String* gotString = NULL;

    const char* input = "add(1, 2 * 3, 4 + 5);";
    MAKE_PARSER(1);

    stmt = popStmt(program);

    if (stmt->type != STMT_EXPRESSION)
    {
        PRINT_ERR("stmt->type is not %d. got = %d", STMT_EXPRESSION,
                  stmt->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    expr = stmt->inner.exprStmt->expression;
    if (expr->type != EXPR_CALL)
    {
        PRINT_ERR("expr->type is not %d. got = %d", EXPR_CALL, expr->type);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    callExpr = expr->inner.callExpr;
    if (callExpr->arguments->len != 3)
    {
        PRINT_ERR("The length of callExpr->arguments is not %d. got = %zu", 3,
                  callExpr->arguments->len);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    if (!testIdentExpr(callExpr->function, "add"))
    {
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    tmp = callExpr->arguments->tail->before;
    gotString = stringifyExpr(tmp->value);
    if (cmpStringStr(gotString, "1") != 0)
    {
        PRINT_ERR("Expected `%s`, got `%s`", "1", getStr(gotString));
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }
    freeString(gotString);

    tmp = tmp->before;
    gotString = stringifyExpr(tmp->value);
    if (cmpStringStr(gotString, "(2 * 3)") != 0)
    {
        PRINT_ERR("Expected `%s`, got `%s`", "(2 * 3)", getStr(gotString));
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }
    freeString(gotString);

    tmp = tmp->before;
    gotString = stringifyExpr(tmp->value);
    if (cmpStringStr(gotString, "(4 + 5)") != 0)
    {
        PRINT_ERR("Expected `%s`, got `%s`", "(4 + 5)", getStr(gotString));
        freeString(gotString);
        freeStmt(stmt);
        testStatus = TEST_FAILED;
        goto EXIT_IF_FAILED;
    }

    freeString(gotString);
    freeStmt(stmt);

EXIT_IF_FAILED:
    freeProgram(program);
    freeParser(p);
    return testStatus;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

int testLetStatement(Stmt* stmt, const char* name, int64_t expectedValue)
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

    if (!testLiteral(letStmt->value, expectedValue))
        return FALSE;

    return TRUE;
}

int testIdentExpr(Expr* expr, const char* expected)
{
    if (!expr)
    {
        PRINT_ERR("expr is NULL", NULL);
        return FALSE;
    }

    if (expr->type != EXPR_IDENT)
    {
        PRINT_ERR("expr->type is not %d. got = %d", EXPR_IDENT, expr->type);
        return FALSE;
    }

    if (cmpStringStr(expr->inner.identExpr->value, expected) != 0)
    {
        PRINT_ERR("given expr is not `%s`. got = `%s`", expected,
                  getStr(expr->inner.identExpr->value));
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
        PRINT_ERR("expr->type in testLiteral is neither %d nor %d. got = %d",
                  EXPR_INTEGER, EXPR_BOOL, expr->type);
        return FALSE;
    }

    return TRUE;
}

MAIN_TEST(
    {
        RUN_TEST(TestLetStmts);
        RUN_TEST(TestReturnStmts);
        RUN_TEST(TestIdentExprs);
        RUN_TEST(TestIntegerLiteralExprs);
        RUN_TEST(TestPrefixExprs);
        RUN_TEST(TestInfixExprs);
        RUN_TEST(TestOperatorPrecedenceParsing);
        RUN_TEST(TestIfExprs);
        RUN_TEST(TestIfElseExprs);
        RUN_TEST(TestFunctionLiteralExprs);
        RUN_TEST(TestCallExprs);
    })

#undef MAIN_TEST_NAME // End TestParser
