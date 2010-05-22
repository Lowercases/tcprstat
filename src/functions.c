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

#include <stdio.h>

#include "config.h"
#include "functions.h"
#include "rtime.h"

char *usage_msg =
    "Usage: rtime [--port <port> | --version | --help ]\n"
    "\t--port <port>, -p    Capture traffic only for tcp/<port>.\n"
    "\t--format <format>, -f\n"
    "\t                     Output format. Argument is a string detailing\n"
    "\t                     how the information is presented. Accepted codes:\n"
    "\t                         %n - Response time count\n"
    "\t                         %a - Response time media in microseconds\n"
    "\t                         %I - Iteration number\n"
    "\t                         %t - Timestamp since iteration zero\n"
    "\t                         %T - Unix timestamp\n"
    "\t                         %% - A literal %\n"
    "\t                     Default is \"" DEFAULT_OUTPUT_FORMAT "\".\n"
    "\t--interval <seconds>, -t\n"
    "\t                     Output interval. Default is "
                                    DEFAULT_OUTPUT_INTERVAL_STR ".\n"
    "\t--iterations <n>, -n\n"
    "\t                     Output iterations. Default is "
                                    DEFAULT_OUTPUT_ITERATIONS_STR
                                    ", 0 means infinity\n"
    "\n"
    "\t--help               Shows program information and usage.\n"
    "\t--version            Shows version information.\n"
    "\n"
;

int
dump_usage(void) {
    fprintf(stderr, "%s", usage_msg);

    return 0;

}

int
dump_help(void) {
    dump_version();
    fprintf(stdout, "%s", usage_msg);

    return 0;

}

int
dump_version(void) {
    fprintf(stdout, "%s %s.\n", PACKAGE_NAME, PACKAGE_VERSION);

    return 0;

}
