#ifndef SLAB_H
#define SLAB_H

#include <cstdint>

#include <list>
#include <vector>
#include <string>
#include <unordered_map>

#include "page.h"

using namespace std;

/*
 * Slab allocator
 * Dynamic memory allocation based on cmd line config
 * Add timestamps for garbage collection
 * Pages cannot be re-assigned across slab classes
 * Chunk size scale by factor of 1.25 rounded up to the next power of 2
 * Each page is of size 1M
 * allocation services on the fly
 * per cache locking
 */

struct item_t {
	uint8_t nkey; // key len in bytes
	uint32_t nbytes; // entire data len in bytes
	// placeholders
	// Timestamp - gc for items
	// page ptr - refcnt and gc for pages
	uint8_t data[]; // stores null-terminated key + value followed by\r\n
};


class cache {
public:
	cache(uint8_t cid, uint32_t size);
	~cache();

	static uint32_t get_kv_len(uint8_t* key, uint8_t nkey);
	item_t* cache_alloc(uint8_t*, uint8_t, uint8_t*, uint32_t);

	int cache_get(uint8_t *key, uint8_t nkey, uint8_t *value);
	bool cache_set(uint8_t *key, uint8_t nkey,
		       uint8_t *value, uint32_t nbytes);
/*
	cache_free();
	cache_destory();

*/
private:
	uint8_t class_id;
	uint32_t item_size;
	uint32_t count;

	list<item_t*> item_list;
	list<item_t*> free_list;
	list<page_t*> page_list;
	/* set up a custom hash func for unit8_t* */
	static unordered_map<string, list<item_t*>::iterator> item_map;

	cache();
	item_t* cache_alloc();
	bool cache_grow();
};

#endif
