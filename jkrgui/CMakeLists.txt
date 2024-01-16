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
	add_library(jkrgui SHARED ${app_SRC}
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
	 "WindowMulT.cpp" "Renderers/ResourceManager.cpp" "Renderers/CustomImagePainter.cpp")
 endif()


 add_custom_command(
	TARGET jkrgui POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_BINARY_DIR}/libjkrgui.so
	${CMAKE_SOURCE_DIR}/libs/Android/libjkrgui.so
)

