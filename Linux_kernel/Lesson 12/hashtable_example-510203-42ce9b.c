#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hashtable.h>
#include <linux/slab.h>

struct object {
	int id;
	char name[16];
	struct hlist_node node;
};

#define NR_BITS 3
static DEFINE_HASHTABLE(htable, NR_BITS);

static void add_object(int id, const char *name)
{
	struct object *obj;

	obj = kmalloc(sizeof(*obj), GFP_KERNEL);
	if (!obj)
		return;

	obj->id = id;
	snprintf(obj->name, sizeof(obj->name), "%s", name);
	hash_add(htable, &obj->node, obj->id);
}

static void add_objects(void)
{
	int i;

	struct {
		int id;
		const char *name;
	} objs[] = {
		{ 1, "obj1" },
		{ 2, "obj2" },
		{ 3, "obj3" },
		{ 9, "obj9" },
	};

	for (i = 0; i < ARRAY_SIZE(objs); i++)
		add_object(objs[i].id, objs[i].name);
}

static void print_object_by_key(int key)
{
	struct object *obj;

	hash_for_each_possible(htable, obj, node, key) {
		if (obj->id == key) {
			pr_info("Found object with key=%d: name=%s\n",
			       key, obj->name);
			return;
		}
	}
	pr_info("Object with key=%d not found\n", key);
}

static void print_all_buckets(void)
{
	int bkt;
	struct object *obj;

	pr_info("Hash table contents:\n");
	hash_for_each(htable, bkt, obj, node) {
		pr_info(" bucket[%d]: id=%d, name=%s\n", bkt, obj->id,
		       obj->name);
	}
}

static void free_all_objects(void)
{
	int bkt;
	struct object *obj;
	struct hlist_node *tmp;

	hash_for_each_safe(htable, bkt, tmp, obj, node) {
		hash_del(&obj->node);
		kfree(obj);
	}
}

static int __init hashtable_module_init(void)
{
	pr_info("Loading hashtable module\n");

	add_objects();

	print_all_buckets();

	print_object_by_key(1);
	print_object_by_key(5);

	return 0;
}

static void __exit hashtable_module_exit(void)
{
	pr_info("Unloading hashtable module\n");

	free_all_objects();
}

module_init(hashtable_module_init);
module_exit(hashtable_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KGD");
MODULE_DESCRIPTION("Example Linux kernel module with hashtable");
