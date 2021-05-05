#include "Parser.hh"

#define EXPECT_PEEK(tokType)                    \
    if (!expectPeek(lexer::TokenType::tokType)) \
    return nullptr

using namespace parser;

const std::unordered_map<lexer::TokenType, Precedence> precedences = {
    {lexer::TokenType::Eq, Precedence::Equals},
    {lexer::TokenType::NotEq, Precedence::Equals},
    {lexer::TokenType::Lt, Precedence::LessGreater},
    {lexer::TokenType::Gt, Precedence::LessGreater},
    {lexer::TokenType::Plus, Precedence::Sum},
    {lexer::TokenType::Minus, Precedence::Sum},
    {lexer::TokenType::Slash, Precedence::Product},
    {lexer::TokenType::Asterisk, Precedence::Product},
    {lexer::TokenType::Lparen, Precedence::Call},
};

Parser::Parser(lexer::Lexer& lexer)
    : mLexer(new lexer::Lexer(std::move(lexer))),
      mCurToken(lexer::Token{lexer::TokenType::Illegal, ""}),
      mPeekToken(lexer::Token{lexer::TokenType::Illegal, ""})
{
    // Declare prefix functions
    registerPrefix(lexer::TokenType::Ident, &Parser::parseIdentifier);
    registerPrefix(lexer::TokenType::Int, &Parser::parseIntegerLiteral);
    registerPrefix(lexer::TokenType::Bang, &Parser::parsePrefixExpression);
    registerPrefix(lexer::TokenType::Minus, &Parser::parsePrefixExpression);
    registerPrefix(lexer::TokenType::True, &Parser::parseBoolean);
    registerPrefix(lexer::TokenType::False, &Parser::parseBoolean);
    registerPrefix(lexer::TokenType::Lparen, &Parser::parseGroupedExpression);
    registerPrefix(lexer::TokenType::If, &Parser::parseIfExpression);
    registerPrefix(lexer::TokenType::Function, &Parser::parseFunctionLiteral);

    // Declare infix functions
    registerInfix(lexer::TokenType::Plus, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::Minus, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::Slash, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::Asterisk, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::Eq, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::NotEq, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::Lt, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::Gt, &Parser::parseInfixExpression);
    registerInfix(lexer::TokenType::Lparen, &Parser::parseCallExpression);

    nextToken();
    nextToken();
}

const std::vector<std::string>& Parser::Errors() { return mErrors; }

void Parser::nextToken()
{
    mCurToken = mPeekToken;
    mPeekToken = mLexer->NextToken();
}

void Parser::peekError(lexer::TokenType tokType)
{
    mErrors.push_back("expected next token to be " +
                      lexer::TokTypeToString(tokType) + ", got " +
                      lexer::TokTypeToString(mPeekToken.Type) + " instead");
}

bool Parser::curTokIs(lexer::TokenType tokType) const
{
    return mCurToken.Type == tokType;
}

bool Parser::peekTokIs(lexer::TokenType tokType) const
{
    return mPeekToken.Type == tokType;
}

Precedence Parser::curPrecedence() const
{
    if (precedences.find(mCurToken.Type) != precedences.end())
    {
        return precedences.at(mCurToken.Type);
    }
    return Precedence::Lowest;
}

Precedence Parser::peekPrecedence() const
{
    if (precedences.find(mPeekToken.Type) != precedences.end())
    {
        return precedences.at(mPeekToken.Type);
    }
    return Precedence::Lowest;
}

bool Parser::expectPeek(lexer::TokenType tokType)
{
    if (peekTokIs(tokType))
    {
        nextToken();
        return true;
    }
    peekError(tokType);
    return false;
}

void Parser::registerPrefix(lexer::TokenType tokType, const PrefixParseFn& fn)
{
    mPrefixParseFns.insert(
        std::pair<lexer::TokenType, PrefixParseFn>(tokType, fn));
}

void Parser::registerInfix(lexer::TokenType tokType, const InfixParseFn& fn)
{
    mInfixParseFns.insert(
        std::pair<lexer::TokenType, InfixParseFn>(tokType, fn));
}

//////////////////////////////////////////////////
//      Implementing IStatement Functions        //
//////////////////////////////////////////////////
Program* Parser::ParseProgram()
{
    auto* program = new Program();

    while (mCurToken.Type != lexer::TokenType::Eof)
    {
        IStatement* stmt = parseStatement();
        if (stmt)
            program->PushStatement(stmt);
        nextToken();
    }

    return program;
}

