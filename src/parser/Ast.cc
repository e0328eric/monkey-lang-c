#include <cstdio>
#include <utility>

#include "Ast.hh"

#ifdef __APPLE__
#define ASSERT(expr) \
    if (!(expr))     \
    __asm__ volatile("int $3")
#else
#define ASSERT(expr, ...) (true)
#endif

using namespace parser;

/*  Program  */
AstType Program::Type() const { return AstType::Program; }

std::string Program::String() const
{
    std::string out{};

    for (auto& stmt : mStatements)
    {
        ASSERT(stmt);
        out += stmt->String();
    }

    return out;
}

Program::Program() : IStatement(), mStatements() {}

Program::~Program()
{
    for (auto stmt : mStatements)
        delete stmt;
}

void Program::PushStatement(IStatement*& stmt)
{
    assert(stmt);
    mStatements.push_back(stmt);
    stmt = nullptr;
}

const std::vector<IStatement*>& Program::GetStatements() const
{
    return mStatements;
}

/*  LetStatement  */
AstType LetStatement::Type() const { return AstType::LetStatement; }

std::string LetStatement::String() const
{
    ASSERT(mName && mValue);

    std::string out;

    out += "let " + mName->String() + " = ";
    if (mValue)
        out += mValue->String() + ";";

    return out;
}

LetStatement::LetStatement() : IStatement(), mName(), mValue() {}

LetStatement::~LetStatement()
{
    delete mName;
    delete mValue;
}

const Identifier* LetStatement::GetName() const { return mName; }

const IExpression* LetStatement::GetValue() const { return mValue; }

void LetStatement::SetName(Identifier*&& name)
{
    mName = name;
    name = nullptr;
}

void LetStatement::SetValue(IExpression*&& value)
{
    mValue = value;
    value = nullptr;
}

/*  ReturnStatement  */
AstType ReturnStatement::Type() const { return AstType::ReturnStatement; }

std::string ReturnStatement::String() const
{
    ASSERT(mReturnValue);

    std::string out{"return "};
    if (mReturnValue)
        out += mReturnValue->String() + ";";

    return out;
}

ReturnStatement::ReturnStatement() : IStatement(), mReturnValue() {}

ReturnStatement::~ReturnStatement() { delete mReturnValue; }

const IExpression* ReturnStatement::GetReturnValue() const
{
    return mReturnValue;
}

void ReturnStatement::SetReturnValue(IExpression*&& expr)
{
    mReturnValue = expr;
    expr = nullptr;
}

/*  ExpressionStatement  */
AstType ExpressionStatement::Type() const
{
    return AstType::ExpressionStatement;
}

std::string ExpressionStatement::String() const
{
    if (mExpression)
        return mExpression->String();
    return "";
}

ExpressionStatement::ExpressionStatement() : IStatement(), mExpression() {}

ExpressionStatement::~ExpressionStatement() { delete mExpression; }

const IExpression* ExpressionStatement::GetExpression() const
{
    return mExpression;
}

void ExpressionStatement::SetExpression(IExpression*&& expr)
{
    mExpression = expr;
    expr = nullptr;
}

/*  BlockStmt  */
AstType BlockStmt::Type() const { return AstType::BlockStmt; }

std::string BlockStmt::String() const
{
    std::string output{};

    for (auto& s : mStatements)
    {
        ASSERT(s);
        output += s->String();
    }

    return output;
}

BlockStmt::BlockStmt() : IStatement(), mStatements() {}

BlockStmt::~BlockStmt()
{
    for (auto* stmt : mStatements)
        delete stmt;
}

void BlockStmt::PushStatement(IStatement*& stmt)
{
    ASSERT(stmt);
    mStatements.push_back(stmt);
    stmt = nullptr;
}

const std::vector<IStatement*>& BlockStmt::GetStatements() const
{
    return mStatements;
}

/*  Identifier  */
AstType Identifier::Type() const { return AstType::Identifier; }

std::string Identifier::String() const { return mValue; }

Identifier::Identifier(std::string value) : IExpression(), mValue(value) {}

const std::string& Identifier::GetValue() const { return mValue; }

/*  IntegerLiteral  */
AstType IntegerLiteral::Type() const { return AstType::IntegerLiteral; }

std::string IntegerLiteral::String() const { return std::to_string(mValue); }

IntegerLiteral::IntegerLiteral(int64_t value) : IExpression(), mValue(value) {}

const int64_t& IntegerLiteral::GetValue() const { return mValue; }

void IntegerLiteral::SetValue(int64_t n) { mValue = n; }

/*  Boolean  */
AstType Boolean::Type() const { return AstType::Boolean; }

