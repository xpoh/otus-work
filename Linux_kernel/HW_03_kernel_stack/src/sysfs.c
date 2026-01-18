#include <linux/device.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "../inc/kernel_stack.h"

static struct kobject *stack_kobj;

static ssize_t push_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count) {
  int value;
  int ret;

  ret = kstrtoint(buf, 10, &value);
  if (ret)
    return ret;

  ret = stack_push(value);
  if (ret != STACK_OK)
    return -ENOMEM;

  return count;
}

static struct kobj_attribute push_attr = __ATTR_WO(push);

static ssize_t pop_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf) {
  int value;
  int ret;

  ret = stack_pop(&value);
  if (ret == STACK_EMPTY)
    return -ENODATA;

  return sysfs_emit(buf, "%d\n", value);
}

static struct kobj_attribute pop_attr = __ATTR_RO(pop);

static ssize_t peek_show(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf) {
  int value;
  int ret;

  ret = stack_peek(&value);
  if (ret == STACK_EMPTY)
    return -ENODATA;

  return sysfs_emit(buf, "%d\n", value);
}

static struct kobj_attribute peek_attr = __ATTR_RO(peek);

static ssize_t size_show(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf) {
  return sysfs_emit(buf, "%d\n", stack_size());
}

static struct kobj_attribute size_attr = __ATTR_RO(size);

static ssize_t is_empty_show(struct kobject *kobj, struct kobj_attribute *attr,
                             char *buf) {
  return sysfs_emit(buf, "%d\n", stack_is_empty());
}

static struct kobj_attribute is_empty_attr = __ATTR_RO(is_empty);

static ssize_t clear_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t count) {
  stack_clear();
  return count;
}

static struct kobj_attribute clear_attr = __ATTR_WO(clear);

static struct attribute *stack_attrs[] = {
    &push_attr.attr,
    &pop_attr.attr,
    &peek_attr.attr,
    &size_attr.attr,
    &is_empty_attr.attr,
    &clear_attr.attr,
    NULL,
};

static const struct attribute_group stack_attr_group = {
    .attrs = stack_attrs,
};

int stack_sysfs_init(void) {
  int ret;

  stack_kobj = kobject_create_and_add("kernel_stack", kernel_kobj);
  if (!stack_kobj)
    return -ENOMEM;

  ret = sysfs_create_group(stack_kobj, &stack_attr_group);
  if (ret) {
    kobject_put(stack_kobj);
    return ret;
  }

  return 0;
}

void stack_sysfs_exit(void) {
  sysfs_remove_group(stack_kobj, &stack_attr_group);
  kobject_put(stack_kobj);
}
