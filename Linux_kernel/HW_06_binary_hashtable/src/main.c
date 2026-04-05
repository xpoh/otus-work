//
// Created by Rekunov Dmitriy on 05.04.2026.
//

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "hashtable_search.h"

/* Module parameters */
static unsigned int array_size = 1024;
module_param(array_size, uint, 0444);
MODULE_PARM_DESC(array_size,
                 "Size of random array (values in 0..array_size-1)");

static unsigned int num_buckets_bits = HASH_TABLE_BITS;
module_param(num_buckets_bits, uint, 0444);
MODULE_PARM_DESC(num_buckets_bits,
                 "Bits for hashtable (compile-time fixed; read-only)");

/* Global context */
struct bucket_search_ctx *g_ctx;

static int __init kernel_hashtable_init(void) {
  int ret;

  pr_info("Loading kernel hashtable search module\n");

  if (array_size == 0) {
    pr_err("array_size must be > 0\n");
    return -EINVAL;
  }

  /* The spec allows num_buckets_bits to be 1..16, but DECLARE_HASHTABLE
   * requires a compile-time constant. Enforce that it matches
   * HASH_TABLE_BITS. */
  if (num_buckets_bits != HASH_TABLE_BITS) {
    pr_warn("num_buckets_bits=%u overridden to compile-time value %d\n",
            num_buckets_bits, HASH_TABLE_BITS);
    num_buckets_bits = HASH_TABLE_BITS;
  }

  g_ctx = kzalloc(sizeof(*g_ctx), GFP_KERNEL);
  if (!g_ctx) {
    pr_err("Failed to allocate context\n");
    return -ENOMEM;
  }

  g_ctx->array_size = array_size;
  g_ctx->num_buckets_bits = num_buckets_bits;
  g_ctx->last_found = 0;
  g_ctx->last_value = 0;
  g_ctx->last_bucket = 0;
  g_ctx->current_bucket_id = 0;
  hash_init(g_ctx->htable);

  ret = ht_build(g_ctx);
  if (ret != BS_OK) {
    pr_err("Failed to build hashtable: %d\n", ret);
    kfree(g_ctx);
    g_ctx = NULL;
    return ret;
  }

  pr_info("Module loaded: array_size=%u buckets=%u\n", g_ctx->array_size,
          HASH_TABLE_SIZE);
  return 0;
}

static void __exit kernel_hashtable_exit(void) {
  pr_info("Unloading kernel hashtable search module\n");
  if (g_ctx) {
    ht_clear(g_ctx);
    kfree(g_ctx);
    g_ctx = NULL;
  }
  pr_info("Module unloaded\n");
}

module_init(kernel_hashtable_init);
module_exit(kernel_hashtable_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION("Kernel hashtable + bsearch module (OTUS Linux kernel)");
MODULE_VERSION("1.0");
