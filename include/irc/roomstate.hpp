#pragma once

#include "./common.hpp"

namespace irc {
struct roomstate : public detail::message_base_mixin, public detail::has_tags_mixin {
public:
  IRC_MESSAGE_BASE_MIXIN_CONSTRUCTORS(roomstate);

  std::string_view channel() const;

protected:
  std::string_view _channel;

  void parse();
};
} // namespace irc
