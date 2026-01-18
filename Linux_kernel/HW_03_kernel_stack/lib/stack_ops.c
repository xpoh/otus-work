#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include "inc/stack.h"
#include "inc/stack_ops.h"

void stack_init(void) {
  struct stack *s = get_stack();

  INIT_LIST_HEAD(&s->elements);
  s->size = 0;
}

int stack_push(int value) {
  struct stack *s = get_stack();
  struct stack_entry *entry;

  entry = kmalloc(sizeof(*entry), GFP_KERNEL);
  if (!entry)
    return STACK_NOMEM;

  entry->data = value;

  list_add(&entry->list, &s->elements);
  s->size++;

  return STACK_OK;
}

int stack_pop(int *value) {
  struct stack *s = get_stack();
  struct stack_entry *entry;

  if (list_empty(&s->elements)) {
    return STACK_EMPTY;
  }

  entry = list_first_entry(&s->elements, struct stack_entry, list);
  *value = entry->data;
  list_del(&entry->list);
  s->size--;

  kfree(entry);
  return STACK_OK;
}

int stack_peek(int *value) {
  struct stack *s = get_stack();
  struct stack_entry *entry;

  if (list_empty(&s->elements)) {
    return STACK_EMPTY;
  }

  entry = list_first_entry(&s->elements, struct stack_entry, list);
  *value = entry->data;

  return STACK_OK;
}

int stack_is_empty(void) {
  struct stack *s = get_stack();
  int empty;

  empty = list_empty(&s->elements);

  return empty;
}

int stack_size(void) {
  struct stack *s = get_stack();
  int size;

  size = s->size;

  return size;
}

void stack_clear(void) {
  struct stack *s = get_stack();
  struct stack_entry *entry, *tmp;

  list_for_each_entry_safe(entry, tmp, &s->elements, list) {
    list_del(&entry->list);
    kfree(entry);
  }
  s->size = 0;
}
