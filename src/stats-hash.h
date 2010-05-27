#if !defined(STATS_HASH_H)
#define STATS_HASH_H

struct hash;

struct hash *hash_new(void);
void hash_del(struct hash *hash);
int hash_get(struct hash *hash,
         uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
         struct timeval *result);
int hash_get_rem(struct hash *hash,
         uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
         struct timeval *result);
int hash_set(struct hash *hash,
         uint32_t laddr, uint32_t raddr, uint16_t lport, uint16_t rport,
         struct timeval value);
         
int hash_clean(struct hash *hash, unsigned long min);
         
#endif
