#include "common.h"

extern s32 D_80048C18[];
extern u8 D_80048C1C;
extern u8 D_80048918;

extern void func_80032630(u8 *queue, u8 *msg, s32 flag);

void func_80104D94(arg0, arg1, arg2)
s32 arg0;
s32 arg1;
s32 arg2;
{
    D_80048C18[4] = arg0;
    D_80048C18[5] = ((unsigned short *) &arg1)[1];
    D_80048C18[6] = ((unsigned short *) &arg2)[1];
    func_80032630(&D_80048918, &D_80048C1C, 0);
}
