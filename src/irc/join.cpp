#include "irc/join.hpp"
#include <regex>

namespace irc {
std::string_view join::channel() const {
  return _channel;
}

std::string_view join::user() const {
  return _user;
}

void join::parse() {
  std::string_view data = parseTags((std::string_view)(*this), _tags);

  static std::regex regex_expr{"^:(\\w+)!\\w+@\\S+ JOIN #(\\w+)"};

  std::cmatch match;
  std::regex_search(std::begin(data), std::end(data), match, regex_expr);

  if (match.empty()) {
    if (_raw.index() == STRING) {
      throw parsing_error{std::get<STRING>(_raw)};
    } else {
      throw parsing_error{};
    }
  }

  _channel = {match[2].first, (size_t)match[2].length()};
  _user = {match[1].first, (size_t)match[1].length()};
}
} // namespace irc
