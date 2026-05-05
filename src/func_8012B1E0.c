#include "common.h"

extern s32 D_8002C370;
extern void func_8002C878();
extern void func_8002DE50(s32 *, s32 *, void *, s32);
extern s32 *func_8001FD74(s32, s32, s32, s32, s32);

void func_8012B1E0(s32 *arg0, s32 arg1) {
    func_8002DE50(arg0, &D_8002C370, (void *) func_8002C878, 4);
    arg0[5] = (s32) func_8001FD74(0, 0, arg1, 1, 0x50);
    arg0[14] = 1;
    arg0[18] = 0;
    *(s16 *) ((u8 *) arg0 + 0x1A) = 1;
    *(s16 *) ((u8 *) arg0 + 0x28) = 1;
    *(s16 *) ((u8 *) arg0 + 0x2E) = 1;
    *(s16 *) ((u8 *) arg0 + 0x1C) = 1;
    *(s16 *) ((u8 *) arg0 + 0x1E) = 1;
    *(s16 *) ((u8 *) arg0 + 0x20) = 0;
    *(s16 *) ((u8 *) arg0 + 0x22) = 0;
    *(s16 *) ((u8 *) arg0 + 0x26) = 1;
    *(s16 *) ((u8 *) arg0 + 0x24) = 0;
    arg0[12] = 0;
    arg0[13] = 0;
    *(s16 *) ((u8 *) arg0 + 0x18) = 0;
    arg0[15] = 0;
    arg0[16] = 0;
    arg0[17] = 0;
}
