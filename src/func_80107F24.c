#include "common.h"

typedef struct Bucket {
    struct Bucket *next;
    struct Entry *chain;
    s32 size;
} Bucket;

typedef struct Entry {
    struct Entry *next;
    s32 size;
} Entry;

extern Bucket *D_8004A9A4;
extern s32 D_8004A998;

extern void *func_800054A0(s32, s32);

Entry *func_80107F24(s32 arg0) {
    Bucket *p;
    Bucket *prev;
    Entry *entry;

    p = D_8004A9A4;
    prev = 0;
    if (p != 0) {
        do {
            if (arg0 == p->size) goto found;
            prev = p;
            p = p->next;
        } while (p != 0);
    }
found:
    if (p == 0) {
        p = func_800054A0(0xC, 4);
        p->next = 0;
        p->chain = 0;
        p->size = arg0;
        if (prev != 0) {
            prev->next = p;
        } else {
            D_8004A9A4 = p;
        }
    }
    {
        Entry *cur = p->chain;
        if (cur != 0) {
            p->chain = cur->next;
            entry = cur;
        } else {
            entry = func_800054A0(arg0 + 8, 8);
            entry->size = arg0;
        }
    }
    entry->next = 0;
    D_8004A998++;
    return entry;
}
