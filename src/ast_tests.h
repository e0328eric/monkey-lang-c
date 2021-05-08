#define MAIN_TEST_NAME TestAst

#include "ast.h"
#include "testing.h"

TEST(TestProgramPushAndPop)
{
    static int testStatus = TEST_SUCESSED;

    Program* program = mkProgram();
    Stmt* stmt1 = mkStmt();
    Stmt* stmt2 = mkStmt();
    Stmt* stmt3 = mkStmt();
    Stmt* stmt4 = mkStmt();
    Stmt* stmt5 = mkStmt();

    // These statement is just for checking that
    // pushing and poping inside the program queue
    stmt1->type = STMT_RETURN;
    stmt2->type = STMT_LET;
    stmt3->type = STMT_EXPRESSION;
    stmt4->type = STMT_EXPRESSION;
    stmt5->type = STMT_LET;

    pushStmt(program, &stmt1);
    pushStmt(program, &stmt2);
    pushStmt(program, &stmt3);
    pushStmt(program, &stmt4);
    pushStmt(program, &stmt5);

    if (program->len != 5)
    {
        PRINT_ERR("program->len is not 5. got = %zu", program->len);
        testStatus = TEST_FAILED;
        goto TEST_IS_FAILED;
    }

    int expected[] = {2, 1, 3, 3, 1};

    Stmt* tmp = NULL;
    for (int i = 0; i < 5; ++i)
    {
        tmp = popStmt(program);
        if ((int)tmp->type != expected[i])
        {
            PRINT_ERR("expected  = %d, got = %d", expected[i], tmp->type);
            testStatus = TEST_FAILED;
            freeStmt(tmp);
            goto TEST_IS_FAILED;
        }
        pushStmt(program, &tmp);
    }

    if (program->len != 5)
    {
        PRINT_ERR("program->len is not 5. got = %zu", program->len);
        testStatus = TEST_FAILED;
        goto TEST_IS_FAILED;
    }

TEST_IS_FAILED:
    freeProgram(program);
    return testStatus;
}

MAIN_TEST(RUN_TEST(TestProgramPushAndPop))

#undef MAIN_TEST_NAME // End TestAst
