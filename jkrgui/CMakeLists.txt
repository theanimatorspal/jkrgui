include("../CMakeCommon.cmake")

file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

file(GLOB_RECURSE app_Precompile_SRC CONFIGURE_DEPENDS 
"*.hpp"
)

if(ANDROID)
	add_library(jkrgui STATIC ${app_SRC}
	"Renderers/TextD/BestText_base.cpp"
	"Renderers/TextD/BestText_base.hpp"
	"Renderers/TextD/BestText.cpp"
	"Renderers/TextD/BestText.hpp"
	"Renderers/ThreeD/glTF.hpp"
	"Renderers/ThreeD/glTF.cpp"
	"Renderers/ThreeD/glTF_base.hpp"
	"Renderers/ThreeD/glTF_base.cpp"
	"Renderers/BestText_Alt.hpp"
	"WindowMulT.hpp"
	"Window.hpp"
	 "WindowMulT.cpp" "Renderers/ResourceManager.cpp" "Renderers/CustomImagePainter.cpp")
	 target_link_libraries(jkrgui ksaivulkan)
	 configure_target(jkrgui)
else()
	add_library(jkrgui STATIC ${app_SRC}
	"Renderers/TextD/BestText_base.cpp"
	"Renderers/TextD/BestText_base.hpp"
	"Renderers/TextD/BestText.cpp"
	"Renderers/TextD/BestText.hpp"
	"Renderers/ThreeD/glTF.hpp"
	"Renderers/ThreeD/glTF.cpp"
	"Renderers/ThreeD/glTF_base.hpp"
	"Renderers/ThreeD/glTF_base.cpp"
	"Renderers/BestText_Alt.hpp"
	"WindowMulT.hpp"
	"Window.hpp"
	 "WindowMulT.cpp" "Renderers/ResourceManager.cpp" "Renderers/CustomImagePainter.cpp" "SDLWindow.hpp" "SDLWindow.cpp" "Misc/MousePicker.cpp" "Misc/MousePicker.hpp" "Renderers/BestText_Alt.cpp")
 endif()

 someFlags(jkrgui)
