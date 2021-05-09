#include <stdio.h>

#ifdef __clang__
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "repl.h"

#define PROMPT ">> "
#endif

void startREPL(void)
{
#ifdef __clang__
    using_history();

    char* line = NULL;
    Lexer* l = NULL;
    Token tok = INIT_TOKEN;

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
#else
    printf("ERROR: Compile this with clang compiler.\n"
           "because gcc cause an error linking readline library.\n");
#endif
}
