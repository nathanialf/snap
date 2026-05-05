#include "common.h"

typedef struct {
    s32 a;
    s32 b;
    s32 c;
    char _pad[2];
    u8 visited;
} Foo;

extern void func_80020030(s32 *);

void func_8011FFD8(Foo *arg0, s32 arg1) {
    if (arg0->visited) {
        return;
    }
    arg0->visited = 1;
    arg0->a += arg1;
    arg0->b += arg1;
    arg0->c += arg1;
    func_80020030((s32 *) arg0->c);
}
