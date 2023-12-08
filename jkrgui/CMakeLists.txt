file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

file(GLOB_RECURSE app_Precompile_SRC CONFIGURE_DEPENDS 
"*.hpp"
)

add_library(jkrgui STATIC ${app_SRC} 
"Components/Button_base.cpp"
"Components/ButtonRect_base.hpp"
"Components/Scrollable_base.hpp"
"Components/ScrollableRect.hpp"
"Components/HLayout.hpp"
"Components/VLayout.hpp"
"Components/FTextButtonRect.hpp"
"Components/BTextButtonRect.hpp"
"Components/ContextMenuList.hpp"
"Components/ContextMenuList.cpp"
"Components/TextLineEdit.hpp"
"Components/ImageButtonRect.hpp"
"Components/FocusProxy.hpp"
"Renderers/TextD/BestText_base.cpp"
"Renderers/TextD/BestText_base.hpp"
"Renderers/TextD/BestText.cpp"
"Renderers/TextD/BestText.hpp"
"Components/TextLineEdit.cpp"
"Renderers/ThreeD/glTF.hpp"
"Renderers/ThreeD/glTF.cpp"
"Renderers/ThreeD/glTF_base.hpp"
"Renderers/ThreeD/glTF_base.cpp"
)

