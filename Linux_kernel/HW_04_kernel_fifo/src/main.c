//
// Created by Rekunov Dmitriy on 15.02.2026.
//

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "fifo_ops.h"

/* Default FIFO size */
#define DEFAULT_FIFO_SIZE 100

/* Module parameter for initial FIFO size */
static int initial_size = DEFAULT_FIFO_SIZE;
module_param(initial_size, int, 0444);
MODULE_PARM_DESC(initial_size, "Initial FIFO size (default: 100)");

/* Module initialization */
static int __init kernel_fifo_init(void) {
  int ret;

  pr_info("Initializing kernel FIFO module\n");

  if (initial_size <= 0) {
    pr_err("Invalid FIFO size: %d\n", initial_size);
    return -EINVAL;
  }

  ret = fifo_init(initial_size);
  if (ret != FIFO_OK) {
    pr_err("Failed to initialize FIFO (error %d)\n", ret);
    return -ENOMEM;
  }

  pr_info("Kernel FIFO module loaded successfully (size: %d)\n", initial_size);
  return 0;
}

/* Module cleanup */
static void __exit kernel_fifo_exit(void) {
  pr_info("Cleaning up kernel FIFO module\n");
  fifo_cleanup();
  pr_info("Kernel FIFO module unloaded\n");
}

module_init(kernel_fifo_init);
module_exit(kernel_fifo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION(
    "Kernel FIFO module using kfifo for OTUS Linux kernel course");
MODULE_VERSION("1.0");
