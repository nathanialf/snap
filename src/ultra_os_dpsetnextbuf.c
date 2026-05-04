#include "common.h"

extern s32 func_8003CD50(void);
extern u32 func_80034D30(void *);

#define __osDpDeviceBusy   func_8003CD50
#define osVirtualToPhysical func_80034D30

#define DPC_STATUS_REG          0x0410000C
#define DPC_START_REG           0x04100000
#define DPC_END_REG             0x04100004
#define DPC_CLR_XBUS_DMEM_DMA   (1 << 0)
#define DPC_STATUS_XBUS_DMEM_DMA (1 << 0)

#define IO_WRITE(addr, data) (*(vu32 *) ((u32) (addr) | 0xA0000000) = (u32) (data))
#define IO_READ(addr)        (*(vu32 *) ((u32) (addr) | 0xA0000000))

#define TRUE 1

s32 func_8013CCA0(void *bufPtr, u64 size) {
    register u32 stat;

    if (__osDpDeviceBusy()) {
        return -1;
    }

    IO_WRITE(DPC_STATUS_REG, DPC_CLR_XBUS_DMEM_DMA);

    while (TRUE) {
        stat = IO_READ(DPC_STATUS_REG);
        if ((stat & DPC_STATUS_XBUS_DMEM_DMA) == 0) {
            break;
        }
    }

    IO_WRITE(DPC_START_REG, osVirtualToPhysical(bufPtr));
    IO_WRITE(DPC_END_REG, osVirtualToPhysical(bufPtr) + size);
    return 0;
}
