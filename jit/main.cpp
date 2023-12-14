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
    Stream << R"""(

function foo(x y)  x + foo(y, 4.0);
function foo(x y) x+y y;
function foo(x y) x + y );
external sin(a);
)""";
    Parser parser(Stream);
    parser.MainLoop();
}
