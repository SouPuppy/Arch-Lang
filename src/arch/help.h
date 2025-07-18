#pragma once

inline void list_cmds_help() {
  puts("These are common Arch commands used in various situations:");

  puts("\nMeta Commands");
  puts("    help       Show help for commands");
  puts("    version    Show version information");

  puts("\nVersion Control");
  puts("    list       List available versions of a module");
  puts("    switch     Change the current version of a module");

  puts("\nProject");
  puts("    init       Initialize an arch project");
}
