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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "config.h"

#define INITIAL_HASH_SZ     2053
#define MAX_LOAD_PERCENT    65

struct session {
    uint32_t laddr, raddr;
    uint16_t lport, rport;
    
    struct timeval tv;
    
    struct session *next;
    
};

struct hash {
    struct session *sessions;
    
    unsigned long sz, count;
        
};

static unsigned long
    hash_fun(uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport);
static int hash_set_internal(struct session *sessions, unsigned long sz,
        uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
        struct timeval tv);
static int hash_load_check(struct hash *hash);
static unsigned long hash_newsz(unsigned long sz);
    
unsigned long initial_hash_sz = INITIAL_HASH_SZ;

struct hash *
hash_new(void) {
    struct hash *ret;
    
    ret = malloc(sizeof(struct hash));
    if (!ret)
        abort();
    
    ret->sz = initial_hash_sz;
    ret->count = 0;
    
    // Don't change following ret->sz for initial_hash_sz. That wouldn't be
    // very thread_safe (not that the whole module is :)
    ret->sessions = malloc(ret->sz * sizeof(struct session));
    if (!ret->sessions)
        abort();
    memset(ret->sessions, 0, ret->sz * sizeof(struct session));
    
    return ret;
    
}

void
hash_del(struct hash *hash) {
    free(hash->sessions);
    free(hash);
    
}

int
hash_get(struct hash *hash,
         uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
         struct timeval *result)
{
    struct session *session;
    unsigned long port;
    
    port = hash_fun(laddr, raddr, lport, rport) % hash->sz;
    for (session = hash->sessions + port; session->next; session = session->next)
        if (
            session->next->raddr == laddr &&
            session->next->laddr == raddr &&
            session->next->rport == lport &&
            session->next->lport == rport
        )
        {
            *result = session->next->tv;
            return 1;
            
        }
        
    return 0;
    
}

int
hash_get_rem(struct hash *hash,
         uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
         struct timeval *result)
{
    struct session *session, *next;
    unsigned long port;

    port = hash_fun(laddr, raddr, lport, rport) % hash->sz;
    for (session = hash->sessions + port; session->next; session = session->next)
        if (
            session->next->raddr == raddr &&
            session->next->laddr == laddr &&
            session->next->rport == rport &&
            session->next->lport == lport
        )
        {
            *result = session->next->tv;
            
            // Now remove
            next = session->next->next;
            free(session->next);
            session->next = next;
            
            hash->count --;
            
            return 1;
            
        }
        
    return 0;
    

}

int
hash_set(struct hash *hash,
         uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
         struct timeval value)
{
    hash_load_check(hash);
    
    if (hash_set_internal(hash->sessions, hash->sz,
                             laddr, raddr, lport, rport, value))
    {
        hash->count ++;
        return 1;
    }
        
    return 0;
                             
}

int
hash_clean(struct hash *hash, unsigned long min) {
    unsigned long i;
 
    for (i = 0; i < hash->sz; i ++) {
        struct session *session;
        
        for (session = hash->sessions + i; session->next; session = session->next)
            if (session->next->tv.tv_sec * 1000000 + session->next->tv.tv_usec <
                    min)
            {
                struct session *next;
                
                next = session->next->next;
                free(session->next);
                session->next = next;
                
                hash->count --;
                
                // This break is to prevent a segmentation fault when
                // session->next is NULL (session will be null next)
                if (!session->next)
                    break;
                
            }
            
    }
    
    return 0;
    
}

static int
hash_set_internal(struct session *sessions, unsigned long sz,
         uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
         struct timeval value)
{
    struct session *session;
    unsigned long port;
    
    port = hash_fun(laddr, raddr, lport, rport) % sz;

    for (session = sessions + port; session->next; session = session->next)
        if (
            session->next->raddr == raddr &&
            session->next->laddr == laddr &&
            session->next->rport == rport &&
            session->next->lport == lport
        )
        {
            session->next->tv = value;
            
            return 0;
            
        }
    
    session->next = malloc(sizeof(struct session));
    if (!session->next)
        abort();
    
    session->next->raddr = raddr;
    session->next->laddr = laddr;
    session->next->rport = rport;
    session->next->lport = lport;
    
    session->next->tv = value;
    
    session->next->next = NULL;
    
    return 1;
    
}

static int
hash_load_check(struct hash *hash) {
    if ((hash->count * 100) / hash->sz > MAX_LOAD_PERCENT) {
        struct session *new_sessions, *old_sessions;
        unsigned long nsz, i;
        
        // New container
        nsz = hash_newsz(hash->sz);
        
        new_sessions = malloc(nsz * sizeof(struct session));
        if (!new_sessions)
            abort();
        
        memset(new_sessions, 0, nsz * sizeof(struct session));
        
        // Rehash
        for (i = 0; i < hash->sz; i ++) {
            struct session *session;
            
            for (session = hash->sessions + i; session->next;
                    session = session->next)
            {
                
                hash_set_internal(new_sessions, nsz, session->laddr,
                        session->raddr, session->lport, session->rport,
                        session->tv);
                        
            }
            
        }

        // Switch
        hash->sz = nsz;
        old_sessions = hash->sessions;
        hash->sessions = new_sessions;
        free(old_sessions);
        
        return 1;

    }
    
    return 0;
    
}

static unsigned long
hash_fun(uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport) {
    unsigned long ret;
    
#if SIZEOF_UNSIGNED_LONG >= 8
    ret = ((uint64_t) laddr << 32) | raddr;
    ret ^= ((uint64_t) lport << 48) | ((uint64_t) rport << 32) |
            ((uint64_t) lport << 16) | rport;
#elif SIZEOF_UNSIGNED_LONG == 4
    ret = laddr ^ raddr;
    ret ^= (lport << 16) | rport;
#else
#error Cannot determine sizeof(unsigned long)
#endif    

    return ret;

    
}

static unsigned long
hash_newsz(unsigned long sz) {
    return sz * 2 + 1;
    
}
