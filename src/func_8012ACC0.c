#include "common.h"

typedef struct Node {
    struct Node *next;
    s32 pad[3];
    s32 _10;
} Node;

typedef struct Foo {
    Node *head;
    s32 pad[7];
    s32 _20;
} Foo;

s32 func_8012ACC0(Foo *arg0, Node **arg1) {
    Node *cur;
    s32 best_diff;

    *arg1 = 0;
    best_diff = 0x7FFFFFFF;

    for (cur = arg0->head; cur != 0; cur = cur->next) {
        if (cur->_10 - arg0->_20 < best_diff) {
            *arg1 = cur;
            best_diff = cur->_10 - arg0->_20;
        }
    }
    return (*arg1)->_10;
}
