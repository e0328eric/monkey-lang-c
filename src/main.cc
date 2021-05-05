#include <iostream>
#include <string>

#include "repl/Repl.hh"

int main()
{
    std::string get;

    std::cout << "Hello mrnugget! This is the Monkey Programming ";
    std::cout << "Language!\n Feel free to type in commands\n";

    Repl(std::cout);

    return 0;
}
