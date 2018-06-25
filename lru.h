#ifndef LRU_H
#define LRU_H

#include <cstdint>
#include <unordered_map>
#include <list>
#include <vector>

using namespace std;

#define key_t uint64_t
#define class_t uint8_t

class LRU {
	public:
	void* get(uint64_t key);
	void set(uint64_t key, void *value);
	LRU(uint64_t c) : capacity{c} {}
#ifdef LRU_DEBUG
	void print_lru();
#endif
	private:
	uint64_t capacity;
	list<void*> obj_list;
	unordered_map<key_t, list<void*>::iterator> obj_map;

	LRU();
};

class LRU_container {
	vector<LRU> lru_vec;
	unordered_map<key_t, class_t> lru_map;
	LRU_container(unsigned int cnt) {
		for (int i=0; i<cnt; i++)
			lru_vec.push_back(LRU(1));
	}
};

#endif
