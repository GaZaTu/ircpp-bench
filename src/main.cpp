#include "irc/parse.hpp"
#include <fstream>
#include <chrono>
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
  std::fstream file{"./data.txt"};
  std::vector<std::string> data;

  int i = 0;
  std::string line;
  while (std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }

    data.push_back(std::move(line));

    if (++i > 1000) {
      break;
    }
  }

  for (int i = 0; i < 1000; i++) {
    for (const auto& line : data) {
      irc::parse(line);
    }
  }

  long long sum = 0;
  for (int i = 0; i < 1000; i++) {
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& line : data) {
      irc::parse(line);
    }

    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    sum += microseconds;
  }

  std::cout << (sum / 1000) << " us" << std::endl;
}
