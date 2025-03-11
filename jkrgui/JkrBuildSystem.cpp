#include "ksai_config.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <glm/glm.hpp>
#include <sol/sol.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include "JkrLuaExe.hpp"
#include <CLI11/CLI11.hpp>

using namespace ksai;
using namespace std;

#ifndef __ANDROID__
namespace BuildSystem {
const string_view DefaultCMakeListsFile = R"CmakeListsFile(cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
include("{0}/CMakeConfig.cmake")
include("{0}/CMakeCommon.cmake")
include("{0}/out/build/${{CMAKE_PRESET_NAME}}/luaExport.cmake")
include("{0}/out/build/${{CMAKE_PRESET_NAME}}/ksaivulkanExport.cmake")
include("{0}/out/build/${{CMAKE_PRESET_NAME}}/jkrengineExport.cmake")


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
        jkrguiApp.set_function("hello", []() {{ printf("Hello from JkrGUI\n");}});
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

string GetLuaCMakeListsDefaultString(const string_view inLibraryName) {
#ifdef ANDROID
    return " ";
#else
    auto env = getenv("JKRGUI_DIR");
    return std::vformat(DefaultCMakeListsFile, std::make_format_args(env, inLibraryName));
#endif
}

namespace fs = std::filesystem;

static void ReplaceStringInFile(fs::path filePath,
                                const std::string_view searchString,
                                const std::string_view replaceString) {
    std::ifstream fileIn(filePath);
    if (!fileIn.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(fileIn)), std::istreambuf_iterator<char>());
    size_t pos               = content.find(searchString);
    const auto ShouldRewrite = not(pos == std::string::npos);
    while (pos != std::string::npos) {
        content.replace(pos, searchString.length(), replaceString);
        pos = content.find(searchString);
    }
    if (ShouldRewrite) {
        std::ofstream fileOut(filePath);
        fileOut << content;
        Log("Replaced in file: " + filePath.string());
    }
}

static void ReplaceString(fs::path path,
                          const std::string_view searchString,
                          const std::string_view replaceString) {
    for (const auto &entry : fs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            ReplaceStringInFile(entry.path(), searchString, replaceString);
        }
    }
}

static void RenameFileName(const fs::path &path,
                           const std::string_view searchString,
                           const std::string_view replaceString) {
    fs::path parentPath = path.parent_path();
    std::string newName = path.filename().string();
    size_t pos          = newName.find(searchString);
    if (pos != std::string::npos) {
        newName.replace(pos, searchString.length(), replaceString);
        fs::path newPath = parentPath / newName;
        if (filesystem::exists(newPath)) {
            filesystem::remove_all(newPath);
        }
        fs::rename(path, newPath);
        Log("Renamed: " + path.string() + " to " + newPath.string());
    }
}

