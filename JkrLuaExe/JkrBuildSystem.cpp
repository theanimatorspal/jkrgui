#include "JkrLuaExe.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <sol/sol.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

const string_view DefaultCMakeListsFile = R"CmakeListsFile(
cmake_minimum_required(VERSION 3.27)
include("{0}/out/build/{1}/luaExport.cmake")
include("{0}/out/build/{1}/ksaivulkanExport.cmake")
include("{0}/out/build/{1}/jkrguiExport.cmake")

project({2})
add_library({2} SHARED {2}.cpp)
target_link_libraries({2} lua ksaivulkan jkrgui)

add_custom_command(
    TARGET {2} POST_BUILD
    COMMAND ${{CMAKE_COMMAND}} -E copy
            $<TARGET_FILE:{2}>
            ${{CMAKE_CURRENT_SOURCE_DIR}}/../
)

include_directories({0}/application)
include_directories({0}/jkrgui)
include_directories({0}/ksaivulkan)
include_directories({0}/ksaivulkan/Vulkan)
include_directories({0}/ksaivulkan/Vendor)
include_directories({0}/ksaivulkan/Vendor/stbi)
include_directories({0}/libs/)
include_directories({0}/aJkrLua)
include_directories({0}/vendor/lua)

if(APPLE)
elseif(ANDROID)
    include_directories("{0}/libs/AndroidInclude/SDL2")
else()
    include_directories({0}/ksaivulkan/Vendor/Tracy/tracy)
endif()

)CmakeListsFile";

const string_view DefaultLuaLibraryFile = R"LuaLibraryString(
#include <lua.h>
#include <lauxlib.h>

#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT
#endif

extern "C" DLLEXPORT int luaopen_{0}(lua_State *L) {{
    return 1;
}}
)LuaLibraryString";

// TODO For android disable this subsystem entirely

string GetLuaLibraryDefaultString(const string_view inLibraryName) {
#ifdef ANDROID
          return " ";
#else
          return std::vformat(DefaultLuaLibraryFile, std::make_format_args(inLibraryName));
#endif
}

string GetLuaCMakeListsDefaultString(const string_view inJkrguiPath, const string_view inBuildType, const string_view inLibraryName) {
#ifdef ANDROID
          return " ";
#else
          return std::vformat(DefaultCMakeListsFile, std::make_format_args(inJkrguiPath, inBuildType, inLibraryName));
#endif
}

namespace JkrEXE {
namespace fs = std::filesystem;
namespace BuildSystem {

void CreateLuaLibraryEnvironment(
 sv inLibraryName, sv inJkrGUIRepoDirectory, sv inNativeDestinationDirectory, sv inBuildType, bool inManualLibLinking) {
          fs::path CurrentPath = fs::current_path();
          fs::path JkrGuiRepoPath = s(inJkrGUIRepoDirectory);
          fs::path NativeDestinationDirectory = s(inNativeDestinationDirectory);

          {
                    fs::path mainCppFilePath = NativeDestinationDirectory / fs::path(string(inLibraryName) + ".cpp");
                    fs::path cmakeListsFilePath = NativeDestinationDirectory / fs::path("CMakeLists.txt");
                    fs::path cmakePresetsFilePath = NativeDestinationDirectory / fs::path("CMakePresets.json");
                    fs::path cmakePresetsJkrGuiLibraryPath = JkrGuiRepoPath / fs::path("CMakePresets.json");

                    if (not fs::exists(NativeDestinationDirectory)) fs::create_directory(inNativeDestinationDirectory);

                    if (not fs::exists(cmakeListsFilePath)) {
                              std::ofstream cmakeListsFile(cmakeListsFilePath, std::fstream::out);
                              if (cmakeListsFile.is_open()) {
                                        cmakeListsFile << GetLuaCMakeListsDefaultString(inJkrGUIRepoDirectory, inBuildType, inLibraryName);
                              } else {
                                        std::cout << "Fuck, Files are not open";
                              }
                              if (not fs::exists(cmakePresetsFilePath)) fs::copy_file(cmakePresetsJkrGuiLibraryPath, cmakePresetsFilePath);
                    }

                    if (not fs::exists(mainCppFilePath)) {
                              std::ofstream mainCppFile(mainCppFilePath, std::fstream::out);
                              if (mainCppFile.is_open()) {
                                        mainCppFile << GetLuaLibraryDefaultString(inLibraryName);
                              } else {
                                        std::cout << "Fuck, File is not open";
                              }
                    }
          }
}

}; // namespace BuildSystem

static umap<s, int> CommandLineCommandsArgumentCount = {{"--build", 0}};
int GetArgumentCount(sv inString) { return CommandLineCommandsArgumentCount[s(inString)]; }

umap<s, v<s>> CommandLine(int ArgCount, char** ArgStrings) {
          umap<s, v<s>> CommandLineStuff;
          int FirstArgIndex = 1;
          for (int i = FirstArgIndex; i < ArgCount; i++) {
                    const s arg(ArgStrings[i]);
                    const int ArgumentCount = GetArgumentCount(arg);
                    if (ArgumentCount == 0) {
                              CommandLineStuff[arg] = v<s>{""};
                    } else if (i + 1 < ArgCount) {
                              v<s> arguments;
                              for (int j = i; j < i + ArgumentCount; j++) {
                                        const s new_arg(ArgStrings[i]);
                                        i++;
                              }
                    } else {
                              std::cout << "Error:\n";
                    }
          }

          return CommandLineStuff;
}

void CreateBuildSystemBindings(sol::state& inS) {
          auto Jkr = inS["Jkr"].get_or_create<sol::table>();
          auto Build = Jkr["BuildSystem"].get_or_create<sol::table>();
          Build.set_function("CreateLuaLibraryEnvironment", &BuildSystem::CreateLuaLibraryEnvironment);
}

} // namespace JkrEXE