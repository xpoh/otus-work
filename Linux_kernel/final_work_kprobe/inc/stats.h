//
// Created by user on 09.05.2026.
//

#ifndef FINAL_WORK_KPROBE_STATS_H
#define FINAL_WORK_KPROBE_STATS_H

#include <linux/stddef.h>
#include <linux/types.h>

#define MAX_TOP_ADDRS 100
#define ADDR_STATS_HASH_BITS 8

struct addr_stat {
  struct hlist_node node;
  __be32 saddr;
  __be32 daddr;
  u64 recv_bytes;
  u64 send_bytes;
};

void stats_init(void);
void stats_exit(void);
void stats_record_traffic(__be32 saddr, __be32 daddr, bool is_send,
                         size_t bytes);
void stats_get_top(char *buf, size_t len, int top_n);

#endif // FINAL_WORK_KPROBE_STATS_H
