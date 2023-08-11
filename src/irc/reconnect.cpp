#include "irc/reconnect.hpp"
#include <regex>

namespace irc {
void reconnect::parse() {
  std::string_view data = (std::string_view)(*this);

  static std::regex regex_expr{"RECONNECT"};

  std::cmatch match;
  std::regex_search(std::begin(data), std::end(data), match, regex_expr);

  if (match.empty()) {
    if (_raw.index() == STRING) {
      throw parsing_error{std::get<STRING>(_raw)};
    } else {
      throw parsing_error{};
    }
  }
}
} // namespace irc
