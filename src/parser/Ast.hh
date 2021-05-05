#ifndef MONKEY_LANG_CPP_SRC_PARSER_AST_H_
#define MONKEY_LANG_CPP_SRC_PARSER_AST_H_

#include <string>
#include <vector>

#include "lexer/Token.hh"

namespace parser
{
/*  Predefined class signature of expressions  */
class IExpression;
class Identifier;

/*  Declare two types  */
/*  Precedence  */
enum class Precedence
{
    Lowest,
    Equals,
    LessGreater,
    Sum,
    Product,
    Prefix,
    Call,
};

enum class AstType
{
    // Statements
    Program,
    LetStatement,
    ReturnStatement,
    ExpressionStatement,
    BlockStmt,

    // Expressions
    Identifier,
    IntegerLiteral,
    Boolean,
    PrefixExpr,
    InfixExpr,
    IfExpr,
    FunctionLiteral,
    CallExpr,
};

/*  Signature of Main Classes  */
class INode
{
  public:
    virtual AstType Type() const = 0;
    virtual std::string String() const = 0;
    virtual ~INode() = default;
};

class IStatement : public INode
{
  public:
    AstType Type() const override = 0;
    std::string String() const override = 0;
    ~IStatement() override = default;
};

class IExpression : public INode
{
  public:
    AstType Type() const override = 0;
    std::string String() const override = 0;
    ~IExpression() override = default;
};

/*  Statements  */
class Program final : public IStatement
{
  public:
    AstType Type() const override;
    std::string String() const override;

    Program();
    ~Program() override;

    void PushStatement(IStatement*& stmt);
    const std::vector<IStatement*>& GetStatements() const;

  private:
    std::vector<IStatement*> mStatements;
};

class LetStatement final : public IStatement
{
  public:
    AstType Type() const override;
    std::string String() const override;

    // LetStatement owns both an identifier and an expression
    LetStatement();
    ~LetStatement() override;

    const Identifier* GetName() const;
    const IExpression* GetValue() const;
    void SetName(Identifier*&& name);
    void SetValue(IExpression*&& value);

  private:
    Identifier* mName = nullptr;
    IExpression* mValue = nullptr;
};

class ReturnStatement final : public IStatement
{
  public:
    AstType Type() const override;
    std::string String() const override;

    ReturnStatement();
    ~ReturnStatement() override;

    const IExpression* GetReturnValue() const;
    void SetReturnValue(IExpression*&& expr);

  private:
    IExpression* mReturnValue = nullptr;
};

class ExpressionStatement final : public IStatement
{
  public:
    AstType Type() const override;
    std::string String() const override;

    ExpressionStatement();
    ~ExpressionStatement() override;

    const IExpression* GetExpression() const;
    void SetExpression(IExpression*&& expr);

  private:
    IExpression* mExpression = nullptr;
};

class BlockStmt final : public IStatement
{
  public:
    AstType Type() const override;
    std::string String() const override;

    BlockStmt();
    ~BlockStmt() override;

    void PushStatement(IStatement*& stmt);
    const std::vector<IStatement*>& GetStatements() const;

  private:
    std::vector<IStatement*> mStatements;
};

/*  Expressions  */
class Identifier final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    Identifier(std::string value);
    ~Identifier() override = default;

    const std::string& GetValue() const;

  private:
    std::string mValue;
};

class IntegerLiteral final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    IntegerLiteral(int64_t value);
    ~IntegerLiteral() override = default;

    const int64_t& GetValue() const;
    void SetValue(int64_t n);

  private:
    int64_t mValue;
};

class Boolean final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    Boolean(bool value);
    ~Boolean() override = default;

    const bool& GetValue() const;
    void SetValue(bool& value);

  private:
    bool mValue;
};

class PrefixExpr final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    PrefixExpr(std::string op, IExpression*& expr);
    ~PrefixExpr() override;

    const std::string& GetOperator() const;
    const IExpression* GetRight() const;

  private:
    std::string mOperator;
    IExpression* mRight = nullptr;
};

class InfixExpr final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    InfixExpr(std::string op, IExpression*& left, IExpression*& right);
    ~InfixExpr() override;

    const std::string& GetOperator() const;
    const IExpression* GetLeft() const;
    const IExpression* GetRight() const;

  private:
    IExpression* mLeft = nullptr;
    std::string mOperator;
    IExpression* mRight = nullptr;
};

class IfExpr final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    IfExpr(IExpression*& condition, BlockStmt*& conseq, BlockStmt*& alter);
    ~IfExpr() override;

    const IExpression* GetCondition() const;
    const BlockStmt* GetConsequence() const;
    const BlockStmt* GetAlternative() const;

  private:
    IExpression* mCondition = nullptr;
    BlockStmt* mConsequence = nullptr;
    BlockStmt* mAlternative = nullptr;
};

class FunctionLiteral final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    FunctionLiteral();
    ~FunctionLiteral() override;

    const std::vector<Identifier*>& GetParameters() const;
    const BlockStmt* GetBody() const;
    void PushParameters(Identifier*&& ident);
    void SetBody(BlockStmt*&& body);

  private:
    std::vector<Identifier*> mParameters;
    BlockStmt* mBody = nullptr;
};

class CallExpr final : public IExpression
{
  public:
    AstType Type() const override;
    std::string String() const override;

    explicit CallExpr(IExpression*& function);
    ~CallExpr() override;

    const IExpression* GetFunction() const;
    const std::vector<IExpression*>& GetArguments() const;
    void SetArgument(std::vector<IExpression*> expr);

  private:
    IExpression* mFunction = nullptr;
    std::vector<IExpression*> mArguments;
};
} // namespace parser

#endif // MONKEY_LANG_CPP_SRC_PARSER_AST_H_
