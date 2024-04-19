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

const string_view DefaultCMakeListsFile = R"CmakeListsFile(cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
include("{0}/CMakeConfig.cmake")
include("{0}/CMakeCommon.cmake")
include("{0}/out/build/${{CMAKE_PRESET_NAME}}/luaExport.cmake")
include("{0}/out/build/${{CMAKE_PRESET_NAME}}/ksaivulkanExport.cmake")
include("{0}/out/build/${{CMAKE_PRESET_NAME}}/jkrguiExport.cmake")


project({1})
add_library({1} SHARED {1}.cpp)
target_link_libraries({1} lua ksaivulkan jkrgui)
configure_target({1})

add_custom_command(
    TARGET {1} POST_BUILD
    COMMAND ${{CMAKE_COMMAND}} -E copy
            $<TARGET_FILE:{1}>
            ${{CMAKE_CURRENT_SOURCE_DIR}}/../
)

)CmakeListsFile";

const string_view DefaultLuaLibraryFile = R"LuaLibraryString(#include <sol/sol.hpp>
#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT
#endif

extern "C" DLLEXPORT int luaopen_{0}(lua_State *L) {{
        sol::state_view s(L);
        auto jkrguiApp = s["jkrguiApp"].get_or_create<sol::table>();
        jkrguiApp.set_function("hello", []() {{ std::cout << "Hello World from jkrguiApp\n"; }});
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
               return std::vformat(DefaultCMakeListsFile, std::make_format_args(inJkrguiPath, inLibraryName));
#endif
}

namespace JkrEXE {
namespace fs = std::filesystem;
namespace BuildSystem {

static void ReplaceStringInFile(fs::path filePath, const std::string_view searchString, const std::string_view replaceString) {
               std::ifstream fileIn(filePath);
               if (!fileIn.is_open()) {
                              std::cerr << "Failed to open file: " << filePath << std::endl;
                              return;
               }

               std::string content((std::istreambuf_iterator<char>(fileIn)), std::istreambuf_iterator<char>());
               size_t pos = content.find(searchString);
               if (pos != std::string::npos) {
                              content.replace(pos, searchString.length(), replaceString);
                              std::ofstream fileOut(filePath);
                              fileOut << content;
                              fileOut.close();
                              std::cout << "Replaced in file: " << filePath << std::endl;
               }
               fileIn.close();
}

static void ReplaceString(fs::path path, const std::string_view searchString, const std::string_view replaceString) {
               for (const auto& entry : fs::recursive_directory_iterator(path)) {
                              if (entry.is_regular_file()) {
                                             ReplaceStringInFile(entry.path(), searchString, replaceString);
                              }
               }
}

static void RenameFileName(const fs::path& path, const std::string_view searchString, const std::string_view replaceString) {
               fs::path parentPath = path.parent_path();
               std::string newName = path.filename().string();
               size_t pos          = newName.find(searchString);
               if (pos != std::string::npos) {
                              newName.replace(pos, searchString.length(), replaceString);
                              fs::path newPath = parentPath / newName;
                              fs::rename(path, newPath);
                              std::cout << "Renamed: " << path << " to " << newPath << std::endl;
               }
}

static void RenameFilesInDirectory(const fs::path& path, const std::string_view searchString, const std::string_view replaceString) {
               static v<fs::path> FilesToBeRenamedFrom;
               for (const auto& entry : fs::recursive_directory_iterator(path)) {
                              if (entry.is_regular_file() or entry.is_directory()) {
                                             size_t pos = entry.path().filename().string().find(searchString);
                                             if (pos != std::string::npos) {
                                                            FilesToBeRenamedFrom.push_back(entry.path());
                                             }
                              }
               }

               for (auto i = FilesToBeRenamedFrom.rbegin(); i != FilesToBeRenamedFrom.rend(); ++i) {
                              RenameFileName(*i, searchString, replaceString);
               }
}

void CreateAndroidEnvironment(const sv inAndroidAppName, const sv inAndroiAppDirectory, const sv inLibraryName, const sv inJkrGUIRepoDirectory) {
               fs::path Src    = s(inJkrGUIRepoDirectory) + "/Android/";
               fs::path Target = "Android";
               if (not fs::exists(Target)) {
                              fs::create_directory(Target);
                              fs::copy(Src, Target, fs::copy_options::recursive | fs::copy_options::skip_existing);
                              ReplaceString(Target, "AndroidApp", inAndroidAppName);
                              RenameFilesInDirectory(Target, "AndroidApp", inAndroidAppName);
               }

               fs::path Assets               = Target / "app" / "src" / "main" / "assets";
               const v<sv> EntriesToBeCopied = {"cache2", "JkrGUIv2", "res", "app.lua"};

               int i                         = 0;
               for (const auto& entry : fs::directory_iterator(fs::current_path())) {
                              bool ShouldCopy = std::any_of(EntriesToBeCopied.begin(), EntriesToBeCopied.end(), [&](auto& i) { return i == entry.path().filename(); });
                              if (ShouldCopy) {
                                             fs::path src    = entry;
                                             fs::path target = Assets / entry.path().filename();
                                             if (not fs::exists(target) and entry.is_directory()) fs::create_directory(target);
                                             fs::copy(src, target, fs::copy_options::recursive | fs::copy_options::skip_existing);
                              }
               }
}

static void CreateLuaLibraryEnvironment(sv inLibraryName, sv inJkrGUIRepoDirectory, sv inNativeDestinationDirectory, sv inBuildType, bool inOverride) {
               fs::path CurrentPath                = fs::current_path();
               fs::path JkrGuiRepoPath             = s(inJkrGUIRepoDirectory);
               fs::path NativeDestinationDirectory = s(inNativeDestinationDirectory);

               {
                              fs::path mainCppFilePath               = NativeDestinationDirectory / fs::path(string(inLibraryName) + ".cpp");
                              fs::path cmakeListsFilePath            = NativeDestinationDirectory / fs::path("CMakeLists.txt");
                              fs::path cmakePresetsFilePath          = NativeDestinationDirectory / fs::path("CMakePresets.json");
                              fs::path cmakePresetsJkrGuiLibraryPath = JkrGuiRepoPath / fs::path("CMakePresets.json");

                              if (not fs::exists(NativeDestinationDirectory)) fs::create_directory(inNativeDestinationDirectory);

                              if (not fs::exists(cmakeListsFilePath) or inOverride) {
                                             std::ofstream cmakeListsFile(cmakeListsFilePath, std::fstream::out);
                                             if (cmakeListsFile.is_open()) {
                                                            cmakeListsFile << GetLuaCMakeListsDefaultString(inJkrGUIRepoDirectory, inBuildType, inLibraryName);
                                             } else {
                                                            std::cout << "Fuck, Files are not open";
                                             }
                                             if (not fs::exists(cmakePresetsFilePath)) fs::copy_file(cmakePresetsJkrGuiLibraryPath, cmakePresetsFilePath);
                              }

                              if (not fs::exists(mainCppFilePath) or inOverride) {
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

static umap<s, int> CommandLineCommandsArgumentCount = {{"--build", 0}, {"--generate", 0}};
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
                                             CommandLineStuff[arg] = arguments;
                              } else {
                                             std::cout << "Error:\n";
                              }
               }

               return CommandLineStuff;
}

void CreateBuildSystemBindings(sol::state& inS) {
               auto Jkr   = inS["Jkr"].get_or_create<sol::table>();
               auto Build = Jkr["BuildSystem"].get_or_create<sol::table>();
               Build.set_function("CreateLuaLibraryEnvironment", &BuildSystem::CreateLuaLibraryEnvironment);
               Build.set_function("CreateAndroidEnvironment", &BuildSystem::CreateAndroidEnvironment);
}

} // namespace JkrEXE