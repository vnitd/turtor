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

void init_physic_memory();

void set_block(uint32_t bit);
void unset_block(uint32_t bit);
uint8_t test_block(uint32_t bit);

#endif // !__PHYSIC_MEMORY_H__