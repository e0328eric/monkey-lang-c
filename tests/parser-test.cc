#include <any>
#include <gtest/gtest.h>
#include <typeinfo>

#include "lexer/Lexer.hh"
#include "parser/Parser.hh"

#define IS_TYPES_EQ(a, b) (a.type().name() == typeid(b).name())
#define DRAIN_ERR_MSG(p)               \
    if (!(p).Errors().empty())         \
    {                                  \
        for (auto& msg : (p).Errors()) \
            std::cerr << msg << "\n";  \
        return;                        \
    }

void testLetStatement(parser::IStatement*, const char*);

bool testLiteralExpression(const parser::IExpression*, std::any);

template <typename T, typename U>
bool testInfixExpression(const parser::IExpression*, T, const std::string&, U);

bool testIntegerLiteral(const parser::IExpression*, int64_t);
bool testIdentifier(const parser::IExpression*, const std::string&);
bool testBooleanLiteral(const parser::IExpression*, bool);

TEST(TestParser, TestLetStatement)
{
    using namespace parser;

    struct
    {
        const char* input;
        const char* expectedIdentifier;
        int64_t expectedValue;
    } tests[] = {
        {"let x = 5;", "x", 5},
        {"let y = 10;", "y", 10},
        {"let foobar = 838383;", "foobar", 838383},
    };

    for (auto& tt : tests)
    {
        lexer::Lexer l(tt.input);
        Parser p(l);

        Program* program = p.ParseProgram();

        ASSERT_NE(program, nullptr) << "program is nullptr!";
        ASSERT_EQ(program->GetStatements().size(), 1);

        auto letStmt = dynamic_cast<LetStatement*>(program->GetStatements()[0]);
        ASSERT_NE(letStmt, nullptr) << "stmt is not LetStatement*";

        ASSERT_EQ(letStmt->GetName()->GetValue(), tt.expectedIdentifier);

        if (!testLiteralExpression(letStmt->GetValue(), tt.expectedValue))
            return;

        delete program;
    }
}

TEST(TestParser, TestReturnStatement)
{
    using namespace parser;
    struct
    {
        const char* input;
        int64_t expectedValue;
    } tests[] = {
        {"return 5;", 5},
        {"return 10;", 10},
        {"return 993322;", 993322},
    };

    for (auto& tt : tests)
    {
        lexer::Lexer l(tt.input);
        Parser p(l);

        Program* program = p.ParseProgram();

        ASSERT_NE(program, nullptr) << "program is nullptr!";
        ASSERT_EQ(program->GetStatements().size(), 1);
        auto stmt =
            dynamic_cast<parser::ReturnStatement*>(program->GetStatements()[0]);
        ASSERT_NE(stmt, nullptr) << "stmt is not ReturnStatement";

        if (!testLiteralExpression(stmt->GetReturnValue(), tt.expectedValue))
            return;

        delete program;
    }
}

TEST(TestParser, TestString)
{
    using namespace parser;

    auto* program = new Program();
    auto* letStmt = new LetStatement{};
    letStmt->SetName(new Identifier{"myVar"});
    letStmt->SetValue(new Identifier{"anotherVar"});

    auto* stmt = static_cast<IStatement*>(letStmt);
    program->PushStatement(stmt);

    ASSERT_EQ(program->String(), "let myVar = anotherVar;");

    delete program;
}

