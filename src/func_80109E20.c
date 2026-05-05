#include "common.h"

typedef struct Node {
    u8 pad0[4];
    void *field_4;
    struct Node *prev;
    struct Node *next;
    void *field_10;
    void *field_14;
} Node;

extern Node *func_80008A30(Node *);
extern void func_80009D0C(Node *);

Node *func_80109E20(Node *arg0, void *arg1) {
    Node *n = func_80008A30(arg0);
    if (arg0->prev != 0) {
        arg0->prev->next = n;
    }
    n->next = arg0;
    n->prev = arg0->prev;
    arg0->prev = n;
    n->field_4 = arg0->field_4;
    n->field_14 = arg0->field_14;
    n->field_10 = 0;
    *(s32 *) ((u8 *) n + 0x50) = (s32) arg1;
    func_80009D0C(n);
    return n;
}
