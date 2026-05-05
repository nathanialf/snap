#include "common.h"

extern u8 D_8004B050[];
extern u8 *D_8004B03C;
extern u8 *D_8004B040;
extern u8 *D_8004B044;
extern u8 *D_8004B048;
extern u8 *D_8004B04C;

void func_80115448(void) {
    u8 *p = D_8004B050;
    D_8004B03C = p;
    D_8004B040 = p;
    *(&D_8004B040 + 1) = p;
    D_8004B048 = p;
    *(&D_8004B048 + 1) = p;
}
