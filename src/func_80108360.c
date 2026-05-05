#include "common.h"

typedef struct Node {
    s32 _w0;
    struct Node *next;
} Node;

extern Node *D_8004AAE8;
extern s32 D_8004ABFC;

s32 func_80108360(void) {
    Node *p = D_8004AAE8;
    s32 count = 0;
    if (p != 0) {
        do {
            count++;
            p = p->next;
        } while (p != 0);
    }
    return count + D_8004ABFC;
}
