#ifndef KERNEL_ALLOC_PARAMS_H
#define KERNEL_ALLOC_PARAMS_H

#include <linux/moduleparam.h>

extern const struct kernel_param_ops param_ops_alloc;
extern const struct kernel_param_ops param_ops_free;
extern const struct kernel_param_ops param_ops_stats;
extern const struct kernel_param_ops param_ops_bitmap_info;

#endif /* KERNEL_ALLOC_PARAMS_H */
