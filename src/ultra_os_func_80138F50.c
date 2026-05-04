#include "common.h"

extern void func_80035A30(void);
extern void func_80035A74(void);
extern s32 func_8003B320(s32, s32, s32);

s32 func_80138F50(s32 arg0, s32 arg1, s32 arg2) {
    register s32 result;
    func_80035A30();
    result = func_8003B320(arg0, arg1, arg2);
    func_80035A74();
    return result;
}
