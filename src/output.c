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
 
#include "output.h"
#include "stats.h"
#include "capture.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static int output(char format[], unsigned long iterations);

time_t timestamp;

void *
output_thread(void *arg) {
    struct output_options *options;
    struct timespec ts;
    unsigned long iterations;
    
    options = arg;
    
    ts = (struct timespec) { options->interval, 0 };
    
    if (!check_format(options->format))
        abort();
    
    time(&timestamp);
    
    for (iterations = 0; !options->iterations || iterations < options->iterations;
            iterations ++)
    {
        nanosleep(&ts, NULL);
        
        output(options->format, iterations);
        
    }
    
    // Iterations finished, signal capturing process
    endcapture();
    
    return NULL;
    
}

static int
output(char format[], unsigned long iterations) {
    char *c;
    time_t current = 0;
    
    struct stats_results *results;
    
    results = get_flush_stats();
    
    for (c = format; c[0]; c ++)
        if (c[0] == '%') {
            int r = 100;
            c ++;
            
            if (c[0] >= '0' && c[0] <= '9') {
                r = 0;
                while (c[0] >= '0' && c[0] <= '9') {
                    r *= 10;
                    r += c[0] - '0';
                    
                    c ++;
                    
                }
                
            }
            
            if (c[0] == 'n')
                printf("%u", stats_count(results, r));
            else if (c[0] == 'a')
                printf("%lu", stats_avg(results, r));
            
            // Timestamping
            else if (c[0] == 'I')
                printf("%lu", iterations);
            else if (c[0] == 't' || c[0] == 'T') {
                if (!current)
                    time(&current);
                
                printf("%lu", current - (c[0] == 't' ? timestamp : 0));
                
            }
            
            // Actual %
            else if (c[0] == '%')
                fputc(c[0], stdout);
            
        }
        else if (c[0] == '\\')
            if (c[1] == 'n') {
                c ++;
                fputc('\n', stdout);
            }
            else if (c[1] == 't') {
                c ++;
                fputc('\t', stdout);
            }
            else if (c[1] == 'r') {
                c ++;
                fputc('\r', stdout);
            }
            else if (c[1] == '\\') {
                c ++;
                fputc('\\', stdout);
            }
            else
                fputc('\\', stdout);
            
        else
            fputc(c[0], stdout);
        
    fflush(stdout);
    
    free_results(results);
    
    return 0;

}

int
check_format(char format[]) {
    char *c;
    
    for (c = format; *c; c ++)
        if (c[0] == '%') {
            int r = -1;
            c ++;
            
            switch (c[0]) {
            
            case '0' ... '9':       // GNU extension
                r = 0;
                while (c[0] >= '0' && c[0] <= '9') {
                    r *= 10;
                    r += c[0] - '0';
                    
                    c ++;
                    
                }
                
                if (r <= 0 || r > 100)
                    return 0;
                
            case '%':
                if (r != -1)
                    return 0;
                
            case 'n':
            case 'a':
            case 'r':
            case 'I':
            case 't':
            case 'T':
                break;

            default:
                return 0;
                
            }
            
        }
        
    return 1;
    
}