static void RenameFilesInDirectory(const fs::path &path,
                                   const std::string_view searchString,
                                   const std::string_view replaceString) {
    static v<fs::path> FilesToBeRenamedFrom;
    for (const auto &entry : fs::recursive_directory_iterator(path)) {
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

constexpr std::string_view AndroidAppString = "AndroidApp";

void CreateAndroidEnvironment(const sv inAndroidAppName,
                              const sv inAndroidAppDirectory,
                              const sv inBuild,
                              const sv inArchitecture) {
    try {
        fs::path Src    = s(getenv("JKRGUI_DIR")) + "/Android/";
        fs::path Target = s(inAndroidAppDirectory);
        if (not filesystem::exists(Target)) {
            filesystem::create_directory(Target);
        }
        if (not fs::exists(Target)) {
            fs::create_directory(Target);
        }
        fs::copy(Src, Target, fs::copy_options::recursive | fs::copy_options::skip_existing);
        ReplaceString(Target, AndroidAppString, inAndroidAppName);
        RenameFilesInDirectory(Target, AndroidAppString, inAndroidAppName);

        // This is android's assets directory, not luaproject's asset directory
        fs::path Assets               = Target / "app" / "src" / "main" / "assets";
        const v<sv> EntriesToBeCopied = {"main.jkr"};

        if (not filesystem::exists(Assets)) {
            filesystem::create_directory(Assets);
        }

        int i = 0;
        for (const auto &entry : fs::directory_iterator(fs::current_path())) {
            bool ShouldCopy = std::any_of(EntriesToBeCopied.begin(),
                                          EntriesToBeCopied.end(),
                                          [&](auto &i) { return i == entry.path().filename(); });
            if (ShouldCopy) {
                fs::path src    = entry;
                fs::path target = Assets / entry.path().filename();
                if (not fs::exists(target) and entry.is_directory()) fs::create_directory(target);
                if (fs::exists(src)) {
                    fs::copy(src, target, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                }
            }
        }

        auto JKRGUI_DIR = fs::path(getenv("JKRGUI_DIR"));
        fs::path jniLibsDir        = Target / "app" / "src" / "main" / "jniLibs";
        fs::path androidlibs = JKRGUI_DIR / "libs" / "Android" / inArchitecture;
        fs::path validation_layers = JKRGUI_DIR / "libs" / "Android" / inArchitecture / "libVkLayer_khronos_validation.so";
        fs::path source      = JKRGUI_DIR / "out" / "build" / inBuild / "jkrgui";
        fs::path destination = jniLibsDir / inArchitecture;

        if (not fs::exists(destination)) fs::create_directory(destination);
        fs::copy_file(androidlibs / "libSDL2.so", destination / "libSDL2.so");
        fs::copy_file(androidlibs / "libVkLayer_khronos_validation.so", destination / "libVkLayer_khronos_validation.so");
        fs::copy_file(source / "libjkrgui.so", destination / "libjkrgui.so", fs::copy_options::overwrite_existing);
        // fs::copy_file(JKRGUI_DIR / "out" / "build" / inBuild / "_deps" / "sdl2-build" / "libSDL2.so", destination / "libSDL2.so", fs::copy_options::overwrite_existing);
        fs::copy_file(validation_layers, destination / "libVkLayer_khronos_validation.so", fs::copy_options::overwrite_existing);

    } catch (const std::exception &e) {
        Log(e.what(), "ERROR");
    }
}

void CreateLuaLibraryEnvironment(sv inLibraryName,
                                 sv inNativeDestinationDirectory,
                                 bool inOverride) {
    fs::path CurrentPath                = fs::current_path();
    fs::path JkrGuiRepoPath             = s(getenv("JKRGUI_DIR"));
    fs::path NativeDestinationDirectory = s(inNativeDestinationDirectory);

    {
        fs::path mainCppFilePath =
             NativeDestinationDirectory / fs::path(string(inLibraryName) + ".cpp");
        fs::path cmakeListsFilePath   = NativeDestinationDirectory / fs::path("CMakeLists.txt");
        fs::path cmakePresetsFilePath = NativeDestinationDirectory / fs::path("CMakePresets.json");
        fs::path cmakePresetsJkrGuiLibraryPath = JkrGuiRepoPath / fs::path("CMakePresets.json");

        if (not fs::exists(NativeDestinationDirectory))
            fs::create_directory(inNativeDestinationDirectory);

        if (not fs::exists(cmakeListsFilePath) or inOverride) {
            std::ofstream cmakeListsFile(cmakeListsFilePath, std::fstream::out);
            if (cmakeListsFile.is_open()) {
                cmakeListsFile << GetLuaCMakeListsDefaultString(inLibraryName);
            } else {
                Log("Files are not open", "ERROR");
            }
            if (not fs::exists(cmakePresetsFilePath))
                fs::copy_file(cmakePresetsJkrGuiLibraryPath, cmakePresetsFilePath);
        }

        if (not fs::exists(mainCppFilePath) or inOverride) {
            std::ofstream mainCppFile(mainCppFilePath, std::fstream::out);
            if (mainCppFile.is_open()) {
                mainCppFile << GetLuaLibraryDefaultString(inLibraryName);
            } else {
                Log("Files are not open", "ERROR");
            }
        }
    }
}

namespace fs = std::filesystem;

void Push(const std::string& dir, const std::string& inFileName, std::ofstream& outputFile) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().filename() == inFileName) {
            std::ifstream infile(entry.path());
            std::string line;
            while (std::getline(infile, line)) {
                if (line.find("require") == std::string::npos) { // Ignore lines starting with 'require'
                    outputFile << line << '\n';
                }
            }
            outputFile << '\n';
        }
    }
}

void Bundle(const std::string& outFileName) {
    auto env = s(getenv("JKRGUI_DIR"));
    std::ofstream outputFile(outFileName);
    if (!outputFile) {
        std::cerr << "Error opening output file!\n";
        return;
    }
    
    outputFile << "\nfunction jkrgui()\n";
    
    std::vector<std::string> files = {"require.lua", "Shader.lua", "Engine.lua", "CallBuffers.lua", "Basic.lua", "General.lua", "inspect.lua"};
    for (const auto& file : files) {
        Push( env + "/luaproject", file, outputFile);
    }
    
    outputFile << "end\n\njkrgui()\n";
    outputFile.close();
}

}; // namespace BuildSystem
#endif