#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */



/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *h = (struct list_head *) malloc(sizeof(struct list_head));
    if (h)
        INIT_LIST_HEAD(h);
    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        free(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;

    tmp->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!tmp->value) {
        q_release_element(tmp);
        return false;
    }

    strncpy(tmp->value, s, (strlen(s) + 1));
    list_add(&tmp->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;

    tmp->value = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!tmp->value) {
        q_release_element(tmp);
        return false;
    }

    strncpy(tmp->value, s, (strlen(s) + 1));
    list_add_tail(&tmp->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_first_entry(head, element_t, list);
    if (tmp && sp) {
        strncpy(sp, tmp->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(&tmp->list);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_last_entry(head, element_t, list);
    if (tmp && sp) {
        strncpy(sp, tmp->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(&tmp->list);
    return tmp;
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
    if (head == NULL || list_empty(head))
        return false;
    int counter = q_size(head) / 2;
    struct list_head *tmp = head->next;
    while (counter--) {
        tmp = tmp->next;
    }
    list_del(tmp);
    q_release_element(list_entry(tmp, element_t, list));
    /*
        struct list_head* fast = head->next;
        struct list_head* slow = head->next;
        if(list_is_singular(head)){
            head = NULL;
            q_release_element(list_entry(head->next, element_t, list));
        }

        while (fast != head && fast->next != head) {
            fast = fast->next->next;
            slow = slow->next;
        }

        list_del(slow->next);
        q_release_element(list_entry(slow->next, element_t, list));
     */


    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *cur, *right;

    list_for_each_safe (cur, right, head) {
        element_t *c = list_entry(cur, element_t, list);
        element_t *r = list_entry(right, element_t, list);
        if (strcmp(c->value, r->value) == 0) {
            list_del(cur);
            q_release_element(c);
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *r, *l;
    list_for_each_safe (l, r, head) {
        l->next = r->next;
        r->prev = l->prev;
        l->prev->next = r;
        r->next->prev = l;
        l->prev = r;
        r->next = l;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

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
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
