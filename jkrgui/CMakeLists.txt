file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

add_library(jkrgui ${app_SRC}  "Renderers/TwoD/2d.cpp" "Renderers/TextD/BestText.cpp" "Components/Button.cpp" "Components/GridSheet.cpp" "Components/FastTextButton.cpp")

