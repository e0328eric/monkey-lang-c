#define MAIN_TEST_NAME TestDynString

#include "dynString.h"
#include "tests.h"

TEST(mkString)
{
    const char* str = "Hello World!";
    size_t strLen = strlen(str);

    String string = mkString(str);
    if (strLen != string.len)
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, string.len);
        freeString(&string);
        return TEST_FAILED;
    }

    if (strcmp(string.inner, str) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", str,
                  string.inner);
        freeString(&string);
        return TEST_FAILED;
    }

    freeString(&string);
    return TEST_SUCESSED;
}

TEST(mkNString)
{
    const char* str = "Hello World!";

    String string = mkNString(str, 5);
    if (string.len != 5)
    {
        PRINT_ERR("String Length are different. expected = %d, got = %zu", 5,
                  string.len);
        freeString(&string);
        return TEST_FAILED;
    }

    if (strcmp(string.inner, "Hello") != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", "Hello",
                  string.inner);
        freeString(&string);
        return TEST_FAILED;
    }

    freeString(&string);
    return TEST_SUCESSED;
}

TEST(appendChar)
{
    const char* str = "Hello World!";
    const char* expected = "Hello World!* f";
    size_t strLen = strlen(expected);

    String string = mkString(str);
    appendChar(&string, '*');
    appendChar(&string, ' ');
    appendChar(&string, 'f');

    if (strLen != string.len)
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, string.len);
        freeString(&string);
        return TEST_FAILED;
    }

    if (strcmp(string.inner, expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  string.inner);
        freeString(&string);
        return TEST_FAILED;
    }

    freeString(&string);
    return TEST_SUCESSED;
}

TEST(appendStr)
{
    const char* source = "Hello";
    const char* toAppend = " World!";
    const char* expected = "Hello World!";
    size_t strLen = strlen(expected);

    String string = mkString(source);
    appendStr(&string, toAppend);

    if (strLen != string.len)
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, string.len);
        freeString(&string);
        return TEST_FAILED;
    }

    if (strcmp(string.inner, expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  string.inner);
        freeString(&string);
        return TEST_FAILED;
    }

    freeString(&string);
    return TEST_SUCESSED;
}

TEST(appendNStr)
{
    const char* source = "Hello";
    const char* toAppend = " World!";
    const char* expected = "Hello Wor";
    size_t strLen = strlen(expected);

    String string = mkString(source);
    appendNStr(&string, toAppend, 4);

    if (strLen != string.len)
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, string.len);
        freeString(&string);
        return TEST_FAILED;
    }

    if (strcmp(string.inner, expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  string.inner);
        freeString(&string);
        return TEST_FAILED;
    }

    freeString(&string);
    return TEST_SUCESSED;
}

TEST(concatString)
{
    const char* str1 = "Hello";
    const char* str2 = " World!";
    const char* str3 = " World!!!";
    const char* expected = "Hello World! World!!!";
    size_t strLen = strlen(expected);

    String string1 = mkString(str1);
    String string2 = mkString(str2);
    String string3 = mkString(str3);
    String string_ = concatString(&string1, &string2);
    String string = concatString(&string_, &string3);

    if (strLen != string.len)
    {
        PRINT_ERR("String Length are different. expected = %zu, got = %zu",
                  strLen, string.len);
        freeString(&string);
        freeString(&string_);
        freeString(&string3);
        freeString(&string2);
        freeString(&string1);
        return TEST_FAILED;
    }

    if (strcmp(string.inner, expected) != 0)
    {
        PRINT_ERR("Strings are different. expected = %s, got = %s", expected,
                  string.inner);
        freeString(&string);
        freeString(&string_);
        freeString(&string3);
        freeString(&string2);
        freeString(&string1);
        return TEST_FAILED;
    }

    freeString(&string);
    freeString(&string_);
    freeString(&string3);
    freeString(&string2);
    freeString(&string1);
    return TEST_SUCESSED;
}

MAIN_TEST(RUN_TEST(mkString) RUN_TEST(mkNString) RUN_TEST(appendChar)
              RUN_TEST(appendStr) RUN_TEST(appendNStr) RUN_TEST(concatString))

#undef MAIN_TEST_NAME // End TestLexer
