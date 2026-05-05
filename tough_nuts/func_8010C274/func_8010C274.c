#include "common.h"

typedef struct Node {
    s32 _0;
    struct Node *next;
} Node;

extern Node *D_8004A9E8[];
extern Node *D_8004AA68[];

extern void func_8000A52C(Node *);

void func_8010C274(void) {
    Node **bucket;
    Node *p;
    Node *nxt;

    bucket = D_8004A9E8;
    do {
        p = *bucket;
        while (p != 0) {
            nxt = p->next;
            func_8000A52C(p);
            p = nxt;
        }
        bucket++;
    } while (bucket != D_8004AA68);
}
