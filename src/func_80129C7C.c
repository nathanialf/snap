#include "common.h"

typedef struct Node {
    struct Node *next;
    struct Node *prev;
} Node;

void func_80129C7C(Node *new_node, Node *after_node) {
    new_node->next = after_node->next;
    new_node->prev = after_node;
    if (after_node->next != 0) {
        after_node->next->prev = new_node;
    }
    after_node->next = new_node;
}
