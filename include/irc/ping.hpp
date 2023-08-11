#pragma once

#include "./common.hpp"

namespace irc {
struct ping : public detail::message_base_mixin {
public:
  IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(ping);

  std::string_view server() const;

protected:
  std::string_view _server;

  void parse();
};
} // namespace irc
