#pragma once
#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1
#include "glm/glm.hpp"
#include "ksai_config.hpp"
#include <sol/sol.hpp>

#ifdef ANDROID
#define JKR_EXPORT __attribute__((visibility("default")))
#else
#define JKR_EXPORT __declspec(dllexport)
#endif

sol::state& GetMainStateRef();
struct DefaultCustomImagePainterPushConstant {
    glm::vec4 x, y, z;
};

using namespace std;
using namespace ksai;

static auto my_exception_handler(lua_State* L,
                                 sol::optional<const std::exception&> maybe_execption,
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
