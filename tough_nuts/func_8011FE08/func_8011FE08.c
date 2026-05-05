#include "common.h"

typedef struct Header {
    u8 _0[0x2];
    s16 _2;
} Header;

extern void func_8001FE88(void *, Header *, s32);

void func_8011FE08(Header *arg0, s32 arg1) {
    if (arg0->_2 > 0) {
        s32 i = 0;
        s32 *p = (s32 *)arg0;
        do {
            void *r = (void *)(p[1] + (s32)arg0);
            p[1] = (s32)r;
            if (r != 0) {
                func_8001FE88(r, arg0, arg1);
            }
            i++;
            p++;
        } while (i < arg0->_2);
    }
}
