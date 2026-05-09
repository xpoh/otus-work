#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <stats.h>
#include <kprobe_traffic.h>

static int __init kprobe_traffic_init(void) {
  int ret;

  pr_info("init\n");

  stats_init();

  ret = kprobe_traffic_register();
  if (ret) {
    pr_err("failed to register kprobes\n");
    stats_exit();

    return ret;
  }

  return 0;
}

static void __exit kprobe_traffic_exit(void) {
  kprobe_traffic_unregister();
  stats_exit();

  pr_info("exit\n");
}

module_init(kprobe_traffic_init);
module_exit(kprobe_traffic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION(
    "Kprobe-based network traffic statistics module for OTUS Linux course");
