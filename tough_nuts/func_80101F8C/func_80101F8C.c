#include "common.h"

typedef struct Item {
    struct Item *next;
    void *unk_4;
} Item;

extern Item *D_80048730;

extern void func_80032630(void *, s32, s32);
extern void func_80001D94(void);

void func_80101F8C(void) {
    Item *p = D_80048730;

    while (p != 0) {
        func_80032630(p->unk_4, 1, 0);
        p = p->next;
    }
    func_80001D94();
}
