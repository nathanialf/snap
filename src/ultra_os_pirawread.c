#include "common.h"

#define PI_STATUS_REG       0x04600010
#define PI_STATUS_DMA_BUSY  (1 << 0)
#define PI_STATUS_IO_BUSY   (1 << 1)
#define IO_READ(addr)       (*(vu32 *) ((u32) (addr) | 0xA0000000))

extern void *D_80000308;
#define osRomBase D_80000308

s32 func_80135190(u32 devAddr, u32 *data) {
    register u32 stat;

    stat = IO_READ(PI_STATUS_REG);
    while (stat & (PI_STATUS_IO_BUSY | PI_STATUS_DMA_BUSY)) {
        stat = IO_READ(PI_STATUS_REG);
    }
    *data = IO_READ((u32) osRomBase | devAddr);

    return 0;
}
