// SPDX-License-Identifier: GPL-2.0
/*
 * kmalloc.h - THETAOS kernel memory allocator
 */

#ifndef KLIB_KMALLOC_H
#define KLIB_KMALLOC_H

#include <stddef.h>

void  kmalloc_init(void);
void *kmalloc(size_t size);
void  kfree(void *ptr);
void  kdebug(void);

#endif /* KLIB_KMALLOC_H */
