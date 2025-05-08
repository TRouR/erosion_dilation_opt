#include <rt_misc.h>

/*
 * Custom stack and heap configuration
 * - RAM region: 0x00003000 - 0x00058000 (336 KB)
 * - ZI data ~318.79 KB → occupies 0x00003000 to ~0x00055000
 * - Last 8 KB used for manual stack/heap allocation
 */

__value_in_regs struct __initial_stackheap __user_initial_stackheap(
    unsigned R0, unsigned SP, unsigned R2, unsigned SL) {

    struct __initial_stackheap config;

    // Heap region: 0x00055000 - 0x00057000 (2 KB)
    config.heap_base  = 0x00057000;   // Heap grows downward
    config.heap_limit = 0x00055000;

    // Stack region: 0x00057000 - 0x00058000 (6 KB)
    config.stack_base  = 0x00057000;  // Stack grows upward
    config.stack_limit = 0x00058000;

    return config;
}
