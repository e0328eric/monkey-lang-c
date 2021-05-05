#ifndef MONKEY_LANG_CPP_SRC_REPL_REPL_H_
#define MONKEY_LANG_CPP_SRC_REPL_REPL_H_

#include <iostream>
#include <string>
#include <vector>

static const char* PROMPT = ">> ";

void Repl(std::ostream&);
void printParseErrors(std::ostream& output,
                      const std::vector<std::string>& msgs);

#endif // MONKEY_LANG_CPP_SRC_REPL_REPL_H_
