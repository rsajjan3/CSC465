#ifndef LINKEDLIST_H_INCLUDED
#define LINKEDLIST_H_INCLUDED

//https://codereview.stackexchange.com/questions/212041/linked-list-written-in-c
typedef struct
{
    int base;
    int sizee;
} Segment;

typedef struct node
{
    Segment data;
    struct node *next;
} NODE;

Segment get_node_val(const NODE *node);

/**
 * Set node value (caller is responsible for ensuring node isn't NULL)
 * @param node
 * @param val - int value
 */
void set_node_val(NODE *node, Segment val);

/***
 * Allocates memory for a node, and assigns data to the new node
 * @param data - integer to store in the new node
 * @return newly allocated node
 */
NODE *create_node(Segment data);

/**
 * Sorted add to the linked list
 * @param head - first node
 * @param to_add - node to add
 * @return head node
 */
NODE *append_node(NODE *head, NODE *to_add);


/**
 * Searches for a value in the linked list
 * @param head - first node
 * @param search_val - value to search for
 * @return instance of node if it exists, or NULL if it doesn't exist
 */
NODE *search_node(const NODE *head, Segment search_val);

/**
 * Deletes the first occurence of "delete_val" from the linked list
 * @param head - first node
 * @param delete_val - value to delete from the linked list (may be head node)
 * @return head node - should be reassigned to head in case of first node being deleted
 */
NODE *delete_node(NODE *head, Segment delete_val);

/**
 * Prints each node in the linked list
 * @param head - first node to start traversing from
 */
void print_all(const NODE *head);

/**
 * Frees all member of the linked list
 * @param head - first node to start traversing from
 */
void free_all(NODE *head);

#endif //LINKEDLIST_H_INCLUDED