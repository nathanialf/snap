#include "common.h"

extern s32 func_80037860(void);
extern u32 func_80034D30(void *);

#define __osSpDeviceBusy    func_80037860
#define osVirtualToPhysical func_80034D30

#define SP_MEM_ADDR_REG     0x04040000
#define SP_DRAM_ADDR_REG    0x04040004
#define SP_RD_LEN_REG       0x04040008
#define SP_WR_LEN_REG       0x0404000C

#define IO_WRITE(addr, data) (*(vu32 *) ((u32) (addr) | 0xA0000000) = (u32) (data))

#define OS_READ 0

s32 func_801354D0(s32 direction, u32 devAddr, void *dramAddr, u32 size) {
    if (__osSpDeviceBusy()) {
        return -1;
    }

    IO_WRITE(SP_MEM_ADDR_REG, devAddr);
    IO_WRITE(SP_DRAM_ADDR_REG, osVirtualToPhysical(dramAddr));

    if (direction == OS_READ) {
        IO_WRITE(SP_WR_LEN_REG, size - 1);
    } else {
        IO_WRITE(SP_RD_LEN_REG, size - 1);
    }

    return 0;
}
