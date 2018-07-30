#include <cstring>
#include <iostream>

#include "cache.h"

// Current TODO's
// - locking
// - timestamps
// - debug logging

vector<uint32_t> cache::cache_sizes = {};

unsigned int next_pow_of_2(unsigned int x) {
	unsigned int i=1;

	while (i < x) {
		i <<= 1;
	}

	return i;
}

void cache::init_cache_sizes() {
	uint32_t size = BASE_CHUNK_SIZE;

	while (size < PAGE_SIZE_B) {
		cache_sizes.push_back(size);
		size =  next_pow_of_2(size * CLASS_SCALE);
	}
}

uint32_t cache::chunk_size(uint8_t classid) {
	return cache_sizes[classid-1];
}

cache::cache(uint8_t class_id) : class_id{class_id} {
	item_size = chunk_size(class_id);

	cout << "cache class " << unsigned(class_id) << ": " <<
		"item size: " << unsigned(item_size) << ": ";

	cache_grow();

	cout << "item count: " << unsigned(count) << endl;
}

/* Grow the cache by acquring a page from mempool
 * Add the chunks to the free list
 */

bool cache::cache_grow() {
	uint32_t off = 0;
	page_t* pg;
	item_t* buf;
	void* mem;

	if (!(mem = alloc_page()))
		return false;

	pg = (page_t*) new page_t;

	pg->mem = mem;
	pg->class_id = class_id;
	pg->ref_cnt = 0;
	page_list.push_back(pg);

	while (off+chunk_size(class_id) < PAGE_SIZE_B) {
		buf = (item_t*) new item_t;
		buf->data = (uint8_t*)mem + off;
		free_list.push_back(buf);

		count++;
		off += chunk_size(class_id);
	}

	return true;
}

/* Try to allocate a page if the free list is empty
 * Move an item from free list to the item list
 * Evict the oldest entry if we are out of pages */

item_t* cache::cache_alloc() {
	item_t* buf;

	if (free_list.size() || cache_grow()) {
		buf = free_list.front();
		free_list.pop_front();
	} else {
		// replace the least used entry
		buf = item_list.back();
		item_list.pop_back();

		string key_str(buf->data, buf->data+buf->nkey);
		item_map.erase(key_str);
	}

	return buf;
}

/* Returns the size of the value in bytes
 * 0 in case the key does not exist
 */

uint32_t cache::cache_get(uint8_t *key, uint8_t nkey, uint8_t *value) {
	item_t *buf;
	list<item_t*>::iterator item_it;

	string key_str(key, key+nkey);
	auto map_it = item_map.find(key_str);

	if (map_it == item_map.end()) return 0;

	item_it = (*map_it).second;
	buf = *item_it;
	item_list.erase(item_it);
	item_list.push_front(buf);
	item_map[key_str] = item_list.begin();

	memcpy(value, buf->data + buf->nkey, buf->nbytes);

	return buf->nbytes;
}

bool cache::cache_set(uint8_t *key, uint8_t nkey,
			   uint8_t *value, uint32_t nbytes) {
	item_t *buf;
	list<item_t*>::iterator item_it;

	string key_str(key, key+nkey);
	auto map_it = item_map.find(key_str);

	if (nkey + nbytes > item_size)
	    return false;

	if (map_it != item_map.end()) {
		item_it = (*map_it).second;
		buf = *item_it;
		item_list.erase(item_it);
	} else {
		buf = cache_alloc();

		memcpy(buf->data, key, nkey);
		buf->nkey = nkey;
	}

	// update timestamp

	memcpy(buf->data + nkey, value, nbytes);
	buf->nbytes = nbytes;

	item_list.push_front(buf);
	item_map[key_str] = item_list.begin();

	return true;
}

bool cache_free(uint8_t *key, uint8_t nkey) {
	return true;
}
