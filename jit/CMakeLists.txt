include("../CMakeCommon.cmake")
file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS
"*.hpp"
"*.cpp"
"*.h"
"*.c"
"*.cc"
)

add_executable(jit ${app_SRC}
"LuaBirali/lexer/lexer.hpp"
"LuaBirali/lexer/operators.hpp"
"LuaBirali/lexer/token.hpp"

"LuaBirali/codegen/ast_primitives.hpp"
"LuaBirali/codegen/ast_primitives.cpp"
"LuaBirali/codegen/ir_base.hpp"
"LuaBirali/codegen/jit_helper.hpp"
"LuaBirali/codegen/parser.hpp"
"LuaBirali/codegen/parser.cpp"


"LuaBirali/lua.hpp"
"LuaBirali/include.hpp"
"LuaBirali/main.cpp"
 "LuaBirali/codegen/parser.cpp") 

CONFIGURE_LLVM(jit)
configure_target(jit)

