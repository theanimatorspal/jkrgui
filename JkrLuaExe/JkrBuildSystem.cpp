#include <filesystem>
#include <fstream>
#include <functional>
#include <sol/sol.hpp>


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
	     const std::string Tdir = "temporary";
	     fs::create_directory(Tdir);
	     CopyRecursive(fs::current_path(), Tdir, [](fs::path p) {
			if (not fs::is_directory(p))
				     return p.stem().generic_string().find(".hpp") != std::string::npos;
			return true;
	     });
}

void NewLuaLibrary(std::string_view inLibraryName, std::string_view inCode) {
	     constexpr auto NewFile = [](const std::string& inFileStr) {
			return std::fstream(inFileStr, std::fstream::out | std::fstream::trunc);
	     };
	     const std::string Tdir = "temporary";
	     const std::string CMakeListsFileString = Tdir + "/CMakeLists.txt";
	     const std::string LibraryFileString = Tdir + "/" + std::string(inLibraryName) + ".cpp";

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
    lua54
)

)""";

	     fs::current_path(Tdir);
	     // fs::remove_all(Tdir);
}

} // namespace JkrEXE