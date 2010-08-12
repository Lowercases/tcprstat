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

#include "tcprstat.h"
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

    // Second argument 0 stands for non-promiscuous mode
    pcap = pcap_open_live("any", CAPTURE_LENGTH, 0, READ_TIMEOUT, errbuf);
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

int
offline_capture(FILE *fcapture) {
    struct bpf_program bpf;
    char errbuf[PCAP_ERRBUF_SIZE];
    char filter[30];
    int r;

    pcap = pcap_fopen_offline(fcapture, errbuf);
    if (!pcap) {
        fprintf(stderr, "pcap: %s\n", errbuf);
        return 1;
        
    }
    
    // Capture only TCP
    if (port)
        sprintf(filter, "tcp port %d", port);
    else
        sprintf(filter, "tcp");
    
    if (pcap_compile(pcap, &bpf, filter, 1, 0)) {
        fprintf(stderr, "pcap: %s\n", pcap_geterr(pcap));
        return 1;
        
    }
    
    if (pcap_setfilter(pcap, &bpf)) {
        fprintf(stderr, "pcap: %s\n", pcap_geterr(pcap));
        return 1;
        
    }
    
    // The -1 here stands for "infinity"
    r = pcap_loop(pcap, -1, process_packet, (unsigned char *) pcap);
    if (r == -1) {
        fprintf(stderr, "pcap: %s\n", pcap_geterr(pcap));
        return 1;
        
    }
    
    return 1;
    
}

void
endcapture(void) {
    if (pcap)
        pcap_breakloop(pcap);
    
}
