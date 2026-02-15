//
// Created by Rekunov Dmitriy on 15.02.2026.
//

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "fifo_ops.h"
#include "params.h"

/* Enqueue parameter */
static int param_set_enqueue(const char *val, const struct kernel_param *kp) {
  int value;
  int ret;

  if (!val) {
    pr_err("enqueue: NULL value\n");
    return -EINVAL;
  }

  ret = kstrtoint(val, 10, &value);
  if (ret != 0) {
    pr_err("enqueue: invalid value '%s'\n", val);
    return -EINVAL;
  }

  ret = fifo_enqueue(value);
  if (ret != FIFO_OK) {
    pr_err("enqueue: failed to add value %d (error %d)\n", value, ret);
    return -EINVAL;
  }

  pr_info("enqueue: added value %d\n", value);
  return 0;
}

const struct kernel_param_ops param_ops_enqueue = {
    .set = param_set_enqueue,
    .get = NULL,
};

module_param_cb(enqueue, &param_ops_enqueue, NULL, 0220);
MODULE_PARM_DESC(enqueue, "Add element to queue");

/* Dequeue parameter */
static int param_get_dequeue(char *buf, const struct kernel_param *kp) {
  int value;

  if (!buf) {
    pr_err("dequeue: NULL buffer\n");
    return -EINVAL;
  }

  value = fifo_dequeue();
  if (value == FIFO_EMPTY) {
    return snprintf(buf, PAGE_SIZE, "error: queue is empty\n");
  } else if (value == FIFO_INVALID) {
    return snprintf(buf, PAGE_SIZE, "error: FIFO not initialized\n");
  } else if (value < 0) {
    return snprintf(buf, PAGE_SIZE, "error: %d\n", value);
  }

  return snprintf(buf, PAGE_SIZE, "%d\n", value);
}

const struct kernel_param_ops param_ops_dequeue = {
    .set = NULL,
    .get = param_get_dequeue,
};

module_param_cb(dequeue, &param_ops_dequeue, NULL, 0440);
MODULE_PARM_DESC(dequeue, "Get and remove element from queue");

/* Peek parameter */
static int param_get_peek(char *buf, const struct kernel_param *kp) {
  int value;

  if (!buf) {
    pr_err("peek: NULL buffer\n");
    return -EINVAL;
  }

  value = fifo_peek();
  if (value == FIFO_EMPTY) {
    return snprintf(buf, PAGE_SIZE, "error: queue is empty\n");
  } else if (value == FIFO_INVALID) {
    return snprintf(buf, PAGE_SIZE, "error: FIFO not initialized\n");
  } else if (value < 0) {
    return snprintf(buf, PAGE_SIZE, "error: %d\n", value);
  }

  return snprintf(buf, PAGE_SIZE, "%d\n", value);
}

const struct kernel_param_ops param_ops_peek = {
    .set = NULL,
    .get = param_get_peek,
};

module_param_cb(peek, &param_ops_peek, NULL, 0440);
MODULE_PARM_DESC(peek, "Get value from queue without removing");

/* Size parameter */
static int param_get_size(char *buf, const struct kernel_param *kp) {
  int size;

  if (!buf) {
    pr_err("size: NULL buffer\n");
    return -EINVAL;
  }

  size = fifo_size();
  if (size < 0) {
    return snprintf(buf, PAGE_SIZE, "error: %d\n", size);
  }

  return snprintf(buf, PAGE_SIZE, "%d\n", size);
}

const struct kernel_param_ops param_ops_size = {
    .set = NULL,
    .get = param_get_size,
};

module_param_cb(size, &param_ops_size, NULL, 0440);
MODULE_PARM_DESC(size, "Get current queue size");

/* Available parameter */
static int param_get_available(char *buf, const struct kernel_param *kp) {
  int available;

  if (!buf) {
    pr_err("available: NULL buffer\n");
    return -EINVAL;
  }

  available = fifo_available();
  if (available < 0) {
    return snprintf(buf, PAGE_SIZE, "error: %d\n", available);
  }

  return snprintf(buf, PAGE_SIZE, "%d\n", available);
}

const struct kernel_param_ops param_ops_available = {
    .set = NULL,
    .get = param_get_available,
};

module_param_cb(available, &param_ops_available, NULL, 0440);
MODULE_PARM_DESC(available, "Get available space in queue");

/* Is empty parameter */
static int param_get_is_empty(char *buf, const struct kernel_param *kp) {
  int empty;

  if (!buf) {
    pr_err("is_empty: NULL buffer\n");
    return -EINVAL;
  }

  empty = fifo_is_empty();
  if (empty < 0) {
    return snprintf(buf, PAGE_SIZE, "error: %d\n", empty);
  }

  return snprintf(buf, PAGE_SIZE, "%d\n", empty);
}

const struct kernel_param_ops param_ops_is_empty = {
    .set = NULL,
    .get = param_get_is_empty,
};

module_param_cb(is_empty, &param_ops_is_empty, NULL, 0440);
MODULE_PARM_DESC(is_empty, "Check if queue is empty (1=empty, 0=not empty)");

/* Is full parameter */
static int param_get_is_full(char *buf, const struct kernel_param *kp) {
  int full;

  if (!buf) {
    pr_err("is_full: NULL buffer\n");
    return -EINVAL;
  }

  full = fifo_is_full();
  if (full < 0) {
    return snprintf(buf, PAGE_SIZE, "error: %d\n", full);
  }

  return snprintf(buf, PAGE_SIZE, "%d\n", full);
}

const struct kernel_param_ops param_ops_is_full = {
    .set = NULL,
    .get = param_get_is_full,
};

module_param_cb(is_full, &param_ops_is_full, NULL, 0440);
MODULE_PARM_DESC(is_full, "Check if queue is full (1=full, 0=not full)");

/* Clear parameter */
static int param_set_clear(const char *val, const struct kernel_param *kp) {
  fifo_clear();
  pr_info("clear: queue cleared\n");
  return 0;
}

const struct kernel_param_ops param_ops_clear = {
    .set = param_set_clear,
    .get = NULL,
};

module_param_cb(clear, &param_ops_clear, NULL, 0220);
MODULE_PARM_DESC(clear, "Clear the queue");
