#include "common.h"

typedef struct Node {
    u8 pad0[4];
    struct Node *prev;     /* 0x4 */
    struct Node *next;     /* 0x8 */
    u8 pad8[4];
    u32 key;               /* 0x10 */
} Node;

typedef struct Node2 {
    u8 pad0[0x20];
    struct Node2 *prev;    /* 0x20 */
    struct Node2 *next;    /* 0x24 */
} Node2;

extern Node *D_8004A9E8[];
extern Node *D_8004AA68[];
extern Node2 *D_8004AAF0[];
extern Node2 *D_8004AB78[];

extern void func_8000848C(Node *, Node *);

void func_80108560(Node *arg0) {
    Node *cur = D_8004A9E8[*(u8 *)((u8 *)arg0 + 0xC)];
    Node *anchor;

    if (cur != 0 && arg0->key < cur->key) {
        do {
            cur = cur->prev;
        } while (cur != 0 && arg0->key < cur->key);
    }

    if (cur != 0) {
        anchor = cur->next;
    } else {
        anchor = D_8004AA68[*(u8 *)((u8 *)arg0 + 0xC)];
    }

    func_8000848C(arg0, anchor);
}

void func_801085E8(Node *arg0) {
    if (arg0->next != 0) {
        arg0->next->prev = arg0->prev;
    } else {
        D_8004A9E8[*(u8 *)((u8 *)arg0 + 0xC)] = arg0->prev;
    }

    if (arg0->prev != 0) {
        arg0->prev->next = arg0->next;
    } else {
        D_8004AA68[*(u8 *)((u8 *)arg0 + 0xC)] = arg0->next;
    }
}

void func_80108650(Node2 *arg0, Node2 *arg1) {
    arg0->next = arg1;
    if (arg1 != 0) {
        arg0->prev = arg1->prev;
        arg1->prev = arg0;
    } else {
        arg0->prev = D_8004AAF0[*(u8 *)((u8 *)arg0 + 0xD)];
        D_8004AAF0[*(u8 *)((u8 *)arg0 + 0xD)] = arg0;
    }

    if (arg0->prev != 0) {
        arg0->prev->next = arg0;
    } else {
        D_8004AB78[*(u8 *)((u8 *)arg0 + 0xD)] = arg0;
    }
}
