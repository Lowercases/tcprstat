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

#if !defined(STATS_H)
#define STATS_H

#include <stdint.h>
#include <netinet/in.h>

struct stats_results;

int init_stats(void);
int free_stats(void);

int inbound(struct timeval tv, struct in_addr laddr, struct in_addr raddr,
                uint16_t lport, uint16_t rport);
int outbound(struct timeval tv, struct in_addr laddr, struct in_addr raddr,
                uint16_t lport, uint16_t rport);
                
struct stats_results *get_flush_stats(void);
int free_results(struct stats_results *);

unsigned stats_count(struct stats_results *r, int percentile);
unsigned long stats_avg(struct stats_results *r, int percentile);
unsigned long stats_sum(struct stats_results *r, int percentile);
unsigned long stats_sqs(struct stats_results *r, int percentile);
unsigned long stats_min(struct stats_results *r, int percentile);
unsigned long stats_max(struct stats_results *r, int percentile);
unsigned long stats_med(struct stats_results *r, int percentile);
unsigned long stats_var(struct stats_results *r, int percentile);
unsigned long stats_std(struct stats_results *r, int percentile);

#endif
