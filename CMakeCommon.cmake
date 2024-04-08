# Define a function to set up the target based on the build type
if(APPLE)
function(configure_target TARGET_NAME)
        target_link_libraries(${TARGET_NAME}
            ${Vulkan_LIBRARIES}
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
            glslang-default-resource-limits
            spirv-cross-c-shared
            spirv-cross-c
            spirv-cross-core
            spirv-cross-cpp
            spirv-cross-glsl
            spirv-cross-hlsl
            spirv-cross-msl
            spirv-cross-reflect
            lua
            freetype
            harfbuzz
            harfbuzz-subset
        )
endfunction()

elseif(ANDROID)

function(configure_target TARGET_NAME)
   target_link_libraries(${TARGET_NAME}
	  ${Vulkan_LIBRARIES}
	  android
	  log
	  brotlicommon-static
		brotlidec-static
		brotlienc-static
		bz2
		freetype
		GenericCodeGen
		glslang-default-resource-limits
		glslang
		harfbuzz-subset
		harfbuzz
		lua
		MachineIndependent
		OSDependent
		png
		png16
		SDL2
		spirv-cross-c
		spirv-cross-core
		spirv-cross-cpp
		spirv-cross-glsl
		spirv-cross-hlsl
		spirv-cross-msl
		spirv-cross-reflect
		spirv-cross-util
		SPIRV-Tools-diff
		SPIRV-Tools-link
		SPIRV-Tools-lint
		SPIRV-Tools-opt
		SPIRV-Tools-reduce
		SPIRV-Tools
		SPIRV
		SPVRemapper
		z
   )
endfunction()


else()


function(configure_target TARGET_NAME)
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_link_libraries(${TARGET_NAME}
            ${Vulkan_LIBRARIES}
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
            glslang-default-resource-limits
            spirv-cross-c-shared
            spirv-cross-c
            spirv-cross-core
            spirv-cross-cpp
            spirv-cross-glsl
            spirv-cross-hlsl
            spirv-cross-msl
            spirv-cross-reflect
            lua
            freetype
            harfbuzz
            harfbuzz-subset
            ws2_32
            wsock32
            dbghelp
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${TARGET_NAME}
            ${Vulkan_LIBRARIES}
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
            glslang-default-resource-limitsd
            spirv-cross-c-sharedd
            spirv-cross-cd
            spirv-cross-cored
            spirv-cross-cppd
            spirv-cross-glsld
            spirv-cross-hlsld
            spirv-cross-msld
            spirv-cross-reflectd
            lua
            freetype
            harfbuzz
            harfbuzz-subset
            ws2_32
            wsock32
            dbghelp
        )
    endif()
endfunction()

endif()





