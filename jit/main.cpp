#ifdef WIN32
#include <Windows.h>
#undef max
#endif
#include "parser.hpp"
#include <sstream>

/* LEXER */
int main()
{
    std::stringstream Stream;
    Stream << "function foo(x,y)  x + foo(y, 4.0);";
    Parser parser(Stream);
    parser.MainLoop();
}
