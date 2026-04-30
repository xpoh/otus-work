//
// Created by Rekunov Dmitriy on 05.04.2026.
//

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/bsearch.h>
#include <linux/hashtable.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sort.h>

#include "hashtable_search.h"

/* Compare two pointers to hash_entry by .value */
static int cmp_entry_ptr(const void *a, const void *b) {
  const struct hash_entry *ea = *(const struct hash_entry *const *)a;
  const struct hash_entry *eb = *(const struct hash_entry *const *)b;

  if (ea->value < eb->value)
    return -1;
  if (ea->value > eb->value)
    return 1;
  return 0;
}

/* Compare a key (unsigned int *) to hash_entry pointer. Used by bsearch. */
static int cmp_key_entry(const void *key, const void *elem) {
  unsigned int k = *(const unsigned int *)key;
  const struct hash_entry *e = *(const struct hash_entry *const *)elem;

  if (k < e->value)
    return -1;
  if (k > e->value)
    return 1;
  return 0;
}

/* Collect entries of a single bucket into a pointer array.
 * *out_arr must be freed by the caller via kfree on success.
 * Returns number of entries on success (>= 0), or negative error. */
static int collect_bucket(struct bucket_search_ctx *ctx, unsigned int bucket,
                          struct hash_entry ***out_arr) {
  struct hash_entry *e;
  struct hash_entry **arr;
  unsigned int len = 0;
  unsigned int i = 0;

  *out_arr = NULL;

  if (bucket >= HASH_TABLE_SIZE)
    return BS_INVALID;

  /* Count */
  hlist_for_each_entry(e, &ctx->htable[bucket], node) { len++; }

  if (len == 0)
    return 0;

  arr = kmalloc_array(len, sizeof(*arr), GFP_KERNEL);
  if (!arr)
    return BS_NOMEM;

  hlist_for_each_entry(e, &ctx->htable[bucket], node) {
    if (i >= len)
      break;
    arr[i++] = e;
  }

  *out_arr = arr;
  return (int)i;
}

int ht_search(struct bucket_search_ctx *ctx, unsigned int x) {
  unsigned int bucket;
  struct hash_entry **arr = NULL;
  struct hash_entry **found;
  int len;

  if (!ctx)
    return BS_INVALID;

  ctx->last_found = 0;
  ctx->last_value = x;
  ctx->last_bucket = 0;

  if (x >= ctx->array_size) {
    pr_info("search: x=%u out of range (max %u)\n", x, ctx->array_size - 1);
    return BS_NOT_FOUND;
  }

  bucket = hash_min(x, HASH_TABLE_BITS);
  ctx->last_bucket = bucket;

  len = collect_bucket(ctx, bucket, &arr);
  if (len < 0)
    return len;
  if (len == 0) {
    pr_info("search: bucket=%u empty, x=%u not found\n", bucket, x);
    return BS_NOT_FOUND;
  }

  sort(arr, len, sizeof(*arr), cmp_entry_ptr, NULL);

  found = bsearch(&x, arr, len, sizeof(*arr), cmp_key_entry);

  if (found) {
    ctx->last_found = 1;
    pr_info("search: found x=%u in bucket=%u\n", x, bucket);
  } else {
    ctx->last_found = 0;
    pr_info("search: x=%u not found in bucket=%u\n", x, bucket);
  }

  kfree(arr);
  return ctx->last_found ? BS_OK : BS_NOT_FOUND;
}

int ht_bucket_dump(struct bucket_search_ctx *ctx, unsigned int bucket_id,
                   char *buf, size_t buf_size) {
  struct hash_entry **arr = NULL;
  int len;
  int written;
  int i;
  size_t off;

  if (!ctx || !buf)
    return BS_INVALID;

  if (bucket_id >= HASH_TABLE_SIZE)
    return BS_INVALID;

  len = collect_bucket(ctx, bucket_id, &arr);
  if (len < 0)
    return len;

  if (len == 0) {
    return scnprintf(buf, buf_size, "bucket=%u len=0:\n", bucket_id);
  }

  sort(arr, len, sizeof(*arr), cmp_entry_ptr, NULL);

  written = scnprintf(buf, buf_size, "bucket=%u len=%d:", bucket_id, len);
  off = written;

  for (i = 0; i < len; i++) {
    if (off >= buf_size - 1)
      break;
    written = scnprintf(buf + off, buf_size - off, " %u", arr[i]->value);
    off += written;
  }

  if (off < buf_size - 1) {
    written = scnprintf(buf + off, buf_size - off, "\n");
    off += written;
  }

  kfree(arr);
  return (int)off;
}
