#ifndef MONKEY_LANG_CPP_SRC_PARSER_PARSER_H_
#define MONKEY_LANG_CPP_SRC_PARSER_PARSER_H_

#include <functional>
#include <memory>
#include <unordered_map>

#include "Ast.hh"
#include "lexer/Lexer.hh"

namespace parser
{
class Parser;

typedef std::function<IExpression*(Parser&)> PrefixParseFn;

typedef std::function<IExpression*(Parser&, IExpression*&)> InfixParseFn;

class Parser
{
  public:
    explicit Parser(lexer::Lexer& lexer);
    Parser() = delete;
    ~Parser() = default;

  public:
    Program* ParseProgram();
    const std::vector<std::string>& Errors();

  private:
    IStatement* parseStatement();
    LetStatement* parseLetStatement();
    ReturnStatement* parseReturnStatement();
    ExpressionStatement* parseExpressionStatement();
    BlockStmt* parseBlockStatement();

  private:
    IExpression* parseExpression(Precedence prec);
    Identifier* parseIdentifier();
    IntegerLiteral* parseIntegerLiteral();
    Boolean* parseBoolean();
    PrefixExpr* parsePrefixExpression();
    InfixExpr* parseInfixExpression(IExpression*& left);
    IExpression* parseGroupedExpression();
    IfExpr* parseIfExpression();
    FunctionLiteral* parseFunctionLiteral();
    CallExpr* parseCallExpression(IExpression*& function);
    std::vector<IExpression*> parseCallArguments();

  private:
    void nextToken();
    void peekError(lexer::TokenType tokType);
    bool curTokIs(lexer::TokenType tokType) const;
    bool peekTokIs(lexer::TokenType tokType) const;
    Precedence curPrecedence() const;
    Precedence peekPrecedence() const;
    bool expectPeek(lexer::TokenType tokType);
    void registerPrefix(lexer::TokenType tokType, const PrefixParseFn& fn);
    void registerInfix(lexer::TokenType tokType, const InfixParseFn& fn);

  private:
    std::unique_ptr<lexer::Lexer> mLexer;
    lexer::Token mCurToken;
    lexer::Token mPeekToken;
    std::vector<std::string> mErrors;
    std::unordered_map<lexer::TokenType, PrefixParseFn> mPrefixParseFns;
    std::unordered_map<lexer::TokenType, InfixParseFn> mInfixParseFns;
};
} // namespace parser

#endif // MONKEY_LANG_CPP_SRC_PARSER_PARSER_H_
