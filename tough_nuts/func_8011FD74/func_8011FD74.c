#include "common.h"

typedef struct {
    u8 *start;
    u8 *cur;
    s32 cap;
} Arena;

void *func_8011FD74(arg0, arg1, arena, count, size)
s32 arg0;
s32 arg1;
Arena *arena;
s32 count;
s32 size;
{
    void *result = 0;
    s32 sz = (count * size + 0xF) & ~0xF;
    if (arena->start + arena->cap >= arena->cur + sz) {
        result = arena->cur;
        arena->cur = arena->cur + sz;
    }
    return result;
}
