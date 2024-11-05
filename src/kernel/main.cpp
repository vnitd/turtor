#include "stdint.h"
#include "stddef.h"

#include "trap.h"
#include "debug.h"
#include "core/physic_memory.h"

extern "C" void KMain(void)
{
    init_idt();
    init_physic_memory();
}
