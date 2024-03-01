file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS
    "*.hpp"
    "*.cpp"
    "*.h"
    "*.c"
)


add_library(lua STATIC ${app_SRC})
