#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/sort.h>
#include <linux/vmalloc.h>

struct my_struct {
	u32 val;
	struct list_head node;
};

#define NR_VAL 100

static struct my_struct arr[NR_VAL];
static LIST_HEAD(my_list);

static void init_arr(void)
{
	int i;

	for (i = 0; i < NR_VAL; ++i) {
		get_random_bytes(&arr[i].val, sizeof(u32));
		arr[i].val = arr[i].val % 1000;
	}
}

static void init_list(void)
{
	int i;

	for (i = 0; i < NR_VAL; ++i)
		list_add_tail(&arr[i].node, &my_list);
}

static void fn_swap_arr(void *a, void *b, int size)
{
	u32 tmp;
	struct my_struct *struct_a, *struct_b;
	struct_a = (struct my_struct *)a;
	struct_b = (struct my_struct *)b;

	tmp = struct_b->val;
	struct_b->val = struct_a->val;
	struct_a->val = tmp;
}

static int fn_cmp_arr(const void *a, const void *b)
{
	struct my_struct *struct_a, *struct_b;

	struct_a = (struct my_struct *)a;
	struct_b = (struct my_struct *)b;
	return struct_a->val - struct_b->val;
}

static void pr_list(void)
{
	struct my_struct *node;

	list_for_each_entry(node, &my_list, node)
		pr_info("val = %u\n", node->val);
}

static int __init my_module_init(void)
{
	int i;

	pr_info("start\n");

	init_arr();

	pr_info("===========\n");
	pr_info("Before sorting:\n");

	for (i = 0; i < NR_VAL; ++i)
		pr_info("val = %u\n", arr[i].val);

	pr_info("===========\n");
	sort((void *)arr, NR_VAL, sizeof(struct my_struct), fn_cmp_arr,
	     fn_swap_arr);

	pr_info("After sorting:\n");
	init_list();
	pr_list();

	pr_info("===========\n");
	return 0;
}

static void __exit my_module_exit(void)
{
	pr_info("end\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KGD");
MODULE_DESCRIPTION(
	"A kernel module that sorts a linked list using the kernel's sort function");
