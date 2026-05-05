#include "common.h"

typedef struct {
    void *ptr;
    s16 hword;
    u8 pad6, byte_7, byte_8, byte_9;
    s8 byte_A;
    u8 padB[9];
} Entry;

typedef struct {
    u8 pad0[0x32];
    s16 hword_32;
    s16 hword_34;
    u8 pad1[0x2C];
    Entry *entries;
} Ctx;

s32 func_8013144C(u8 *arg0, Ctx *arg1) {
    s32 v1, a2;
    Entry *e;

    v1 = (arg0[0x36] * arg0[0x33] * arg0[0x30]) >> 6;
    e = &arg1->entries[arg0[0x31]];
    a2 = (e->byte_9 * arg1->hword_32 * arg1->hword_34) >> 14;
    if (e->byte_A >= 0) {
        a2 = (u32) (e->byte_A * a2) >> 7;
    }
    v1 = (u32) (v1 * a2) >> 15;
    return (s16) v1;
}
