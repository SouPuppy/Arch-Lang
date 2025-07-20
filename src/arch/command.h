#include <string>
#include <memory>
#include <filesystem>
#include "arch/icli/icli.h"

inline bool check_project_exists(const std::string& path) {
  return std::filesystem::exists(path + "/.arch");
}

inline void project_initialize(const std::filesystem::path& dot_arch_path) {
  std::filesystem::create_directory(dot_arch_path);
  
  std::filesystem::create_directory(dot_arch_path / "modules");
}

inline void cmd_init_project(int argc, const char *argv[]) {
  std::string project_path = ".";
  if (argc != 0) project_path = argv[0];

  /* check if exists */
  if (!std::filesystem::exists(project_path)) {
    std::cerr << "Invalid directory to initialize an `arch` project\n";
    exit(1);
  }
  std::filesystem::path dot_arch_path = std::filesystem::path(project_path) / ".arch";
  
  Interactive_CLI icli("Initialize an Arch project", {
    std::make_shared<CLI_PromptContinue>(
      "Directory already initialized. Continue?",
      [=]() { return check_project_exists(project_path); },
      [=]() {}
    ),
    std::make_shared<CLI_Execute>(
      [=]() { return true; },
      [=]() { project_initialize(dot_arch_path); }
    )
  });

  icli.run();
}
