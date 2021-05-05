#include <utility>

#include "Lexer.hh"

using namespace lexer;

bool isLetter(char);
bool isDigit(char);
TokenType lookupIdent(const std::string&);

Lexer::Lexer(const char* str)
    : mInput(str), mPosition(0), mReadPosition(0), mCh('\0')
{
    readChar();
}

Lexer::Lexer(std::string& string)
    : mInput(string), mPosition(0), mReadPosition(0), mCh('\0')
{
    readChar();
}

Lexer::Lexer(Lexer&& lex) noexcept : mPosition(0), mReadPosition(0), mCh('\0')
{
    mInput = std::move(lex.mInput);
    readChar();
}

void Lexer::readChar()
{
    if ((size_t)mReadPosition >= mInput.length())
        mCh = '\0';
    else
        mCh = mInput[(size_t)mReadPosition];
    mPosition = mReadPosition;
    ++mReadPosition;
}

char Lexer::peekChar()
{
    if ((size_t)mReadPosition >= mInput.length())
    {
        return 0;
    }
    return mInput[(size_t)mReadPosition];
}

Token Lexer::NextToken()
{
    Token tok{TokenType::Illegal, ""};

    skipWhitespace();

    switch (mCh)
    {
    case '=':
        if (peekChar() == '=')
        {
            readChar();
            tok = Token{TokenType::Eq, "=="};
        }
        else
        {
            tok = Token{TokenType::Assign, "="};
        }
        break;
    case '+':
        tok = Token{TokenType::Plus, "+"};
        break;
    case '-':
        tok = Token{TokenType::Minus, "-"};
        break;
    case '*':
        tok = Token{TokenType::Asterisk, "*"};
        break;
    case '/':
        tok = Token{TokenType::Slash, "/"};
        break;
    case '!':
        if (peekChar() == '=')
        {
            readChar();
            tok = Token{TokenType::NotEq, "!="};
        }
        else
        {
            tok = Token{TokenType::Bang, "!"};
        }
        break;
    case '<':
        tok = Token{TokenType::Lt, "<"};
        break;
    case '>':
        tok = Token{TokenType::Gt, ">"};
        break;
    case ',':
        tok = Token{TokenType::Comma, ","};
        break;
    case ';':
        tok = Token{TokenType::SemiColon, ";"};
        break;
    case '(':
        tok = Token{TokenType::Lparen, "("};
        break;
    case ')':
        tok = Token{TokenType::Rparen, ")"};
        break;
    case '{':
        tok = Token{TokenType::Lbrace, "{"};
        break;
    case '}':
        tok = Token{TokenType::Rbrace, "}"};
        break;
    case 0:
        tok = Token{TokenType::Eof, ""};
        break;
    default:
        if (isLetter(mCh))
        {
            readIdentifier(tok);
            return tok;
        }
        else if (isdigit(mCh))
        {
            readNumber(tok);
            return tok;
        }
    }

    readChar();
    return tok;
}

void Lexer::readIdentifier(Token& token)
{
    int position = mPosition;
    while (isLetter(mCh))
        readChar();

    token.Literal =
        mInput.substr((size_t)position, (size_t)(mPosition - position));
    token.Type = lookupIdent(token.Literal);
}

void Lexer::readNumber(Token& token)
{
    int position = mPosition;
    while (isDigit(mCh))
        readChar();

    token.Literal =
        mInput.substr((size_t)position, (size_t)(mPosition - position));
    token.Type = TokenType::Int;
}

void Lexer::skipWhitespace()
{
    while (mCh == ' ' || mCh == '\t' || mCh == '\n' || mCh == '\r')
        readChar();
}

bool isLetter(char chr)
{
    return ('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z') ||
           chr == '_';
}

bool isDigit(char chr) { return '0' <= chr && chr <= '9'; }

TokenType lookupIdent(const std::string& ident)
{
    if (ident == "fn")
        return lexer::TokenType::Function;
    else if (ident == "let")
        return lexer::TokenType::Let;
    else if (ident == "if")
        return lexer::TokenType::If;
    else if (ident == "else")
        return lexer::TokenType::Else;
    else if (ident == "true")
        return lexer::TokenType::True;
    else if (ident == "false")
        return lexer::TokenType::False;
    else if (ident == "return")
        return lexer::TokenType::Return;
    else
        return lexer::TokenType::Ident;
}
