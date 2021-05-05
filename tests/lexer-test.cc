#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "lexer/Lexer.hh"

typedef struct
{
    lexer::TokenType ExptType;
    std::string ExptLiteral;
} ExptToken;

TEST(TestLexer, TestNextToken)
{
    using namespace lexer;

    const char* input = "=+(){},;";
    std::vector<ExptToken> tests = {
        {TokenType::Assign, "="}, {TokenType::Plus, "+"},
        {TokenType::Lparen, "("}, {TokenType::Rparen, ")"},
        {TokenType::Lbrace, "{"}, {TokenType::Rbrace, "}"},
        {TokenType::Comma, ","},  {TokenType::SemiColon, ";"},
    };

    Lexer l(input);

    for (auto& exptTok : tests)
    {
        auto tok = l.NextToken();
        EXPECT_EQ(tok.Type, exptTok.ExptType);
        EXPECT_EQ(tok.Literal, exptTok.ExptLiteral);
    }
}

TEST(TestLexer, TestComplexLexing)
{
    using namespace lexer;

    const char* input = R"foo(
let five = 5;
let ten = 10;

let add = fn(x, y) {
	x + y;
};

let result = add(five, ten);
!-/*5;
5 < 10 > 5;

if (5 < 10) {
	return true;
} else  {
	return false;
}

10 == 10;
10 != 9;
)foo";

    std::vector<ExptToken> tests = {
        {TokenType::Let, "let"},     {TokenType::Ident, "five"},
        {TokenType::Assign, "="},    {TokenType::Int, "5"},
        {TokenType::SemiColon, ";"}, {TokenType::Let, "let"},
        {TokenType::Ident, "ten"},   {TokenType::Assign, "="},
        {TokenType::Int, "10"},      {TokenType::SemiColon, ";"},
        {TokenType::Let, "let"},     {TokenType::Ident, "add"},
        {TokenType::Assign, "="},    {TokenType::Function, "fn"},
        {TokenType::Lparen, "("},    {TokenType::Ident, "x"},
        {TokenType::Comma, ","},     {TokenType::Ident, "y"},
        {TokenType::Rparen, ")"},    {TokenType::Lbrace, "{"},
        {TokenType::Ident, "x"},     {TokenType::Plus, "+"},
        {TokenType::Ident, "y"},     {TokenType::SemiColon, ";"},
        {TokenType::Rbrace, "}"},    {TokenType::SemiColon, ";"},
        {TokenType::Let, "let"},     {TokenType::Ident, "result"},
        {TokenType::Assign, "="},    {TokenType::Ident, "add"},
        {TokenType::Lparen, "("},    {TokenType::Ident, "five"},
        {TokenType::Comma, ","},     {TokenType::Ident, "ten"},
        {TokenType::Rparen, ")"},    {TokenType::SemiColon, ";"},
        {TokenType::Bang, "!"},      {TokenType::Minus, "-"},
        {TokenType::Slash, "/"},     {TokenType::Asterisk, "*"},
        {TokenType::Int, "5"},       {TokenType::SemiColon, ";"},
        {TokenType::Int, "5"},       {TokenType::Lt, "<"},
        {TokenType::Int, "10"},      {TokenType::Gt, ">"},
        {TokenType::Int, "5"},       {TokenType::SemiColon, ";"},
        {TokenType::If, "if"},       {TokenType::Lparen, "("},
        {TokenType::Int, "5"},       {TokenType::Lt, "<"},
        {TokenType::Int, "10"},      {TokenType::Rparen, ")"},
        {TokenType::Lbrace, "{"},    {TokenType::Return, "return"},
        {TokenType::True, "true"},   {TokenType::SemiColon, ";"},
        {TokenType::Rbrace, "}"},    {TokenType::Else, "else"},
        {TokenType::Lbrace, "{"},    {TokenType::Return, "return"},
        {TokenType::False, "false"}, {TokenType::SemiColon, ";"},
        {TokenType::Rbrace, "}"},    {TokenType::Int, "10"},
        {TokenType::Eq, "=="},       {TokenType::Int, "10"},
        {TokenType::SemiColon, ";"}, {TokenType::Int, "10"},
        {TokenType::NotEq, "!="},    {TokenType::Int, "9"},
        {TokenType::SemiColon, ";"},
    };

    Lexer l(input);

    for (auto& exptTok : tests)
    {
        auto tok = l.NextToken();
        EXPECT_EQ(tok.Type, exptTok.ExptType);
        EXPECT_EQ(tok.Literal, exptTok.ExptLiteral);
    }
}
