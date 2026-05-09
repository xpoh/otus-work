#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "../inc/param.h"
#include "../inc/stats.h"

static int __init kprobe_traffic_init(void) {
  pr_info("init\n");

  int ret;

  ret = stats_init();
  if (ret) {
    pr_err("failed to init stats\n");

    return ret;
  }

  ret = params_init();
  if (ret) {
    pr_err("failed to init params\n");

    stats_exit();

    return ret;
  }

  return 0;
}

static void __exit kprobe_traffic_exit(void) {
  params_exit();
  stats_exit();

  pr_info("exit\n");
}

module_init(kprobe_traffic_init);
module_exit(kprobe_traffic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION(
    "Kprobe-based network traffic statistics module for OTUS Linux course");
