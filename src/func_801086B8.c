#include "common.h"

typedef struct Node2 {
    u8 pad0[0x24];
    struct Node2 *next;    /* 0x24 */
    u32 key;               /* 0x28 */
} Node2;

extern Node2 *D_8004AB78[];
extern void func_80008650(Node2 *, Node2 *);

void func_801086B8(Node2 *arg0) {
    Node2 *cur = D_8004AB78[*(u8 *)((u8 *)arg0 + 0xD)];
    while (cur != 0 && cur->key < arg0->key) {
        cur = cur->next;
    }
    func_80008650(arg0, cur);
}
