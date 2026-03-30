#include "bitmap.h"

long bm_find_free_region(const unsigned char *bmap, size_t total_bits,
                         size_t num_blocks) {
  size_t count = 0;
  size_t start = 0;
  size_t i;

  for (i = 0; i < total_bits; i++) {
    if (!bm_test_bit(bmap, i)) {
      if (count == 0)
        start = i;
      count++;
      if (count == num_blocks)
        return (long)start;
    } else {
      count = 0;
    }
  }

  return -1;
}

void bm_set_region(unsigned char *bmap, size_t start, size_t num_blocks) {
  size_t i;

  for (i = start; i < start + num_blocks; i++)
    bmap[i / 8] |= (1 << (i % 8));
}

void bm_clear_region(unsigned char *bmap, size_t start, size_t num_blocks) {
  size_t i;

  for (i = start; i < start + num_blocks; i++)
    bmap[i / 8] &= ~(1 << (i % 8));
}

int bm_test_bit(const unsigned char *bmap, size_t bit) {
  return (bmap[bit / 8] >> (bit % 8)) & 1;
}

size_t bm_count_set(const unsigned char *bmap, size_t total_bits) {
  size_t count = 0;
  size_t i;

  for (i = 0; i < total_bits; i++) {
    if (bm_test_bit(bmap, i))
      count++;
  }

  return count;
}

size_t bm_fragmentation_percent(const unsigned char *bmap, size_t total_bits) {
  size_t free_regions = 0;
  size_t free_blocks = 0;
  int in_free = 0;
  size_t i;

  for (i = 0; i < total_bits; i++) {
    if (!bm_test_bit(bmap, i)) {
      free_blocks++;
      if (!in_free) {
        free_regions++;
        in_free = 1;
      }
    } else {
      in_free = 0;
    }
  }

  if (free_blocks == 0 || free_regions <= 1)
    return 0;

  return (free_regions - 1) * 100 / free_regions;
}
