#include "common.h"

typedef struct Node {
    s32 _0;
    s32 _4;
    struct Node *_8;
    struct Node *_C;
    struct Node *_10;
    struct Node *_14;
    s32 _18[14];
    s32 _50;
} Node;

extern Node *func_80008A30(Node *);
extern void func_80009D0C(Node *);

Node *func_80109E94(Node *arg0, s32 arg1) {
    Node *new;
    Node *cur;

    new = func_80008A30(arg0);
    cur = arg0->_10;
    if (cur != 0) {
        Node *next;
        next = cur->_8;
        if (next != 0) {
            cur = next;
            do {
                next = cur->_8;
                if (next == 0) break;
                cur = next;
            } while (1);
        }
        cur->_8 = new;
        new->_C = cur;
    } else {
        arg0->_10 = new;
        new->_C = 0;
    }
    new->_14 = arg0;
    new->_10 = 0;
    new->_8 = 0;
    new->_4 = arg0->_4;
    new->_50 = arg1;
    func_80009D0C(new);
    return new;
}
