//
// Created by Rekunov Dmitriy on 05.04.2026.
//

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "hashtable_search.h"

/* ---- search (write) ---- */
static int param_set_search(const char *val, const struct kernel_param *kp) {
  unsigned int x;
  int ret;

  if (!val || !g_ctx)
    return -EINVAL;

  ret = kstrtouint(val, 0, &x);
  if (ret) {
    pr_err("search: invalid value '%s'\n", val);
    return -EINVAL;
  }

  (void)ht_search(g_ctx, x);
  return 0;
}

static const struct kernel_param_ops param_ops_search = {
    .set = param_set_search,
    .get = NULL,
};
module_param_cb(search, &param_ops_search, NULL, 0220);
MODULE_PARM_DESC(search, "Search for a number in the hashtable");

/* ---- result (read) ---- */
static int param_get_result(char *buf, const struct kernel_param *kp) {
  if (!buf || !g_ctx)
    return -EINVAL;

  return scnprintf(buf, PAGE_SIZE, "found=%d value=%u bucket=%u\n",
                   g_ctx->last_found, g_ctx->last_value, g_ctx->last_bucket);
}

static const struct kernel_param_ops param_ops_result = {
    .set = NULL,
    .get = param_get_result,
};
module_param_cb(result, &param_ops_result, NULL, 0440);
MODULE_PARM_DESC(result, "Result of the last search");

/* ---- rebuild (write) ---- */
static int param_set_rebuild(const char *val, const struct kernel_param *kp) {
  unsigned int v;
  int ret;

  if (!val || !g_ctx)
    return -EINVAL;

  ret = kstrtouint(val, 0, &v);
  if (ret) {
    pr_err("rebuild: invalid value '%s'\n", val);
    return -EINVAL;
  }

  if (v == 0) {
    pr_info("rebuild: zero value, ignoring\n");
    return 0;
  }

  ret = ht_rebuild(g_ctx);
  if (ret != BS_OK) {
    pr_err("rebuild: failed (%d)\n", ret);
    return ret;
  }

  pr_info("rebuild: hashtable rebuilt\n");
  return 0;
}

static const struct kernel_param_ops param_ops_rebuild = {
    .set = param_set_rebuild,
    .get = NULL,
};
module_param_cb(rebuild, &param_ops_rebuild, NULL, 0220);
MODULE_PARM_DESC(rebuild, "Rebuild hashtable with fresh random values");

/* ---- bucket_id (write) ---- */
static int param_set_bucket_id(const char *val, const struct kernel_param *kp) {
  unsigned int id;
  int ret;

  if (!val || !g_ctx)
    return -EINVAL;

  ret = kstrtouint(val, 0, &id);
  if (ret) {
    pr_err("bucket_id: invalid value '%s'\n", val);
    return -EINVAL;
  }

  if (id >= HASH_TABLE_SIZE) {
    pr_err("bucket_id: %u out of range (max %u)\n", id, HASH_TABLE_SIZE - 1);
    return -EINVAL;
  }

  g_ctx->current_bucket_id = id;
  return 0;
}

static const struct kernel_param_ops param_ops_bucket_id = {
    .set = param_set_bucket_id,
    .get = NULL,
};
module_param_cb(bucket_id, &param_ops_bucket_id, NULL, 0220);
MODULE_PARM_DESC(bucket_id, "Set bucket index for bucket_dump");

/* ---- bucket_dump (read) ---- */
static int param_get_bucket_dump(char *buf, const struct kernel_param *kp) {
  int ret;

  if (!buf || !g_ctx)
    return -EINVAL;

  ret = ht_bucket_dump(g_ctx, g_ctx->current_bucket_id, buf, PAGE_SIZE);
  if (ret < 0)
    return scnprintf(buf, PAGE_SIZE, "error: invalid bucket_id=%u\n",
                     g_ctx->current_bucket_id);

  return ret;
}

static const struct kernel_param_ops param_ops_bucket_dump = {
    .set = NULL,
    .get = param_get_bucket_dump,
};
module_param_cb(bucket_dump, &param_ops_bucket_dump, NULL, 0440);
MODULE_PARM_DESC(bucket_dump, "Dump sorted contents of the current bucket");
