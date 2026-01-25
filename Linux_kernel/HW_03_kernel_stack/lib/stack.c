#include "inc/stack.h"

static struct stack stack_instance;

struct stack *get_stack(void) { return &stack_instance; }
