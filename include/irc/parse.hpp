#pragma once

#include "./clearchat.hpp"
#include "./join.hpp"
#include "./ping.hpp"
#include "./privmsg.hpp"
#include "./reconnect.hpp"
#include "./roomstate.hpp"
#include "./unknown.hpp"
#include "./usernotice.hpp"

namespace irc {
namespace detail {
template <auto Start, auto End, auto Inc, class F>
constexpr void constexpr_for(F&& f) {
  if constexpr (Start < End) {
    if (!f(std::integral_constant<decltype(Start), Start>())) {
      return;
    }

    constexpr_for<Start + Inc, End, Inc>(f);
  }
}

template <typename>
struct tag {};

template <typename T, typename V>
struct variant_get_index;

template <typename T, typename... Ts>
struct variant_get_index<T, std::variant<Ts...>>
    : std::integral_constant<size_t, std::variant<tag<Ts>...>(tag<T>()).index()> {};
} // namespace detail

using message =
    std::variant<unknown, privmsg, ping, clearchat, join, usernotice, /* userstate, */ roomstate, /* notice, */ reconnect>;

template <typename T>
constexpr size_t index_v = detail::variant_get_index<T, message>::value;

template <typename T>
constexpr inline T& get(message& m) {
  return std::get<index_v<T>>(m);
}

template <typename T>
constexpr inline const T& get(const message& m) {
  return std::get<index_v<T>>(m);
}

template <typename T>
constexpr inline T* get_if(message& m) {
  return std::get_if<index_v<T>>(&m);
}

template <typename T>
constexpr inline const T* get_if(const message& m) {
  return std::get_if<index_v<T>>(&m);
}

namespace detail {
template <typename S>
message parse(S& raw) {
  message result;

  detail::constexpr_for<(size_t)1, std::variant_size_v<message>, (size_t)1>([&result, &raw](auto i) {
    using T = std::variant_alternative_t<i, message>;

    try {
      result = T{raw};
      return false;
    } catch (parsing_error& e) {
      if constexpr (std::is_same_v<S, std::string>) {
        raw = std::move(e.raw);
      }
    }

    return true;
  });

  if (result.index() == index_v<unknown>) {
    result = unknown{raw};
  }

  return result;
}
} // namespace detail

inline message parse(std::string& raw) {
  return detail::parse<std::string>(raw);
}

inline message parse(std::string_view raw) {
  return detail::parse<std::string_view>(raw);
}

std::string_view stringify(const message& m);
} // namespace irc
