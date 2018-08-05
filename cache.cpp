#include <cstring>
#include <iostream>

#include "cache.h"

unordered_map<string, list<item_t*>::iterator> cache::item_map;

// Current TODO's
// - locking
// - timestamps
// - debug logging

cache::cache(uint8_t cid, uint32_t size) : class_id{cid}, item_size{size} {
	cout << "cache class " << unsigned(class_id) << ": " <<
		"item size: " << unsigned(item_size) << ": ";

	cache_grow();
	/* handle failures by setting init memeber*/

	cout << "item count: " << unsigned(count) << endl;
}

cache::~cache() {
	while(!page_list.empty())
		delete page_list.back();
}

uint32_t cache::get_kv_len(uint8_t* key, uint8_t nkey) {
	item_t *buf;
	list<item_t*>::iterator item_it;

	string key_str(key, key+nkey);
	auto map_it = item_map.find(key_str);

	if (map_it != item_map.end()) {
		item_it = (*map_it).second;
		buf = *item_it;
		return buf->nkey + buf->nbytes;
	}

	return 0;
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

	while (off+item_size < PAGE_SIZE_B) {
		buf = (item_t*) ((uint8_t*)mem + off);
		free_list.push_back(buf);

		count++;
		off += item_size;
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

int cache::cache_get(uint8_t *key, uint8_t nkey, uint8_t *value) {
	item_t *buf;
	list<item_t*>::iterator item_it;

	string key_str(key, key+nkey);
	auto map_it = item_map.find(key_str);

	if (map_it == item_map.end()) return -1;

	item_it = (*map_it).second;
	buf = *item_it;
	item_list.erase(item_it);
	item_list.push_front(buf);
	item_map[key_str] = item_list.begin();

	memcpy(value, buf->data + buf->nkey, buf->nbytes);

	return (int)buf->nbytes;
}

bool cache::cache_set(uint8_t *key, uint8_t nkey,
			   uint8_t *value, uint32_t nbytes) {
	item_t *buf;
	list<item_t*>::iterator item_it;

	string key_str(key, key+nkey);
	auto map_it = item_map.find(key_str);

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
