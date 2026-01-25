#ifndef STACK_OPS_H
#define STACK_OPS_H

void stack_init(void);
int stack_push(int value);
int stack_pop(int *value);
int stack_peek(int *value);
int stack_is_empty(void);
int stack_size(void);
void stack_clear(void);

#endif
