#ifndef __QUEUE_H_
#define __QUEUE_H_

#include "atomic.h"

typedef struct {
	volatile void* top;
} top_aba_t;

// Pseudostructure for lock-free list elements.
// The only requirement is that the 5th-8th byte of
// each element should be available to be used as
// the pointer for the implementation of a singly-linked
// list. 
struct queue_elem_t {
	char 				*_dummy;
	volatile struct queue_elem_t 	*next;
};

typedef struct {
	unsigned long long 	_pad0[8];
	top_aba_t		both;
	unsigned long long 	_pad1[8];
} lf_fifo_queue_t;

#define LF_FIFO_QUEUE_STATIC_INIT	{{0, 0, 0, 0, 0, 0, 0, 0}, {0}, {0, 0, 0, 0, 0, 0, 0, 0}}
					  
/******************************************************************************/

static __inline__ void lf_fifo_queue_init(lf_fifo_queue_t *queue);
static __inline__ int lf_fifo_enqueue(lf_fifo_queue_t *queue, void *element);
static __inline__ void *lf_fifo_dequeue(lf_fifo_queue_t *queue);

/******************************************************************************/

static __inline__ void lf_fifo_queue_init(lf_fifo_queue_t *queue)
{
	queue->both.top = 0;
}

/******************************************************************************/

static __inline__ void *lf_fifo_dequeue(lf_fifo_queue_t *queue)
{
	top_aba_t head;
	top_aba_t next;

	while(1) {
		head.top = queue->both.top;
		if (head.top == 0) {
			return NULL;
		}
		next.top = ((struct queue_elem_t *)head.top)->next;
		if (compare_and_swap64((volatile unsigned long long *)&(queue->both), *((unsigned long long*)&head), *((unsigned long long*)&next))) {
			return ((void *)head.top);
		}
	}
}

/******************************************************************************/

static __inline__ int lf_fifo_enqueue(lf_fifo_queue_t *queue, void *element)
{
	top_aba_t old_top;
	top_aba_t new_top;
	
	while(1) {
		old_top.top = queue->both.top;

		((struct queue_elem_t *)element)->next = (struct queue_elem_t *)old_top.top;
		new_top.top = element;
		if (compare_and_swap64((volatile unsigned long long*)&(queue->both), *((unsigned long long*)&old_top), *((unsigned long long*)&new_top))) {
			return 0;
		}
	}
}

#endif

