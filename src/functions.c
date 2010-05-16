#include <stdio.h>

#include "config.h"
#include "functions.h"

char *usage_msg =
        "Usage:\n"
        "\t--help           Shows program information and usage.\n"
        "\t--version        Shows version information.\n"
        "\n"
;

int
dump_usage(void) {
    fprintf(stderr, usage_msg);

    return 0;

}

int
dump_help(void) {
    dump_version();
    fprintf(stdout, usage_msg);

    return 0;

}

int
dump_version(void) {
    fprintf(stdout, "%s %s.\n", PACKAGE_NAME, PACKAGE_VERSION);

    return 0;

}
