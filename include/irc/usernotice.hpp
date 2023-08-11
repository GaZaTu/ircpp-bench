#pragma once

#include "./common.hpp"

namespace irc {
struct usernotice : public detail::message_base_mixin, public detail::has_tags_mixin {
public:
  IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(usernotice);

  std::string_view channel() const;

  std::string_view message() const;

protected:
  std::string_view _channel;
  std::string_view _message;

  void parse();
};
} // namespace irc
