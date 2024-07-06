#include "headers.h"

void linked_list_push(LinkedList *linked_list, void *data) {
    LinkedListItem *old_head = linked_list->head;
    linked_list->head = malloc(sizeof(LinkedListItem));
    linked_list->head->data = data;
    linked_list->head->next = old_head;
    linked_list->size++;
}

void linked_list_remove_using_before(LinkedList *linked_list, LinkedListItem *before_item) {
    if (before_item == NULL) {
        if (linked_list->size == 0) return;
        LinkedListItem *old_head = linked_list->head;
        linked_list->head = linked_list->head->next;
        free(old_head);
    } else {
        if (before_item->next == NULL) return;
        LinkedListItem *removed_item = before_item->next;
        before_item->next = before_item->next->next;
        free(removed_item);
    }
    linked_list->size--;
}

void linked_list_filter(LinkedList *linked_list, bool (*filter_function)(void *)) {
    for (LinkedListItem *item = linked_list->head, *previous_item = NULL;
         item != NULL;) {
        if (!filter_function(item->data)) {
            item = item->next;
            linked_list_remove_using_before(linked_list, previous_item);
        } else {
            previous_item = item, item = item->next;
        }

    }
}
