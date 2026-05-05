#include "common.h"

typedef struct Node {
    char _0[8];
    struct Node *next;
    char _C[0x18];
    u16 flags;
    char _26[0x26];
} Node;

extern s32 D_8004A890;

extern void func_80027A00(u8 *);

void func_80117768(s32 *arg0) {
    Node *p = (Node *) arg0[0x12];
    if (p != 0) {
        do {
            if (!(p->flags & 4)) {
                *(s32 *) ((u8 *) p + 0x4C) = D_8004A890;
                func_80027A00((u8 *) p + 0x10);
                D_8004A890 = *(s32 *) ((u8 *) p + 0x4C) - 8;
            }
            p = p->next;
        } while (p != 0);
    }
}
