#include "common.h"

typedef struct Node {
    struct Node *next;
} Node;

Node *func_801311F8(arg0, arg1, arg2, arg3)
s32 *arg0;
unsigned char arg1, arg2, arg3;
{
    Node *n = (Node *) arg0[28];
    if (n != 0) {
        arg0[28] = (s32) n->next;
        n->next = 0;
        if (arg0[26] == 0) {
            arg0[26] = (s32) n;
        } else {
            ((Node *) arg0[27])->next = n;
        }
        arg0[27] = (s32) n;
        *((u8 *) n + 0x31) = arg3;
        *((u8 *) n + 0x32) = arg1;
        *((u8 *) n + 0x33) = arg2;
        *(Node **) ((u8 *) n + 0x14) = n;
    }
    return n;
}
