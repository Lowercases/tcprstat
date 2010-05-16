#if !defined(LOCAL_ADDRESSES_H)
#define LOCAL_ADDRESSES_H

#include <netinet/in.h>

int get_addresses(void);
int free_addresses(void);

int is_local_address(struct in_addr);

#endif
