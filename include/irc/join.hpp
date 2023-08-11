#pragma once

#include "./common.hpp"

namespace irc {
struct join : public detail::message_base_mixin, public detail::has_tags_mixin {
public:
  IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(join);

  std::string_view channel() const;

  std::string_view user() const;

protected:
  std::string_view _channel;
  std::string_view _user;

  void parse();
};
} // namespace irc
