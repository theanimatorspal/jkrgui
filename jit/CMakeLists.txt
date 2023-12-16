include("../CMakeCommon.cmake")
file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS
"*.hpp"
"*.cpp"
"*.h"
"*.c"
"*.cc"
)

add_executable(jit ${app_SRC} main.cpp
    lexer.hpp lexer.cpp
    parser.hpp parser.cpp
    irgenerator.hpp irgenerator.cpp
    expressions.cpp
    expressions.hpp
         jit.cpp )

CONFIGURE_LLVM(jit)
configure_target(jit)
