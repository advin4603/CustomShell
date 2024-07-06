#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct LinkedListItem LinkedListItem;

struct LinkedListItem {
    void *data;
    LinkedListItem *next;
};

typedef struct LinkedList {
    LinkedListItem *head;
    size_t size;
} LinkedList;

void linked_list_push(LinkedList *, void *data);

void linked_list_filter(LinkedList *, bool (*filter_function)(void *));

void linked_list_remove_using_before(LinkedList *, LinkedListItem *before_item);

#endif // LINKED_LIST_H
