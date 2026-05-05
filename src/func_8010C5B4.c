#include "common.h"

extern void *func_8000C550(void *);

void func_8010C5B4(s32 *arg0, f32 arg1) {
    s32 *cur = (s32 *) arg0[0x12];
    if (cur != 0) {
        do {
            *(f32 *) ((u8 *) cur + 0x78) = arg1;
            cur = (s32 *) func_8000C550(cur);
        } while (cur != 0);
    }
}

void func_8010C5FC(s32 *arg0, f32 arg1) {
    s32 *cur = (s32 *) arg0[0x12];
    if (cur != 0) {
        do {
            s32 *inner;
            *(f32 *) ((u8 *) cur + 0x78) = arg1;
            inner = (s32 *) cur[0x20];
            if (inner != 0) {
                do {
                    *(f32 *) ((u8 *) inner + 0x9C) = arg1;
                } while ((inner = (s32 *) inner[0]) != 0);
            }
            cur = (s32 *) func_8000C550(cur);
        } while (cur != 0);
    }
}

void func_8010C660(s32 *arg0, f32 arg1) {
    s32 *cur = (s32 *) arg0[0x12];
    if (cur != 0) {
        do {
            s32 *inner = (s32 *) cur[0x20];
            if (inner != 0) {
                do {
                    *(f32 *) ((u8 *) inner + 0x9C) = arg1;
                } while ((inner = (s32 *) inner[0]) != 0);
            }
            cur = (s32 *) func_8000C550(cur);
        } while (cur != 0);
    }
}
