//
// Created by Rekunov Dmitriy on 21.12.2025.
//
#include <asm-generic/errno-base.h>

#include "idx_param.h"
#include "ch_val_param.h"
#include "my_str_param.h"
#include "update.h"

int update_hello_world(void)
{
	if (idx >= MY_STR_MAX_SIZE) {
		return E2BIG;
	}

	my_str_param[idx] = ch_val;
	my_str_param[MY_STR_MAX_SIZE - 1] = '\0';

	return 0;
}
