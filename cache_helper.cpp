#include "cache_helper.h"
#include <iostream>

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

uint32_t cache_helper::get_true_size(uint32_t sz) {
	return sizeof(item_t) + sz;
}

cache* cache_helper::get_cache(uint8_t class_id) {
	return caches[class_id-1];
}

uint8_t cache_helper::get_class_id(uint32_t size) {
	int low = 0, high = max_class_id-1;
	int mid;

	if (size > get_chunk_size(high) || size < 0) return 0;

	while (low <= high) {
		mid = low + (high - low)/2;

		if (size > get_chunk_size(mid) &&
		    size <= get_chunk_size(mid+1)) {
			return mid+2;
		} else if (size <= get_chunk_size(mid)) {
			high = mid-1;
		} else if (size > get_chunk_size(mid+1)) {
			low = mid+1;
		}
	}

	return 1;
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

cache_helper::~cache_helper() {
	while (!caches.empty())
		delete caches.back();
}

int cache_helper::cache_get(uint8_t *key, uint8_t nkey, uint8_t *value) {
	uint8_t cid = get_class_id(get_true_size(cache::get_kv_len(key, nkey)));

	if (cid == 0) return -1;

	return get_cache(cid)->cache_get(key, nkey, value);
}

bool cache_helper::cache_set(uint8_t *key, uint8_t nkey,
		       uint8_t *value, uint32_t nbytes) {
	uint8_t cid = get_class_id(get_true_size(nkey + nbytes));

	if (cid == 0) return false;

	return get_cache(cid)->cache_set(key, nkey, value, nbytes);
}
