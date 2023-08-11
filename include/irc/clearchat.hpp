#pragma once

#include "./common.hpp"

namespace irc {
struct clearchat : public detail::message_base_mixin, public detail::has_tags_mixin {
public:
  IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(clearchat);

  std::string_view channel() const;

  std::string_view targetUser() const;

protected:
  std::string_view _channel;
  std::string_view _targetUser;

  void parse();
};
} // namespace irc
