#ifndef MONKEY_LANG_CPP_SRC_LEXER_TOKEN_H_
#define MONKEY_LANG_CPP_SRC_LEXER_TOKEN_H_

#include <iostream>
#include <string>

namespace lexer
{
enum class TokenType;

struct Token
{
    TokenType Type;
    std::string Literal;
};

enum class TokenType
{
    Illegal,
    Eof,

    // Identifiers + literals
    Ident,
    Int,

    // Operator
    Assign,
    Plus,
    Minus,
    Bang,
    Asterisk,
    Slash,
    Lt,
    Gt,
    Eq,
    NotEq,

    // Delimiters
    Comma,
    SemiColon,
    Lparen,
    Rparen,
    Lbrace,
    Rbrace,

    // Keywords
    Function,
    Let,
    True,
    False,
    If,
    Else,
    Return,
};

std::string TokTypeToString(TokenType tokType);
} // namespace lexer

std::ostream& operator<<(std::ostream& o, const lexer::Token& token);
std::ostream& operator<<(std::ostream& o, const lexer::TokenType& tokType);

#endif // MONKEY_LANG_CPP_SRC_LEXER_TOKEN_H_