TEST(TestParser, TestIndentifierExpression)
{
    using namespace parser;

    const char* input = "foobar;";

    lexer::Lexer l(input);
    Parser p(l);

    Program* program = p.ParseProgram();
    ASSERT_EQ(p.Errors().size(), 0);

    ASSERT_NE(program, nullptr) << "program is nullptr!";
    ASSERT_EQ(program->GetStatements().size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
    ASSERT_NE(stmt, nullptr) << "program->GetStatements()[0] is not"
                             << " ExpressionStatement.";

    auto ident = dynamic_cast<const Identifier*>(stmt->GetExpression());
    ASSERT_NE(ident, nullptr) << "stmt->GetExpression() is not Identifier";

    ASSERT_EQ(ident->GetValue(), "foobar")
        << "ident->GetValue() is not `foobar`. got = " << ident->GetValue();

    delete program;
}

TEST(TestParser, TestIntegerLiteralExpression)
{
    using namespace parser;

    const char* input = "5;";
    lexer::Lexer l{input};
    Parser p{l};

    Program* program = p.ParseProgram();
    ASSERT_EQ(p.Errors().size(), 0);

    ASSERT_NE(program, nullptr) << "program is nullptr!";
    ASSERT_EQ(program->GetStatements().size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
    ASSERT_NE(stmt, nullptr) << "program->GetStatements()[0] is not"
                             << " ExpressionStatement.";

    auto ident = dynamic_cast<const IntegerLiteral*>(stmt->GetExpression());
    ASSERT_NE(ident, nullptr) << "stmt->GetExpression() is not IntegerLiteral";

    ASSERT_EQ(ident->GetValue(), 5)
        << "ident->GetValue() is not 5. got = " << ident->GetValue();

    delete program;
}

TEST(TestParser, TestParsingPrefixExpressions)
{
    using namespace parser;

    struct
    {
        std::string input;
        std::string op;
        std::any rightValue;
    } prefixTests[] = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
        {"!true;", "!", true},
        {"!false;", "!", false},
    };

    for (auto& tt : prefixTests)
    {
        lexer::Lexer l{tt.input};
        Parser p{l};

        Program* program = p.ParseProgram();
        ASSERT_EQ(p.Errors().size(), 0);

        ASSERT_NE(program, nullptr) << "program is nullptr!";
        ASSERT_EQ(program->GetStatements().size(), 1);

        auto stmt =
            dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
        ASSERT_NE(stmt, nullptr)
            << "program->GetStatements()[0] is not ExpressionStatement*";

        auto exp = dynamic_cast<const PrefixExpr*>(stmt->GetExpression());
        ASSERT_NE(exp, nullptr) << "stmt is not PrefixExpr.";

        ASSERT_EQ(exp->GetOperator(), tt.op)
            << "exp->GetOperator() is not " << tt.op
            << " got = " << exp->GetOperator();
        if (!testLiteralExpression(exp->GetRight(), tt.rightValue))
            return;

        delete program;
    }
}

TEST(TestParser, TestParsingInfixExpressions)
{
    using namespace parser;
    struct
    {
        std::string input;
        std::any leftValue;
        std::string op;
        std::any rightValue;
    } infixTests[] = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
        {"true == true", true, "==", true},
        {"true != false", true, "!=", false},
        {"false == false", false, "==", false},
    };

    for (auto& tt : infixTests)
    {
        lexer::Lexer l{tt.input};
        Parser p{l};

        Program* program = p.ParseProgram();
        ASSERT_EQ(p.Errors().size(), 0);

        ASSERT_NE(program, nullptr) << "program is nullptr!";
        ASSERT_EQ(program->GetStatements().size(), 1);

        auto stmt =
            dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
        ASSERT_NE(stmt, nullptr)
            << "program->GetStatements()[0] is not ExpressionStatement*";

        if (!testInfixExpression(stmt->GetExpression(), tt.leftValue, tt.op,
                                 tt.rightValue))
            return;

        delete program;
    }
}

