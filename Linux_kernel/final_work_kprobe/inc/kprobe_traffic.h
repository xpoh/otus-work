#ifndef KPROBE_TRAFFIC_H
#define KPROBE_TRAFFIC_H

#define MAX_TOP_ADDRS 100
#define ADDR_STATS_HASH_BITS 8

int kprobe_traffic_register(void);
void kprobe_traffic_unregister(void);

#endif
