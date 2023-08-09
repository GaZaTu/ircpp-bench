#pragma once

#include <functional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#if __has_include("simd")
#include <simd>
#else
#include <experimental/simd>
#endif

namespace irc {
class parsing_error : public std::exception {
public:
  std::string raw;

  parsing_error() {
  }

  parsing_error(std::string& r) : raw(std::move(r)) {
  }
};

using tags_map = std::unordered_map<std::string_view, std::string_view>;

std::string_view consumeTags(std::string_view raw, tags_map& tags) {
  if (raw[0] != '@') {
    return raw;
  }

  tags.reserve(20);

#if __has_include("simd")
  namespace std_simd = std;
#else
  namespace std_simd = std::experimental;
#endif

  std_simd::native_simd<decltype(raw)::value_type> vector;

  std::string_view key;
  std::string_view value;

  size_t offset = 1;
  for (size_t i = offset; i < raw.length(); i += vector.size()) {
    vector.copy_from(&raw[i], std_simd::element_aligned);

    auto mask = (vector == '=') || (vector == ';') || (vector == ' ');
    while (std_simd::any_of(mask)) {
      auto end_idx_in_vector = std_simd::find_first_set(mask);
      mask[end_idx_in_vector] = false;

      auto view = raw.substr(offset, (i + end_idx_in_vector) - offset);
      offset = i + end_idx_in_vector + 1;

      switch (raw[offset - 1]) {
      case '=':
        key = view;
        break;

      case ';':
        tags.emplace(key, view);
        break;

      case ' ':
        tags.emplace(key, view);
        goto consume_tags_end;
      }
    }
  }

consume_tags_end:
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
