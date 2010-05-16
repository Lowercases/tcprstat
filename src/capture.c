#include "rtime.h"
#include "capture.h"
#include "process-packet.h"

#include <pcap.h>

pcap_t *pcap;

void *
capture(void *arg) {
    struct bpf_program bpf;
    char errbuf[PCAP_ERRBUF_SIZE];
    char filter[30];
    int r;

    pcap = pcap_open_live("any", 140000, 0, 2000, errbuf);
    if (!pcap) {
        fprintf(stderr, "pcap: %s\n", errbuf);
        return NULL;
        
    }
    
    // Capture only TCP
    if (port)
        sprintf(filter, "tcp port %d", port);
    else
        sprintf(filter, "tcp");
    
    if (pcap_compile(pcap, &bpf, filter, 1, 0)) {
        fprintf(stderr, "pcap: %s\n", pcap_geterr(pcap));
        return NULL;
        
    }
    
    if (pcap_setfilter(pcap, &bpf)) {
        fprintf(stderr, "pcap: %s\n", pcap_geterr(pcap));
        return NULL;
        
    }
    
    // The -1 here stands for "infinity"
    r = pcap_loop(pcap, -1, process_packet, (unsigned char *) pcap);
    if (r == -1) {
        fprintf(stderr, "pcap: %s\n", pcap_geterr(pcap));
        return NULL;
        
    }
    
    return NULL;
    
}

void
endcapture(void) {
    if (pcap)
        pcap_breakloop(pcap);
    
}