TEST(TestParser, TestOperatorPrecedenceparsing)
{
    using namespace parser;

    struct
    {
        std::string input;
        std::string expected;
    } tests[] = {
        {
            "true",
            "true",
        },
        {
            "false",
            "false",
        },
        {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },
        {
            "-a * b",
            "((-a) * b)",
        },
        {
            "!-a",
            "(!(-a))",
        },
        {
            "a + b + c",
            "((a + b) + c)",
        },
        {
            "a + b - c",
            "((a + b) - c)",
        },
        {
            "a * b * c",
            "((a * b) * c)",
        },
        {
            "a * b / c",
            "((a * b) / c)",
        },
        {
            "a + b / c",
            "(a + (b / c))",
        },
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        {
            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        {
            "!(true == true)",
            "(!(true == true))",
        },
    };

    for (auto& tt : tests)
    {
        lexer::Lexer l{tt.input};
        Parser p{l};
        Program* program = p.ParseProgram();
        DRAIN_ERR_MSG(p);

        ASSERT_NE(program, nullptr) << "program is nullptr!";

        auto actual = program->String();
        ASSERT_EQ(actual, tt.expected);

        delete program;
    }
}

TEST(TestParser, TestIfExpression)
{
    using namespace parser;
    const char* input = "if (x < y) { x }";

    lexer::Lexer l{input};
    Parser p{l};
    Program* program = p.ParseProgram();
    DRAIN_ERR_MSG(p)

    ASSERT_EQ(program->GetStatements().size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
    ASSERT_NE(stmt, nullptr)
        << "program->GetStatements()[0] is not ExpressionStatement.";

    auto exp = dynamic_cast<const IfExpr*>(stmt->GetExpression());
    ASSERT_NE(exp, nullptr) << "stmt->GetExpression() is not IfExpr.";

    if (!testInfixExpression(exp->GetCondition(), "x", "<", "y"))
        return;

    ASSERT_EQ(exp->GetConsequence()->GetStatements().size(), 1)
        << "consequence is not 1 statements. got = "
        << exp->GetConsequence()->GetStatements().size();

    auto consequence = dynamic_cast<const ExpressionStatement*>(
        exp->GetConsequence()->GetStatements()[0]);
    ASSERT_NE(consequence, nullptr);

    if (!testIdentifier(consequence->GetExpression(), "x"))
        return;
    ASSERT_EQ(exp->GetAlternative(), nullptr);

    delete program;
}

TEST(TestParser, TestIfElseExpression)
{
    using namespace parser;
    const char* input = "if (x < y) { x } else { y }";

    lexer::Lexer l{input};
    Parser p{l};
    Program* program = p.ParseProgram();
    DRAIN_ERR_MSG(p)

    ASSERT_EQ(program->GetStatements().size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
    ASSERT_NE(stmt, nullptr)
        << "program->GetStatements()[0] is not ExpressionStatement.";

    auto exp = dynamic_cast<const IfExpr*>(stmt->GetExpression());
    ASSERT_NE(exp, nullptr) << "stmt->GetExpression() is not IfExpr.";

    if (!testInfixExpression(exp->GetCondition(), "x", "<", "y"))
        return;

    ASSERT_EQ(exp->GetConsequence()->GetStatements().size(), 1)
        << "consequence is not 1 statements. got = "
        << exp->GetConsequence()->GetStatements().size();

    auto consequence = dynamic_cast<const ExpressionStatement*>(
        exp->GetConsequence()->GetStatements()[0]);
    ASSERT_NE(consequence, nullptr);
    if (!testIdentifier(consequence->GetExpression(), "x"))
        return;

    ASSERT_EQ(exp->GetAlternative()->GetStatements().size(), 1)
        << "consequence is not 1 statements. got = "
        << exp->GetConsequence()->GetStatements().size();

    auto alternative = dynamic_cast<const ExpressionStatement*>(
        exp->GetAlternative()->GetStatements()[0]);
    ASSERT_NE(alternative, nullptr);
    if (!testIdentifier(alternative->GetExpression(), "y"))
        return;

    delete program;
}

TEST(TestParser, TestFunctionLiteralParsing)
{
    using namespace parser;
    const char* input = "fn(x, y) { x + y; }";

    lexer::Lexer l{input};
    Parser p{l};
    Program* program = p.ParseProgram();
    DRAIN_ERR_MSG(p)

    ASSERT_EQ(program->GetStatements().size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
    ASSERT_NE(stmt, nullptr)
        << "program->GetStatements()[0] is not ExpressionStatement.";

    auto exp = dynamic_cast<const FunctionLiteral*>(stmt->GetExpression());
    ASSERT_NE(exp, nullptr) << "stmt->GetExpression() is not FunctionLiteral.";

    ASSERT_EQ(exp->GetParameters().size(), 2);

    testLiteralExpression(exp->GetParameters()[0], "x");
    testLiteralExpression(exp->GetParameters()[1], "y");

    ASSERT_EQ(exp->GetBody()->GetStatements().size(), 1);

    auto bodyStmt = dynamic_cast<const ExpressionStatement*>(
        exp->GetBody()->GetStatements()[0]);
    ASSERT_NE(bodyStmt, nullptr);

    testInfixExpression(bodyStmt->GetExpression(), "x", "+", "y");

    delete program;
}

TEST(TestParser, TestFunctionParameterParsing)
{
    using namespace parser;

    struct
    {
        std::string input;
        std::vector<std::string> expectParams;
    } tests[] = {
        {"fn () {};", {}},
        {"fn (x) {};", {"x"}},
        {"fn (x, y, z) {};", {"x", "y", "z"}},
    };

    for (auto& tt : tests)
    {
        lexer::Lexer l{tt.input};
        Parser p{l};
        Program* program = p.ParseProgram();
        DRAIN_ERR_MSG(p)

        auto stmt =
            dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
        auto function =
            dynamic_cast<const FunctionLiteral*>(stmt->GetExpression());

        ASSERT_EQ(function->GetParameters().size(), tt.expectParams.size());

        for (size_t i = 0; i < tt.expectParams.size(); ++i)
        {
            testLiteralExpression(function->GetParameters()[i],
                                  tt.expectParams[i]);
        }

        delete program;
    }
}

TEST(TestParser, TestCallExpressionParsing)
{
    using namespace parser;
    const char* input = "add(1, 2 * 3, 4 + 5);";

    lexer::Lexer l{input};
    Parser p{l};
    auto program = p.ParseProgram();
    DRAIN_ERR_MSG(p)

    ASSERT_EQ(program->GetStatements().size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->GetStatements()[0]);
    ASSERT_NE(stmt, nullptr);

    auto expr = dynamic_cast<const CallExpr*>(stmt->GetExpression());
    ASSERT_NE(expr, nullptr);

    testIdentifier(expr->GetFunction(), "add");
    ASSERT_EQ(expr->GetArguments().size(), 3);

    testLiteralExpression(expr->GetArguments()[0], 1);
    testInfixExpression(expr->GetArguments()[1], 2, "*", 3);
    testInfixExpression(expr->GetArguments()[2], 4, "+", 5);
}

bool testLiteralExpression(const parser::IExpression* exp, std::any expected)
{
    if IS_TYPES_EQ (expected, int)
    {
        return testIntegerLiteral(exp, (int64_t)std::any_cast<int>(expected));
    }
    if IS_TYPES_EQ (expected, int64_t)
    {
        return testIntegerLiteral(exp, std::any_cast<int64_t>(expected));
    }
    if IS_TYPES_EQ (expected, std::string)
    {
        return testIdentifier(exp, std::any_cast<std::string>(expected));
    }
    if IS_TYPES_EQ (expected, const char*)
    {
        return testIdentifier(exp, std::any_cast<const char*>(expected));
    }
    if IS_TYPES_EQ (expected, bool)
    {
        return testBooleanLiteral(exp, std::any_cast<bool>(expected));
    }
    std::cerr << "type of exp not handled.";
    return false;
}

template <typename T, typename U>
bool testInfixExpression(const parser::IExpression* exp, T left,
                         const std::string& op, U right)
{
    auto opExp = dynamic_cast<const parser::InfixExpr*>(exp);
    if (!opExp)
    {
        std::cerr << "exp is not InfixExpr*.";
        return false;
    }

    if (!testLiteralExpression(opExp->GetLeft(), left))
    {
        return false;
    }
    if (opExp->GetOperator() != op)
    {
        std::cerr << "exp->GetOperator() is not " << op
                  << ". got = " << opExp->GetOperator();
        return false;
    }
    if (!testLiteralExpression(opExp->GetRight(), right))
    {
        return false;
    }

    return true;
}

bool testIntegerLiteral(const parser::IExpression* il, int64_t value)
{
    using namespace parser;

    const auto integ = dynamic_cast<const IntegerLiteral*>(il);
    if (!integ)
    {
        std::cerr << "il is not IntegerLiteral\n";
        return false;
    }
    if (integ->GetValue() != value)
    {
        std::cerr << "integ->GetValue() not " << value
                  << " got = " << integ->GetValue();
        return false;
    }

    return true;
}

bool testIdentifier(const parser::IExpression* exp, const std::string& value)
{
    using namespace parser;
    auto ident = dynamic_cast<const Identifier*>(exp);
    if (!ident)
    {
        std::cerr << "exp not Identifier*.";
        return false;
    }
    if (ident->GetValue() != value)
    {
        std::cerr << "ident->GetValue() not " << value
                  << ". got = " << ident->GetValue();
        return false;
    }

    return true;
}

bool testBooleanLiteral(const parser::IExpression* exp, bool value)
{
    using namespace parser;
    auto bo = dynamic_cast<const Boolean*>(exp);
    if (!bo)
    {
        std::cerr << "exp not boolean";
        return false;
    }
    if (bo->GetValue() != value)
    {
        std::cerr << "bo->GetValue() not " << std::boolalpha << value
                  << ". got = " << bo->GetValue();
        return false;
    }

    return true;
}
