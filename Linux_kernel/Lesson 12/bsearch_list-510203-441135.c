#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/bsearch.h>
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

#define NR_VAL 1000

static struct my_struct arr[NR_VAL];
static LIST_HEAD(my_list);

static void init_arr(void)
{
	int i;

	for (i = 0; i < NR_VAL; ++i) {
		get_random_bytes(&arr[i].val, sizeof(u32));
		arr[i].val = arr[i].val % NR_VAL;
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
	return (int)(struct_a->val - struct_b->val);
}

static bool srh_in_arr(struct my_struct *target)
{
	struct my_struct *result;

	result = NULL;
	result = bsearch(target, arr, NR_VAL, sizeof(struct my_struct),
			 fn_cmp_arr);
	if (!result)
		pr_info("result is NULL\n");
	return result != NULL;
}

static int __init my_module_init(void)
{
	struct my_struct target;

	pr_info("start\n");

	init_arr();

	sort((void *)arr, NR_VAL, sizeof(struct my_struct), fn_cmp_arr,
	     fn_swap_arr);

	init_list();

	target.val = 35;
	while (!srh_in_arr(&target) && target.val < NR_VAL)
		target.val += 1;

	if (target.val != NR_VAL)
		pr_info("%u found in arr!\n", target.val);

	for (int i = 0; i < NR_VAL; ++i)
		pr_info("val = %u\n", arr[i].val);
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
