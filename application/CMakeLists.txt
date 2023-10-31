file(GLOB app_SRC CONFIGURE_DEPENDS 
"*.hpp"
"*.cpp"
"*.h"
"*.c"
"NodeEditor/*.hpp"
"NodeEditor/*.cpp"
"NodeEditor/*.h"
"NodeEditor/*.c"
)

message(STATUS "${app_SRC}")

add_executable(application ${app_SRC})

target_link_libraries(application 
	${Vulkan_LIBRARIES}
	ksaivulkan 
	jkrgui 
	SDL2
	lua54
	SPIRVd
	MachineIndependentd
	SPIRV-Toolsd
	SPIRV-Tools-diffd
	SPIRV-Tools-linkd
	SPIRV-Tools-lintd
	SPIRV-Tools-optd
	SPIRV-Tools-reduce
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
)

#	harfbuzz
#	harfbuzz-subset
#	freetype
# Add source to this project's executable.


# TODO: Add tests and install targets if needed.


