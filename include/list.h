
#ifndef LIBHASHISH_LIST_H_
#define LIBHASHISH_LIST_H_

struct lhi_list_head
{
	struct lhi_list_head *	next;
	struct lhi_list_head *	prev;
};


static inline void __lhi_list_add(struct lhi_list_head *obj,
				 struct lhi_list_head *prev,
				 struct lhi_list_head *next)
{
	prev->next = obj;
	obj->prev = prev;
	next->prev = obj;
	obj->next = next;
}

static inline void lhi_list_add_tail(struct lhi_list_head *obj,
				    struct lhi_list_head *head)
{
	__lhi_list_add(obj, head->prev, head);
}

static inline void lhi_list_add_head(struct lhi_list_head *obj,
				    struct lhi_list_head *head)
{
	__lhi_list_add(obj, head, head->next);
}

static inline void lhi_list_del(struct lhi_list_head *obj)
{
	obj->next->prev = obj->prev;
	obj->prev->next = obj->next;
}

static inline int lhi_list_empty(struct lhi_list_head *head)
{
	return head->next == head;
}

#define lhi_container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - ((size_t) &((type *)0)->member));})

#define lhi_list_entry(ptr, type, member) \
	lhi_container_of(ptr, type, member)

#define lhi_list_at_tail(pos, head, member) \
	((pos)->member.next == (head))

#define lhi_list_at_head(pos, head, member) \
	((pos)->member.prev == (head))

#define lhi_LIST_HEAD(name) \
	struct lhi_list_head name = { &(name), &(name) }

#define lhi_list_first(pos, head, member) \
	pos = lhi_list_entry((head)->next, typeof(*pos), member);

#define lhi_list_for_each_entry(pos, head, member)				\
	for (pos = lhi_list_entry((head)->next, typeof(*pos), member);	\
	     &(pos)->member != (head);	\
	     (pos) = lhi_list_entry((pos)->member.next, typeof(*(pos)), member))

#define lhi_list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = lhi_list_entry((head)->next, typeof(*pos), member),	\
		n = lhi_list_entry(pos->member.next, typeof(*pos), member);	\
	     &(pos)->member != (head);					\
	     pos = n, n = lhi_list_entry(n->member.next, typeof(*n), member))

#define lhi_init_list_head(head) \
	do { (head)->next = (head); (head)->prev = (head); } while (0)

#endif /* LIBHASHISH_LIST_H_ */
