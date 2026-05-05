#include "common.h"

typedef struct Node {
    struct Node *next;
} Node;

Node *func_8013125C(s32 *arg0, u8 *arg1) {
    Node *prev = 0;
    Node *target = (Node *) (arg1 - 4);
    Node *cur = (Node *) arg0[26];

    if (cur != 0) {
        do {
            if (cur == target) {
                if (prev != 0) {
                    prev->next = cur->next;
                } else {
                    arg0[26] = (s32) cur->next;
                }
                if (cur == (Node *) arg0[27]) {
                    arg0[27] = (s32) prev;
                }
                cur->next = (Node *) arg0[28];
                arg0[28] = (s32) cur;
                return prev;
            }
            prev = cur;
            cur = cur->next;
        } while (cur != 0);
    }
    return prev;
}
