#include "common.h"

extern f32 func_80032A20(void);
extern f32 func_80038DE0(void);
extern f32 func_80019960(void);

f32 func_80119CD0(void) {
    return 1.0f / func_80032A20();
}

f32 func_80119CF8(void) {
    return 1.0f / func_80038DE0();
}

f32 func_80119D20(void) {
    return 1.0f / func_80019960();
}
