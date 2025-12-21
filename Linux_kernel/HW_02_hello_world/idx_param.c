//
// Created by Rekunov Dmitriy on 21.12.2025.
//
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int idx;

static int idx_param_set(const char *val, const struct kernel_param *kp)
{
	int ret;

	ret = kstrtoint(val, 10, &idx);
	if (ret) {
		return ret;
	}

	pr_info("set idx = %d\n", idx);

	return 0;
}

static int idx_param_get(char *buf, const struct kernel_param *kp)
{
	return sprintf(buf, "%d", idx);
}

static const struct kernel_param_ops idx_params = {
	.set = idx_param_set,
	.get = idx_param_get,
};

module_param_cb(idx, &idx_params, &idx, 0664);
MODULE_PARM_DESC(idx, "index char in string");
