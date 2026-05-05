#include "common.h"

typedef struct Item {
    s32 _0;
    struct Container *_4;
    struct Item *_8;
    struct Item *_C;
} Item;

typedef struct Container {
    u8 _0[0xF];
    s8 _F;
    u8 _10[0x38];
    Item * volatile _48;
} Container;

extern void func_80008B54(void);

void func_8010A18C(Item *arg0) {
    if (arg0 == arg0->_4->_48) {
        arg0->_4->_48 = arg0->_8;
        if (arg0->_4->_48 == 0) {
            arg0->_4->_F = 0;
        }
    }
    if (arg0->_C != 0) {
        arg0->_C->_8 = arg0->_8;
    }
    if (arg0->_8 != 0) {
        arg0->_8->_C = arg0->_C;
    }
    func_80008B54();
}
