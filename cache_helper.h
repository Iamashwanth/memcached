#ifndef CACHE_HELPER_H
#define CACHE_HELPER_H

#include <cmath>

#include "cache.h"

#define BASE_CHUNK_SIZE 128
#define CLASS_SCALE 1.25

class cache_helper {
public:
	cache_helper();
	uint8_t get_class_id(uint32_t size);

private:
	vector<cache*> caches;
	vector<uint32_t> cache_sizes;
	uint8_t max_class_id; // equivalent to number of elements

	void alloc_caches();
	void init_cache_sizes();
	bool is_valid_class_id(uint8_t id);
	uint32_t get_chunk_size(uint8_t classid);
	cache* get_cache(uint8_t class_id);
};

#endif
