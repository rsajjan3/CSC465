#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"

//https://codereview.stackexchange.com/questions/212041/linked-list-written-in-c

Segment get_node_val(const NODE *node) {
    return node->data;
}

void set_node_val(NODE *node, Segment val) {
    node->data = val;
}

static size_t node_size(void) {
    return sizeof(NODE);
}

static void *allocate_node(void) {
    return malloc(node_size());
}

NODE *create_node(Segment data) {
    NODE *new_node;
    if ((new_node = allocate_node()) == NULL) {
        return NULL;
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

NODE *append_node(NODE *head, NODE *to_add) {
    NODE *current;
    if (head == NULL) {
        return NULL;
    }
    else if(head->data.base >= to_add->data.base)
    {
        to_add->next = head;
        head = to_add;
    }
    else
    {
        current = head;
        while (current->next != NULL && current->next->data.base < to_add->data.base) {
            current = current->next;
        }
        
        to_add->next = current->next;
        current->next = to_add;
    }
    return head;
}

NODE *search_node(const NODE *head, Segment search_val) {
    for (const NODE *current = head; current != NULL; current = current->next) {
        if (current->data.base == search_val.base && current->data.sizee == search_val.sizee) {
            return (NODE *) current;
        }
    }
    return NULL;
}

NODE *delete_node(NODE *head, Segment delete_val) {
    // Taken from "Linus Torvalds - The mind behind Linux" Ted Talk
    // https://youtu.be/qrYt4bbEUrU
    NODE **indirect = &head;
    while ((*indirect) != NULL && (*indirect)->data.base != delete_val.base)
        indirect = &(*indirect)->next;
    
    *indirect = (*indirect)->next;
    return head;
}

void print_all(const NODE *head) {
    for (const NODE *current = head; current != NULL; current = current->next) {
        printf("(Address: %d|Size: %d)->", current->data.base, current->data.sizee);
    }
    puts("NULL");
}

void free_all(NODE *head) {
    for (NODE *current = head; current != NULL; current = current->next) {
        free(head);
    }
}