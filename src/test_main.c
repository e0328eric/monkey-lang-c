#include <stdlib.h>

#include "ast_tests.h"
#include "lexer_tests.h"
#include "parser_tests.h"
#include "string_tests.h"

#define IS_MAIN_FILE
#include "testing.h"

int main(void)
{
    RUN_MAIN_TEST(TestDynString);
    RUN_MAIN_TEST(TestLexer);
    RUN_MAIN_TEST(TestAst);
    RUN_MAIN_TEST(TestParser);

    return 0;
}
