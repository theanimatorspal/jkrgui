include("../CMakeCommon.cmake")
file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS
"*.hpp"
"*.cpp"
"*.h"
"*.c"
"*.cc"
)

add_executable(jit ${app_SRC} LuaBirali/lexer.hpp
"LuaBirali/main.cpp" 
"LuaBirali/parser.hpp"
"LuaBirali/include.hpp"
"LuaBirali/lua.hpp"
"LuaBirali/ir.hpp") 

CONFIGURE_LLVM(jit)
configure_target(jit)

