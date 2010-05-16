#include <pcap.h>

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

struct address_list {
    struct in_addr in_addr;
    struct address_list *next;
    
} address_list;

int
get_addresses(void) {
    pcap_if_t *devlist, *curr;
    pcap_addr_t *addr;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct address_list *address_list_curr;

    if (pcap_findalldevs(&devlist, errbuf)) {
        fprintf(stderr, "pcap: %s\n", errbuf);
        return 1;
    }
    
    address_list_curr = &address_list;
    
    for (curr = devlist; curr; curr = curr->next) {
        if (curr->flags & PCAP_IF_LOOPBACK)
            continue;
            
        for (addr = curr->addresses; addr; addr = addr->next) {
            struct sockaddr *realaddr;
            
            if (addr->addr)
                realaddr = addr->addr;
            else if (addr->dstaddr)
                realaddr = addr->dstaddr;
            else
                continue;
            
            if (realaddr->sa_family == AF_INET ||
                    realaddr->sa_family == AF_INET6)
            {
                struct sockaddr_in *sin;
                
                sin = (struct sockaddr_in *) realaddr;
                
                address_list_curr->next = malloc(sizeof(struct address_list));
                if (!address_list_curr->next)
                    abort();
                
                address_list_curr->next->in_addr = sin->sin_addr;
                address_list_curr->next->next = NULL;
                address_list_curr = address_list_curr->next;
                
            }
            
        }
        
    }
    
    pcap_freealldevs(devlist);
    
    return 0;

}

int
free_addresses(void) {
    struct address_list *next;
    
    while (address_list.next) {
        next = address_list.next->next;
        free(address_list.next);
        address_list.next = next;
        
    }
    
    return 0;
    
}

int
is_local_address(struct in_addr addr) {
    struct address_list *curr;
    
    for (curr = address_list.next; curr; curr = curr->next)
        if (curr->in_addr.s_addr == addr.s_addr)
            return 1;
        
    return 0;
    
}
