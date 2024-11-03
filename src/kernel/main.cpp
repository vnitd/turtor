#include "stdint.h"
#include "stddef.h"

#include "trap.h"
#include "debug.h"

extern "C" void KMain(void)
{
    // char *p = (char *)0xb8000;
    // p[0x28] = 'C';
    // p[0x29] = 0xf;

    init_idt();
    // ASSERT(0);
}
