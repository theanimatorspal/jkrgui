# Define a function to set up the target based on the build type
function(configure_target TARGET_NAME)
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_link_libraries(${TARGET_NAME}
            ${Vulkan_LIBRARIES}
            ksaivulkan 
            jkrgui 
            SDL2
            SPIRV
            MachineIndependent
            SPIRV-Tools
            SPIRV-Tools-diff
            SPIRV-Tools-link
            SPIRV-Tools-lint
            SPIRV-Tools-opt
            SPIRV-Tools-reduce
            SPIRV-Tools-shared
            OSDependent
            GenericCodeGen
            OGLCompiler
            HLSL
            glslang-default-resource-limits
            spirv-cross-c-shared
            spirv-cross-c
            spirv-cross-core
            spirv-cross-cpp
            spirv-cross-glsl
            spirv-cross-hlsl
            spirv-cross-msl
            spirv-cross-reflect
            lua54
            freetype
            harfbuzz
            harfbuzz-subset
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${TARGET_NAME}
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
    endif()
endfunction()

function(configure_llvm TARGET_NAME)
        target_link_libraries(${TARGET_NAME} libclang liblldb libomp LLVM-C LTO Remarks)
endfunction()
