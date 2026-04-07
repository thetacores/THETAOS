// SPDX-License-Identifier: GPL-2.0
/*
 * kmalloc.c - THETAOS kernel memory allocator
 *
 * Free-list allocator over a static 4 MB heap.
 * Allocations are 8-byte aligned.
 * Adjacent free blocks are coalesced on kfree().
 */

#include <klib/kmalloc.h>
#include <klib/kprintf.h>

#include <stdint.h>

#define HEAP_SIZE    (4 * 1024 * 1024)   /* 4 MB */
#define ALIGN        8u
#define ALIGN_UP(n)  (((n) + ALIGN - 1) & ~(ALIGN - 1))

/*
 * Every allocation is preceded by this header.
 * `size` is the usable data region (not including the header itself).
 */
typedef struct block {
	size_t       size;
	uint32_t     free;
	struct block *next;
} block_t;

#define HEADER_SIZE  ALIGN_UP(sizeof(block_t))

static uint8_t  heap[HEAP_SIZE] __attribute__((aligned(ALIGN)));
static block_t *head;

void kmalloc_init(void)
{
	head        = (block_t *)heap;
	head->size  = HEAP_SIZE - HEADER_SIZE;
	head->free  = 1;
	head->next  = NULL;
	kprintf("[HEAP] heap allocator initialized (4 MB)\n");
}

void *kmalloc(size_t size)
{
	if (!size)
		return NULL;

	size = ALIGN_UP(size);

	block_t *cur = head;

	while (cur) {
		if (cur->free && cur->size >= size) {
			/* Split if there is room for another block after this one. */
			if (cur->size >= size + HEADER_SIZE + ALIGN) {
				block_t *split = (block_t *)((uint8_t *)cur + HEADER_SIZE + size);

				split->size  = cur->size - size - HEADER_SIZE;
				split->free  = 1;
				split->next  = cur->next;
				cur->size    = size;
				cur->next    = split;
			}
			cur->free = 0;
			return (uint8_t *)cur + HEADER_SIZE;
		}
		cur = cur->next;
	}

	return NULL; /* out of memory */
}

void kfree(void *ptr)
{
	if (!ptr)
		return;

	block_t *blk = (block_t *)((uint8_t *)ptr - HEADER_SIZE);

	blk->free = 1;

	/* Coalesce with consecutive free neighbours. */
	block_t *cur = head;

	while (cur && cur->next) {
		if (cur->free && cur->next->free) {
			cur->size += HEADER_SIZE + cur->next->size;
			cur->next  = cur->next->next;
		} else {
			cur = cur->next;
		}
	}
}

void kdebug(void)
{
	block_t *cur = head;
	size_t   idx = 0;

	while (cur) {
		kprintf("[kmalloc] block %d: addr=0x%x size=%d %s\n",
				(int)idx,
				(uint32_t)((uint8_t *)cur + HEADER_SIZE),
				(int)cur->size,
				cur->free ? "FREE" : "USED");
		cur = cur->next;
		idx++;
	}
}
