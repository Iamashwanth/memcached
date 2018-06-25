#include <iostream>
#include "lru.h"

void* LRU::get(key_t key) {
	void *obj;
	list<void*>::iterator list_it;
	auto map_it = obj_map.find(key);

	if (map_it != obj_map.end()) {
		list_it = (*map_it).second;
		obj = *list_it;
		obj_list.erase(list_it);
		obj_list.push_front(obj);
		obj_map[key] = obj_list.begin();
		return obj_list.front();
	}

	return NULL;
}

void LRU::set(key_t key, void *value) {
	auto it = obj_map.find(key);

	if (it != obj_map.end()) {
		obj_list.erase((*it).second);
	}
	else if (obj_list.size() >= capacity)
		obj_list.pop_back();

	obj_list.push_front(value);
	obj_map[key] = obj_list.begin();
}

#ifdef LRU_DEBUG
void LRU::print_lru() {
	for(const auto obj : obj_list)
		std::cout << obj << std::endl;
}
#endif
