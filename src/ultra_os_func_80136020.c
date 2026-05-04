#include "common.h"

extern u32 func_80038D70(void);
extern void func_80038D90(u32);

typedef struct {
    s32 a;
    s32 b;
} Entry;

extern Entry D_80099420[];

void func_80136020(s32 arg0, s32 arg1, s32 arg2) {
    register u32 saveMask = func_80038D70();
    Entry *p = &D_80099420[arg0];
    p->a = arg1;
    p->b = arg2;
    func_80038D90(saveMask);
}
