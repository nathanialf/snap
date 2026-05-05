#include "common.h"

typedef struct Item {
    u8 _0[4];
    struct Container *_4;
    u8 _8[0x58];
    void *_60;
    void *_64[2];
    void *_6C;
} Item;

typedef struct Container {
    u8 _0[0xF];
    s8 _F;
    u8 _10[0x38];
    Item *_48;
} Container;

extern void func_8000888C(void *);
extern void func_80008960(void *);
extern void func_80008BFC(Item *);

void func_8010A2B0(Item *arg0) {
    Container *parent;
    s32 i;
    void **p;
    void *cur;
    void *next;

    parent = arg0->_4;
    parent->_F = 0;
    parent->_48 = 0;

    for (i = 0; i < 8; i += 4) {
        void *p2 = *(void **) ((u8 *) arg0 + 0x64 + i);
        if (p2 != 0) {
            func_8000888C(p2);
        }
    }

    cur = arg0->_6C;
    while (cur != 0) {
        next = *(void **) cur;
        func_80008960(cur);
        cur = next;
    }

    func_80008BFC(arg0);
}
