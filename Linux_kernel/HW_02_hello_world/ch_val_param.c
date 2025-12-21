//
// Created by Rekunov Dmitriy on 21.12.2025.
//
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "update.h"

MODULE_LICENSE("GPL");

char ch_val;

static int ch_val_param_set(const char *val, const struct kernel_param *kp)
{
	pr_debug("val = %s\n", val);

	if (strlen(val) != 1) {
		return EINVAL;
	}

	ch_val = val[0];

	pr_info("set ch_val = %c\n", ch_val);

	return update_hello_world();
}

static int ch_val_param_get(char *buf, const struct kernel_param *kp)
{
	return sprintf(buf, "%c", ch_val);
}

static const struct kernel_param_ops ch_val_params = {
	.set = ch_val_param_set,
	.get = ch_val_param_get,
};

module_param_cb(ch_val, &ch_val_params, &ch_val, 0664);
MODULE_PARM_DESC(ch_val, "index char in string");
