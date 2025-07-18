#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <arch/version.h>
#include "help.h"
#include "command.h"

using namespace std;

constexpr char arch_usage_string[] =
    "    arch [-h | --help] [-v | --version]\n"
    "        | <command> [<args>]";

constexpr char arch_more_info_string[] =
    "'arch help' list all available commands and some general\n"
    "information. See 'arch help <command>' to read about a \n"
    "specific command.";


int main(int argc, const char * argv[]) {

    /* skip the first argument (path) */
    argc--;
    argv++;
    if (!argv[0]) argv[0] = "--help";

    /* [-v | --version] */
    printf("arch %s (%s %s)\n", ARCH_VERSION_COMPLETE, ARCH_COMMIT_HASH, ARCH_COMMIT_DATE);
    if (!strcmp("--version", argv[0]) || !strcmp("-v", argv[0])) {
        exit(0);
    }

    /* [-h | --help] */
    if (!argc) {
        /* no input from the user; default helps */
        printf("\nUsage:\n%s\n\n", arch_usage_string);
        list_cmds_help();
        printf("\n%s\n\n", arch_more_info_string);
        exit(1);
    }

    if (!strcmp("--help", argv[0]) || !strcmp("-h", argv[0])) {
        exit(0);
    }

    /* execute command */
    if (!strcmp(argv[0], "init")) {
        cmd_init_project(argc - 1, argv + 1);
        return 0;
    }

    return 0;
}
