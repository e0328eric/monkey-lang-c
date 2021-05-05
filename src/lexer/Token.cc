#include "Token.hh"

std::string lexer::TokTypeToString(lexer::TokenType tokType)
{
    using namespace lexer;

    switch (tokType)
    {
    case TokenType::Ident:
        return "IDENT";
    case TokenType::Int:
        return "INT";
    case TokenType::Assign:
        return "=";
    case TokenType::Plus:
        return "+";
    case TokenType::Minus:
        return "-";
    case TokenType::Asterisk:
        return "*";
    case TokenType::Slash:
        return "/";
    case TokenType::Bang:
        return "!";
    case TokenType::Lt:
        return "<";
    case TokenType::Gt:
        return ">";
    case TokenType::Eq:
        return "==";
    case TokenType::NotEq:
        return "!=";
    case TokenType::Comma:
        return ",";
    case TokenType::SemiColon:
        return ";";
    case TokenType::Lparen:
        return "(";
    case TokenType::Rparen:
        return ")";
    case TokenType::Lbrace:
        return "{";
    case TokenType::Rbrace:
        return "}";
    case TokenType::Function:
        return "FUNCTION";
    case TokenType::Let:
        return "LET";
    case TokenType::If:
        return "IF";
    case TokenType::Else:
        return "ELSE";
    case TokenType::True:
        return "__TRUE";
    case TokenType::False:
        return "FALSE";
    case TokenType::Return:
        return "RETURN";
    case TokenType::Eof:
        return "EOF";
    case TokenType::Illegal:
        return "ILLEGAL";
    }
}

std::ostream& operator<<(std::ostream& o, const lexer::Token& token)
{
    o << "{ TokenType: `" << token.Type << "`, Literal: " << token.Literal
      << "}\n";
    return o;
}

std::ostream& operator<<(std::ostream& o, const lexer::TokenType& tokType)
{
    o << lexer::TokTypeToString(tokType);
    return o;
}
