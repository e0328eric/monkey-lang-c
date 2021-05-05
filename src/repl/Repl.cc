#include <cstdlib>
#include <cstring>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
#include <string>

#include "Repl.hh"
#include "lexer/Lexer.hh"
#include "lexer/Token.hh"
#include "parser/Parser.hh"

void Repl(std::ostream& output)
{
    char* line = nullptr;
    parser::Program* program = nullptr;

    using_history();

    while (true)
    {
        line = readline(PROMPT);
        if (!line || strcmp(line, ":quit") == 0)
        {
            free(line);
            break;
        }

        add_history(line);

        lexer::Lexer l{line};
        parser::Parser p{l};
        program = p.ParseProgram();

        if (!p.Errors().empty())
        {
            printParseErrors(output, p.Errors());
            delete program;
            free(line);
            line = nullptr;
            continue;
        }

        output << program->String() << std::endl;

        delete program;
        free(line);
        line = nullptr;
    }
}

void printParseErrors(std::ostream& output,
                      const std::vector<std::string>& msgs)
{
    for (auto& msg : msgs)
        output << "\t" + msg + "\n";
}
