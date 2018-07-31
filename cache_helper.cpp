#include "cache_helper.h"

unsigned int next_pow_of_2(unsigned int x) {
	unsigned int i=1;

	while (i < x) {
		i <<= 1;
	}

	return i;
}

void cache_helper::init_cache_sizes() {
	uint32_t size = BASE_CHUNK_SIZE;

	while (size < PAGE_SIZE_B) {
		cache_sizes.push_back(size);
		size =  next_pow_of_2(size * CLASS_SCALE);
		max_class_id++;
	}
}

bool cache_helper::is_valid_class_id(uint8_t id) {
	return id <= max_class_id;
}

// get routines

uint32_t cache_helper::get_chunk_size(uint8_t class_id) {
	return cache_sizes[class_id-1];
}

cache* cache_helper::get_cache(uint8_t class_id) {
	return caches[class_id-1];
}

uint8_t cache_helper::get_class_id(uint32_t size) {
	return 0;
}

void cache_helper::alloc_caches() {
	uint8_t class_id = 1;

	while (is_valid_class_id(class_id)) {
		caches.push_back(new cache(class_id, get_chunk_size(class_id)));
		class_id++;
	}
}

cache_helper::cache_helper() {
	max_class_id = 0;
	init_cache_sizes();
	alloc_caches();
}

