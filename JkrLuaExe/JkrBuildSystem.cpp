#include "JkrLuaExe.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <sol/sol.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace JkrEXE {
namespace fs = std::filesystem;

void CopyRecursive(const fs::path& inSrc, const fs::path& target, const std::function<bool(fs::path)>& predicate) {
          for (const auto& dirEntry : fs::recursive_directory_iterator(inSrc)) {
                    const auto& p = dirEntry.path();
                    if (predicate(p)) {
                              const auto relativeSrc = fs::relative(p, inSrc);
                              const auto targetParentPath = target / relativeSrc.parent_path();
                              fs::create_directories(targetParentPath);
                              fs::copy(p, targetParentPath, fs::copy_options::overwrite_existing);
                    }
          }
}

void CopyAllHeaderFiles(fs::path& insrc, fs::path& indst) {
          const s Tdir = "temporary";
          fs::create_directory(Tdir);
          CopyRecursive(fs::current_path(), Tdir, [](fs::path p) {
                    if (not fs::is_directory(p)) return p.stem().generic_string().find(".hpp") != s::npos;
                    return true;
          });
}

void NewLuaLibrary(sv inLibraryName, sv inCode) {
          constexpr auto NewFile = [](const s& inFileStr) { return std::fstream(inFileStr, std::fstream::out | std::fstream::trunc); };
          const s Tdir = "temporary";
          const s CMakeListsFileString = Tdir + "/CMakeLists.txt";
          const s LibraryFileString = Tdir + "/" + s(inLibraryName) + ".cpp";

          fs::create_directory(Tdir);
          auto LibraryFile = NewFile(LibraryFileString);
          auto CmakeListsFile = NewFile(CMakeListsFileString);

          LibraryFile << inCode;
          CmakeListsFile << R"""(

cmake_minimum_required(VERSION 3.27)

)""" <<

           "add_library(library SHARED " << LibraryFileString << ")"

                         <<
           R"""(

target_link_libraries(library
	jkrgui ksaivulkan
)

)""";

          fs::current_path(Tdir);
          // fs::remove_all(Tdir);
}

namespace BuildSystem {

void CreateLuaLibraryEnvironment(sv inJkrGUIRepoDirectory, sv inNex, sv inDstDirectory, sv inBuildType, bool inManualLibLinking) {
          const v<sv> BuildType = {"x64-release", "x64-debug", "x86-relase", "x86-debug"};
          fs::path src = s(inJkrGUIRepoDirectory);
          fs::path dst = s(inDstDirectory);
          CopyAllHeaderFiles(src, dst);

          if (not inManualLibLinking) {
          } else {
          }
}

}; // namespace BuildSystem

static umap<s, int> CommandLineCommandsArgumentCount = {{"--build", 0}};
int GetArgumentCount(sv inString) { return CommandLineCommandsArgumentCount[s(inString)]; }

umap<s, v<s>> CommandLine(int ArgCount, char** ArgStrings) {
          umap<s, v<s>> CommandLineStuff;
          for (int i = 0; i < ArgCount; i++) {
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