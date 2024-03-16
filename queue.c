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
#define q_insert_base(head, sp, to)                               \
    if (!head)                                                    \
        return false;                                             \
    element_t *tmp = malloc(sizeof(element_t));                   \
    if (!tmp)                                                     \
        return false;                                             \
    tmp->value = (char *) malloc(sizeof(char) * (strlen(s) + 1)); \
    if (!tmp->value) {                                            \
        q_release_element(tmp);                                   \
        return false;                                             \
    }                                                             \
    strncpy(tmp->value, s, (strlen(s) + 1));                      \
    !strcmp(to, "head") ? list_add(&tmp->list, head)              \
                        : list_add_tail(&tmp->list, head);        \
    return true;


#define q_remove_base(head, sp, bufsize, from)                     \
    if (!head || list_empty(head))                                 \
        return NULL;                                               \
    element_t *tmp = !strcmp(from, "first")                        \
                         ? list_first_entry(head, element_t, list) \
                         : list_last_entry(head, element_t, list); \
    strncpy(sp, tmp->value, bufsize);                              \
    sp[bufsize - 1] = '\0';                                        \
    list_del(&tmp->list);                                          \
    return tmp;

#define q_scend_base(head, dir)                                   \
    if (!head || list_empty(head) || list_is_singular(head))      \
        return q_size(head);                                      \
    struct list_head *tmp = head->prev;                           \
    char *cur = list_entry(tmp, element_t, list)->value;          \
    tmp = tmp->prev;                                              \
    while (tmp != head) {                                         \
        element_t *t = list_entry(tmp, element_t, list);          \
        if (strcmp(dir, "ascend") ? strcmp(cur, t->value) >= 0    \
                                  : strcmp(cur, t->value) <= 0) { \
            struct list_head *del = tmp;                          \
            tmp = tmp->prev;                                      \
            list_del(del);                                        \
            q_release_element(t);                                 \
        } else {                                                  \
            cur = t->value;                                       \
            tmp = tmp->prev;                                      \
        }                                                         \
    }                                                             \
    return q_size(head);

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
    q_insert_base(head, s, "head");
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    q_insert_base(head, s, "tail");
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    q_remove_base(head, sp, bufsize, "first");
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    q_remove_base(head, sp, bufsize, "last");
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
        bool nowdel = r != head && !strcmp(nl->value, nr->value);
        if (nowdel || del) {
            list_del(l);
            q_release_element(nl);
            del = nowdel;
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
        if (((descend * 2) - 1) * strcmp(l->value, r->value) > 0) {
            list_move_tail(left->next, head);
        } else {
            list_move_tail(right->next, head);
        }
    }
    if (list_empty(left))
        list_splice_tail(right, head);
    else
        list_splice_tail(left, head);
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
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_scend_base(head, "ascend");
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_scend_base(head, "descend");
}

/* Merge another queue with first queue, which is in ascending/descending
 * order */
void merge_with_first_q(struct list_head *another_q,
                        struct list_head *first_q,
                        bool descend)
{
    LIST_HEAD(tmp_head);
    while (!list_empty(first_q) && !list_empty(another_q)) {
        element_t *f = list_first_entry(first_q, element_t, list);
        element_t *a = list_first_entry(another_q, element_t, list);
        if (((descend * 2) - 1) * strcmp(f->value, a->value) > 0) {
            list_move_tail(first_q->next, &tmp_head);
        } else {
            list_move_tail(another_q->next, &tmp_head);
        }
    }
    if (list_empty(first_q))
        list_splice_tail_init(another_q, &tmp_head);
    else
        list_splice_tail_init(first_q, &tmp_head);
    list_splice(&tmp_head, first_q);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    int len = 0;

    // approach 1
    /*
    queue_contex_t *first_q = list_entry(head->next, queue_contex_t, chain);
    struct list_head *tmp, *tmphead = head->next;

    list_for_each (tmp, tmphead) {
        tmphead = head;
        queue_contex_t *another_q = list_entry(tmp, queue_contex_t, chain);
        len += another_q->size;
        list_splice_init(another_q->q, first_q->q);
    }
    q_sort(first_q->q, descend);
    */
    // approach 2
    queue_contex_t *first_q = list_entry(head->next, queue_contex_t, chain);
    struct list_head *tmp, *tmphead = head->next;
    list_for_each (tmp, tmphead) {
        tmphead = head;
        queue_contex_t *another_q = list_entry(tmp, queue_contex_t, chain);
        len += another_q->size;
        merge_with_first_q(another_q->q, first_q->q, descend);
    }
    return len;
    // https://leetcode.com/problems/merge-k-sorted-lists/
}

// Integrate lib/list_sort into the lab0-c project.

/*
 * Returns a list organized in an intermediate format suited
 * to chaining of merge() calls: null-terminated, no reserved or
 * sentinel head node, "prev" links not maintained.
 */
