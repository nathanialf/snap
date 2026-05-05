#include "common.h"

typedef struct Slot {
    s32 unk_0;
    struct Slot *next;
} Slot;

extern s16 D_8004AC02;
extern s32 D_8004ABFC;
extern u16 D_8004AC00;
extern Slot *D_8004AAE8;
extern u8 D_800432E4[];
extern u8 D_80043300[];

extern void *func_800054A0(s32, s32);
extern void func_80025F24(u8 *, ...);

Slot *func_80108394(void) {
    Slot *e;

    if (D_8004AC02 == -1) {
        goto skip_check;
    }
    if (D_8004AC02 <= D_8004ABFC) {
        goto err_budget;
    }
skip_check:
    e = D_8004AAE8;
    if (e == 0) {
        e = func_800054A0(D_8004AC00, 8);
        D_8004AAE8 = e;
    }
    if (e == 0) {
        goto err_empty;
    }
    if (e != 0) {
        D_8004AAE8 = e->next;
        D_8004ABFC++;
        return e;
    }
    return 0;

err_budget:
    func_80025F24(D_800432E4);
    while (1) {}

err_empty:
    func_80025F24(D_80043300);
    while (1) {}
}
