#include "common.h"

typedef struct Node {
    u8 _0[0x4];
    s32 _4;
    u8 _8[0x4];
    struct Node *_C;
    u8 _10[0x2];
    u16 _12;
} Node;

extern Node *func_800385B0(void);

Node *func_801258AC(void) {
    Node *node = func_800385B0();
    while (node->_4 != -1) {
        if (node->_4 > 0 && node->_4 < 0x7F && (node->_12 & 3)) {
            return node;
        }
        node = node->_C;
    }
    return 0;
}
