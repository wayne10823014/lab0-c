#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *node = (malloc(sizeof(struct list_head)));
    if (!node) {
        return NULL;
    }
    INIT_LIST_HEAD(node);
    return node;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        free(entry->value);
        free(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    char *str = malloc(sizeof(char) * strlen(s) + 1);
    if (!str) {
        free(node);
        return false;
    }
    strncpy(str, s, strlen(s) + 1);
    node->value = str;
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        return false;
    }
    char *str = malloc(sizeof(char) * strlen(s) + 1);
    if (!str) {
        free(node);
        return false;
    }
    strncpy(str, s, strlen(s) + 1);
    node->value = str;
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node = list_first_entry(head, element_t, list);
    list_del(&node->list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *node = list_last_entry(head, element_t, list);
    list_del(&node->list);
    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *node = head->next;
    for (struct list_head *fast = head->next;
         fast != head->prev && fast->next != head->prev;
         fast = fast->next->next) {
        node = node->next;
    }
    element_t *tmp = list_entry(node, element_t, list);
    list_del(node);
    free(tmp->value);
    free(tmp);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    // char *maxVal = NULL;
    if (!head || list_empty(head)) {
        return false;
    }
    element_t *cur, *next;
    bool del = false;
    list_for_each_entry_safe (cur, next, head, list) {
        if (&next->list != head && !strcmp(cur->value, next->value)) {
            list_del(&cur->list);
            q_release_element(cur);
            del = true;
        } else if (del) {
            del = false;
            list_del(&cur->list);
            q_release_element(cur);
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL || list_empty(head))
        return;
    struct list_head *cur = head->next, *next = head->next->next;
    while (cur != head && next != head) {
        cur->prev->next = next;
        next->next->prev = cur;
        cur->next = next->next;
        next->prev = cur->prev;
        cur->prev = next;
        next->next = cur;
        cur = cur->next;
        next = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *front = head->prev, *cur = head, *next = NULL;
    while (next != head) {
        next = cur->next;
        cur->next = front;
        cur->prev = next;
        front = cur;
        cur = next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    struct list_head *cur, *next, tmp, *tmp_head = head;
    INIT_LIST_HEAD(&tmp);
    int i = 0;
    list_for_each_safe (cur, next, head) {
        i++;
        if (i == k) {
            list_cut_position(&tmp, tmp_head, cur);
            q_reverse(&tmp);
            list_splice_init(&tmp, tmp_head);
            i = 0;
            tmp_head = next->prev;
        }
    }
    return;
}

// Sort elements of queue in ascending order
void merge_two_list(struct list_head *head,
                    struct list_head *left,
                    struct list_head *right,
                    bool descend)
{
    LIST_HEAD(tmp);

    while (!list_empty(left) && !list_empty(right)) {
        element_t *left_entry = list_entry(left->next, element_t, list);
        element_t *right_entry = list_entry(right->next, element_t, list);
        if (descend ? strcmp(left_entry->value, right_entry->value) > 0
                    : strcmp(left_entry->value, right_entry->value) < 0) {
            list_move_tail(left->next, &tmp);
        } else {
            list_move_tail(right->next, &tmp);
        }
    }
    if (list_empty(left)) {
        list_splice_tail_init(right, &tmp);
    }
    if (list_empty(right)) {
        list_splice_tail_init(left, &tmp);
    }
    list_splice_init(&tmp, head);
}
void merge_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *mid = head;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        mid = mid->next;

    LIST_HEAD(left);
    LIST_HEAD(right);

    list_cut_position(&left, head, mid);
    list_splice_init(head, &right);

    merge_sort(&left, descend);
    merge_sort(&right, descend);

    merge_two_list(head, &left, &right, descend);
}

void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    merge_sort(head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    struct list_head *node = head->prev;
    struct list_head *pnode = node->prev;
    char *maxVal = NULL;

    for (; node != head; node = pnode) {
        element_t *entry = container_of(node, element_t, list);
        pnode = node->prev;
        if (!maxVal || strcmp(entry->value, maxVal) > 0) {
            maxVal = entry->value;
        } else {
            list_del(node);
            q_release_element(entry);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    queue_contex_t *qhead = list_first_entry(head, queue_contex_t, chain);
    list_del_init(&qhead->chain);
    queue_contex_t *cur = NULL;

    list_for_each_entry (cur, head, chain) {
        list_splice_init(cur->q, qhead->q);
        qhead->size += cur->size;
    }

    list_add(&qhead->chain, head);
    q_sort(qhead->q, descend);
    return qhead->size;
}
