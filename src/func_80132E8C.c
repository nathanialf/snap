#include "common.h"

extern s32 func_80037860(void);
extern void func_8003CD80(s32);

void func_80132E8C(s32 arg0) {
    if (func_80037860() != 0) {
        do {
        } while (func_80037860() != 0);
    }
    func_8003CD80(0x125);
}
