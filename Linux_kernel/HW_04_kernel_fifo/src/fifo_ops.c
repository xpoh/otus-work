//
// Created by Rekunov Dmitriy on 15.02.2026.
//

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kfifo.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "fifo_ops.h"

/* FIFO device structure */
struct fifo_device {
  struct kfifo queue; /* Built-in kfifo buffer */
  int max_size;       /* Maximum queue size */
};

static struct fifo_device *fifo_dev = NULL;

/* Initialize FIFO with given size */
int fifo_init(int size) {
  int ret;

  if (size <= 0) {
    pr_err("Invalid FIFO size: %d\n", size);
    return FIFO_INVALID;
  }

  fifo_dev = kmalloc(sizeof(*fifo_dev), GFP_KERNEL);
  if (!fifo_dev) {
    pr_err("Failed to allocate FIFO device\n");
    return FIFO_NOMEM;
  }

  fifo_dev->max_size = size;

  /* Allocate kfifo buffer */
  ret = kfifo_alloc(&fifo_dev->queue, size * sizeof(struct fifo_entry),
                    GFP_KERNEL);
  if (ret) {
    pr_err("Failed to allocate kfifo buffer\n");
    kfree(fifo_dev);
    fifo_dev = NULL;
    return FIFO_NOMEM;
  }

  pr_info("FIFO initialized with size %d\n", size);
  return FIFO_OK;
}

/* Cleanup FIFO */
void fifo_cleanup(void) {
  if (fifo_dev) {
    kfifo_free(&fifo_dev->queue);
    kfree(fifo_dev);
    fifo_dev = NULL;
    pr_info("FIFO cleanup completed\n");
  }
}

/* Add element to queue */
int fifo_enqueue(int value) {
  struct fifo_entry entry;
  int ret;

  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return FIFO_INVALID;
  }

  if (kfifo_is_full(&fifo_dev->queue)) {
    pr_warn("FIFO is full, cannot enqueue %d\n", value);
    return FIFO_FULL;
  }

  entry.data = value;
  ret = kfifo_in(&fifo_dev->queue, &entry, sizeof(entry));
  if (ret != sizeof(entry)) {
    pr_err("Failed to enqueue value %d\n", value);
    return FIFO_FULL;
  }

  pr_debug("Enqueued value: %d\n", value);
  return FIFO_OK;
}

/* Remove and return element from queue */
int fifo_dequeue(void) {
  struct fifo_entry entry;
  int ret;

  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return FIFO_INVALID;
  }

  if (kfifo_is_empty(&fifo_dev->queue)) {
    pr_warn("FIFO is empty, cannot dequeue\n");
    return FIFO_EMPTY;
  }

  ret = kfifo_out(&fifo_dev->queue, &entry, sizeof(entry));
  if (ret != sizeof(entry)) {
    pr_err("Failed to dequeue value\n");
    return FIFO_EMPTY;
  }

  pr_debug("Dequeued value: %d\n", entry.data);
  return entry.data;
}

/* Get value from queue without removing */
int fifo_peek(void) {
  struct fifo_entry entry;
  int ret;

  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return FIFO_INVALID;
  }

  if (kfifo_is_empty(&fifo_dev->queue)) {
    pr_warn("FIFO is empty, cannot peek\n");
    return FIFO_EMPTY;
  }

  ret = kfifo_out_peek(&fifo_dev->queue, &entry, sizeof(entry));
  if (ret != sizeof(entry)) {
    pr_err("Failed to peek value\n");
    return FIFO_EMPTY;
  }

  pr_debug("Peeked value: %d\n", entry.data);
  return entry.data;
}

/* Check if queue is empty */
int fifo_is_empty(void) {
  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return FIFO_INVALID;
  }

  return kfifo_is_empty(&fifo_dev->queue) ? 1 : 0;
}

/* Check if queue is full */
int fifo_is_full(void) {
  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return FIFO_INVALID;
  }

  return kfifo_is_full(&fifo_dev->queue) ? 1 : 0;
}

/* Get current number of elements */
int fifo_size(void) {
  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return FIFO_INVALID;
  }

  return kfifo_len(&fifo_dev->queue) / sizeof(struct fifo_entry);
}

/* Get available space */
int fifo_available(void) {
  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return FIFO_INVALID;
  }

  return kfifo_avail(&fifo_dev->queue) / sizeof(struct fifo_entry);
}

/* Clear the queue */
void fifo_clear(void) {
  if (!fifo_dev) {
    pr_err("FIFO not initialized\n");
    return;
  }

  kfifo_reset(&fifo_dev->queue);
  pr_info("FIFO cleared\n");
}
