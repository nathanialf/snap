#include "common.h"

typedef struct Node {
    u8 pad0[8];
    struct Node *next;     /* 0x8 */
    u8 pad8[4];
    u32 key;               /* 0x10 */
} Node;

extern Node *D_8004AA68[];
extern void func_8000848C(Node *, Node *);

void func_801084F4(Node *arg0) {
    Node *cur = D_8004AA68[*(u8 *)((u8 *)arg0 + 0xC)];
    while (cur != 0 && cur->key < arg0->key) {
        cur = cur->next;
    }
    func_8000848C(arg0, cur);
}
