#include "common.h"

extern s32 *D_800428F0;

extern void func_80029CA0(void *);
extern void func_80029C7C(void *, s32 *);

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

void func_8012ABA0(s32 *arg0) {
    s32 *p = D_800428F0;
    arg0[0] = p[0xB];
    p[0xB] = (s32) arg0;
}

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

void func_8012AC18(s32 *arg0, s32 arg1) {
    func_80029CA0((void *) arg1);
    func_80029C7C((void *) arg1, arg0 + 5);
}
