#ifndef __PHYSIC_MEMORY_H__
#define __PHYSIC_MEMORY_H__

#include "stdbool.h"
#include "stdint.h"

#define BLOCK_SIZE 4096
#define BLOCKS_PER_BYTE 8

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

void print_physic_memory();

void set_block(uint32_t bit);
void unset_block(uint32_t bit);
uint8_t test_block(uint32_t bit);
int32_t find_first_free_blocks(uint32_t num_blocks);
void initalize_memory_manager(uint32_t size, uint32_t start_address);
void initialize_memory_region(uint32_t base_address, uint32_t size);
void deinitialize_memory_region(uint32_t base_address, uint32_t size);
uint32_t *allocate_blocks(uint32_t num_blocks);
void free_blocks(uint32_t *address, uint32_t num_blocks);

#endif // !__PHYSIC_MEMORY_H__