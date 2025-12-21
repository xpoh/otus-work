#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
	pr_info("init\n");

	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitriy Rekunov");
MODULE_DESCRIPTION("work 2 for OTUS linux kernel course");