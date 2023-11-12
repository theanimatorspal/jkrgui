file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

file(GLOB_RECURSE app_pre_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.h"
)


add_library(ksaivulkan ${app_SRC})

