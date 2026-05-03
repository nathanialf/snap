#include "common.h"

extern s32 D_8004A890;
extern s32 D_8004A894;
extern s32 D_8004A898;
extern s32 D_8004A89C;
extern s32 func_80014C28(s32 arg0, s32 *arg1);

void func_80114D60(s32 arg0) {
    func_80014C28(arg0, &D_8004A890);
}

void func_80114D84(s32 arg0) {
    func_80014C28(arg0, &D_8004A894);
}

void func_80114DA8(s32 arg0) {
    func_80014C28(arg0, &D_8004A898);
}

void func_80114DCC(s32 arg0) {
    func_80014C28(arg0, &D_8004A89C);
}