if(APPLE)
elseif(DONTDOTHIS)
function(configure_llvm TARGET_NAME)
        target_link_libraries(${TARGET_NAME}
            LLVMWindowsManifest
            LLVMXRay
            LLVMLibDriver
            LLVMDlltoolDriver
            LLVMCoverage
            LLVMLineEditor
            LLVMXCoreDisassembler
            LLVMXCoreCodeGen
            LLVMXCoreDesc
            LLVMXCoreInfo
            LLVMX86TargetMCA
            LLVMX86Disassembler
            LLVMX86AsmParser
            LLVMX86CodeGen
            LLVMX86Desc
            LLVMX86Info
            LLVMWebAssemblyDisassembler
            LLVMWebAssemblyAsmParser
            LLVMWebAssemblyCodeGen
            LLVMWebAssemblyUtils
            LLVMWebAssemblyDesc
            LLVMWebAssemblyInfo
            LLVMVEDisassembler
            LLVMVEAsmParser
            LLVMVECodeGen
            LLVMVEDesc
            LLVMVEInfo
            LLVMSystemZDisassembler
            LLVMSystemZAsmParser
            LLVMSystemZCodeGen
            LLVMSystemZDesc
            LLVMSystemZInfo
            LLVMSparcDisassembler
            LLVMSparcAsmParser
            LLVMSparcCodeGen
            LLVMSparcDesc
            LLVMSparcInfo
            LLVMRISCVTargetMCA
            LLVMRISCVDisassembler
            LLVMRISCVAsmParser
            LLVMRISCVCodeGen
            LLVMRISCVDesc
            LLVMRISCVInfo
            LLVMPowerPCDisassembler
            LLVMPowerPCAsmParser
            LLVMPowerPCCodeGen
            LLVMPowerPCDesc
            LLVMPowerPCInfo
            LLVMNVPTXCodeGen
            LLVMNVPTXDesc
            LLVMNVPTXInfo
            LLVMMSP430Disassembler
            LLVMMSP430AsmParser
            LLVMMSP430CodeGen
            LLVMMSP430Desc
            LLVMMSP430Info
            LLVMMipsDisassembler
            LLVMMipsAsmParser
            LLVMMipsCodeGen
            LLVMMipsDesc
            LLVMMipsInfo
            LLVMLoongArchDisassembler
            LLVMLoongArchAsmParser
            LLVMLoongArchCodeGen
            LLVMLoongArchDesc
            LLVMLoongArchInfo
            LLVMLanaiDisassembler
            LLVMLanaiCodeGen
            LLVMLanaiAsmParser
            LLVMLanaiDesc
            LLVMLanaiInfo
            LLVMHexagonDisassembler
            LLVMHexagonCodeGen
            LLVMHexagonAsmParser
            LLVMHexagonDesc
            LLVMHexagonInfo
            LLVMBPFDisassembler
            LLVMBPFAsmParser
            LLVMBPFCodeGen
            LLVMBPFDesc
            LLVMBPFInfo
            LLVMAVRDisassembler
            LLVMAVRAsmParser
            LLVMAVRCodeGen
            LLVMAVRDesc
            LLVMAVRInfo
            LLVMARMDisassembler
            LLVMARMAsmParser
            LLVMARMCodeGen
            LLVMARMDesc
            LLVMARMUtils
            LLVMARMInfo
            LLVMAMDGPUTargetMCA
            LLVMAMDGPUDisassembler
            LLVMAMDGPUAsmParser
            LLVMAMDGPUCodeGen
            LLVMAMDGPUDesc
            LLVMAMDGPUUtils
            LLVMAMDGPUInfo
            LLVMAArch64Disassembler
            LLVMAArch64AsmParser
            LLVMAArch64CodeGen
            LLVMAArch64Desc
            LLVMAArch64Utils
            LLVMAArch64Info
            LLVMOrcJIT
            LLVMWindowsDriver
            LLVMMCJIT
            LLVMJITLink
            LLVMInterpreter
            LLVMExecutionEngine
            LLVMRuntimeDyld
            LLVMOrcTargetProcess
            LLVMOrcShared
            LLVMDWP
            LLVMDebugInfoLogicalView
            LLVMDebugInfoGSYM
            LLVMOption
            LLVMObjectYAML
            LLVMObjCopy
            LLVMMCA
            LLVMMCDisassembler
            LLVMLTO
            LLVMPasses
            LLVMCFGuard
            LLVMCoroutines
            LLVMipo
            LLVMVectorize
            LLVMLinker
            LLVMInstrumentation
            LLVMFrontendOpenMP
            LLVMFrontendOpenACC
            LLVMFrontendHLSL
            LLVMExtensions
            LLVMDWARFLinkerParallel
            LLVMDWARFLinker
            LLVMGlobalISel
            LLVMMIRParser
            LLVMAsmPrinter
            LLVMSelectionDAG
            LLVMCodeGen
            LLVMTarget
            LLVMObjCARCOpts
            LLVMCodeGenTypes
            LLVMIRPrinter
            LLVMInterfaceStub
            LLVMFileCheck
            LLVMFuzzMutate
            LLVMScalarOpts
            LLVMInstCombine
            LLVMAggressiveInstCombine
            LLVMTransformUtils
            LLVMBitWriter
            LLVMAnalysis
            LLVMProfileData
            LLVMSymbolize
            LLVMDebugInfoBTF
            LLVMDebugInfoPDB
            LLVMDebugInfoMSF
            LLVMDebugInfoDWARF
            LLVMObject
            LLVMTextAPI
            LLVMMCParser
            LLVMIRReader
            LLVMAsmParser
            LLVMMC
            LLVMDebugInfoCodeView
            LLVMBitReader
            LLVMFuzzerCLI
            LLVMCore
            LLVMRemarks
            LLVMBitstreamReader
            LLVMBinaryFormat
            LLVMTargetParser
            LLVMTableGen
            LLVMSupport
            LLVMDemangle
        )
endfunction()
endif()


function(copyToDestination cpyName)
  if(ANDROID)
    # add_custom_command(TARGET ${cpyName} POST_BUILD
    #   COMMAND ${CMAKE_COMMAND} -E copy
    #   ${CMAKE_CURRENT_BINARY_DIR}/lib${cpyName}.so
    #   ${CMAKE_SOURCE_DIR}/libs/Android/lib${cpyName}.so
    # )
    # add_custom_command(TARGET ${cpyName} POST_BUILD
    #   COMMAND ${CMAKE_COMMAND} -E copy
    #   ${CMAKE_CURRENT_BINARY_DIR}/lib${cpyName}.so
    #   ${CMAKE_SOURCE_DIR}/SDL4droid/aJkrLua_app/app/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/lib${cpyName}.so
    # )

    # add_custom_command(TARGET ${cpyName} POST_BUILD
    #   COMMAND ${CMAKE_COMMAND} -E copy
    #   ${CMAKE_SOURCE_DIR}/libs/Android/liblua.so
    #   ${CMAKE_SOURCE_DIR}/SDL4droid/aJkrLua_app/app/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/liblua.so
    # )
  endif()
endfunction()

function(someFlags inName)
     set_property(TARGET ${inName} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endfunction()

