#include "irc/privmsg.hpp"

#if __has_include("ctre.hpp")
#include "ctre.hpp"
#else
#include <regex>
#endif

namespace irc {
std::string_view privmsg::channel() const {
  return _channel;
}

std::string_view privmsg::sender() const {
  return _sender;
}

std::string_view privmsg::message() const {
  return _message;
}

bool privmsg::action() const {
  return _action;
}

void privmsg::parse() {
  std::string_view data = parseTags((std::string_view)(*this), _tags);

#if __has_include("ctre.hpp")
  auto match = ctre::starts_with<"^:([^!]+)![^@]+@[^ ]+ PRIVMSG #([^ ]+) :">(data);
  bool matched = !!match;
#else
  static std::regex regex_expr{"^:([^!]+)![^@]+@[^ ]+ PRIVMSG #([^ ]+) :"};

  std::cmatch match;
  std::regex_search(std::begin(data), std::end(data), match, regex_expr);
  bool matched = !match.empty();
#endif

  if (!matched) {
    if (_raw.index() == STRING) {
      throw parsing_error{std::get<STRING>(_raw)};
    } else {
      throw parsing_error{};
    }
  }

#if __has_include("ctre.hpp")
  _channel = match.get<2>().to_view();
  _sender = match.get<1>().to_view();
  _message = {match.end(), data.length() - match.get<0>().to_view().length()};
#else
  _channel = {match[2].first, (size_t)match[2].length()};
  _sender = {match[1].first, (size_t)match[1].length()};
  _message = {match.suffix().first, (size_t)match.suffix().length()};
#endif

  constexpr char ACTION_START[] = "\x01""ACTION";
  constexpr char ACTION_END[] = "\x01";

  if (_message.starts_with(ACTION_START) && _message.ends_with(ACTION_END)) {
    _action = true;
    _message = _message.substr(sizeof(ACTION_START), (_message.size() - sizeof(ACTION_START)) + sizeof(ACTION_END));
  }
}
} // namespace irc
