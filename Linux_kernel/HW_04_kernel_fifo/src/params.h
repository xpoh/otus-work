//
// Created by Rekunov Dmitriy on 15.02.2026.
//

#ifndef KERNEL_FIFO_PARAMS_H
#define KERNEL_FIFO_PARAMS_H

#include <linux/moduleparam.h>

/* Module parameter operations */
extern const struct kernel_param_ops param_ops_enqueue;
extern const struct kernel_param_ops param_ops_dequeue;
extern const struct kernel_param_ops param_ops_peek;
extern const struct kernel_param_ops param_ops_size;
extern const struct kernel_param_ops param_ops_available;
extern const struct kernel_param_ops param_ops_is_empty;
extern const struct kernel_param_ops param_ops_is_full;
extern const struct kernel_param_ops param_ops_clear;

#endif // KERNEL_FIFO_PARAMS_H
