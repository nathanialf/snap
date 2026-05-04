#include "common.h"

extern u32 func_800357B0(void *);

#define __osProbeTLB func_800357B0

#define K0BASE 0x80000000
#define K1BASE 0xA0000000
#define K2BASE 0xC0000000
#define IS_KSEG0(x) ((u32) (x) >= K0BASE && (u32) (x) < K1BASE)
#define IS_KSEG1(x) ((u32) (x) >= K1BASE && (u32) (x) < K2BASE)
#define K0_TO_PHYS(x) ((u32) (x) & 0x1FFFFFFF)
#define K1_TO_PHYS(x) ((u32) (x) & 0x1FFFFFFF)

u32 func_80134D30(void *addr) {
    if (IS_KSEG0(addr)) {
        return K0_TO_PHYS(addr);
    } else if (IS_KSEG1(addr)) {
        return K1_TO_PHYS(addr);
    } else {
        return __osProbeTLB(addr);
    }
}
