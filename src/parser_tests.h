#include "ast.h"
#define MAIN_TEST_NAME TestParser

#include "parser.h"
#include "testing.h"

#define FALSE 0
#define TRUE  1

#define MAKE_PARSER(expectedLen)                                       \
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

/* Function Signatures */
int testLetStatement(Stmt*, const char*);

TEST(TestLetStatements)
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

TEST(TestReturnStatements)
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
#endif // _MONKEY_PARSER_TEST_H_

MAIN_TEST(RUN_TEST(TestLetStatements) RUN_TEST(TestReturnStatements))

#undef MAIN_TEST_NAME // End TestParser