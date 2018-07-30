#include <cstdint>
#include "page.h"

uint8_t *mem_pool;
unsigned int pool_size;
unsigned int used;

/* allocates a pool of memory to serve page requests
 * size - mem pool size in MB
 */

bool page_allocator_init(unsigned int size) {
	mem_pool = (uint8_t*)new uint8_t[size * PAGE_SIZE_B];

	if (!mem_pool) return false;

	pool_size = size;
	used = 0;
	return true;
}

void* alloc_page() {
	if (mem_pool && used < pool_size) {
		return mem_pool + PAGE_SIZE_B * (used++);
	}

	return nullptr;
}

/* setup sig handler */
void page_allocator_exit() {
}
