#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "allocator.h"

static int __init kernel_alloc_init(void) {
  int ret;

  pr_info("initializing bitmap memory allocator module\n");

  ret = allocator_init();
  if (ret != ALLOC_OK) {
    pr_err("failed to initialize allocator (error %d)\n", ret);
    return -ENOMEM;
  }

  pr_info("module loaded successfully\n");
  return 0;
}

static void __exit kernel_alloc_exit(void) {
  pr_info("cleaning up bitmap memory allocator module\n");
  allocator_cleanup();
  pr_info("module unloaded\n");
}

module_init(kernel_alloc_init);
module_exit(kernel_alloc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION("Bitmap-based memory allocator kernel module");
MODULE_VERSION("1.0");
