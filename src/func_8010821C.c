#include "common.h"

typedef struct Node {
    u8 pad0[8];
    struct Node *prev;
    struct Node *next;
    s32 bucket_idx;
} Node;

typedef struct Item {
    struct Item *_0;
    struct Item *_4;
    u8 pad8[0x10];
    struct Item *_18;
} Item;

extern Node *D_8004A9B0[];
extern void func_8000821C(Item *);

void func_8010821C(Node *arg0) {
    if (arg0->next != 0) {
        arg0->next->prev = arg0->prev;
    } else {
        D_8004A9B0[arg0->bucket_idx] = arg0->prev;
    }
    if (arg0->prev != 0) {
        arg0->prev->next = arg0->next;
    }
}

void func_80108268(Item *arg0) {
    Item **p = (Item **) ((u8 *) arg0->_18);
    func_8000821C(arg0);
    if (arg0->_4 != 0) {
        arg0->_4->_0 = arg0->_0;
    } else {
        p[6] = arg0->_0;
    }
    if (arg0->_0 != 0) {
        arg0->_0->_4 = arg0->_4;
    } else {
        p[7] = arg0->_4;
    }
}
