#pragma once
#include <concepts>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <queue>
#include <span>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#define GETTER inline auto
#define SETTER inline void

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

void* operator new(std::size_t count);
void operator delete(void* ptr) noexcept;

namespace ksai {
template <typename... T> using var           = std::variant<T...>;
using s                                      = std::string;
using sv                                     = std::string_view;

template <typename T> using v                = std::vector<T>;
template <typename T> using up               = std::unique_ptr<T>;
template <typename T> using sp               = std::shared_ptr<T>;
template <typename T> using opt              = std::optional<T>;
template <typename T> using optref           = std::optional<std::reference_wrapper<T>>;
template <typename T, typename U> using p    = std::pair<T, U>;
template <typename T, typename U> using umap = std::unordered_map<T, U>;
template <typename T, typename U> using map  = std::map<T, U>;
using Number                                 = double;
using Integer                                = int;
using ui                                     = uint32_t;
using si                                     = int32_t;
using sz                                     = size_t;
using sc                                     = signed char;
using uc                                     = unsigned char;

#define mu std::make_unique
#define mv std::move
#define mksh std::make_shared
} // namespace ksai

#ifdef ANDROID
extern "C" int __android_log_print(int prio, const char* tag, const char* fmt, ...);
#endif

template <typename T> inline void ksai_print(T inT) {
#ifdef ANDROID
    __android_log_print(6, "KSAI::%s", "=================================");
    if constexpr (std::is_same_v<T, ksai::s>) {
        __android_log_print(6, "KSAI::%s", inT.c_str());
    }

    if constexpr (std::is_same_v<T, ksai::sv>) {
        __android_log_print(6, "KSAI::%s", inT.dat());
    }

#else
    std::cout << inT << "\n";
    // printf(inT);
    //  printf(inT.data(), t...);
    //  printf("\n");
#endif // ANDROID
}

template <typename... T> inline void ksai_print(std::string_view inT, T&&... t) {
#ifdef ANDROID
    __android_log_print(6, "KSAI:: %s", inT.data());
#else
    // printf(inT.data(), t...);
#endif // ANDROID
}
