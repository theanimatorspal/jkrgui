
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  
function(RunOnce)
if(MSVC)
if(POLICY CMP0141)
cmake_policy(SET CMP0141 NEW)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
endif()
cmake_policy(SET CMP0069 NEW) 
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif()
endfunction()

function(ClangTidyChecks)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
set(CLANG_TIDY_CHECKS
    "clang-analyzer-*,hicpp-*,readability-simplify-boolean-expr,readability-delete-null-pointer,portability-simd-intrinsics"
)

set(CLANG_TIDY_ARGS
    --warnings-as-errors=*
    -header-filter=.*
    --checks=${CLANG_TIDY_CHECKS}
    --format-style=file
)

set(CMAKE_C_CLANG_TIDY   "${CLANG_TIDY_EXE};${CLANG_TIDY_ARGS}")
set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};${CLANG_TIDY_ARGS}")
else()
    message(WARNING "clang-tidy not found! Static analysis will be skipped.")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
elseif (MSVC)
    add_compile_options(/W4 /permissive-)
endif()

if(MSVC)
    message(STATUS "MSVC detected: enabling AddressSanitizer")
    add_compile_options(/fsanitize=address /Zi /Od)
    add_link_options(/INCREMENTAL:NO /fsanitize=address)
else()
    set(SANITIZERS "")
    set(SANITIZERS "${SANITIZERS}address,undefined")
    if(UNIX AND NOT APPLE)
        set(SANITIZERS "${SANITIZERS},leak")
    elseif(APPLE)
        set(SANITIZERS "${SANITIZERS},leak")
    endif()
    add_compile_options(-fsanitize=${SANITIZERS} -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=${SANITIZERS})
endif()
endfunction()

if(EMSCRIPTEN)
    # Disable cmake_ninja_dyndep for modules / PCH
    set(CMAKE_NINJA_FORCE_RESPONSE_FILE 1)
endif()


function(PrecompileStdHeaders TARGET_NAME)
    target_precompile_headers(${TARGET_NAME} PRIVATE
        # Standard C++ Headers
        <algorithm>
        <any>
        <array>
        <atomic>
        <bit>
        <bitset>
        <cassert>
        <cctype>
        <cerrno>
        <cfenv>
        <charconv>
        <chrono>
        <cinttypes>
        <climits>
        <clocale>
        <cmath>
        <codecvt>
        <compare>
        <complex>
        <concepts>
        <condition_variable>
        <coroutine>
        <csetjmp>
        <csignal>
        <cstdarg>
        <cstddef>
        <cstdint>
        <cstdio>
        <cstdlib>
        <cstring>
        <ctime>
        <deque>
        <exception>
        <execution>
        <expected>
        <filesystem>
        <format>
        <forward_list>
        <fstream>
        <functional>
        <future>
        <initializer_list>
        <iomanip>
        <ios>
        <iosfwd>
        <iostream>
        <istream>
        <iterator>
        <limits>
        <list>
        <locale>
        <map>
        <memory>
        <memory_resource>
        <mutex>
        <new>
        <numbers>
        <numeric>
        <optional>
        <ostream>
        <queue>
        <random>
        <ranges>
        <ratio>
        <regex>
        <scoped_allocator>
        <set>
        <shared_mutex>
        <source_location>
        <span>
        <sstream>
        <stack>
        <stdexcept>
        <streambuf>
        <string>
        <string_view>
        <system_error>
        <thread>
        <tuple>
        <type_traits>
        <typeindex>
        <typeinfo>
        <unordered_map>
        <unordered_set>
        <utility>
        <valarray>
        <variant>
        <vector>
        <version>

        # C Standard Library Headers (as C++ headers)
        <cctype>
        <cerrno>
        <cfenv>
        <cfloat>
        <climits>
        <clocale>
        <cmath>
        <csetjmp>
        <csignal>
        <cstdarg>
        <cstddef>
        <cstdint>
        <cstdio>
        <cstdlib>
        <cstring>
        <ctime>
        <cwchar>
        <cwctype>

    # Third-Party Headers (Example: Vulkan, GLM, Sol2)
    #     <sol/sol.hpp>
    #     <vulkan/vulkan.hpp>
    #     <glm/glm.hpp>
    #     <glm/fwd.hpp>
    #     <glm/gtc/matrix_transform.hpp>
    #     <glm/gtx/matrix_decompose.hpp>
    #     <glm/gtc/type_ptr.hpp>
    #     <glm/gtx/quaternion.hpp>
    #     <glm/ext.hpp>
    #     <glm/gtx/transform.hpp>

    )
endfunction()


add_definitions(-DSDL_MAIN_HANDLED )
