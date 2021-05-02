#define MAIN_TEST_NAME TestLexer

#include "lexer.h"
#include "tests.h"
#include "token.h"

#define TOKEN(tokType, literal)    \
    {                              \
        tokType, mkString(literal) \
    }
#define T_ENDTEST (-2)

#define TEST_LEXER                                                            \
    Lexer* l = mkLexer(input);                                                \
                                                                              \
    for (int i = 0; tests[i].expectedType != -2; ++i)                         \
    {                                                                         \
        tok = nextToken(l);                                                   \
        if (tok.type != tests[i].expectedType)                                \
        {                                                                     \
            PRINT_ERR("TokenType wrong. expected = %s, got = %s",             \
                      printTokType(tests[i].expectedType),                    \
                      printTokType(tok.type));                                \
            isTestPassed = TEST_FAILED;                                       \
            goto FREE_AND_EXIT;                                               \
        }                                                                     \
                                                                              \
        if (cmpString(tests[i].expectedLiteral, tok.literal) != 0)            \
        {                                                                     \
            PRINT_ERR("TokenLiteral wrong. expected = %s, got = %s",          \
                      getStr(tests[i].expectedLiteral), getStr(tok.literal)); \
            isTestPassed = TEST_FAILED;                                       \
            goto FREE_AND_EXIT;                                               \
        }                                                                     \
                                                                              \
        freeToken(&tok);                                                      \
    }                                                                         \
                                                                              \
    FREE_AND_EXIT:                                                            \
    if (!isTestPassed)                                                        \
        freeToken(&tok);                                                      \
    freeLexer(l);                                                             \
    return isTestPassed

TEST(TestNextToken)
{
    int isTestPassed = TEST_SUCESSED;
    Token tok;
    const char* input = "=+(){},;";

    struct
    {
        TokenType expectedType;
        String* expectedLiteral;
    } tests[] = {
        TOKEN(T_ASSIGN, "="), TOKEN(T_PLUS, "+"),      TOKEN(T_LPAREN, "("),
        TOKEN(T_RPAREN, ")"), TOKEN(T_LBRACE, "{"),    TOKEN(T_RBRACE, "}"),
        TOKEN(T_COMMA, ","),  TOKEN(T_SEMICOLON, ";"), TOKEN(T_EOF, ""),
        TOKEN(T_ENDTEST, ""),
    };

    TEST_LEXER;
}

TEST(TestComplexLexing)
{
    int isTestPassed = TEST_SUCESSED;
    Token tok;
    const char* input = "let five = 5;\n"
                        "let ten = 10;\n\n"
                        "let add = fn(x, y) {\n"
                        "  x + y;\n"
                        "};\n\n"
                        "let result = add(five, ten);\n"
                        "!-*/5;\n"
                        "5 < 10 > 5;\n\n"
                        "if (5 < 10) {\n"
                        "  return true;\n"
                        "} else {\n"
                        "  return false;\n"
                        "}\n\n"
                        "10 == 10;\n"
                        "10 != 9;\n";

    struct
    {
        TokenType expectedType;
        String* expectedLiteral;
    } tests[] = {
        TOKEN(T_LET, "let"),     TOKEN(T_IDENT, "five"),
        TOKEN(T_ASSIGN, "="),    TOKEN(T_INT, "5"),
        TOKEN(T_SEMICOLON, ";"), TOKEN(T_LET, "let"),
        TOKEN(T_IDENT, "ten"),   TOKEN(T_ASSIGN, "="),
        TOKEN(T_INT, "10"),      TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_LET, "let"),     TOKEN(T_IDENT, "add"),
        TOKEN(T_ASSIGN, "="),    TOKEN(T_FUNCTION, "fn"),
        TOKEN(T_LPAREN, "("),    TOKEN(T_IDENT, "x"),
        TOKEN(T_COMMA, ","),     TOKEN(T_IDENT, "y"),
        TOKEN(T_RPAREN, ")"),    TOKEN(T_LBRACE, "{"),
        TOKEN(T_IDENT, "x"),     TOKEN(T_PLUS, "+"),
        TOKEN(T_IDENT, "y"),     TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_RBRACE, "}"),    TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_LET, "let"),     TOKEN(T_IDENT, "result"),
        TOKEN(T_ASSIGN, "="),    TOKEN(T_IDENT, "add"),
        TOKEN(T_LPAREN, "("),    TOKEN(T_IDENT, "five"),
        TOKEN(T_COMMA, ","),     TOKEN(T_IDENT, "ten"),
        TOKEN(T_RPAREN, ")"),    TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_BANG, "!"),      TOKEN(T_MINUS, "-"),
        TOKEN(T_ASTERISK, "*"),  TOKEN(T_SLASH, "/"),
        TOKEN(T_INT, "5"),       TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_INT, "5"),       TOKEN(T_LT, "<"),
        TOKEN(T_INT, "10"),      TOKEN(T_GT, ">"),
        TOKEN(T_INT, "5"),       TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_IF, "if"),       TOKEN(T_LPAREN, "("),
        TOKEN(T_INT, "5"),       TOKEN(T_LT, "<"),
        TOKEN(T_INT, "10"),      TOKEN(T_RPAREN, ")"),
        TOKEN(T_LBRACE, "{"),    TOKEN(T_RETURN, "return"),
        TOKEN(T_TRUE, "true"),   TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_RBRACE, "}"),    TOKEN(T_ELSE, "else"),
        TOKEN(T_LBRACE, "{"),    TOKEN(T_RETURN, "return"),
        TOKEN(T_FALSE, "false"), TOKEN(T_SEMICOLON, ";"),
        TOKEN(T_RBRACE, "}"),    TOKEN(T_INT, "10"),
        TOKEN(T_EQ, "=="),       TOKEN(T_INT, "10"),
        TOKEN(T_SEMICOLON, ";"), TOKEN(T_INT, "10"),
        TOKEN(T_NOT_EQ, "!="),   TOKEN(T_INT, "9"),
        TOKEN(T_SEMICOLON, ";"), TOKEN(T_EOF, ""),
        TOKEN(T_ENDTEST, ""),
    };

    TEST_LEXER;
}

MAIN_TEST(RUN_TEST(TestNextToken) RUN_TEST(TestComplexLexing))
#undef MAIN_TEST_NAME // End TestLexer
