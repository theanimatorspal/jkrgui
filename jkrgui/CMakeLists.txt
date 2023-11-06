file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

file(GLOB_RECURSE app_Precompile_SRC CONFIGURE_DEPENDS 
"*.hpp"
)

add_library(jkrgui ${app_SRC} "Components/Button_base.cpp" "Components/ButtonRect_base.hpp" )

