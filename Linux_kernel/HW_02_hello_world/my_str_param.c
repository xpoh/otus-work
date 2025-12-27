//
// Created by Rekunov Dmitriy on 21.12.2025.
//
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include "my_str_param.h"

MODULE_LICENSE("GPL");

char my_str_param[MY_STR_MAX_SIZE] = "";

static int my_str_param_get(char *buf, const struct kernel_param *kp)
{
	return sprintf(buf, "%s", my_str_param);
}

static const struct kernel_param_ops my_str_params = {
	.set = NULL,
	.get = my_str_param_get,
};

module_param_cb(my_str, &my_str_params, &my_str_param,
		0444); // read only fo all
MODULE_PARM_DESC(my_str, "Hellow world string param");
