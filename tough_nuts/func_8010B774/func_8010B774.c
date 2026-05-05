#include "common.h"

typedef struct Node {
    struct Node *next;
    void *field_4;
    void *field_8;
} Node;

extern u8 *D_8004AC44;
extern Node *func_8000B6F0(void *);

s32 func_8010B774(arg0, arg1, arg2)
void *arg0;
void *arg1;
void *arg2;
{
    Node *n;
    if (arg0 == 0) arg0 = D_8004AC44;
    if (arg2 == 0) arg2 = D_8004AC44;
    n = func_8000B6F0(arg0);
    n->field_4 = arg2;
    n->field_8 = arg1;
    if (*(Node **) ((u8 *) arg0 + 0x3C) == 0) {
        *(Node **) ((u8 *) arg0 + 0x40) = n;
        *(Node **) ((u8 *) arg0 + 0x3C) = n;
    } else {
        (*(Node **) ((u8 *) arg0 + 0x40))->next = n;
        *(Node **) ((u8 *) arg0 + 0x40) = n;
    }
    n->next = 0;
    *(s32 *) ((u8 *) arg0 + 0x44) += 1;
    return 0;
}
