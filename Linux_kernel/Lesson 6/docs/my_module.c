#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/stat.h>

static int int_val = 10;
#define MAX_SIZE 64
static char string_val[MAX_SIZE] = "Default string";

static int int_val_set(const char *val, const struct kernel_param *kp)
{
    int ret;
    ret = kstrtoint(val, 10, &int_val);
    if(ret)
        return ret;

    pr_info("int_val value = %d\n", int_val);
    return 0;
}

static int int_val_get(char *buf, const struct kernel_param *kp)
{
    return sprintf(buf, "%d", int_val);
}

static const struct kernel_param_ops int_val_params = {
    .set = int_val_set,
    .get = int_val_get,
};

module_param_cb(int_val, &int_val_params, &int_val, 0664);
MODULE_PARM_DESC(int_val, "Integer value to change");

static int string_val_set(const char *val, const struct kernel_param *kp)
{
    size_t len = strlen(val);
    if (len >= MAX_SIZE)
    {
        pr_err("string_val input string too long\n");
        return -EINVAL;
    }

    strncpy(string_val, val, MAX_SIZE - 1);
    string_val[MAX_SIZE - 1] = '\0';

    pr_info("string_val value = %s\n", string_val);
    return 0;
}

static int string_val_get(char *buf, const struct kernel_param *kp)
{
    return sprintf(buf, "%s", string_val);
}

static const struct kernel_param_ops string_val_params = {
    .set = string_val_set,
    .get = string_val_get,
};

module_param_cb(string_val, &string_val_params, &string_val, 0664);
MODULE_PARM_DESC(string_val, "Str value to change");

static int __init hello_init(void)
{
	pr_info("Hello, World from the kernel!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye, World from the kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION("A simple Hello World module for the Linux kernel");