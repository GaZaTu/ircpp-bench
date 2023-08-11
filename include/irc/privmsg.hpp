#pragma once

#include "./common.hpp"

namespace irc {
struct privmsg : public detail::message_base_mixin, public detail::has_tags_mixin {
public:
  IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(privmsg);

  std::string_view channel() const;

  std::string_view sender() const;

  std::string_view message() const;

  bool action() const;

protected:
  std::string_view _channel;
  std::string_view _sender;
  std::string_view _message;
  bool _action = false;

  void parse();
};
} // namespace irc
