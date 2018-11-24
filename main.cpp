#include <stdint.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#define KIB 1024
#define MIB (1024 * KIB)
#define GIB (1024 * MIB)

#define STEPBY 64

typedef uint64_t cycles_t;

static inline void code_barrier(void) {
	asm volatile("cpuid\n" :: "a" (0) : "%rbx", "%rcx", "%rdx");
}

static inline void data_barrier(void) {
	asm volatile("mfence\n" ::: "memory");
}

static inline cycles_t rdtscp(void) {
	cycles_t cycles_lo, cycles_hi;
	
	asm volatile("rdtscp\n" :
		"=a" (cycles_lo), "=d" (cycles_hi) ::
		"%rcx");

	return ((uint64_t)cycles_hi << 32) | cycles_lo;
}

void evict_cache(volatile char* p) {
	for(int i = 0; i < (1*GIB); i += STEPBY) {
		*(p + i) = 0x42;
	}
}

uint64_t profile_access(volatile char *p) {
	uint64_t past, now;

	data_barrier();
	code_barrier();
	past = rdtscp();
	data_barrier();

	*p = 0x5A;
	
	data_barrier();
	now = rdtscp();
	code_barrier();
	data_barrier();

	return now - past;
}

int main() {

	char* p = (char*)malloc(1);

	char* evict = (char*)malloc(1 * GIB);

	for(int i = 0; i < 16; i++) {
		if (i % 3 == 0)
			evict_cache(evict);
		std::cout << profile_access(p) << std::endl;
	}

	free(p);
	free(evict);
	return 0;
}

// static inline cycles_t rdtsc(void) {
// 	cycles_t cycles_lo, cycles_hi;
// 
// 	asm volatile("rdtsc\n" :
// 		"=a" (cycles_lo), "=d" (cycles_hi));
// 	return ((uint64_t)cycles_hi << 32) | cycles_lo;
// }

