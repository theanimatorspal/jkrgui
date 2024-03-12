// JkrLuaExe.h : Header file for your target.

#pragma once
#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/2d.hpp>
#include <WindowMulT.hpp>
#include <sol/sol.hpp>

using namespace std;
using namespace ksai;

namespace JkrEXE {
static auto my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_execption,
			   sol::string_view description) -> int {
	     std::cout << "An Exception "
		        "has occured in "
		        "the function "
		        "here is what "
		        "it says: ";
	     if (maybe_execption) {
			std::cout << "(Straight from the exception) ";
			const std::exception& ex = *maybe_execption;
			std::cout << ex.what() << std::endl;
	     } else {
			std::cout << "(From the Descriptor Parameter) ";
			std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
			std::cout << std::endl;
	     }
	     return sol::stack::push(L, description);
};
} // namespace JkrEXE