include("../CMakeCommon.cmake")
file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS
"*.hpp"
"*.cpp"
"*.h"
"*.c"
"*.cc"
)

add_executable(jit ${app_SRC}
LuaBirali/confbirali.hpp
LuaBirali/lexerbirali.hpp
LuaBirali/parserbirali.hpp
LuaBirali/lexerbirali.cpp
LuaBirali/astbirali.hpp
LuaBirali/jithelper.hpp
LuaBirali/jithelper.cpp
LuaBirali/astbirali.cpp
LuaBirali/irbirali.hpp
LuaBirali/irbirali.cpp
LuaBirali/parserbirali.cpp
LuaBirali/main.cpp
LuaBirali/jitbirali.cpp)

CONFIGURE_LLVM(jit)
configure_target(jit)

