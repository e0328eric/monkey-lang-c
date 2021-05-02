#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "repl.h"

#define PROMPT ">> "

void startREPL(void)
{
    char* line = (char*)NULL;
    Lexer* l = (Lexer*)NULL;
    Token tok = (Token){.type = -2, .literal = NULL};

    using_history();

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
        do
        {
            tok = nextToken(l);
            printf("{ %s : %s }\n", printTokType(tok.type),
                   getStr(tok.literal));
            freeToken(&tok);
        } while (tok.type != T_EOF);

        freeLexer(l);
        free(line);
    }
}
