#include "common.h"

typedef struct Node {
    struct Node *next;
} Node;

Node *func_80133A04(Node **arg0) {
    struct Node *new_var;
    Node *old = *arg0;
    if (0) {
    }
    new_var = (*arg0)->next;
    *arg0 = new_var;
    return old;
}
