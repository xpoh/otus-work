#define pr_fmt(fmt) KBUILD_MODNAME ": stats: " fmt

#include <linux/hashtable.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sort.h>
#include <linux/spinlock.h>

#include <stats.h>

static DEFINE_HASHTABLE(addr_stats, ADDR_STATS_HASH_BITS);
static DEFINE_SPINLOCK(stats_lock);

void stats_init(void) {
  hash_init(addr_stats);
  pr_debug("stats initialized\n");
}

void stats_exit(void) {
  struct addr_stat *entry;
  struct hlist_node *tmp;
  int bkt;

  spin_lock(&stats_lock);
  hash_for_each_safe(addr_stats, bkt, tmp, entry, node) {
    hash_del(&entry->node);
    kfree(entry);
  }
  spin_unlock(&stats_lock);
}

void stats_record_traffic(__be32 saddr, __be32 daddr, size_t bytes) {
  struct addr_stat *entry;
  u64 key = ((u64)saddr << 32) | (u32)daddr;

  spin_lock(&stats_lock);
  hash_for_each_possible(addr_stats, entry, node, key) {
    if (entry->saddr == saddr && entry->daddr == daddr) {
      entry->bytes += bytes;
      spin_unlock(&stats_lock);
      return;
    }
  }

  entry = kmalloc(sizeof(*entry), GFP_ATOMIC);
  if (!entry) {
    spin_unlock(&stats_lock);
    return;
  }

  entry->saddr = saddr;
  entry->daddr = daddr;
  entry->bytes = bytes;
  hash_add(addr_stats, &entry->node, key);
  spin_unlock(&stats_lock);
}

static int cmp_bytes_desc(const void *a, const void *b) {
  const struct addr_stat *sa = *(const struct addr_stat **)a;
  const struct addr_stat *sb = *(const struct addr_stat **)b;

  if (sa->bytes < sb->bytes)
    return 1;
  if (sa->bytes > sb->bytes)
    return -1;
  return 0;
}

void stats_get_top(char *buf, size_t len, int top_n) {
  struct addr_stat **entries;
  struct addr_stat *entry;
  int bkt;
  int count = 0;
  int pos = 0;
  int allocated;
  int i;

  entries = kmalloc_array(MAX_TOP_ADDRS, sizeof(*entries), GFP_KERNEL);
  if (!entries)
    return;

  spin_lock(&stats_lock);
  hash_for_each(addr_stats, bkt, entry, node) {
    if (count >= MAX_TOP_ADDRS)
      break;
    entries[count++] = entry;
  }
  spin_unlock(&stats_lock);

  if (count == 0) {
    buf[0] = '\0';
    kfree(entries);
    return;
  }

  allocated = count;
  if (top_n > count)
    top_n = count;

  sort(entries, allocated, sizeof(*entries), cmp_bytes_desc, NULL);

  for (i = 0; i < top_n && pos < len; i++) {
    int n = scnprintf(buf + pos, len - pos, "%pI4 : %pI4: %llu\n",
                      &entries[i]->saddr, &entries[i]->daddr,
                      entries[i]->bytes);
    if (!n)
      break;
    pos += n;
  }

  kfree(entries);
}
