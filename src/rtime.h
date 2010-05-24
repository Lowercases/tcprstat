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

#if !defined(RTIME_H)
#define RTIME_H

#define DEFAULT_OUTPUT_FORMAT \
    "%T\\t%n\\t%s\\t%M\\t%m\\t%a\\t%h\\t%S\\t" \
    "%90M\\t%90a\\t%90S\\t%99M\\t%99a\\t%99S\\n"
#define DEFAULT_OUTPUT_INTERVAL 10
#define MAX_OUTPUT_INTERVAL 60 * 60 * 24
#define DEFAULT_OUTPUT_ITERATIONS 1

#define DEFAULT_SHOW_HEADER 1

extern int port;
extern char *program_name;

void terminate(int signal);

#endif
