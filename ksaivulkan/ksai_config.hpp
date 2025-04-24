#pragma once
#include <concepts>
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
#include <random>
#include <variant>
#include <atomic>
#include <queue>
#include <chrono>
#include <thread>
#include <fstream>
#include <cmath>
#include <set>
#include <algorithm>
#include <mutex>
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

void *operator new(std::size_t count);
void operator delete(void *ptr) noexcept;

namespace ksai {
template <typename... T> using var           = std::variant<T...>;
using s                                      = std::string;
using sv                                     = std::string_view;

template <typename T> using v                = std::vector<T>;
template <typename T, size_t N> using a      = std::array<T, N>;
template <typename T> using up               = std::unique_ptr<T>;
template <typename T> using sp               = std::shared_ptr<T>;
template <typename T> using opt              = std::optional<T>;
template <typename T> using optref           = std::optional<std::reference_wrapper<T>>;
template <typename T> using atm              = std::atomic<T>;
template <typename T> using span             = std::span<T>;
template <typename T, typename U> using p    = std::pair<T, U>;
template <typename T, typename U> using umap = std::unordered_map<T, U>;
template <typename T, typename U> using map  = std::map<T, U>;
template <typename T> using rmrf             = std::remove_cvref_t<T>;
using Number                                 = double;
using Integer                                = int;
using ui                                     = uint32_t;
using si                                     = int32_t;
using sz                                     = size_t;
using sc                                     = signed char;
using uc                                     = unsigned char;
using mtx                                    = std::mutex;
using randev                                 = std::random_device;
using rangen                                 = std::mt19937;

#define mu std::make_unique
#define mv std::move
#define mksh std::make_shared
#define mkar std::to_array
void Log(sv msg, sv type = "INFO");
void SetLogCallBack(std::function<int(sv, sv, sv)> inCallBack);
extern std::function<int(sv, sv, sv)> gksai_log_callback;

} // namespace ksai

#ifdef ANDROID
extern "C" int __android_log_print(int prio, const char *tag, const char *fmt, ...);
#endif

template <typename... T> inline void ksai_print(std::string_view inT, T &&...t) {
#ifdef ANDROID
      __android_log_print(6, inT.data(), t...);
#else
      printf(inT.data(), t...);
#endif // ANDROID
}

template <typename... T> inline void ksai_print(const char *inT, T &&...t) {
#ifdef ANDROID
      __android_log_print(6, inT, t...);
#else
      printf(inT, t...);
#endif // ANDROID
}

inline void ksai_print(const char *inT) { ksai_print("KSAI::%s", inT); }

template <typename T> inline void ksai_print(T inT) {
#ifdef ANDROID
      __android_log_print(6, "KSAI::%s", "=================================");
      if constexpr (std::is_same_v<T, ksai::s>) {
            T t = inT;
            __android_log_print(6, "KSAI::%s", t.c_str());
      }

      if constexpr (std::is_same_v<T, ksai::sv>) {
            T t = inT;
            __android_log_print(6, "KSAI::%s", t.data());
      }

#else
      std::cout << inT << "\n";
#endif // ANDROID
}