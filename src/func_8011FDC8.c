#include "common.h"

typedef struct {
    s16 _h0;
    s16 count;
    struct {
        s32 val;
        s32 _w4;
    } entries[];
} Foo;

void func_8011FDC8(Foo *arg0, s32 arg1) {
    s32 i;
    for (i = 0; i < arg0->count; i++) {
        arg0->entries[i].val += arg1;
    }
}
