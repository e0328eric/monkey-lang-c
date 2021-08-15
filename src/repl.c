#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linenoise.h>

#include "lexer.h"
#include "parser.h"
#include "repl.h"

#define PROMPT     ">> "
#define FMT_RED    "\x1b[1m\x1b[91m"
#define FMT_NORMAL "\x1b[0m"

void printParserErrors(String**);

void startREPL(void)
{
    char* line        = NULL;
    Lexer* l          = NULL;
    Parser* p         = NULL;
    Program* program  = NULL;
    String* stringify = NULL;

    linenoiseHistorySetMaxLen(15);

    fprintf(stdout, "[ Monkey Language REPL ]\n");
    fprintf(stdout, "Press Ctrl+D or type :q to quit the REPL\n");
    fprintf(stdout, "-------------------------------\n\n");

    while (1)
    {
        line = linenoise(PROMPT);

        if (!line || strcmp(line, ":q") == 0)
        {
            linenoiseFree(line);
            break;
        }

        linenoiseHistoryAdd(line);

        l       = mkLexer(line);
        p       = mkParser(l);
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
        linenoiseFree(line);
    }
}

void printParserErrors(String** errors)
{
    for (int i = 0; errors[i]; ++i)
    {
        printf(FMT_RED "[ERROR]: %s" FMT_NORMAL "\n\n", getStr(errors[i]));
        freeString(errors[i]);
    }
    free(errors);
}
