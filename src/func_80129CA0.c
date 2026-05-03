#include "common.h"

typedef struct Node {
    struct Node *next;
    struct Node *prev;
} Node;

void func_80129CA0(Node *node) {
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    if (node->prev != 0) {
        node->prev->next = node->next;
    }
}
