#include "irc/parse.hpp"

namespace irc {
std::string_view stringify(const message& m) {
  std::string_view result;

  detail::constexpr_for<(size_t)0, std::variant_size_v<message>, (size_t)1>([&result, &m](auto i) {
    if (m.index() != i) {
      return true;
    }

    result = (std::string_view)std::get<i>(m);
    return false;
  });

  return result;
}
} // namespace irc
