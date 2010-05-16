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

#if !defined(STATS_H)
#define STATS_H

#include <stdint.h>
#include <netinet/in.h>

int init_stats(void);
int free_stats(void);

int inbound(struct in_addr laddr, struct in_addr raddr,
                uint16_t lport, uint16_t rport);
int outbound(struct in_addr laddr, struct in_addr raddr,
                uint16_t lport, uint16_t rport);
                
int get_flush_stats(unsigned *count, long *average);

#endif
