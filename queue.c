#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
        q_release_element(entry);
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
    // <approach 1>
    /*
    int counter = q_size(head) / 2;
    struct list_head *tmp = head->next;
    while (counter--) {
        tmp = tmp->next;
    }
    list_del(tmp);
    q_release_element(list_entry(tmp, element_t, list));
    */
    // <approach 2>
    if (list_is_singular(head)) {
        struct list_head *del = head->next;
        list_del(del);
        q_release_element(list_entry(del, element_t, list));
    }
    struct list_head *fast = head->next->next;
    struct list_head *slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    struct list_head *del = slow;
    list_del(del);
    q_release_element(list_entry(del, element_t, list));

    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *l, *r;
    bool del = false;
    list_for_each_safe (l, r, head) {
        element_t *nl = list_entry(l, element_t, list);
        element_t *nr = list_entry(r, element_t, list);
        if (l->next != head && !strcmp(nl->value, nr->value)) {
            list_del(l);
            q_release_element(nl);
            del = true;
        } else if (del) {
            list_del(l);
            q_release_element(nl);
            del = false;
        }
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    // <approach 2>
    struct list_head *l = head->next, *r;
    int len = q_size(head);
    for (int i = 0; i < len - 1; i += 2) {
        r = l->next;
        struct list_head *first = l, *second = r;
        l = r->next;
        list_del(first);
        list_del(second);
        list_add_tail(second, head);
        list_add_tail(first, head);
    }
    if (len % 2 == 1) {
        list_del(l);
        list_add_tail(l, head);
    }
    // <approach1>
    /*
    struct list_head *l, *r;
    for (l = (head)->next; l != (head); l = l->next) {
        r = l->next;
        if (r == head)
            break;

        l->next = r->next;
        r->prev = l->prev;
        l->prev->next = r;
        r->next->prev = l;
        l->prev = r;
        r->next = l;
    }
    */

    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *r, *l;
    list_for_each_safe (l, r, head) {
        l->next = l->prev;
        l->prev = r;
    }
    l->next = l->prev;
    l->prev = r;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    struct list_head *l, *r, *leader;
    struct list_head group;
    int counter = k - 1;
    for (l = (head)->next, r = l->next, leader = head; l != (head);
         l = r, r = l->next, --counter) {
        if (counter == 0) {
            list_cut_position(&group, leader, l);
            q_reverse(&group);
            list_splice_init(&group, leader);
            leader = r->prev;
            counter = k;
        }
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Merge each elements of queue in ascending/descending order */
void merge(struct list_head *head,
           struct list_head *left,
           struct list_head *right,
           bool descend)
{
    while (!list_empty(left) && !list_empty(right)) {
        element_t *l = list_entry(left->next, element_t, list);
        element_t *r = list_entry(right->next, element_t, list);
        if (descend) {
            if (strcmp(l->value, r->value) > 0) {
                list_move_tail(left->next, head);
            } else {
                list_move_tail(right->next, head);
            }
        } else {
            if (strcmp(l->value, r->value) < 0) {
                list_move_tail(left->next, head);
            } else {
                list_move_tail(right->next, head);
            }
        }
    }
    if (!list_empty(left))
        list_splice_tail(left, head);
    else
        list_splice_tail(right, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head l, r;
    INIT_LIST_HEAD(&l);
    INIT_LIST_HEAD(&r);

    list_cut_position(&l, head, slow);
    list_splice_init(head, &r);

    q_sort(&l, descend);
    q_sort(&r, descend);

    merge(head, &l, &r, descend);
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);

    struct list_head *tmp = head->prev;
    char *curMin = list_entry(tmp, element_t, list)->value;
    tmp = tmp->prev;

    while (tmp != head) {
        element_t *na = list_entry(tmp, element_t, list);

        if (strcmp(curMin, na->value) <= 0) {
            struct list_head *del = tmp;
            tmp = tmp->prev;
            list_del(del);
            q_release_element(na);
        } else {
            curMin = na->value;
            tmp = tmp->prev;
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *tmp = head->prev;
    char *curMax = list_entry(tmp, element_t, list)->value;
    tmp = tmp->prev;

    while (tmp != head) {
        element_t *t = list_entry(tmp, element_t, list);

        if (strcmp(curMax, t->value) >= 0) {
            struct list_head *del = tmp;
            tmp = tmp->prev;
            list_del(del);
            q_release_element(t);
        } else {
            curMax = t->value;
            tmp = tmp->prev;
        }
    }

    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}


/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    queue_contex_t *first_q = list_entry(head->next, queue_contex_t, chain);
    struct list_head *tmp;

    list_for_each (tmp, head) {
        if (tmp == head->next) {
            continue;
        }

        queue_contex_t *another_q = list_entry(tmp, queue_contex_t, chain);
        if (list_empty(another_q->q))
            continue;
        len += another_q->size;
        list_splice_init(another_q->q, first_q->q);
    }
    q_sort(first_q->q, descend);
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return len;
}