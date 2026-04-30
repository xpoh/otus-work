//
// Created by Rekunov Dmitriy on 05.04.2026.
//

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/hashtable.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include <linux/slab.h>

#include "hashtable_search.h"

int ht_build(struct bucket_search_ctx *ctx) {
  unsigned int i;
  unsigned int val;
  struct hash_entry *entry;

  if (!ctx)
    return BS_INVALID;

  for (i = 0; i < ctx->array_size; i++) {
    val = get_random_u32() % ctx->array_size;

    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
      pr_err("ht_build: failed to allocate entry %u\n", i);
      ht_clear(ctx);
      return BS_NOMEM;
    }

    entry->value = val;
    INIT_HLIST_NODE(&entry->node);
    hash_add(ctx->htable, &entry->node, val);
  }

  pr_info("ht_build: added %u entries\n", ctx->array_size);
  return BS_OK;
}

void ht_clear(struct bucket_search_ctx *ctx) {
  struct hash_entry *entry;
  struct hlist_node *tmp;
  int bkt;

  if (!ctx)
    return;

  hash_for_each_safe(ctx->htable, bkt, tmp, entry, node) {
    hash_del(&entry->node);
    kfree(entry);
  }
}

int ht_rebuild(struct bucket_search_ctx *ctx) {
  if (!ctx)
    return BS_INVALID;

  ht_clear(ctx);
  /* Re-init to be safe. */
  hash_init(ctx->htable);
  return ht_build(ctx);
}