__attribute__((nonnull)) static struct list_head *merge_list_sort(
    struct list_head *a,
    struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

/*
 * Combine final list merge with restoration of standard doubly-linked
 * list structure.  This approach duplicates code from merge(), but
 * runs faster than the tidier alternatives of either a separate final
 * prev-link restoration pass, or maintaining the prev links
 * throughout.
 */
__attribute__((nonnull)) static void merge_final(struct list_head *head,
                                                 struct list_head *a,
                                                 struct list_head *b)
{
    struct list_head *tail = head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

/**
 * list_sort - sort a list
 * @priv: private data, opaque to list_sort(), passed to @cmp
 * @head: the list to sort
 * @cmp: the elements comparison function
 *
 * The comparison function @cmp must return > 0 if @a should sort after
 * @b ("@a > @b" if you want an ascending sort), and <= 0 if @a should
 * sort before @b *or* their original order should be preserved.  It is
 * always called with the element that came first in the input in @a,
 * and list_sort is a stable sort, so it is not necessary to distinguish
 * the @a < @b and @a == @b cases.
 *
 * This is compatible with two styles of @cmp function:
 * - The traditional style which returns <0 / =0 / >0, or
 * - Returning a boolean 0/1.
 * The latter offers a chance to save a few cycles in the comparison
 * (which is used by e.g. plug_ctx_cmp() in block/blk-mq.c).
 *
 * A good way to write a multi-word comparison is::
 *
 *	if (a->high != b->high)
 *		return a->high > b->high;
 *	if (a->middle != b->middle)
 *		return a->middle > b->middle;
 *	return a->low > b->low;
 *
 *
 * This mergesort is as eager as possible while always performing at least
 * 2:1 balanced merges.  Given two pending sublists of size 2^k, they are
 * merged to a size-2^(k+1) list as soon as we have 2^k following elements.
 *
 * Thus, it will avoid cache thrashing as long as 3*2^k elements can
 * fit into the cache.  Not quite as good as a fully-eager bottom-up
 * mergesort, but it does use 0.2*n fewer comparisons, so is faster in
 * the common case that everything fits into L1.
 *
 *
 * The merging is controlled by "count", the number of elements in the
 * pending lists.  This is beautifully simple code, but rather subtle.
 *
 * Each time we increment "count", we set one bit (bit k) and clear
 * bits k-1 .. 0.  Each time this happens (except the very first time
 * for each bit, when count increments to 2^k), we merge two lists of
 * size 2^k into one list of size 2^(k+1).
 *
 * This merge happens exactly when the count reaches an odd multiple of
 * 2^k, which is when we have 2^k elements pending in smaller lists,
 * so it's safe to merge away two lists of size 2^k.
 *
 * After this happens twice, we have created two lists of size 2^(k+1),
 * which will be merged into a list of size 2^(k+2) before we create
 * a third list of size 2^(k+1), so there are never more than two pending.
 *
 * The number of pending lists of size 2^k is determined by the
 * state of bit k of "count" plus two extra pieces of information:
 *
 * - The state of bit k-1 (when k == 0, consider bit -1 always set), and
 * - Whether the higher-order bits are zero or non-zero (i.e.
 *   is count >= 2^(k+1)).
 *
 * There are six states we distinguish.  "x" represents some arbitrary
 * bits, and "y" represents some arbitrary non-zero bits:
 * 0:  00x: 0 pending of size 2^k;           x pending of sizes < 2^k
 * 1:  01x: 0 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * 2: x10x: 0 pending of size 2^k; 2^k     + x pending of sizes < 2^k
 * 3: x11x: 1 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * 4: y00x: 1 pending of size 2^k; 2^k     + x pending of sizes < 2^k
 * 5: y01x: 2 pending of size 2^k; 2^(k-1) + x pending of sizes < 2^k
 * (merge and loop back to state 2)
 *
 * We gain lists of size 2^k in the 2->3 and 4->5 transitions (because
 * bit k-1 is set while the more significant bits are non-zero) and
 * merge them away in the 5->2 transition.  Note in particular that just
 * before the 5->2 transition, all lower-order bits are 11 (state 3),
 * so there is one list of each smaller size.
 *
 * When we reach the end of the input, we merge all the pending
 * lists, from smallest to largest.  If you work through cases 2 to
 * 5 above, you can see that the number of elements we merge with a list
 * of size 2^k varies from 2^(k-1) (cases 3 and 5 when x == 0) to
 * 2^(k+1) - 1 (second merge of case 5 when x == 2^(k-1) - 1).
 */
__attribute__((nonnull)) void list_sort(struct list_head *head)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    /*
     * Data structure invariants:
     * - All lists are singly linked and null-terminated; prev
     *   pointers are not maintained.
     * - pending is a prev-linked "list of lists" of sorted
     *   sublists awaiting further merging.
     * - Each of the sorted sublists is power-of-two in size.
     * - Sublists are sorted by size and age, smallest & newest at front.
     * - There are zero to two sublists of each size.
     * - A pair of pending sublists are merged as soon as the number
     *   of following pending elements equals their size (i.e.
     *   each time count reaches an odd multiple of that size).
     *   That ensures each later final merge will be at worst 2:1.
     * - Each round consists of:
     *   - Merging the two sublists selected by the highest bit
     *     which flips when count is incremented, and
     *   - Adding an element from the input as a size-1 sublist.
     */
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge_list_sort(b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge_list_sort(pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(head, pending, list);
}