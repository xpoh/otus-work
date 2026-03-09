//
// Created by Rekunov Dmitriy on 15.02.2026.
//

#ifndef KERNEL_FIFO_OPS_H
#define KERNEL_FIFO_OPS_H

/* Error codes */
#define FIFO_OK 0       /* Operation successful */
#define FIFO_EMPTY -1   /* Queue is empty */
#define FIFO_FULL -2    /* Queue is full */
#define FIFO_NOMEM -3   /* No memory */
#define FIFO_INVALID -4 /* Invalid parameter */

/* FIFO entry structure */
struct fifo_entry {
  int data; /* Data (integer) */
};

/* FIFO operations */
int fifo_init(int size);
void fifo_cleanup(void);
int fifo_enqueue(int value);
int fifo_dequeue(void);
int fifo_peek(void);
int fifo_is_empty(void);
int fifo_is_full(void);
int fifo_size(void);
int fifo_available(void);
void fifo_clear(void);

#endif // KERNEL_FIFO_OPS_H
