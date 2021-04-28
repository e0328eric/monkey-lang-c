#define MAIN_TEST_NAME TestLexer
#include "tests.h"

TEST(AlwaysPass) { return TEST_SUCESSED; }

MAIN_TEST(RUN_TEST(AlwaysPass))
#undef MAIN_TEST_NAME // End TestLexer
