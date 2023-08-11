#include "irc/clearchat.hpp"
#include <regex>

namespace irc {
std::string_view clearchat::channel() const {
  return _channel;
}

std::string_view clearchat::targetUser() const {
  return _targetUser;
}

void clearchat::parse() {
  std::string_view data = parseTags((std::string_view)(*this), _tags);

  static std::regex regex_expr{"^:\\S+ CLEARCHAT #(\\w+) :(\\w+)"};

  std::cmatch match;
  std::regex_search(std::begin(data), std::end(data), match, regex_expr);

  if (match.empty()) {
    if (_raw.index() == STRING) {
      throw parsing_error{std::get<STRING>(_raw)};
    } else {
      throw parsing_error{};
    }
  }

  _channel = {match[1].first, (size_t)match[1].length()};
  _targetUser = {match[2].first, (size_t)match[2].length()};
}
} // namespace irc
