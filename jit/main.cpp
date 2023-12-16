#ifdef WIN32
#include <Windows.h>
#undef max
#endif
#include "jitcompilerfromir.hpp"
#include <sstream>

/* LEXER */
int main()
{
    std::stringstream Stream;
    Stream << R"""(
function plus1(x) x + 1;
plus1(2);
plus1(5);
)""";
    JitCompiler Generator(Stream);
    Generator.MainLoop();
}
