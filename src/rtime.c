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
#include "stats.h"

struct option long_options[] = {
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'V' },
    
    { "port", required_argument, NULL, 'p' },

    { NULL, 0, NULL, '\0' }

};
char *short_options = "hVp";

pthread_t capture_thread_id;

// Global options
int port;

int
main(int argc, char *argv[]) {
    struct sigaction sa;
    char c;
    int option_index = 0;
    
    unsigned capturecount;
    long captureavg;

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
    
    pthread_join(capture_thread_id, NULL);
    
    get_flush_stats(&capturecount, &captureavg);
    
    free_stats();
    free_addresses();
    
    printf("Count: %d, avg: %ld\n", capturecount, captureavg);
    
    return EXIT_SUCCESS;

}

void
terminate(int signal) {
    endcapture();
        
}
