#include "stdint.h"
#include "stddef.h"

#include "trap.h"
#include "debug.h"
#include "memory.h"

extern "C" void KMain(void)
{
    init_idt();
    init_memory();
    init_kvm();
}
