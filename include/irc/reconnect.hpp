#pragma once

#include "./common.hpp"

namespace irc {
struct reconnect : public detail::message_base_mixin, public detail::has_tags_mixin {
public:
  IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(reconnect);

protected:
  void parse();
};
} // namespace irc
