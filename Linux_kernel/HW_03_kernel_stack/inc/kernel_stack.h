#ifndef KERNEL_STACK_H
#define KERNEL_STACK_H

#include "../lib/inc/stack.h"
#include "../lib/inc/stack_ops.h"

int stack_sysfs_init(void);
void stack_sysfs_exit(void);

#endif
