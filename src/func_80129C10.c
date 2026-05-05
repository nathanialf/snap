#include "common.h"

extern s32 *D_800428F0;
extern void func_8002A6A0(s32);
extern void func_800325A0(void);

typedef struct Node {
    struct Node *next;
    struct Node *prev;
} Node;

void func_80129C10(s32 arg0) {
    if (D_800428F0 == 0) {
        D_800428F0 = (s32 *) arg0;
        func_8002A6A0(arg0);
    }
}

void func_80129C44(void) {
    if (D_800428F0 != 0) {
        func_800325A0();
        D_800428F0 = 0;
    }
}

void func_80129C7C(Node *new_node, Node *after_node) {
    new_node->next = after_node->next;
    new_node->prev = after_node;
    if (after_node->next != 0) {
        after_node->next->prev = new_node;
    }
    after_node->next = new_node;
}

void func_80129CA0(Node *node) {
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    if (node->prev != 0) {
        node->prev->next = node->next;
    }
}
