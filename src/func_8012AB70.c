#include "common.h"

extern s32 *D_800428F0;

s32 *func_8012AB70(void) {
    s32 *r = 0;
    s32 *p = D_800428F0;
    s32 *head = (s32 *) p[0xB];
    if (head != 0) {
        p[0xB] = head[0];
        r = head;
        head[0] = 0;
    }
    return r;
}
