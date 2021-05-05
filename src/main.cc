#include <iostream>
#include <string>

#include "repl/Repl.hh"

#define __DEBUG__

int main()
{
    std::string get;

    std::cout << "Hello mrnugget! This is the Monkey Programming ";
    std::cout << "Language!\n Feel free to type in commands\n";

    Repl(std::cout);

#ifdef __DEBUG__
    system("leaks monkey > leaks_result; cat leaks_result | grep leaked && rm "
           "-rf leaks_result");
#endif

    return 0;
}
