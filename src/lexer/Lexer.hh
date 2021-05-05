#ifndef MONKEY_LANG_CPP_SRC_LEXER_LEXER_H_
#define MONKEY_LANG_CPP_SRC_LEXER_LEXER_H_

#include <string>

#include "Token.hh"

namespace lexer
{
class Lexer
{
  public:
    explicit Lexer(const char* str);
    explicit Lexer(std::string& string);
    Lexer(Lexer&& lex) noexcept;
    ~Lexer() = default;

    Token NextToken();

  private:
    void readChar();
    char peekChar();
    void skipWhitespace();
    void readIdentifier(Token& token);
    void readNumber(Token& token);

  private:
    std::string mInput;
    int mPosition;
    int mReadPosition;
    char mCh;
};
} // namespace lexer
#endif // MONKEY_LANG_CPP_SRC_LEXER_LEXER_H_
