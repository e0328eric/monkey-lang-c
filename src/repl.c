#include <stdio.h>

#ifdef __clang__
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "repl.h"

#define PROMPT ">> "
#define RED    "\x1b[1m\x1b[91m"
#endif

void printParserErrors(String**);

void startREPL(void)
{
#ifdef __clang__
    using_history();

    char* line = NULL;
    Lexer* l = NULL;
    Parser* p = NULL;
    Program* program = NULL;
    String* stringify = NULL;

    while (1)
    {
        line = readline(PROMPT);

        if (!line || strcmp(line, ":quit") == 0)
        {
            free(line);
            break;
        }

        add_history(line);

        l = mkLexer(line);
        p = mkParser(l);
        program = parseProgram(p);

        if (getErrLen(p) != 0)
        {
            printParserErrors(getErrors(p));
            continue;
        }

        stringify = stringifyProgram(program);
        printf("%s\n", getStr(stringify));

        freeString(stringify);
        freeProgram(program);
        freeParser(p);
        free(line);
    }
#else
    printf("ERROR: Compile this with clang compiler.\n"
           "because gcc cause an error linking readline library.\n");
#endif
}

void printParserErrors(String** errors)
{
    for (int i = 0; errors[i]; ++i)
    {
        printf(RED "[ERROR]: %s\n\x1b[0m", getStr(errors[i]));
        freeString(errors[i]);
    }
    free(errors);
}
