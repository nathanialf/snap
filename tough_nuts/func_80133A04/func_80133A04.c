#include "common.h"

typedef struct Node {
    struct Node *next;
} Node;

Node *func_80133A04(Node **arg0) {
    Node *old;
    Node *next;
    old = *arg0;
    next = old->next;
    *arg0 = next;
    return old;
}
