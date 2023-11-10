file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
)

set(CMAKE_BUILD_TYPE Debug)

link_directories(${VULKAN_LIBRARY_PATH})


add_executable(aa_unit_testing ${app_SRC} "testmain.cpp")

target_link_libraries(aa_unit_testing 
	${Vulkan_LIBRARIES}
	ksaivulkan 
	jkrgui 
	SDL2
	SPIRVd
	MachineIndependentd
	SPIRV-Toolsd
	SPIRV-Tools-diffd
	SPIRV-Tools-linkd
	SPIRV-Tools-lintd
	SPIRV-Tools-optd
	SPIRV-Tools-reduced
	SPIRV-Tools-sharedd
	OSDependentd
	GenericCodeGend
	OGLCompilerd
	HLSLd
	glslang-default-resource-limitsd
	spirv-cross-c-sharedd
	spirv-cross-cd
	spirv-cross-cored
	spirv-cross-cppd
	spirv-cross-glsld
	spirv-cross-hlsld
	spirv-cross-msld
	spirv-cross-reflectd
	lua54
	freetype
	harfbuzz
	harfbuzz-subset
)
