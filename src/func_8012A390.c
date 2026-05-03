#include "common.h"

s32 func_8012A390(s32 *arg0) {
    s32 *p = (s32 *) arg0[6];
    if (p == 0) {
        return 0;
    }
    return (s32) ((f32) arg0[9] / *(f32 *) (p + 2));
}
