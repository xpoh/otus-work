#define pr_fmt(fmt) KBUILD_MODNAME ": params: " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int top_n = 10;

module_param(top_n, int, 0644);
MODULE_PARM_DESC(top_n, "number of top addresses to report");

static int top_addrs_get(char *buf, const struct kernel_param *kp) {
    return strlen(buf);
}


static const struct kernel_param_ops top_addrs_ops = {
    .get = top_addrs_get,
};

module_param_cb(top_addrs, &top_addrs_ops, NULL, 0444);
MODULE_PARM_DESC(top_addrs, "top N addresses by traffic (read only)");

int params_init(void) { return 0; }

void params_exit(void) {}
