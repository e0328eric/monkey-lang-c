#define MAIN_TEST_NAME TestDynString

#include "dynString.h"
#include "testing.h"

TEST(mkString)
{
    const char* str = "Hello World!";
    size_t strLen = strlen(str);

    String* string = mkString(str);
    if (strLen != getLen(string))
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, getLen(string));
        freeString(string);
        return TEST_FAILED;
    }

    if (strcmp(getStr(string), str) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", str,
                  getStr(string));
        freeString(string);
        return TEST_FAILED;
    }

    freeString(string);
    return TEST_SUCESSED;
}

TEST(mkNString)
{
    const char* str = "Hello World!";

    String* string = mkNString(str, 5);
    if (getLen(string) != 5)
    {
        PRINT_ERR("String Length are different. expected = %d, got = %zu", 5,
                  getLen(string));
        freeString(string);
        return TEST_FAILED;
    }

    if (strcmp(getStr(string), "Hello") != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", "Hello",
                  getStr(string));
        freeString(string);
        return TEST_FAILED;
    }

    freeString(string);
    return TEST_SUCESSED;
}

TEST(appendChar)
{
    const char* str = "Hello World!";
    const char* expected = "Hello World!* f";
    size_t strLen = strlen(expected);

    String* string = mkString(str);
    appendChar(string, '*');
    appendChar(string, ' ');
    appendChar(string, 'f');

    if (strLen != getLen(string))
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, getLen(string));
        freeString(string);
        return TEST_FAILED;
    }

    if (strcmp(getStr(string), expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  getStr(string));
        freeString(string);
        return TEST_FAILED;
    }

    freeString(string);
    return TEST_SUCESSED;
}

TEST(appendStr)
{
    const char* source = "Hello";
    const char* toAppend = " World!";
    const char* expected = "Hello World!";
    size_t strLen = strlen(expected);

    String* string = mkString(source);
    appendStr(string, toAppend);

    if (strLen != getLen(string))
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, getLen(string));
        freeString(string);
        return TEST_FAILED;
    }

    if (strcmp(getStr(string), expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  getStr(string));
        freeString(string);
        return TEST_FAILED;
    }

    freeString(string);
    return TEST_SUCESSED;
}

TEST(appendNStr)
{
    const char* source = "Hello";
    const char* toAppend = " World!";
    const char* expected = "Hello Wor";
    size_t strLen = strlen(expected);

    String* string = mkString(source);
    appendNStr(string, toAppend, 4);

    if (strLen != getLen(string))
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, getLen(string));
        freeString(string);
        return TEST_FAILED;
    }

    if (strcmp(getStr(string), expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  getStr(string));
        freeString(string);
        return TEST_FAILED;
    }

    freeString(string);
    return TEST_SUCESSED;
}

TEST(concatString)
{
    const char* str1 = "Hello";
    const char* str2 = " World!";
    const char* str3 = " World!!!";
    const char* expected = "Hello World! World!!!";
    size_t strLen = strlen(expected);

    String* string = mkString(str1);
    String* string2 = mkString(str2);
    String* string3 = mkString(str3);
    concatString(string, string2);
    concatString(string, string3);

    if (strLen != getLen(string))
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, getLen(string));
        freeString(string);
        freeString(string2);
        freeString(string3);
        return TEST_FAILED;
    }

    if (strcmp(getStr(string), expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  getStr(string));
        freeString(string);
        freeString(string2);
        freeString(string3);
        return TEST_FAILED;
    }

    freeString(string);
    freeString(string2);
    freeString(string3);
    return TEST_SUCESSED;
}

MAIN_TEST(RUN_TEST(mkString) RUN_TEST(mkNString) RUN_TEST(appendChar)
              RUN_TEST(appendStr) RUN_TEST(appendNStr) RUN_TEST(concatString))

#undef MAIN_TEST_NAME // End TestLexer
