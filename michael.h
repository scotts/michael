/*
 * Copyright (C) 2007  Scott Schneider, Christos Antonopoulos
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __MAGED_H__
#define __MAGED_H__

#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "atomic.h"
#include "queue.h"

struct Descriptor;
typedef struct Descriptor descriptor;
struct Procheap;
typedef struct Procheap procheap;

#define TYPE_SIZE	4
#define PTR_SIZE	sizeof(void*)
#define HEADER_SIZE	(TYPE_SIZE + PTR_SIZE)

#define LARGE		0
#define SMALL		1

#define	PAGESIZE	4096
#define SBSIZE		(16 * PAGESIZE)
#define DESCSBSIZE	(1024 * sizeof(descriptor))

#define ACTIVE		0
#define FULL		1
#define PARTIAL		2
#define EMPTY		3

#define	MAXCREDITS	64 // 2^(bits for credits in active)
#define GRANULARITY	8

/* We need to squeeze this in 64-bits, but conceptually
 * this is the case:
 *	descriptor* DescAvail;
 */
typedef struct {
	unsigned long long 	DescAvail:46, tag:18;
} descriptor_queue;

/* Superblock descriptor structure. We bumped avail and count 
 * to 24 bits to support larger superblock sizes. */
typedef struct {
	unsigned long long 	avail:24,count:24, state:2, tag:14;
} anchor;

struct Descriptor {
	struct queue_elem_t	lf_fifo_queue_padding;
	volatile anchor		Anchor;
	descriptor*		Next;
	void*			sb;		// pointer to superblock
	procheap*		heap;		// pointer to owner procheap
	unsigned int		sz;		// block size
	unsigned int		maxcount;	// superblock size / sz
};

typedef struct {
	lf_fifo_queue_t		Partial;	// initially empty
	unsigned int		sz;		// block size
	unsigned int		sbsize;		// superblock size
} sizeclass;

typedef struct {
	unsigned long long	ptr:58, credits:6;
} active;

struct Procheap {
	volatile active		Active;		// initially NULL
	volatile descriptor*	Partial;	// initially NULL
	sizeclass*		sc;		// pointer to parent sizeclass
};

extern void* malloc(size_t sz);
extern void free(void* ptr);

#endif	/* __MAGED_H__ */

