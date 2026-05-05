#include "common.h"

typedef struct Item {
    s32 state;
    s32 _4;
    s32 _8;
    struct Item *next;
} Item;

extern Item *D_8004873C;
extern Item *D_80048744;
extern Item *D_80048734;
extern Item *D_80048754;
extern Item *D_8004874C;

s32 func_80100B84(arg0)
s32 arg0;
{
    Item *p;

    if (D_8004873C != 0 && D_8004873C->state == 1) {
        return 0;
    }
    for (p = D_80048744; p != 0; p = p->next) {
        if (p->state == 1) {
            return 0;
        }
    }
    for (p = D_80048734; p != 0; p = p->next) {
        if (p->state == 1) {
            return 0;
        }
    }
    if (D_80048754 != 0 && D_80048754->state == 1) {
        return 0;
    }
    for (p = D_8004874C; p != 0; p = p->next) {
        if (p->state == 1) {
            return 0;
        }
    }
    return 1;
}