IStatement* Parser::parseStatement()
{
    switch (mCurToken.Type)
    {
    case lexer::TokenType::Let:
        return parseLetStatement();
    case lexer::TokenType::Return:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

LetStatement* Parser::parseLetStatement()
{
    auto* stmt = new LetStatement{};

    EXPECT_PEEK(Ident);
    stmt->SetName(new Identifier{mCurToken.Literal});
    EXPECT_PEEK(Assign);

    nextToken();

    stmt->SetValue(parseExpression(Precedence::Lowest));
    if (peekTokIs(lexer::TokenType::SemiColon))
        nextToken();

    return stmt;
}

ReturnStatement* Parser::parseReturnStatement()
{
    auto* stmt = new ReturnStatement{};
    nextToken();

    stmt->SetReturnValue(parseExpression(Precedence::Lowest));
    if (peekTokIs(lexer::TokenType::SemiColon))
        nextToken();

    return stmt;
}

ExpressionStatement* Parser::parseExpressionStatement()
{
    auto* stmt = new ExpressionStatement{};
    stmt->SetExpression(parseExpression(Precedence::Lowest));

    if (peekTokIs(lexer::TokenType::SemiColon))
        nextToken();

    return stmt;
}

BlockStmt* Parser::parseBlockStatement()
{
    auto* block = new BlockStmt{};
    nextToken();

    while (!curTokIs(lexer::TokenType::Rbrace) &&
           !curTokIs(lexer::TokenType::Eof))
    {
        IStatement* stmt = parseStatement();
        if (stmt)
            block->PushStatement(stmt);
        nextToken();
    }

    return block;
}

//////////////////////////////////////////////////
//      Implementing IExpression Functions       //
//////////////////////////////////////////////////
IExpression* Parser::parseExpression(Precedence prec)
{
    PrefixParseFn prefix = mPrefixParseFns[mCurToken.Type];
    if (!prefix)
    {
        mErrors.emplace_back("no prefix parse function for " +
                             lexer::TokTypeToString(mCurToken.Type) + " found");
        return nullptr;
    }
    auto leftExp = prefix(*this);

    while (!peekTokIs(lexer::TokenType::SemiColon) && prec < peekPrecedence())
    {
        InfixParseFn infix = mInfixParseFns[mPeekToken.Type];
        if (!infix)
            return leftExp;

        nextToken();

        leftExp = infix(*this, leftExp);
    }

    return leftExp;
}

Identifier* Parser::parseIdentifier()
{
    return new Identifier{mCurToken.Literal};
}

IntegerLiteral* Parser::parseIntegerLiteral()
{
    auto* lit = new IntegerLiteral{0};
    try
    {
        lit->SetValue(std::stoll(mCurToken.Literal));
    }
    catch (std::invalid_argument& e)
    {
        mErrors.push_back("Given number is not an integer. got = " +
                          mCurToken.Literal);
        return nullptr;
    }
    catch (std::out_of_range& e)
    {
        mErrors.emplace_back("Given integer is too large to store it");
        return nullptr;
    }

    return lit;
}

Boolean* Parser::parseBoolean()
{
    return new Boolean{curTokIs(lexer::TokenType::True)};
}

PrefixExpr* Parser::parsePrefixExpression()
{
    std::string op = mCurToken.Literal;

    nextToken();
    auto expr = parseExpression(Precedence::Prefix);

    return new PrefixExpr{op, expr};
}

InfixExpr* Parser::parseInfixExpression(IExpression*& left)
{
    std::string op = mCurToken.Literal;

    Precedence prec = curPrecedence();
    nextToken();
    auto right = parseExpression(prec);

    return new InfixExpr{op, left, right};
}

IExpression* Parser::parseGroupedExpression()
{
    nextToken();
    IExpression* exp = parseExpression(Precedence::Lowest);

    EXPECT_PEEK(Rparen);
    return exp;
}

IfExpr* Parser::parseIfExpression()
{
    EXPECT_PEEK(Lparen);
    nextToken();

    auto condition = parseExpression(Precedence::Lowest);

    EXPECT_PEEK(Rparen);
    EXPECT_PEEK(Lbrace);

    auto consequence = parseBlockStatement();

    BlockStmt* alternative = nullptr;
    if (peekTokIs(lexer::TokenType::Else))
    {
        nextToken();
        EXPECT_PEEK(Lbrace);
        alternative = parseBlockStatement();
    }

    return new IfExpr{condition, consequence, alternative};
}

FunctionLiteral* Parser::parseFunctionLiteral()
{
    auto* lit = new FunctionLiteral{};

    EXPECT_PEEK(Lparen);
    if (peekTokIs(lexer::TokenType::Rparen))
    {
        nextToken();
    }
    else
    {
        nextToken();
        lit->PushParameters(new Identifier{mCurToken.Literal});

        while (peekTokIs(lexer::TokenType::Comma))
        {
            nextToken();
            nextToken();
            lit->PushParameters(new Identifier{mCurToken.Literal});
        }
        EXPECT_PEEK(Rparen);
    }
    EXPECT_PEEK(Lbrace);

    lit->SetBody(parseBlockStatement());

    return lit;
}

CallExpr* Parser::parseCallExpression(IExpression*& function)
{
    auto* exp = new CallExpr(function);
    exp->SetArgument(parseCallArguments());

    return exp;
}

std::vector<IExpression*> Parser::parseCallArguments()
{
    std::vector<IExpression*> args;

    if (peekTokIs(lexer::TokenType::Rparen))
    {
        nextToken();
        return args;
    }

    nextToken();
    args.push_back(parseExpression(Precedence::Lowest));

    while (peekTokIs(lexer::TokenType::Comma))
    {
        nextToken();
        nextToken();
        args.push_back(parseExpression(Precedence::Lowest));
    }

    if (!expectPeek(lexer::TokenType::Rparen))
    {
        std::vector<IExpression*> tmp{nullptr};
        return tmp;
    }

    return args;
}
