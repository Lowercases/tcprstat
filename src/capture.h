#if !defined CAPTURE_H
#define CAPTURE_H

#include <pcap.h>

void *capture(void *);
void endcapture(void);

#endif
