#include "common.h"

typedef struct ListNode {
    struct ListNode *next;
    void *unk_4;
    void *unk_8;
} ListNode;

typedef struct List {
    u8 padding[0x3C];
    ListNode *head;
    ListNode *tail;
    s32 count;
} List;

extern List *D_8004AC44;

extern ListNode *func_8000B6F0(void);

s32 func_8010B774(List *arg0, void *arg1, void *arg2) {
    ListNode *n;

    if (arg0 == 0) {
        arg0 = D_8004AC44;
    }
    if (arg2 == 0) {
        arg2 = D_8004AC44;
    }
    n = func_8000B6F0();
    n->unk_8 = arg1;
    n->unk_4 = arg2;
    if (arg0->head == 0) {
        arg0->tail = n;
        arg0->head = n;
    } else {
        arg0->tail->next = n;
        arg0->tail = n;
    }
    n->next = 0;
    arg0->count++;
    return 0;
}
