#include "irc/ping.hpp"
#include <regex>

namespace irc {
std::string_view ping::server() const {
  return _server;
}

void ping::parse() {
  std::string_view data = (std::string_view)(*this);

  static std::regex regex_expr{"^PING :(.*)"};

  std::cmatch match;
  std::regex_search(std::begin(data), std::end(data), match, regex_expr);

  if (match.empty()) {
    if (_raw.index() == STRING) {
      throw parsing_error{std::get<STRING>(_raw)};
    } else {
      throw parsing_error{};
    }
  }

  _server = {match[1].first, (size_t)match[1].length()};
}
} // namespace irc
