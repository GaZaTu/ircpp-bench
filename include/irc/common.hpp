#pragma once

#include <functional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
#include <charconv>

#if __has_include("simd")
#include <simd>
#else
#include <experimental/simd>
#endif

namespace irc {
// namespace detail {
// template <auto Start, auto End, auto Inc, class F>
// constexpr void constexpr_for(F&& f) {
//   if constexpr (Start < End) {
//     if (!f(std::integral_constant<decltype(Start), Start>())) {
//       return;
//     }

//     constexpr_for<Start + Inc, End, Inc>(f);
//   }
// }
// }

class parsing_error : public std::exception {
public:
  std::string raw;

  parsing_error() {
  }

  parsing_error(std::string& r) : raw(std::move(r)) {
  }
};

// using tags_map = std::unordered_map<std::string_view, std::string_view>;

class tags_map {
public:
  using value_type = std::pair<std::string_view, std::string_view>;
  using list_type = std::vector<value_type>;

  tags_map() {
  }

  size_t size() const noexcept {
    return _list.size();
  }

  void reserve(size_t n) {
    _list.reserve(n);
  }

  void emplace(value_type::first_type key, value_type::second_type value) {
    _list.emplace_back(key, value);
  }

  value_type::second_type at(value_type::first_type search) const {
    for (auto& [key, value] : _list) {
      if (key == search) {
        return value;
      }
    }

    return {};
  }

  value_type::second_type operator[](value_type::first_type search) const {
    return at(search);
  }

  template <typename T>
  T get(std::string_view key) const {
    auto view = at(key);

    if constexpr (std::is_same_v<T, std::string_view>) {
      return view;
    } else if constexpr (std::is_same_v<T, std::string>) {
      auto result = (std::string)view;
      // TODO: unescape
      return result;
    } else if constexpr (std::is_same_v<T, bool>) {
      return view != "0";
    } else if constexpr (std::is_arithmetic_v<T>) {
      T result = 0;
      std::from_chars(std::begin(view), std::end(view), result);
      return result;
    } else {
      return {};
    }
  }

  list_type::const_iterator begin() const noexcept {
    return _list.begin();
  }

  list_type::const_iterator end() const noexcept {
    return _list.end();
  }

private:
  list_type _list;
};

std::string_view consumeTags(std::string_view raw, tags_map& tags) {
  constexpr char TAGS_BEGIN = '@';
  constexpr char KEY_END = '=';
  constexpr char VALUE_END = ';';
  constexpr char TAGS_END = ' ';

  if (raw[0] != TAGS_BEGIN) {
    return raw;
  }

  static int REQUIRED_CAPACITY = 16;
  tags.reserve(REQUIRED_CAPACITY);

#if __has_include("simd")
  namespace std_simd = std;
#else
  namespace std_simd = std::experimental;
#endif

  std_simd::fixed_size_simd<char, 32> vector;

  std::string_view key;
  std::string_view value;

  size_t offset = 1;
  for (size_t i = offset; i < raw.length(); i += vector.size()) {
    vector.copy_from(&raw[i], std_simd::element_aligned);

    auto mask = (vector == KEY_END) || (vector == VALUE_END) || (vector == TAGS_END);
    while (std_simd::any_of(mask)) {
      auto end_idx_in_vector = std_simd::find_first_set(mask);
      mask[end_idx_in_vector] = false;

      auto end_idx = i + end_idx_in_vector;
      auto view = raw.substr(offset, end_idx - offset);
      offset = end_idx + 1;

      switch (raw[end_idx]) {
      case KEY_END:
        key = view;
        break;

      case VALUE_END:
        tags.emplace(key, view);
        break;

      case TAGS_END:
        tags.emplace(key, view);
        goto consume_tags_loop_exit;
      }
    }
  }
consume_tags_loop_exit:

  if (REQUIRED_CAPACITY < tags.size()) {
    REQUIRED_CAPACITY = tags.size();
  }

  return raw.substr(offset);
}

std::tuple<std::string_view, tags_map> consumeTags(std::string_view raw) {
  tags_map tags;
  return {consumeTags(raw, tags), std::move(tags)};
}

namespace regex {
std::regex join{"^:(\\w+)!\\w+@\\S+ JOIN #(\\w+)"};
std::regex privmsg{"^:(\\w+)!\\w+@\\S+ PRIVMSG #(\\w+) :"};
std::regex ping{"^PING :(.*)"};
std::regex clearchat{"^:\\S+ CLEARCHAT #(\\w+) :(\\w+)"};
std::regex usernotice{"^:\\S+ USERNOTICE #(\\w+) :"};
std::regex userstate{"^:\\S+ USERSTATE #(\\w+)"};
std::regex roomstate{"^:\\S+ ROOMSTATE #(\\w+)"};
std::regex reconnect{"RECONNECT"};
} // namespace regex
} // namespace irc
