#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "allocator.h"
#include "params.h"

/* alloc parameter: echo <bytes> > /sys/module/kernel_alloc/parameters/alloc */
static int param_set_alloc(const char *val, const struct kernel_param *kp) {
  size_t bytes;
  int ret;
  void *ptr;

  if (!val) {
    pr_err("alloc: NULL value\n");
    return -EINVAL;
  }

  ret = kstrtoul(val, 0, (unsigned long *)&bytes);
  if (ret) {
    pr_err("alloc: invalid value '%s'\n", val);
    return -EINVAL;
  }

  ptr = allocator_alloc(bytes);
  if (!ptr)
    return -ENOMEM;

  return 0;
}

const struct kernel_param_ops param_ops_alloc = {
    .set = param_set_alloc,
    .get = NULL,
};

module_param_cb(alloc, &param_ops_alloc, NULL, 0220);
MODULE_PARM_DESC(alloc, "Allocate memory (size in bytes)");

/* free parameter: echo <address> > /sys/module/kernel_alloc/parameters/free */
static int param_set_free(const char *val, const struct kernel_param *kp) {
  unsigned long addr;
  int ret;

  if (!val) {
    pr_err("free: NULL value\n");
    return -EINVAL;
  }

  ret = kstrtoul(val, 0, &addr);
  if (ret) {
    pr_err("free: invalid value '%s'\n", val);
    return -EINVAL;
  }

  ret = allocator_free((void *)addr);
  if (ret != ALLOC_OK)
    return -EINVAL;

  return 0;
}

const struct kernel_param_ops param_ops_free = {
    .set = param_set_free,
    .get = NULL,
};

module_param_cb(free, &param_ops_free, NULL, 0220);
MODULE_PARM_DESC(free, "Free memory (address as hex, e.g. 0xffffc9000c000000)");

/* stats parameter: cat /sys/module/kernel_alloc/parameters/stats */
static int param_get_stats(char *buf, const struct kernel_param *kp) {
  struct stats_info stats = allocator_get_stats();

  return scnprintf(buf, PAGE_SIZE,
                   "Total: %zu KB | Free: %zu KB | Allocated: %zu KB | "
                   "Fragmentation: %zu%%\n",
                   stats.total_memory / 1024, stats.free_memory / 1024,
                   stats.allocated_memory / 1024, stats.fragmentation_percent);
}

const struct kernel_param_ops param_ops_stats = {
    .set = NULL,
    .get = param_get_stats,
};

module_param_cb(stats, &param_ops_stats, NULL, 0444);
MODULE_PARM_DESC(stats, "Get allocator statistics");

/* bitmap_info parameter: cat /sys/module/kernel_alloc/parameters/bitmap_info */
static int param_get_bitmap_info(char *buf, const struct kernel_param *kp) {
  return allocator_bitmap_info(buf, PAGE_SIZE);
}

const struct kernel_param_ops param_ops_bitmap_info = {
    .set = NULL,
    .get = param_get_bitmap_info,
};

module_param_cb(bitmap_info, &param_ops_bitmap_info, NULL, 0444);
MODULE_PARM_DESC(bitmap_info, "Show bitmap allocation state");
