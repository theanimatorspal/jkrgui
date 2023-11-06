file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

file(GLOB_RECURSE app_Precompile_SRC CONFIGURE_DEPENDS 
"*.hpp"
)

add_library(jkrgui ${app_SRC}  "Renderers/TwoD/2d.cpp" "Renderers/TextD/BestText.cpp" "Components/Button.cpp" "Components/GridSheet.cpp" "Components/FastTextButton.cpp" "Components/ContextMenu.cpp" "Components/ScrollableProxy.cpp" "Components/ScrollableProxy.hpp" "Components/AreaProxy.cpp")

