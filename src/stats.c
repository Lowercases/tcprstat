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

/**
 * Of course this implementation has to change :)
 * Initial implementation: a simple linked list
 */

#define TIMEOUT_USEC           10000000
#define CLEAN_INTERVAL_USEC    2000000
#define INITIAL_STAT_SZ         2000

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>

#include <stdio.h>

#include "stats-hash.h"

struct hash *sessions;

unsigned long *stats;
unsigned statscount, statssz;

pthread_mutex_t sessions_mutex = PTHREAD_MUTEX_INITIALIZER,
    stats_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t clean_thread_id;

int exiting;

static void *clean_thread(void *);
static int lock_sessions(void), unlock_sessions(void),
    lock_stats(void), unlock_stats(void);

static unsigned long isqrt(unsigned long) __attribute__ ((const));
    
int
init_stats(void) {
    stats = malloc((statssz = INITIAL_STAT_SZ) * sizeof(unsigned long));
    if (!stats)
        abort();
    
    sessions = hash_new();
    if (!sessions)
        abort();
    
    // Stat cleaner thread
    pthread_create(&clean_thread_id, NULL, clean_thread, NULL);
    
    return 0;
    
}

int
free_stats(void) {
    exiting = 1;
    pthread_kill(clean_thread_id, SIGINT);
    pthread_join(clean_thread_id, NULL);
    
    lock_sessions();

    hash_del(sessions);
    
    unlock_sessions();
    
    lock_stats();
    
    free(stats);
    
    unlock_stats();
    
    return 0;
    
}

int
inbound(struct timeval tv, struct in_addr laddr, struct in_addr raddr,
        uint16_t lport, uint16_t rport)
{
    lock_sessions();
    
    hash_set(sessions, laddr.s_addr, raddr.s_addr, lport, rport, tv);
    
    unlock_sessions();
    
    return 0;
    
}

int
outbound(struct timeval tv, struct in_addr laddr, struct in_addr raddr,
         uint16_t lport, uint16_t rport)
{
    struct timeval start;
    unsigned long newstat;
    
    lock_sessions();
    
    if (hash_get_rem(sessions, laddr.s_addr, raddr.s_addr, lport, rport, &start))
    {
        newstat = (tv.tv_sec - start.tv_sec) * 1000000 +
                    (tv.tv_usec - start.tv_usec);
                    
        unlock_sessions();

        // Add a stat
        lock_stats();
        
        if (statscount == statssz) {
            stats = realloc(stats, (statssz *= 2) * sizeof(unsigned long));
            if (!stats)
                abort();
            
        }
            
        stats[statscount ++] = newstat;
        
        unlock_stats();
        
        return 0;
        
    }
            
    unlock_sessions();
    
    return 1;    
    
}

static int
lock_stats(void) {
    return pthread_mutex_lock(&stats_mutex);
    
}

static int
unlock_stats(void) {
    return pthread_mutex_unlock(&stats_mutex);
    
}

static int
lock_sessions(void) {
    return pthread_mutex_lock(&sessions_mutex);
    
}

static int
unlock_sessions(void) {
    return pthread_mutex_unlock(&sessions_mutex);
    
}

static void *
clean_thread(void *arg) {
    struct timeval tv;
    struct timespec ts = {
        CLEAN_INTERVAL_USEC / 1000000,
        (CLEAN_INTERVAL_USEC % 1000000) * 1000
    };
    
    do {
        
        nanosleep(&ts, NULL);
    
        lock_sessions();
        
        gettimeofday(&tv, NULL);
        
        // Notice we only advance when we don't delete
        hash_clean(sessions, tv.tv_sec * 1000000 + tv.tv_usec - TIMEOUT_USEC);
            
        unlock_sessions();
        
    }
    while (!exiting);
    
    return NULL;
    
}

/*** Results ***/
struct stats_results {
    unsigned long *stats;
    unsigned statscount, statssz;
    
    int sorted;
    
};

static void sort_results(struct stats_results *results);
int compare_stats(const void *, const void *);

struct stats_results *
get_flush_stats(void) {
    struct stats_results *ret;
    
    ret = malloc(sizeof(struct stats_results));
    if (!ret)
        abort();
    memset(ret, 0, sizeof(struct stats_results));
    
    lock_stats();
    
    ret->stats = stats;
    ret->statscount = statscount;
    ret->statssz = statssz;
    
    ret->sorted = 0;
    
    stats = malloc((statssz = INITIAL_STAT_SZ) * sizeof(unsigned long));
    if (!stats)
        abort();
    statscount = 0;
    
    unlock_stats();
    
    return ret;
    
}

static void
sort_results(struct stats_results *results) {
    qsort(results->stats, results->statscount, sizeof(unsigned long),
          compare_stats);
    results->sorted = 1;
    
}

