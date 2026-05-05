#include "common.h"

extern u8 D_8004B050[];
extern u8 *D_8004B03C;
extern u8 *D_8004B040;
extern u8 *D_8004B044;
extern u8 *D_8004B048;
extern u8 *D_8004B04C;

void func_80115448(void) {
    D_8004B03C = D_8004B050;
    D_8004B040 = D_8004B050;
    D_8004B044 = D_8004B050;
    D_8004B048 = D_8004B050;
    D_8004B04C = D_8004B050;
}
