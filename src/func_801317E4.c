#include "common.h"

typedef struct {
    void *ptr;
    s16 hword;
    u8 pad6;
    u8 byte_7;
    u8 byte_8;
    u8 byte_9;
    u8 padA[10];
} Entry;

void func_801317E4(s32 *arg0, u8 *arg1, s32 index) {
    ((Entry *) arg0[25])[index].ptr = arg1;
    ((Entry *) arg0[25])[index].byte_7 = arg1[1];
    ((Entry *) arg0[25])[index].byte_9 = arg1[0];
    ((Entry *) arg0[25])[index].byte_8 = arg1[2];
    ((Entry *) arg0[25])[index].hword = *(s16 *) (arg1 + 0xC);
}
