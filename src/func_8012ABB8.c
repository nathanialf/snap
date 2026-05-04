#include "common.h"

extern void func_80029CA0(s32 *);
extern void func_80029C7C(s32 *, s32 *);

void func_8012ABB8(s32 *arg0) {
    s32 *node = (s32 *) arg0[0x14 / 4];
    if (node != 0) {
        do {
            func_80029CA0(node);
            func_80029C7C(node, &arg0[0x4 / 4]);
            node = (s32 *) arg0[0x14 / 4];
        } while (node != 0);
    }
}
