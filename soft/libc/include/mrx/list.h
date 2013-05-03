/*
 * Libc for ManRiX OS
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 * File: list.h
 *
 * premitives for the linked list;
 *
 */

#ifndef __LIST_H
#define __LIST_H


/*
 * A doubly linked list primitive
 */
struct linked_list {
	/*struct linked_list*/void *next, *prev;
};

typedef struct linked_list *list_t;

#define list_isempty(list)	(((list)->next == (list))? 1:0)

/* The last entry in the list */
#define last_entry(head, ptr)	((ptr) = (head)->prev)

/* The first entry in the list */
#define first_entry(head, ptr)	((ptr) = (head)->next)

/*
 * Queue implementation for linked list.
 */
/*
 * Add to the queue.
 */
 #define list_enqueue(head, pointer, member, type)	\
 	{						\
 		type nxt = (type)((head)->next);		\
 		(head)->next = (pointer);		\
 		(pointer)->member.prev = (type)(head);	\
 		(pointer)->member.next = nxt;		\
		if(nxt == (type)head)				\
			(head)->prev = (pointer);		\
		else					\
 			nxt->member.prev = (type)(pointer);\
 	}
/*
 * Remove from the queue.
 * The caller should if the queue is empty.
 */
#define list_dequeue(head, pointer, member, type)	\
	{					\
		type nxt;			\
		(pointer) = (type)(head)->prev;	\
		nxt = (type)(pointer)->member.prev;\
		(head)->prev = nxt;		\
		if(nxt == (type)head)			\
			(head)->next = head;	\
		else				\
			nxt->member.next = head;\
	}

/*
 *Stack implementation of the linked list.
 */
/*
 * Add to the linked list
 */
#define list_add(head, pointer, member, type)		\
		{					\
			type nxt = (type)(head)->next;	\
			(head)->next = (pointer);	\
			(pointer)->member.prev = (type)(head);\
			(pointer)->member.next = (type)nxt;	\
			if(nxt == (type) head)			\
				(head)->prev = (pointer);	\
			else							\
				nxt->member.prev = (type)(pointer);			\
		}

/*
 * Remove from the linked list.
 * The caller should check if stack is empty.
 */
#define list_remove(head, pointer, member, type)	\
		{					\
			type nxt;			\
			(pointer) = (type)(head)->next;	\
			nxt = (type)(pointer)->member.next;\
			(head)->next = nxt;		\
			if(nxt == (type)head)		\
				(head)->prev = head;			\
			else							\
				nxt->member.prev = head;		\
		}

/*
 * Remove from the middle of the list
 */
#define list_del(head,item, member, type)	\
{										\
	type prev = (item)->member.prev;	\
	type next = (item)->member.next;	\
	if(prev == (type)head)				\
		((list_t)prev)->next = (next);				\
	else								\
		prev->member.next = (next);		\
	if(next == (type)head)				\
		((list_t)next)->prev = (prev);				\
	else								\
		next->member.prev = (prev);		\
}

#define list_add_next(prv, prevmem, ptype, item, imem, itype)	\
{					\
	ptype nxt = (ptype)(prv)->prevmem.next;	\
	(prv)->prevmem.next = (item);	\
	(item)->imem.prev = (ptype)(prv);\
	(item)->imem.next = (ptype)nxt;	\
}

/*
 * Initialize the linked list
 * Both next and previous point to the head.
 */
static inline void list_init(list_t l)
{
	l->next = l;
	l->prev = l;
}

#endif  /* __KERNEL_LIST_H */
