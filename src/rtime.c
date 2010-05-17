/**
 *   rtime -- Extract stats about TCP response times
 *   Copyright (C) 2010  Ignacio Nin
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
**/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>

#include "rtime.h"
#include "functions.h"
#include "local-addresses.h"
#include "capture.h"
#include "output.h"
#include "stats.h"

struct option long_options[] = {
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'V' },
    
    { "port", required_argument, NULL, 'p' },
    { "format", required_argument, NULL, 'f' },
    { "interval", required_argument, NULL, 't' },

    { NULL, 0, NULL, '\0' }

};
char *short_options = "hVpf:t:";

pthread_t capture_thread_id, output_thread_id;

// Global options
int port;
int interval = 30;
struct output_options output_options = {
    DEFAULT_OUTPUT_FORMAT,
    DEFAULT_OUTPUT_INTERVAL,
};

int
main(int argc, char *argv[]) {
    struct sigaction sa;
    char c;
    int option_index = 0;
        
    // Parse command line options
    do {
        c = getopt_long(argc, argv, short_options, long_options, &option_index);

        switch (c) {

        case -1:
            break;
            
        case 'p':
            port = strtol(optarg, NULL, 0);
            if (port <= 0 || port > 65535) {
                fprintf(stderr, "Invalid port\n");
                return EXIT_FAILURE;
            }
            
            break;
            
        case 'f':
            if (!check_format(optarg)) {
                fprintf(stderr, "Bad format provided: `%s'\n", optarg);
                return EXIT_FAILURE;
            }
            
            output_options.format = optarg;
            
            break;
            
        case 't':
            output_options.interval = strtol(optarg, NULL, 10);
            if (interval <= 0 || interval >= MAX_OUTPUT_INTERVAL) {
                fprintf(stderr, "Bad interval provided\n");
                return EXIT_FAILURE;
            }
            
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
    
    // Set up signals
    sa.sa_handler = terminate;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;
    sa.sa_restorer = NULL;
    
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    
    // Get local addresses
    if (get_addresses() != 0)
        return EXIT_FAILURE;
    
    // Stats
    init_stats();
    
    // Fire up capturing thread
    pthread_create(&capture_thread_id, NULL, capture, NULL);
    
    // Options thread
    pthread_create(&output_thread_id, NULL, output_thread, &output_options);
    
    pthread_join(capture_thread_id, NULL);
    pthread_kill(output_thread_id, SIGINT);
    
    free_stats();
    free_addresses();
    
    return EXIT_SUCCESS;

}

void
terminate(int signal) {
    endcapture();
        
}
