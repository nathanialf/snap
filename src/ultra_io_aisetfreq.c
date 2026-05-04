#include "common.h"

extern s32 D_80042CF8;
#define osViClock D_80042CF8

#define AI_DACRATE_REG 0x04500010
#define AI_BITRATE_REG 0x04500014
#define IO_WRITE(addr, data) (*(vu32 *) (((u32) (addr)) | 0xA0000000) = (u32) (data))

s32 func_801383C0(u32 frequency) {
    register unsigned int dacRate;
    register unsigned char bitRate;
    register float f;

    f = osViClock / (float) frequency + .5f;
    dacRate = f;

    if (dacRate < 132) {
        return -1;
    }

    bitRate = dacRate / 66;
    if (bitRate > 16) {
        bitRate = 16;
    }

    IO_WRITE(AI_DACRATE_REG, dacRate - 1);
    IO_WRITE(AI_BITRATE_REG, bitRate - 1);

    return osViClock / (s32) dacRate;
}
