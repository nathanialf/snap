#include "common.h"

typedef struct Node {
    u8 pad0[4];
    struct Node *prev;     /* 0x4 */
    struct Node *next;     /* 0x8 */
} Node;

extern Node *D_8004A9E8[];
extern Node *D_8004AA68[];

void func_8010848C(Node *arg0, Node *arg1) {
    arg0->next = arg1;
    if (arg1 != 0) {
        arg0->prev = arg1->prev;
        arg1->prev = arg0;
    } else {
        arg0->prev = D_8004A9E8[*(u8 *)((u8 *)arg0 + 0xC)];
        D_8004A9E8[*(u8 *)((u8 *)arg0 + 0xC)] = arg0;
    }

    if (arg0->prev != 0) {
        arg0->prev->next = arg0;
    } else {
        D_8004AA68[*(u8 *)((u8 *)arg0 + 0xC)] = arg0;
    }
}
