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
