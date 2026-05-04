#include "common.h"

extern void func_80009984(s32 *);
extern void func_80009A50(s32 *);
extern s32 *func_8000C550(s32 *);

void func_8010C704(s32 *arg0) {
    s32 *p = (s32 *) arg0[0x48 / 4];
    if (p != 0) {
        do {
            s32 *q;
            func_80009984(p);
            q = (s32 *) p[0x80 / 4];
            if (q != 0) {
                do {
                    func_80009A50(q);
                    q = (s32 *) q[0];
                } while (q != 0);
            }
            p = func_8000C550(p);
        } while (p != 0);
    }
}

void func_8010C76C(s32 *arg0) {
    s32 *p = (s32 *) arg0[0x48 / 4];
    if (p != 0) {
        do {
            s32 *q;
            q = (s32 *) p[0x80 / 4];
            if (q != 0) {
                do {
                    func_80009A50(q);
                    q = (s32 *) q[0];
                } while (q != 0);
            }
            p = func_8000C550(p);
        } while (p != 0);
    }
}
