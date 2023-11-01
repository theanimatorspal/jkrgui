file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)


add_library(jkrgui ${app_SRC} "Renderers/FastText.hpp" "Renderers/FastText.cpp")