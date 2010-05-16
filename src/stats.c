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

struct session {
    struct in_addr laddr, raddr;
    uint16_t lport, rport;
    
    struct timeval start;
    
    struct session *next;
    
} session_list;

long *stats;
unsigned statscount, statssz;

pthread_mutex_t sessions_mutex = PTHREAD_MUTEX_INITIALIZER,
    stats_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t clean_thread_id;

int exiting;

static void *clean_thread(void *);
static int lock_sessions(void), unlock_sessions(void),
    lock_stats(void), unlock_stats(void);

int
init_stats(void) {
    stats = malloc((statssz = INITIAL_STAT_SZ) * sizeof(long));
    if (!stats)
        abort();
    
    // Stat cleaner thread
    pthread_create(&clean_thread_id, NULL, clean_thread, NULL);
    
    return 0;
    
}

int
free_stats(void) {
    struct session *next;
    
    exiting = 1;
    pthread_kill(clean_thread_id, SIGINT);
    
    lock_sessions();
    
    while (session_list.next) {
        next = session_list.next->next;
        free(session_list.next);
        session_list.next = next;
        
    }
    
    unlock_sessions();
    
    lock_stats();
    
    free(stats);
    
    unlock_stats();
    
    return 0;
    
}

int
inbound(struct in_addr laddr, struct in_addr raddr,
        uint16_t lport, uint16_t rport)
{
    struct session *prev;
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    
    lock_sessions();
    
    for (prev = &session_list; prev->next; prev = prev->next)
        if (prev->next->laddr.s_addr == laddr.s_addr &&
            prev->next->raddr.s_addr == raddr.s_addr &&
            prev->next->lport == lport &&
            prev->next->rport == rport
        )
        {
            prev->next->start = tv;
            
            unlock_sessions();
            
            return 0;
        }
            
    prev->next = malloc(sizeof(struct session));
    if (!prev->next)
        abort();
    memset(prev->next, 0, sizeof(struct session));
    
    prev->next->laddr = laddr;
    prev->next->raddr = raddr;
    prev->next->lport = lport;
    prev->next->rport = rport;
    
    prev->next->start = tv;
    
    unlock_sessions();
    
    return 0;
    
}

int
outbound(struct in_addr laddr, struct in_addr raddr,
         uint16_t lport, uint16_t rport)
{
    struct session *prev;
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    
    lock_sessions();
    
    for (prev = &session_list; prev->next; prev = prev->next)
        if (prev->next->laddr.s_addr == laddr.s_addr &&
            prev->next->raddr.s_addr == raddr.s_addr &&
            prev->next->lport == lport &&
            prev->next->rport == rport
        )
        {
            struct session *next;
            long newstat;
            
            newstat = 
                    (tv.tv_sec - prev->next->start.tv_sec) * 1000000 +
                    (tv.tv_usec - prev->next->start.tv_usec);
                    
            next = prev->next->next;
            free(prev->next);
            prev->next = next;
            
            unlock_sessions();

            // Add a stat
            lock_stats();
            
            if (statscount == statssz) {
                exit(9);
                stats = realloc(stats, (statssz *= 2) * sizeof(long));
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

int
get_flush_stats(unsigned *count, long *average) {
    long avg = 0;
    unsigned i;
    
    lock_stats();
    
    *count = statscount;
    
    for (i = 0; i < statscount; i ++)
        avg += stats[i];
    
    avg /= statscount;
    
    // Flush
    statscount = 0;
    
    unlock_stats();
    
    *average = avg;
    
    return 0;
    
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
    struct session *prev;
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
        for (prev = &session_list; prev->next; )
            if (prev->next->start.tv_sec * 1000000 + prev->next->start.tv_usec >
                    tv.tv_sec * 1000000 + tv.tv_usec + TIMEOUT_USEC)
            {
                struct session *next;
                
                next = prev->next->next;
                free(prev->next);
                prev->next = next;
                
            }
            else
                prev = prev->next;
            
        unlock_sessions();
        
    }
    while (!exiting);
    
    return NULL;
    
}
