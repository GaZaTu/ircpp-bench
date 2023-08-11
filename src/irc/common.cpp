#include "irc/common.hpp"

#if __has_include("simd")
#include <simd>
#else
#include <experimental/simd>
#endif

namespace irc {
parsing_error::parsing_error() {
}

parsing_error::parsing_error(std::string& r) : raw(std::move(r)) {
}

tags_map::tags_map() {
}

size_t tags_map::size() const noexcept {
  return _list.size();
}

void tags_map::reserve(size_t n) {
  _list.reserve(n);
}

void tags_map::emplace(tags_map::value_type::first_type key, tags_map::value_type::second_type value) {
  _list.emplace_back(key, value);
}

tags_map::value_type::second_type tags_map::at(tags_map::value_type::first_type search) const {
  for (auto& [key, value] : _list) {
    if (key == search) {
      return value;
    }
  }

  return {};
}

tags_map::value_type::second_type tags_map::operator[](tags_map::value_type::first_type search) const {
  return at(search);
}

tags_map::list_type::const_iterator tags_map::begin() const noexcept {
  return _list.begin();
}

tags_map::list_type::const_iterator tags_map::end() const noexcept {
  return _list.end();
}

std::string_view parseTags(std::string_view raw, tags_map& tags) {
  constexpr char TAGS_BEGIN = '@';
  constexpr char KEY_END = '=';
  constexpr char VALUE_END = ';';
  constexpr char TAGS_END = ' ';

  if (raw[0] != TAGS_BEGIN) {
    return raw;
  }

  static int REQUIRED_CAPACITY = 16;
  tags.reserve(REQUIRED_CAPACITY);

#if __has_include("simd")
  namespace std_simd = std;
#else
  namespace std_simd = std::experimental;
#endif

  constexpr int SIMD_SIZE =
// 512bit SIMD is slow (on hardware we tested atleast: i3-1115G4, r9-7950X) so 256bit SIMD is forced here
#ifdef IRCPP_ALLOW_SIMD_512BIT
    std::is_trivially_constructible_v<std_simd::fixed_size_simd<char, 64>> ? 64 :
#endif
    std::is_trivially_constructible_v<std_simd::fixed_size_simd<char, 32>> ? 32 :
    std::is_trivially_constructible_v<std_simd::fixed_size_simd<char, 16>> ? 16 : 0;

  std_simd::fixed_size_simd<char, SIMD_SIZE> vector;

  bool looking_for_key = true;

  std::string_view key;
  std::string_view value;

  size_t offset = 1;
  for (size_t i = offset; i < raw.length(); i += vector.size()) {
    vector.copy_from(&raw[i], std_simd::element_aligned);

    auto mask = (vector == KEY_END) || (vector == VALUE_END) || (vector == TAGS_END);
    while (std_simd::any_of(mask)) {
      auto end_idx_in_vector = std_simd::find_first_set(mask);
      mask[end_idx_in_vector] = false;

      auto prev_offset = offset;
      auto end_idx = i + end_idx_in_vector;
      auto view = raw.substr(offset, end_idx - offset);
      offset = end_idx + 1;

      switch (raw[end_idx]) {
      case KEY_END:
        if (__builtin_expect(!looking_for_key, false)) {
          offset = prev_offset;
          break;
        }
        looking_for_key = false;
        key = view;
        break;

      case VALUE_END:
        looking_for_key = true;
        tags.emplace(key, view);
        break;

      case TAGS_END:
        tags.emplace(key, view);
        goto consume_tags_loop_exit;
      }
    }
  }
consume_tags_loop_exit:

  if (REQUIRED_CAPACITY < tags.size()) {
    REQUIRED_CAPACITY = tags.size();
  }

  return raw.substr(offset);
}

std::tuple<std::string_view, tags_map> parseTags(std::string_view raw) {
  tags_map tags;
  return {parseTags(raw, tags), std::move(tags)};
}

namespace detail {
message_base_mixin::message_base_mixin() : _raw(std::string_view{}) {
  // empty
}

message_base_mixin::message_base_mixin(std::string&& raw) : _raw(raw) {
  // empty
}

message_base_mixin::message_base_mixin(std::string_view raw) : _raw(raw) {
  // empty
}

message_base_mixin::operator std::string_view() const {
  if (_raw.index() == VIEW) {
    return std::get<VIEW>(_raw);
  } else {
    return std::get<STRING>(_raw);
  }
}

message_base_mixin::operator std::string() const {
  return (std::string)(std::string_view)(*this);
}

const tags_map& has_tags_mixin::tags() const {
  return _tags;
}

std::string_view has_tags_mixin::operator[](std::string_view key) const {
  return _tags.at(key);
}
} // namespace detail
} // namespace irc
