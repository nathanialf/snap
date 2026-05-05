#include "common.h"

s32 func_8011FD74(arg0, arg1, pool, count, elem_size)
s32 arg0;
s32 arg1;
s32 *pool;
s32 count;
s32 elem_size;
{
    s32 aligned = (count * elem_size + 0xF) & -0x10;
    s32 alloc_addr = pool[1];
    s32 result = 0;

    if (pool[0] + pool[2] >= alloc_addr + aligned) {
        result = alloc_addr;
        pool[1] = alloc_addr + aligned;
    }
    return result;
}
