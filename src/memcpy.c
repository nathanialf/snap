#include "common.h"

void *func_80137660(void *s1, const void *s2, u32 n) {
    u8 *su1 = (u8 *) s1;
    const u8 *su2 = (const u8 *) s2;
    while (n > 0) {
        *su1 = *su2;
        su1++;
        su2++;
        n--;
    }
    return (void *) s1;
}
