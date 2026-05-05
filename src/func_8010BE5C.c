#include "common.h"

extern u8 *D_8004AC44;
extern void func_80008F2C(void *);

typedef struct Node {
    struct Node *next;
} Node;

void func_8010BE5C(u8 *arg0) {
    Node *cur, *next;
    if (arg0 == 0) arg0 = D_8004AC44;
    cur = *(Node **) (arg0 + 0x18);
    if (cur != 0) {
        do {
            next = cur->next;
            func_80008F2C(cur);
            cur = next;
        } while (cur != 0);
    }
}
