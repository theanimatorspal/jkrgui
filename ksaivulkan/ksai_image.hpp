#pragma once
#include <concepts>
#include <functional>
#include <type_traits>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <cassert>
#include <climits>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace ksai::image {

template <typename... T>
using var = std::variant<T...>;
using s = std::string;
using sv = std::string_view;

template <typename T>
using v = std::vector<T>;
template <typename T>
using up = std::unique_ptr<T>;
template <typename T>
using sp = std::shared_ptr<T>;
template <typename T>
using opt = std::optional<T>;
template <typename T>
using optref = std::optional<std::reference_wrapper<T>>;
template <typename T, typename U>
using p = std::pair<T, U>;
using Number = double;
using Integer = int;
using std::make_unique;
using std::move;
using ui = uint32_t;
using uc = unsigned char;

template <typename T>
concept ImageConcept = requires(T& t) {
    typename T::value_type;
    typename T::size_type;
    typename T::iterator;
    typename T::const_iterator;
    {
        std::declval<T>().size()
    } -> std::same_as<typename T::size_type>;
    {
        std::declval<T>().begin()
    } -> std::same_as<typename T::iterator>;
    {
        std::declval<T>().end()
    } -> std::same_as<typename T::iterator>;
    {
        std::declval<T>().cbegin()
    } -> std::same_as<typename T::const_iterator>;
    {
        std::declval<T>().cend()
    } -> std::same_as<typename T::const_iterator>;
};

template <typename T, typename U>
concept ManipulatorConcept = requires(T& t, U& u, int x, int y, int w, int h, int c) {
    {
        t(u, u, x, y, w, h, c)
    } -> std::same_as<void>;
    {
        std::is_invocable_v<void, T, U&, U&, int, int, int, int, int>
    };
};

inline const auto flipvertically
    = []<typename T>(const T& from, T& to, int x, int y, int w, int h, int c) {
          ui fid = x + y * w * c;
          ui oid = x + (h - y - 1) * w * c;
          to[fid] = from[oid];
      };

inline const auto fliphorizontally = []<typename T>(const T& from, T& to, int x, int y, int w, int h, int c) {
    ui fid = x + y * w * c;
    ui oid = (w * c - x - 1) + y * w * c;
    to[fid] = from[oid];
};

template <ImageConcept T, ManipulatorConcept<T>... F>
void process(ui inw, ui inh, ui inComp, T& inoutImage, F&... inOp)
{
    ([&] {
        T flippedimage;
        flippedimage.resize(inoutImage.size());

        for (int y = 0; y < inh; ++y) {
            for (int x = 0; x < inw * inComp; ++x) {
                inOp(inoutImage, flippedimage, x, y, inw, inh, inComp);
            }
        }

        inoutImage = flippedimage;
    }(),
        ...);
}

}
