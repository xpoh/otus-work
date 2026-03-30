#ifndef KERNEL_ALLOC_ALLOCATOR_H
#define KERNEL_ALLOC_ALLOCATOR_H

#include <linux/spinlock.h>
#include <linux/types.h>

#define ALLOC_TOTAL_SIZE (10 * 1024 * 1024)                      /* 10 MiB */
#define ALLOC_BLOCK_SIZE 4096                                    /* 4 KiB */
#define ALLOC_TOTAL_BLOCKS (ALLOC_TOTAL_SIZE / ALLOC_BLOCK_SIZE) /* 2560 */
#define ALLOC_BITMAP_BYTES ((ALLOC_TOTAL_BLOCKS + 7) / 8)        /* 320 */

#define ALLOC_OK 0
#define ALLOC_NOMEM (-1)
#define ALLOC_INVALID (-2)
#define ALLOC_NOT_FOUND (-3)

#define MAX_ALLOCATIONS 1024

struct allocation_info {
  size_t start_block;
  size_t num_blocks;
  void *ptr;
};

struct stats_info {
  size_t total_blocks;
  size_t free_blocks;
  size_t allocated_blocks;
  size_t total_memory;
  size_t free_memory;
  size_t allocated_memory;
  size_t fragmentation_percent;
};

struct memory_allocator {
  unsigned char *bitmap;
  void *memory_pool;
  size_t total_blocks;
  size_t block_size;
  spinlock_t lock;
  struct allocation_info allocations[MAX_ALLOCATIONS];
  size_t num_allocations;
};

int allocator_init(void);
void *allocator_alloc(size_t bytes);
int allocator_free(void *ptr);
struct stats_info allocator_get_stats(void);
void allocator_cleanup(void);

/* Format bitmap state into buffer. Returns number of bytes written. */
int allocator_bitmap_info(char *buf, size_t buf_size);

#endif /* KERNEL_ALLOC_ALLOCATOR_H */
