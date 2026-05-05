#include "common.h"

extern u8 *D_80096910;
extern s32 *D_80096914;

s32 func_80122674(s32 arg0) {
    if (D_80096910[arg0] != 0 && D_80096914[arg0] >= 0) {
        return 1;
    }
    return 0;
}
