#include "common.h"

extern u8 *D_80048734;
extern u8 *D_80048738;
extern u8 *D_80048744;
extern u8 *D_80048748;
extern u8 *D_8004874C;
extern u8 *D_80048750;

void func_80100C74(u8 *arg0) {
    u8 *cur;
    s32 key;

    cur = D_80048738;
    if (cur != 0) {
        key = *(s32 *) (arg0 + 4);
        if (*(s32 *) (cur + 4) < key) {
            do {
                cur = *(u8 **) (cur + 0x10);
            } while (cur != 0 && *(s32 *) (cur + 4) < key);
        }
    }

    *(u8 **) (arg0 + 0x10) = cur;
    if (cur != 0) {
        *(u8 **) (arg0 + 0xC) = *(u8 **) (cur + 0xC);
        *(u8 **) (cur + 0xC) = arg0;
    } else {
        *(u8 **) (arg0 + 0xC) = D_80048734;
        D_80048734 = arg0;
    }

    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = arg0;
        } else {
            D_80048738 = arg0;
        }
    }
}

void func_80100D04(u8 *arg0) {
    u8 *next = *(u8 **) (arg0 + 0x10);
    if (next != 0) {
        *(u8 **) (next + 0xC) = *(u8 **) (arg0 + 0xC);
    } else {
        D_80048734 = *(u8 **) (arg0 + 0xC);
    }
    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = *(u8 **) (arg0 + 0x10);
        } else {
            D_80048738 = *(u8 **) (arg0 + 0x10);
        }
    }
}

void func_80100D54(u8 *arg0) {
    u8 *cur;
    s32 key;

    cur = D_80048748;
    if (cur != 0) {
        key = *(s32 *) (arg0 + 4);
        if (*(s32 *) (cur + 4) < key) {
            do {
                cur = *(u8 **) (cur + 0x10);
            } while (cur != 0 && *(s32 *) (cur + 4) < key);
        }
    }

    *(u8 **) (arg0 + 0x10) = cur;
    if (cur != 0) {
        *(u8 **) (arg0 + 0xC) = *(u8 **) (cur + 0xC);
        *(u8 **) (cur + 0xC) = arg0;
    } else {
        *(u8 **) (arg0 + 0xC) = D_80048744;
        D_80048744 = arg0;
    }

    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = arg0;
        } else {
            D_80048748 = arg0;
        }
    }
}

void func_80100DE4(u8 *arg0) {
    u8 *next = *(u8 **) (arg0 + 0x10);
    if (next != 0) {
        *(u8 **) (next + 0xC) = *(u8 **) (arg0 + 0xC);
    } else {
        D_80048744 = *(u8 **) (arg0 + 0xC);
    }
    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = *(u8 **) (arg0 + 0x10);
        } else {
            D_80048748 = *(u8 **) (arg0 + 0x10);
        }
    }
}

void func_80100E34(u8 *arg0) {
    *(u8 **) (arg0 + 0xC) = 0;
    *(u8 **) (arg0 + 0x10) = D_80048750;
    if (D_80048750 != 0) {
        *(u8 **) (D_80048750 + 0xC) = arg0;
    } else {
        D_8004874C = arg0;
    }
    D_80048750 = arg0;
}

void func_80100E6C(u8 *arg0) {
    u8 *next = *(u8 **) (arg0 + 0x10);
    if (next != 0) {
        *(u8 **) (next + 0xC) = *(u8 **) (arg0 + 0xC);
    } else {
        D_8004874C = *(u8 **) (arg0 + 0xC);
    }
    {
        u8 *prev = *(u8 **) (arg0 + 0xC);
        if (prev != 0) {
            *(u8 **) (prev + 0x10) = *(u8 **) (arg0 + 0x10);
        } else {
            D_80048750 = *(u8 **) (arg0 + 0x10);
        }
    }
}
