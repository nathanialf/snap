#include "common.h"

typedef struct Node {
    u8 pad0[4];
    void *field_4;
    struct Node *prev;
    struct Node *next;
    struct Node *field_10;
    void *field_14;
} Node;

extern Node *func_80008A30(Node *);
extern void func_80009D0C(Node *);

Node *func_80109E94(Node *arg0, void *arg1) {
    Node *n = func_80008A30(arg0);
    Node *cur = arg0->field_10;
    if (cur != 0) {
        while (cur->prev != 0) {
            cur = cur->prev;
        }
        cur->prev = n;
        n->next = cur;
    } else {
        arg0->field_10 = n;
        n->next = 0;
    }
    n->field_14 = arg0;
    n->field_10 = 0;
    n->prev = 0;
    n->field_4 = arg0->field_4;
    *(s32 *) ((u8 *) n + 0x50) = (s32) arg1;
    func_80009D0C(n);
    return n;
}
