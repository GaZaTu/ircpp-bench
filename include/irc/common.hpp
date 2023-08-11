#pragma once

#include <exception>
#include <string>
#include <vector>
#include <charconv>
#include <variant>

namespace irc {
class parsing_error : public std::exception {
public:
  std::string raw;

  parsing_error();

  parsing_error(std::string& r);
};

// using tags_map = std::unordered_map<std::string_view, std::string_view>;

class tags_map {
public:
  using value_type = std::pair<std::string_view, std::string_view>;
  using list_type = std::vector<value_type>;

  tags_map();

  size_t size() const noexcept;

  void reserve(size_t n);

  void emplace(value_type::first_type key, value_type::second_type value);

  value_type::second_type at(value_type::first_type search) const;

  value_type::second_type operator[](value_type::first_type search) const;

  template <typename T>
  inline T get(std::string_view key) const {
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

  list_type::const_iterator begin() const noexcept;

  list_type::const_iterator end() const noexcept;

private:
  list_type _list;
};

std::string_view parseTags(std::string_view raw, tags_map& tags);

std::tuple<std::string_view, tags_map> parseTags(std::string_view raw);

namespace detail {
#define IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(NAME)                   \
  NAME(std::string&& raw) : message_base_mixin(raw) { parse(); }    \
  NAME(std::string_view raw) : message_base_mixin(raw) { parse(); } \
  NAME(NAME&& other) = default;                                     \
  NAME& operator=(NAME&& other) = default;                          \
  NAME(const NAME& other) { *this = other; }                        \
  NAME& operator=(const NAME& other) {                              \
    _raw = (std::string)other;                                      \
    parse();                                                        \
    return *this;                                                   \
  }

struct message_base_mixin {
public:
  message_base_mixin();

  message_base_mixin(std::string&& raw);

  message_base_mixin(std::string_view raw);

  explicit operator std::string_view() const;

  explicit operator std::string() const;

protected:
  static constexpr int VIEW = 0;
  static constexpr int STRING = 1;

  std::variant<std::string_view, std::string> _raw;
};

struct has_tags_mixin {
public:
  const tags_map& tags() const;

  std::string_view operator[](std::string_view key) const;

protected:
  tags_map _tags;
};
}
} // namespace irc