std::string Boolean::String() const { return mValue ? "true" : "false"; }

Boolean::Boolean(bool value) : IExpression(), mValue(value) {}

const bool& Boolean::GetValue() const { return mValue; }

void Boolean::SetValue(bool& value) { mValue = value; }

/*  PrefixExpr  */
AstType PrefixExpr::Type() const { return AstType::PrefixExpr; }

std::string PrefixExpr::String() const
{
    ASSERT(mRight);

    return "(" + mOperator + mRight->String() + ")";
}

PrefixExpr::PrefixExpr(std::string op, IExpression*& expr)
    : IExpression(), mOperator(op), mRight(expr)
{
    expr = nullptr;
}

PrefixExpr::~PrefixExpr() { delete mRight; }

const std::string& PrefixExpr::GetOperator() const { return mOperator; }

const IExpression* PrefixExpr::GetRight() const { return mRight; }

/*  InfixExpr  */
AstType InfixExpr::Type() const { return AstType::InfixExpr; }

std::string InfixExpr::String() const
{
    ASSERT(mLeft && mRight);

    return "(" + mLeft->String() + " " + mOperator + " " + mRight->String() +
           ")";
}

InfixExpr::InfixExpr(std::string op, IExpression*& left, IExpression*& right)
    : IExpression(), mLeft(left), mOperator(op), mRight(right)
{
}

InfixExpr::~InfixExpr()
{
    delete mLeft;
    delete mRight;
}

const std::string& InfixExpr::GetOperator() const { return mOperator; }

const IExpression* InfixExpr::GetLeft() const { return mLeft; }

const IExpression* InfixExpr::GetRight() const { return mRight; }

/*  IfExpr  */
AstType IfExpr::Type() const { return AstType::IfExpr; }

std::string IfExpr::String() const
{
    std::string output;

    output += "if" + mCondition->String() + " " + mConsequence->String();
    if (!mAlternative)
        output += "else " + mAlternative->String();

    return output;
}

IfExpr::IfExpr(IExpression*& condition, BlockStmt*& conseq, BlockStmt*& alter)
    : IExpression(), mCondition(condition), mConsequence(conseq),
      mAlternative(alter)
{
}

IfExpr::~IfExpr()
{
    delete mCondition;
    delete mConsequence;
    if (mAlternative)
        delete mAlternative;
}

const IExpression* IfExpr::GetCondition() const { return mCondition; }

const BlockStmt* IfExpr::GetConsequence() const { return mConsequence; }

const BlockStmt* IfExpr::GetAlternative() const { return mAlternative; }

/*  FunctionLiteral  */
AstType FunctionLiteral::Type() const { return AstType::FunctionLiteral; }

std::string FunctionLiteral::String() const
{
    ASSERT(mBody);

    std::string output{"fn("};

    for (size_t i = 0; i < mParameters.size(); ++i)
    {
        ASSERT(mParameters[i]);
        output += mParameters[i]->String();
        if (i < mParameters.size() - 1)
            output += ", ";
    }
    output += ") " + mBody->String();

    return output;
}

FunctionLiteral::FunctionLiteral() : IExpression(), mParameters(), mBody() {}

FunctionLiteral::~FunctionLiteral()
{
    delete mBody;
    for (auto* param : mParameters)
        delete param;
}

const std::vector<Identifier*>& FunctionLiteral::GetParameters() const
{
    return mParameters;
}

const BlockStmt* FunctionLiteral::GetBody() const { return mBody; }

void FunctionLiteral::PushParameters(Identifier*&& ident)
{
    if (ident)
        mParameters.push_back(ident);
    ident = nullptr;
}

void FunctionLiteral::SetBody(BlockStmt*&& body)
{
    mBody = body;
    body = nullptr;
}

/*  CallExpr  */
AstType CallExpr::Type() const { return AstType::CallExpr; }

std::string CallExpr::String() const
{
    std::string output;

    output += mFunction->String() + "(";
    for (size_t i = 0; i < mArguments.size(); ++i)
    {
        output += mArguments[i]->String();
        if (i < mArguments.size() - 1)
            output += ", ";
    }
    output += ")";

    return output;
}

CallExpr::CallExpr(IExpression*& function)
    : IExpression(), mFunction(function), mArguments()
{
    function = nullptr;
}

CallExpr::~CallExpr()
{
    delete mFunction;
    for (auto* arg : mArguments)
        delete arg;
}

const IExpression* CallExpr::GetFunction() const { return mFunction; }

const std::vector<IExpression*>& CallExpr::GetArguments() const
{
    return mArguments;
}

void CallExpr::SetArgument(std::vector<IExpression*> expr)
{
    mArguments = std::move(expr);
}
