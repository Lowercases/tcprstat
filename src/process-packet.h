#if !defined(PROCESS_PACKET_H)
#define PROCESS_PACKET_H

#include <netinet/ip.h>
#include <pcap.h>

void process_packet(unsigned char *, const struct pcap_pkthdr *,
                    const unsigned char *);
int process_ip(pcap_t *dev, const struct ip *ip);

#endif
