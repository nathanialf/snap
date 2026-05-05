#include "common.h"

typedef struct Bucket {
    struct Bucket *next;
    void *unk_4;
    s32 key;
} Bucket;

extern Bucket *D_8004A9A4;
extern s32 D_8004A998;
extern u8 D_800432A0[];

extern void func_80025F24(u8 *, ...);

void func_80108024(s32 *arg0) {
    Bucket *p;
    s32 want;

    p = D_8004A9A4;
    if (p == 0) goto check;
    want = arg0[1];
    do {
        if (want == p->key) goto check;
        p = p->next;
    } while (p != 0);
check:
    if (p == 0) {
        func_80025F24(D_800432A0);
        while (1) {}
    }
    arg0[0] = (s32) p->unk_4;
    p->unk_4 = arg0;
    D_8004A998--;
}
