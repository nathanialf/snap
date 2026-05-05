#include "common.h"

extern void func_80009984(void *);
extern void func_80009A50(void *);
extern void *func_8000C550(void *);

void func_8010C704(s32 *arg0) {
    s32 *outer = (s32 *) arg0[0x12];
    if (outer != 0) {
        do {
            s32 *inner;
            func_80009984(outer);
            inner = (s32 *) outer[0x20];
            if (inner != 0) {
                do {
                    func_80009A50(inner);
                    inner = (s32 *) inner[0];
                } while (inner != 0);
            }
            outer = (s32 *) func_8000C550(outer);
        } while (outer != 0);
    }
}

void func_8010C76C(s32 *arg0) {
    s32 *outer = (s32 *) arg0[0x12];
    if (outer != 0) {
        do {
            s32 *inner = (s32 *) outer[0x20];
            if (inner != 0) {
                do {
                    func_80009A50(inner);
                    inner = (s32 *) inner[0];
                } while (inner != 0);
            }
            outer = (s32 *) func_8000C550(outer);
        } while (outer != 0);
    }
}
