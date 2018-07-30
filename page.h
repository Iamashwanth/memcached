#ifndef PAGE_H
#define PAGE_H

#define PAGE_SIZE_B 1048576 // 1MB

struct page_t {
	uint8_t class_id;
	uint16_t ref_cnt; // Can go as high as 1MB/128B ~ 2^13. Need min 14 bits
	void *mem;
};

bool page_allocator_init(unsigned int size);
void page_allocator_exit();
void* alloc_page();


#endif
