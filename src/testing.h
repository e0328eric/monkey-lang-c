#ifndef _MONKEY_LANG_TESTS_TEST_H_
#define _MONKEY_LANG_TESTS_TEST_H_

// Basic standard headers might using at all test headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_FAILED   0
#define TEST_SUCESSED 1

#define FAILED_COLOR   "\x1b[1m\x1b[91m"
#define NOTE_COLOR     "\x1b[1m\x1b[36m"
#define SUCESSED_COLOR "\x1b[1m\x1b[92m"
#define RESET_COLOR    "\x1b[0m"

#define TEST_ROW_SIZE 83

#ifndef MAIN_TEST_NAME
#define MAIN_TEST_NAME
#endif

#define PASTER_CORE(x, y) x##___##y
#define PASTER(x, y)      PASTER_CORE(x, y)

#define MAIN_TEST_CORE(mainName, testLists) \
    int mainName(void)                      \
    {                                       \
        int __resultStatus = TEST_SUCESSED; \
        int isPassed;                       \
        testLists;                          \
        return __resultStatus;              \
    }
#define MAIN_TEST(testLists) MAIN_TEST_CORE(MAIN_TEST_NAME, testLists)

#define RUN_MAIN_TEST(mainName)                                             \
    {                                                                       \
        for (int i = 0; i < TEST_ROW_SIZE; ++i)                             \
            printf("=");                                                    \
        printf("\n");                                                       \
        const char* __title = "< Testing `" #mainName "`>";                 \
        size_t __titleLen = strlen(__title);                                \
        size_t __padding = (TEST_ROW_SIZE - __titleLen) / 2;                \
        __padding = __padding <= 0 ? 0 : __padding;                         \
        for (size_t i = 0; i < __padding; ++i)                              \
            printf(" ");                                                    \
        printf("%s", __title);                                              \
        for (size_t i = __titleLen; i < TEST_ROW_SIZE; ++i)                 \
            printf(" ");                                                    \
        printf("\n\n");                                                     \
        if (mainName() == TEST_FAILED)                                      \
            printf(FAILED_COLOR "\n[" #mainName "]  "                       \
                                "  Test Failed...\n" RESET_COLOR);          \
        else                                                                \
            printf(SUCESSED_COLOR "\n[" #mainName "]  "                     \
                                  "  All tests are passed!\n" RESET_COLOR); \
        for (int i = 0; i < TEST_ROW_SIZE; ++i)                             \
            printf("=");                                                    \
        printf("\n\n");                                                     \
    }

#define TEST(subName) int PASTER(MAIN_TEST_NAME, subName)(void)
#define RUN_TEST(subName)                               \
    isPassed = PASTER(MAIN_TEST_NAME, subName)();       \
    if (isPassed == TEST_SUCESSED)                      \
        printf(SUCESSED_COLOR "(" #subName ")    "      \
                              "passed!\n" RESET_COLOR); \
    else                                                \
        printf(FAILED_COLOR "(" #subName ")    "        \
                            "failed...\n" RESET_COLOR); \
    __resultStatus &= isPassed;

#define PRINT_ERR(fmtString, ...) \
    printf(FAILED_COLOR "ERROR: " fmtString "\n", __VA_ARGS__)
#define PRINT_NOTE(fmtString, ...) \
    printf(NOTE_COLOR "NOTE:  " fmtString "\n", __VA_ARGS__)

#endif // _MONKEY_LANG_TESTS_TEST_H_
