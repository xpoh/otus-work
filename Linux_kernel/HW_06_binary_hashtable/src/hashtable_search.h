//
// Created by Rekunov Dmitriy on 05.04.2026.
//

#ifndef HASHTABLE_SEARCH_H
#define HASHTABLE_SEARCH_H

#include <linux/hashtable.h>
#include <linux/types.h>

/* Return codes */
#define BS_OK 0              /* success */
#define BS_INVALID -EINVAL   /* bad parameter */
#define BS_NOMEM -ENOMEM     /* out of memory */
#define BS_NOT_FOUND -ENOENT /* value not found */

/* Compile-time number of bits for hashtable (64 buckets). */
#define HASH_TABLE_BITS 6
#define HASH_TABLE_SIZE (1U << HASH_TABLE_BITS)

/* Element stored in hashtable. */
struct hash_entry {
  struct hlist_node node;
  unsigned int value;
};

/* Module context. */
struct bucket_search_ctx {
  unsigned int array_size;
  unsigned int num_buckets_bits;

  DECLARE_HASHTABLE(htable, HASH_TABLE_BITS);

  int last_found;
  unsigned int last_value;
  unsigned int last_bucket;

  unsigned int current_bucket_id;
};

/* Global context pointer, defined in main.c */
extern struct bucket_search_ctx *g_ctx;

/* build.c - hashtable population and teardown */
int ht_build(struct bucket_search_ctx *ctx);
void ht_clear(struct bucket_search_ctx *ctx);
int ht_rebuild(struct bucket_search_ctx *ctx);

/* search.c - bsearch-based lookup and bucket dump */
int ht_search(struct bucket_search_ctx *ctx, unsigned int x);
int ht_bucket_dump(struct bucket_search_ctx *ctx, unsigned int bucket_id,
                   char *buf, size_t buf_size);

#endif /* HASHTABLE_SEARCH_H */
