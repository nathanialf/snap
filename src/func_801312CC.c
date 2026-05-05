#include "common.h"

typedef struct Node {
    struct Node *next;
} Node;

Node *func_801312CC(arg0, arg1, arg2)
s32 *arg0;
unsigned char arg1, arg2;
{
    Node *cur = (Node *) arg0[26];
    if (cur != 0) {
        do {
            u8 *p = (u8 *) cur;
            if (arg1 == p[0x32] && arg2 == p[0x31] &&
                p[0x35] != 3 && p[0x35] != 4) {
                return cur;
            }
            cur = cur->next;
        } while (cur != 0);
    }
    return 0;
}
