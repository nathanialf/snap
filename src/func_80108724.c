#include "common.h"

typedef struct Node2 {
    u8 pad0[0x20];
    struct Node2 *prev;    /* 0x20 */
    struct Node2 *next;    /* 0x24 */
    u32 key;               /* 0x28 */
} Node2;

extern Node2 *D_8004AAF0[];
extern Node2 *D_8004AB78[];

extern void func_80008650(Node2 *, Node2 *);

void func_80108724(Node2 *arg0) {
    Node2 *cur = D_8004AAF0[*(u8 *)((u8 *)arg0 + 0xD)];
    Node2 *anchor;

    if (cur != 0 && arg0->key < cur->key) {
        do {
            cur = cur->prev;
        } while (cur != 0 && arg0->key < cur->key);
    }

    if (cur != 0) {
        anchor = cur->next;
    } else {
        anchor = D_8004AB78[*(u8 *)((u8 *)arg0 + 0xD)];
    }

    func_80008650(arg0, anchor);
}

void func_801087AC(Node2 *arg0) {
    if (arg0->next != 0) {
        arg0->next->prev = arg0->prev;
    } else {
        D_8004AAF0[*(u8 *)((u8 *)arg0 + 0xD)] = arg0->prev;
    }

    if (arg0->prev != 0) {
        arg0->prev->next = arg0->next;
    } else {
        D_8004AB78[*(u8 *)((u8 *)arg0 + 0xD)] = arg0->next;
    }
}
