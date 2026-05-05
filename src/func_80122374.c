#include "common.h"

typedef struct Header {
    u8 _0[0x2];
    s16 _2;
} Header;

extern s32 func_80032980(s32);
extern Header *D_800968EC;
extern u8 *D_80096910;
extern s32 *D_80096914;

s32 func_80122374(s32 arg0, s32 arg1) {
    s32 prev;
    if ((u32)arg1 < (u32)D_800968EC->_2) {
        prev = func_80032980(1);
        D_80096910[arg0] = 1;
        D_80096914[arg0] = arg1;
        func_80032980(prev);
        return arg1;
    }
    return -1;
}
