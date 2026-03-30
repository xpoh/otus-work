#ifndef KERNEL_ALLOC_BITMAP_H
#define KERNEL_ALLOC_BITMAP_H

#include <linux/types.h>

long bm_find_free_region(const unsigned char *bmap, size_t total_bits,
                         size_t num_blocks);

void bm_set_region(unsigned char *bmap, size_t start, size_t num_blocks);

void bm_clear_region(unsigned char *bmap, size_t start, size_t num_blocks);

int bm_test_bit(const unsigned char *bmap, size_t bit);

size_t bm_count_set(const unsigned char *bmap, size_t total_bits);

size_t bm_fragmentation_percent(const unsigned char *bmap, size_t total_bits);

#endif /* KERNEL_ALLOC_BITMAP_H */
