#ifndef __SYNCHRO_ATOMIC_H__
#define __SYNCHRO_ATOMIC_H__

#define __fool_gcc(x) (*(struct {unsigned int a[100];} *)x)

inline unsigned long fetch_and_store(volatile unsigned long *address,
				     unsigned long value);
inline void atmc_add(volatile long *address, long value);
inline unsigned long compare_and_swap32(volatile unsigned long *address,
				unsigned long old_value,
				unsigned long new_value);

inline unsigned long compare_and_swap64(volatile unsigned long long *address,
				unsigned long long old_value,
				unsigned long long new_value);

inline unsigned long compare_and_swap_ptr(volatile unsigned long *address, 
				unsigned long old_value,
				unsigned long new_value);
/***************************************************************************/

#define fetch_and_store(address, value) \
({ \
 volatile unsigned long ret_val; \
\
 __asm__ __volatile__ ("lock; xchgl %0, %1" \
 		       :"=r" ((ret_val)) \
 		       :"m" (__fool_gcc((address))), "0" ((value)) \
 		       :"memory"); \
 ret_val; \
})

#define atmc_add(address, value) \
({ \
  register volatile long val = (value); \
\
  __asm__ __volatile__ ("lock; addl %1, %0" \
  			: "=m" (*(address)) \
  			: "r" (val)); \
})

#define compare_and_swap32(address, old_value, new_value) \
({ \
	unsigned long ret_val = 0; \
	__asm__ __volatile__ ("lock\n\t" \
			"cmpxchgl %2, (%1)\n\t" \
			"sete (%3)\n\t" \
			: \
			: "a" (old_value), "r" (address), "r" (new_value), \
			"r" (&ret_val) \
			: "memory"); \
	ret_val; \
})

#define compare_and_swap64(address, old_value, new_value) \
({ \
	unsigned long ret_val = 0; \
	__asm__ __volatile__ ("lock\n\t" \
			"cmpxchg8b (%0)\n\t" \
			"sete (%1)\n\t" \
			: \
			: "r" (address), "r" (&ret_val), \
			"d" (*(((unsigned int*)&(old_value))+1)), "a" (*(((unsigned int*)&(old_value))+0)), \
			"c" (*(((unsigned int*)&(new_value))+1)), "b" (*(((unsigned int*)&(new_value))+0)) \
			: "memory"); \
			ret_val; \
})

#define compare_and_swap_ptr(address, old_value, new_value) compare_and_swap32(address, old_value, new_value)

#endif

