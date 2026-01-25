#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "../inc/kernel_stack.h"

static int __init kernel_stack_init(void) {
  int ret;

  stack_init();

  ret = stack_sysfs_init();
  if (ret) {
    pr_err("failed to create sysfs interface\n");
    return ret;
  }

  pr_info("init\n");
  return 0;
}

static void __exit kernel_stack_exit(void) {
  stack_sysfs_exit();
  stack_clear();
  pr_info("exit\n");
}

module_init(kernel_stack_init);
module_exit(kernel_stack_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION("Kernel stack module for OTUS Linux linux course");
