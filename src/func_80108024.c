#include "common.h"

typedef struct Node8024 {
    struct Node8024 *next;   /* offset 0 */
    struct Node8024 *prev;   /* offset 4 */
    s32 key;                 /* offset 8 */
} Node8024;

extern Node8024 *D_8004A9A4;
extern s32 D_800432A0;
extern s32 D_8004A998;

extern void func_80025F24(s32 *);

void func_80108024(Node8024 *arg0) {
    Node8024 *cur = D_8004A9A4;
    if (cur != 0) {
        while (arg0->key != cur->key) {
            cur = cur->next;
            if (cur == 0) {
                break;
            }
        }
    }
    if (cur == 0) {
        func_80025F24(&D_800432A0);
        for (;;);
    }
    arg0->next = cur->prev;
    cur->prev = arg0;
    --D_8004A998;
}
