#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

struct option long_options[] = {
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'V' },

    { NULL, 0, NULL, '\0' }

};
char *short_options = "hV";

int
main(int argc, char *argv[]) {
    char c;
    int option_index = 0;

    do {
        c = getopt_long(argc, argv, short_options, long_options, &option_index);

        switch (c) {

        case -1:
            break;

        case 'h':
            dump_help();
            return EXIT_SUCCESS;

        case 'V':
            dump_version();
            return EXIT_SUCCESS;

        default:
            dump_usage();
            return EXIT_FAILURE;

        }

    }
    while (c != -1);

    return EXIT_SUCCESS;

}
