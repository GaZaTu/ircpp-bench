#pragma once

#include <string>

namespace irc::out {
using namespace std::string_literals;

inline std::string pass(std::string_view pass) {
  return "PASS "s + (std::string)pass;
}

inline std::string nick(std::string_view nick) {
  return "NICK "s + (std::string)nick;
}

inline std::string pong(std::string_view src) {
  return "PONG :"s + (std::string)src;
}

inline std::string privmsg(std::string_view chn, std::string_view msg) {
  return "PRIVMSG #"s + (std::string)chn + " :"s + (std::string)msg;
}

inline std::string join(std::string_view chn) {
  return "JOIN #"s + (std::string)chn;
}

inline std::string part(std::string_view chn) {
  return "PART #"s + (std::string)chn;
}

inline std::string capreq(std::string_view cap) {
  return "CAP REQ :"s + (std::string)cap;
}
} // namespace irc::out
