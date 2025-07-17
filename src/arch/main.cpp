#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>

#include "arch/syntax/parser.h"

using namespace std;

constexpr char arch_usage_string[] =
    "arch [-v | --version] [-h | --help]";

static void runREPL() {
  std::string line_buffer;
  while (true) {
    /* prompt input */
    std::cout << "> ";

    /* read input line; exit loop if EOF */
    if (!std::getline(std::cin, line_buffer)) {
      std::cout << std::endl;
      break;
    }

    /* parse user input */
    auto result = Arch::Syntax::parse(line_buffer);

    /* handle exit */
    if (result.exit()) break;
    if (result.ok()) {};
  }
}

static void runFile(const std::string &path);

int main(const int argc, char *argv[]) {
  if (argc == 1) return runREPL(), 0;
  if (argc == 2) return runFile(argv[1]), 0;

  /* invalid arguments */
  std::cout << arch_usage_string << std::endl;
  return 1;
}

static std::string readFile(const std::string &path) {
  /* open file */
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error("File not found: " + path);
  }

  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file) {
    throw std::runtime_error("Cannot open file: " + path);
  }

  /* pre-size string, then read */
  const std::streamsize size = file.tellg();
  if (size < 0) {
    throw std::runtime_error("Failed to get file size: " + path);
  }
  file.seekg(0, std::ios::beg);

  std::string contents(size, '\0');
  if (!file.read(contents.data(), size)) {
    throw std::runtime_error("Failed to read file: " + path);
  }

  return contents;
}

void runFile(const std::string &path) {
  const std::string source = readFile(path);
  auto result = Arch::Syntax::parse(source);
}
