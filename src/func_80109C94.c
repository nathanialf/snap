#include "common.h"

extern void func_80008960(s32 *);
extern void func_80008A04(s32 *);

void func_80109C94(s32 *arg0) {
    s32 *p = (s32 *) arg0[0x80 / 4];

    if (p != 0) {
        do {
            s32 *q = (s32 *) p[0x90 / 4];
            if (q != 0) {
                do {
                    s32 *next = (s32 *) q[0];
                    func_80008960(q);
                    q = next;
                } while (q != 0);
            }
            {
                s32 *next_p = (s32 *) p[0];
                func_80008A04(p);
                p = next_p;
            }
        } while (p != 0);
    }
    arg0[0x80 / 4] = 0;
}
