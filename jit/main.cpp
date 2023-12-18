#include <fstream>
#ifdef WIN32
#include <Windows.h>
#undef max
#endif
#include "jitcompilerfromir.hpp"
#include <sstream>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT double putchard(double X)
{
    fputc((char) X, stderr);
    return 0;
}

/* LEXER */
int main()
{
    std::ifstream File("main.lua");
    std::stringstream Stream;
    Stream << File.rdbuf();
    JitCompiler Generator(Stream);
    Generator.MainLoop();
}