int
compare_stats(const void *void1, const void *void2) {
    const unsigned long *stat1, *stat2;
    stat1 = void1;
    stat2 = void2;
    
    if (*stat1 < *stat2)
        return -1;
    else if (*stat1 > *stat2)
        return 1;
    else
        return 0;
    
}

int
free_results(struct stats_results *results) {
    free(results->stats);
    free(results);
    
    return 0;
    
}

unsigned
stats_count(struct stats_results *results, int percentile) {
    if (percentile == 0 || percentile == 100)
        return results->statscount;
    
    return (results->statscount * percentile) / 100;
    
}

unsigned long
stats_avg(struct stats_results *results, int percentile) {
    unsigned long n;
    unsigned long avg = 0;
    unsigned long i;
    
    if (!results->statscount)
        return 0;

    if (percentile == 0 || percentile == 100)
        n = results->statscount;
    else {
        if (!results->sorted)
            sort_results(results);
        
        n = (results->statscount * percentile ) / 100;
        
    }
    
    if (!n)
        return 0;
    
    for (i = 0; i < n; i ++)
        avg += results->stats[i];
    
    avg /= n;
    
    return avg;
    
}

unsigned long
stats_sum(struct stats_results *results, int percentile) {
    unsigned long n;
    unsigned long sum = 0;
    unsigned long i;
    
    if (!results->statscount)
        return 0;

    if (percentile == 0 || percentile == 100)
        n = results->statscount;
    else {
        if (!results->sorted)
            sort_results(results);
        
        n = (results->statscount * percentile ) / 100;
        
    }
    
    if (!n)
        return 0;
    
    for (i = 0; i < n; i ++)
        sum += results->stats[i];
    
    return sum;
    
}

unsigned long
stats_sqs(struct stats_results *results, int percentile) {
    unsigned long n;
    unsigned long sqs = 0;
    unsigned long i;
    
    if (!results->statscount)
        return 0;

    if (percentile == 0 || percentile == 100)
        n = results->statscount;
    else {
        if (!results->sorted)
            sort_results(results);
        
        n = (results->statscount * percentile ) / 100;
        
    }
    
    if (!n)
        return 0;
    
    for (i = 0; i < n; i ++)
        sqs += results->stats[i] * results->stats[i];
    
    return sqs;
    
}

unsigned long
stats_min(struct stats_results *results, int percentile) {
    if (!results->statscount)
        return 0;
    
    if (!results->sorted)
        sort_results(results);
    
    return results->stats[0];
    
}

unsigned long
stats_max(struct stats_results *results, int percentile) {
    unsigned long n;
    
    if (!results->statscount)
        return 0;
    
    if (percentile == 0 || percentile == 100)
        n = results->statscount;
    else
        n = (results->statscount * percentile) / 100;
    
    if (!n)
        return 0;   // Is this correct? or should [0] be returned?
    
    if (!results->sorted)
        sort_results(results);
    
    return results->stats[n - 1];
    
}

unsigned long
stats_med(struct stats_results *results, int percentile) {
    unsigned long n;
    
    if (!results->statscount)
        return 0;
    
    if (percentile == 0 || percentile == 100)
        n = results->statscount;
    else
        n = (results->statscount * percentile) / 100;
    
    if (!n)
        return 0;   // Is this correct? or should [0] be returned?
    
    if (!results->sorted)
        sort_results(results);
    
    return results->stats[n / 2];
    
}

unsigned long
stats_var(struct stats_results *results, int percentile) {
    unsigned long avg, var;
    unsigned long n;
    unsigned long i;
    
    if (!results->statscount)
        return 0;
    
    if (percentile == 0 || percentile == 100)
        n = results->statscount;
    else
        n = (results->statscount * percentile) / 100;
    
    if (!n)
        return 0;   // Is this correct? or should [0] be returned?
    
    avg = stats_avg(results, percentile);
    
    // Variance is the sum of squares, divided by n, less the square of the avg
    
    // Sum of squares
    var = 0;
    for (i = 0; i < n; i ++)
        var += results->stats[i] * results->stats[i];
    
    var /= n;
    var -= avg * avg;
    
    return var;
    
}

unsigned long
stats_std(struct stats_results *results, int percentile) {
    return isqrt(stats_var(results, percentile));
    
}

// Based in code from
// http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C
// It stipulates that content is available under the
// GNU Free Documentation License
static unsigned long
isqrt(unsigned long x)
{
    unsigned long op, res, one;

    op = x;
    res = 0;

    // "one" starts at the highest power of four <= than the argument.
    one = 1;
    while (one < op) one <<= 2;
    while (one > op) one >>= 2;

    while (one) {
        if (op >= res + one) {
            op -= res + one;
            res += one << 1;
        }
        res >>= 1;
        one >>= 2;
    }
    
    return res;
    
}
