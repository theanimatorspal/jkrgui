#pragma once
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <climits>
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include <sstream>
#include <optional>

template<typename ... T>
using var = std::variant<T...>;
using s = std::string;
using sv = std::string_view;

template<typename T>
using v = std::vector<T>;
template<typename T>
using up = std::unique_ptr<T>;
template<typename T>
using sp = std::shared_ptr<T>;
template<typename T>
using opt = std::optional<T>;
template<typename T>
using optref = std::optional<std::reference_wrapper<T>>;
template<typename T, typename U>
using p = std::pair<T, U>;
using Number = double;
using Integer = int;

#define mu std::make_unique
#define mv std::move

