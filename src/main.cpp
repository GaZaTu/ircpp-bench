#include "irc/parse.hpp"
#include <fstream>
#include <chrono>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
  int LINES = 1000;
  int ITERATIONS = 1000;

  std::fstream file{"./data.txt"};
  std::vector<std::string> data;

  int i = 0;
  std::string line;
  while (std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }

    data.push_back(std::move(line));

    if (++i > LINES) {
      break;
    }
  }

  for (int i = 0; i < ITERATIONS; i++) {
    for (const auto& line : data) {
      irc::parse(line);

      // std::cout << line << std::endl;

      // auto privmsg = irc::get<irc::privmsg>(irc::parse(line));

      // for (const auto& [key, value] : privmsg.tags()) {
      //   std::cout << key << ": " << value << std::endl;
      // }

      // std::cout << "tags.size: " << privmsg.tags().size() << std::endl;
      // std::cout << "channel: " << privmsg.channel() << std::endl;
      // std::cout << "sender: " << privmsg.sender() << std::endl;
      // std::cout << "message: " << privmsg.message() << std::endl;

      // return 0;

      // irc::consumeTags(line);

      // auto [msg, tags] = irc::consumeTags(line);

      // for (const auto& [key, value] : tags) {
      //   std::cout << key << ": " << value << std::endl;
      // }

      // return 0;
    }
  }

  long long sum = 0;
  for (int i = 0; i < ITERATIONS; i++) {
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& line : data) {
      irc::parse(line);

      // irc::consumeTags(line);
    }

    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    sum += microseconds;
  }

  std::cout << (sum / ITERATIONS) << " us" << std::endl;

  return 0;
}
