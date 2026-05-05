#include "common.h"

typedef struct Node {
    struct Node *next;
} Node;

extern void func_8000888C(void *);
extern void func_80008960(Node *);
extern void func_80008BFC(void *);

void func_8010A2B0(s32 *arg0) {
    s32 *p;
    s32 i;
    s32 *cur;
    Node *node;
    Node *next;

    *(u8 *) ((u8 *) arg0[1] + 0xF) = 0;
    *(s32 *) ((u8 *) arg0[1] + 0x48) = 0;

    cur = arg0;
    for (i = 0; i != 8; i += 4) {
        if (*(void **) ((u8 *) cur + 0x64) != 0) {
            func_8000888C(*(void **) ((u8 *) cur + 0x64));
        }
        cur = (s32 *) ((u8 *) cur + 4);
    }

    node = *(Node **) ((u8 *) arg0 + 0x6C);
    while (node != 0) {
        next = node->next;
        func_80008960(node);
        node = next;
    }

    func_80008BFC(arg0);
}
