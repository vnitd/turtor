#include "stdint.h"
#include "stddef.h"

#include "trap.h"
#include "debug.h"
#include "core/physic_memory.h"

extern "C" void KMain(void)
{
    init_idt();
    print_physic_memory();

    uint32_t num_SMAP_entries = *(uint32_t *)0x9000;
    struct E820 *mem_map = (struct E820 *)0x9008;
}
