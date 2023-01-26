#include <stdio.h>
#include <stdlib.h>

struct node {
    void *data;
    struct node *next;
    struct node *prev;
};

void list_init(struct node **head)
{
    *head = NULL;
}

void list_node_add(struct node **head, struct node *node)
{
    node->prev = NULL;
    node->next = *head;

    *head = node;
}

void list_node_delete(struct node **head, int (*is_true)(struct node *, void *), void *m, void (*cb)(void *))
{
    struct node *n = *head;
    struct node *t;

    while (n) {
        if (is_true(n, m)) {
            t = n;
            if (n->next)
                n->next->prev = n->prev;
            if (n->prev)
                n->prev->next = n->next;
            else
                *head = n->next;
            n = n->next;
            cb(t->data);
            continue;
        }
        n = n->next;
    }

}

void list_iter(struct node **head, void (*cb)(struct node *n, void *cbd), void *cbd)
{
    struct node *n = *head;

    while (n) {
        cb(n, cbd);
        n = n->next;
    }
}

struct node *list_node_new(void *data)
{
    struct node *n;

    n = malloc(sizeof(struct node));

    if (n) {
        n->data = data;
        n->prev = NULL;
        n->next = NULL;
    }

    return n;
}
