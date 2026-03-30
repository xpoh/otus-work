#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "allocator.h"
#include "bitmap.h"

static struct memory_allocator alloc;

int allocator_init(void) {
  memset(&alloc, 0, sizeof(alloc));

  alloc.total_blocks = ALLOC_TOTAL_BLOCKS;
  alloc.block_size = ALLOC_BLOCK_SIZE;
  spin_lock_init(&alloc.lock);

  alloc.bitmap = kzalloc(ALLOC_BITMAP_BYTES, GFP_KERNEL);
  if (!alloc.bitmap)
    return ALLOC_NOMEM;

  alloc.memory_pool = vzalloc(ALLOC_TOTAL_SIZE);
  if (!alloc.memory_pool) {
    kfree(alloc.bitmap);
    alloc.bitmap = NULL;
    return ALLOC_NOMEM;
  }

  pr_info("allocator initialized: %zu blocks of %zu bytes (%zu bytes total)\n",
          alloc.total_blocks, alloc.block_size,
          alloc.total_blocks * alloc.block_size);

  return ALLOC_OK;
}

void *allocator_alloc(size_t bytes) {
  size_t num_blocks;
  long start;
  void *ptr;
  unsigned long flags;

  if (bytes == 0) {
    pr_err("alloc: cannot allocate 0 bytes\n");
    return NULL;
  }

  num_blocks = (bytes + ALLOC_BLOCK_SIZE - 1) / ALLOC_BLOCK_SIZE;

  spin_lock_irqsave(&alloc.lock, flags);

  start = bm_find_free_region(alloc.bitmap, alloc.total_blocks, num_blocks);
  if (start < 0) {
    spin_unlock_irqrestore(&alloc.lock, flags);
    pr_err("alloc: no free region for %zu bytes (%zu blocks)\n", bytes,
           num_blocks);
    return NULL;
  }

  if (alloc.num_allocations >= MAX_ALLOCATIONS) {
    spin_unlock_irqrestore(&alloc.lock, flags);
    pr_err("alloc: max allocations reached\n");
    return NULL;
  }

  bm_set_region(alloc.bitmap, start, num_blocks);

  ptr = (char *)alloc.memory_pool + start * ALLOC_BLOCK_SIZE;

  alloc.allocations[alloc.num_allocations].start_block = start;
  alloc.allocations[alloc.num_allocations].num_blocks = num_blocks;
  alloc.allocations[alloc.num_allocations].ptr = ptr;
  alloc.num_allocations++;

  spin_unlock_irqrestore(&alloc.lock, flags);

  pr_info("allocated %zu bytes (%zu blocks) at %px (block %ld)\n", bytes,
          num_blocks, ptr, start);

  return ptr;
}

int allocator_free(void *ptr) {
  size_t i;
  unsigned long flags;

  if (!ptr) {
    pr_err("free: NULL pointer\n");
    return ALLOC_INVALID;
  }

  spin_lock_irqsave(&alloc.lock, flags);

  for (i = 0; i < alloc.num_allocations; i++) {
    if (alloc.allocations[i].ptr == ptr) {
      bm_clear_region(alloc.bitmap, alloc.allocations[i].start_block,
                      alloc.allocations[i].num_blocks);

      pr_info("freed %zu blocks at %px (block %zu)\n",
              alloc.allocations[i].num_blocks, ptr,
              alloc.allocations[i].start_block);

      /* Remove from allocations array by shifting */
      alloc.num_allocations--;
      if (i < alloc.num_allocations)
        alloc.allocations[i] = alloc.allocations[alloc.num_allocations];

      spin_unlock_irqrestore(&alloc.lock, flags);
      return ALLOC_OK;
    }
  }

  spin_unlock_irqrestore(&alloc.lock, flags);
  pr_err("free: pointer %px not found\n", ptr);
  return ALLOC_NOT_FOUND;
}

struct stats_info allocator_get_stats(void) {
  struct stats_info stats;
  unsigned long flags;

  spin_lock_irqsave(&alloc.lock, flags);

  stats.total_blocks = alloc.total_blocks;
  stats.allocated_blocks = bm_count_set(alloc.bitmap, alloc.total_blocks);
  stats.free_blocks = stats.total_blocks - stats.allocated_blocks;
  stats.total_memory = stats.total_blocks * alloc.block_size;
  stats.free_memory = stats.free_blocks * alloc.block_size;
  stats.allocated_memory = stats.allocated_blocks * alloc.block_size;
  stats.fragmentation_percent =
      bm_fragmentation_percent(alloc.bitmap, alloc.total_blocks);

  spin_unlock_irqrestore(&alloc.lock, flags);

  return stats;
}

void allocator_cleanup(void) {
  if (alloc.memory_pool) {
    vfree(alloc.memory_pool);
    alloc.memory_pool = NULL;
  }
  if (alloc.bitmap) {
    kfree(alloc.bitmap);
    alloc.bitmap = NULL;
  }
  alloc.num_allocations = 0;

  pr_info("allocator cleaned up\n");
}

int allocator_bitmap_info(char *buf, size_t buf_size) {
  int len = 0;
  size_t i;
  unsigned long flags;

  spin_lock_irqsave(&alloc.lock, flags);

  /* Show first 80 blocks as visual map */
  len += scnprintf(buf + len, buf_size - len, "Bitmap (first 80 blocks): [");
  for (i = 0; i < 80 && i < alloc.total_blocks; i++)
    len += scnprintf(buf + len, buf_size - len, "%c",
                     bm_test_bit(alloc.bitmap, i) ? 'X' : '.');

  len += scnprintf(buf + len, buf_size - len, "]\n");

  /* List all current allocations */
  len += scnprintf(buf + len, buf_size - len, "Active allocations: %zu\n",
                   alloc.num_allocations);
  for (i = 0; i < alloc.num_allocations; i++) {
    len += scnprintf(buf + len, buf_size - len,
                     "  [%zu] block %zu, %zu blocks, ptr %px\n", i,
                     alloc.allocations[i].start_block,
                     alloc.allocations[i].num_blocks, alloc.allocations[i].ptr);
  }

  spin_unlock_irqrestore(&alloc.lock, flags);

  return len;
}
