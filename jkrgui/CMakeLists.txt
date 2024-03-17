include("../CMakeCommon.cmake")

file(GLOB_RECURSE app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

if(ANDROID)
	add_library(jkrgui STATIC ${app_SRC}
	"Renderers/TextD/BestText_base.cpp"
	"Renderers/TextD/BestText_base.hpp"
	"Renderers/TextD/BestText.cpp"
	"Renderers/TextD/BestText.hpp"
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
	# "Renderers/TextD/BestText_base.cpp"
	# "Renderers/TextD/BestText_base.hpp"
	# "Renderers/TextD/BestText.cpp"
	# "Renderers/TextD/BestText.hpp"
	# "Renderers/ThreeD/glTF_base.hpp"
	# "Renderers/ThreeD/glTF_base.cpp"
	# "Renderers/BestText_Alt.hpp"
	# "WindowMulT.hpp"
	# "Window.hpp"
	#  "WindowMulT.cpp" "Renderers/ResourceManager.cpp" "Renderers/CustomImagePainter.cpp" "SDLWindow.hpp" "SDLWindow.cpp" "Misc/FrameCapturer.cpp" "Misc/FrameCapturer.hpp" "Renderers/BestText_Alt.cpp"
	#  "TrivialShadersLegacy.hpp"
	#  "Audio/AudioDevice.cpp" "Audio/FourierAudioGenerator.cpp"
	#  "Audio/AudioDevice.hpp" "Audio/FourierAudioGenerator.hpp"
	#   "Neural/SimpleNN.cpp" "Neural/ConvolutionalNN.cpp" "Renderers/TwoD/ShapeExt.cpp" "Misc/GPUNeural.cpp" "Misc/GPUNeural.hpp")
	)
 endif()

 someFlags(jkrgui)
