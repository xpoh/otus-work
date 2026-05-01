#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init kprobe_traffic_init(void) {
  pr_info("init\n");

  return 0;
}

static void __exit kprobe_traffic_exit(void) { pr_info("exit\n"); }

module_init(kprobe_traffic_init);
module_exit(kprobe_traffic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION(
    "Kprobe-based network traffic statistics module for OTUS Linux course");
