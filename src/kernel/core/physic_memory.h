#ifndef __PHYSIC_MEMORY_H__
#define __PHYSIC_MEMORY_H__

#include "stdbool.h"
#include "stdint.h"

#define MEMMAP 0x10000

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BYTE 8
extern uint64_t max_blocks;

struct E820
{
    uint64_t address;
    uint64_t length;
    uint32_t type;
} __attribute__((packed));

struct FreeMemRegion
{
    uint64_t address;
    uint64_t length;
};

uint64_t print_physic_memory();

void set_block(uint64_t bit);
void unset_block(uint64_t bit);
uint8_t test_block(uint64_t bit);
int64_t find_first_free_blocks(uint64_t num_blocks);
void initalize_memory_manager(uint64_t size, uint64_t start_address);
void initialize_memory_region(uint64_t base_address, uint64_t size);
void deinitialize_memory_region(uint64_t base_address, uint64_t size);
void *allocate_blocks(uint64_t num_blocks);
void free_blocks(uint64_t address, uint64_t num_blocks);

#endif // !__PHYSIC_MEMORY_H__