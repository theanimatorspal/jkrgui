#pragma once
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <string>

using sv = std::string_view;
template<typename T>
using v = std::vector<T>;
template<typename T, typename U>
using map = std::map<T, U>;
template<typename T, typename U>
using umap = std::unordered_map<T, U>;
template<typename T, typename U>
using p = std::pair<T, U>;
template<typename T>
using u = std::unique_ptr<T>;
template<typename T>
using opt = std::optional<T>;
using u32 = uint32_t;
#define mu std::make_unique
#define mv std::move
#define mksh std::make_shared
