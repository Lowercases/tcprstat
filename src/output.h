/**
 *   tcprstat -- Extract stats about TCP response times
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

#if !defined(OUTPUT_H)
#define OUTPUT_H

#include <sys/time.h>

struct output_options {
    char *format;
    int interval;       // Seconds
    unsigned long iterations;
    
    int show_header;
    char *header;
    
};

// Parameter of output_thread is of type output_thread_options
void *output_thread(void *arg);
int output_offline_start(struct output_options *arg);
int output_offline_update(struct timeval tv);

int check_format(char format[]);

#endif

