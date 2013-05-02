/*
 * ManRiX Microkernel
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
 * File: irq.c
 *
 * This file handles interrupt requests. We only redirect the IRQ to the user
 * level threads.
 */

#include <string.h>

#include <asm/asm.h>
#include <kernel/irq.h>
#include <kernel/list.h>
#include <kernel/scheduling.h>
#include <kernel/vm.h>
#include <kernel/spinlock.h>

/* Cache for IRQ structures. */
kmem_cache_t IRQ_cache = NULL;

/* Hash table of irq IDs  */
irqhashtable_t irqhash = {NULL};
spinlock_t	irqhashlock = SPIN_LOCK_UNLOCKED;

/* The irq handlers for 15 IRQ lines */
struct irq pc_irq[MAX_IRQ];// = {[0,0, NULL, NULL, NULL, NULL] ... [0,0, NULL, NULL,NULL,NULL]};

/* The next free id for allocation */
static int next_id = 16;

#define HANDLER_OK 	(0xDEAD)

/* Insert the irq in the global hash table. */
static inline void irqhash_insert(irq_t irq)
{
	irqhashtable_t p;

	spin_lock(&irqhashlock);
	p = &irqhash[hash_fn(irq->id)];
	irq->next = p->irq;
	p->irq = irq;
	spin_unlock(&irqhashlock);
}

/* Remove the process from the global hash table. */
static inline void irqhash_remove(irq_t irq)
{
	irqhashtable_t bucket;
	irq_t link;

	spin_lock(&irqhashlock);
	bucket = &irqhash[hash_fn(irq->id)];
	//p = bucket->irq;

	if(bucket->irq == irq)
	{
		bucket->irq = irq->next;
	}
	else
	{
		for(link=bucket->irq; (link->next!=irq) || (link->next != NULL); link = link->next)
		;
		link->next = irq->next;
	}

	spin_unlock(&irqhashlock);
}

/* Find irq by its id. */
irq_t find_by_irqid(int id)
{
	irqhashtable_t bucket;
	irq_t p, link;

	spin_lock(&irqhashlock);
	bucket = &irqhash[hash_fn(id)];
	p = bucket->irq;

	/* optimize for average case. */
	if(p->id == id)
	{
		link = p;
		goto end;
	}
	else
	{

		for(link=p->next; (link->id!=id); link=link->next)
			if(link->next == NULL)
			{
				link = NULL;
				goto end;
			}
	}
end:
	spin_unlock(&irqhashlock);
	return link;
}

/*
 * Initialize IRQ
 */
void IRQ_init()
{
	int i;
	list_t list;
	IRQ_cache = kmem_cache_create("IRQ cache",sizeof(struct irq), 32, NULL, NULL);
	irqhash = (irqhashtable_t)kmem_alloc(PAGE_SIZE);

	if(!IRQ_cache || !irqhash)
		panic("IRQ_init: Not Enough Memory to boot ManRiX\n");

	memset(irqhash, 0, PAGE_SIZE);
	memset(&pc_irq, 0, sizeof(pc_irq));

	for(i=0;i<MAX_IRQ;i++)
	{
		list = &pc_irq[i].list;
		list_init(list);
	}

	IRQ_arch_init();
}

/*
 * Called On every Interrupt.
 * It Finds the Handler and schedules it for Running.
 */
void handle_IRQ(int irqno)
{
	irq_t irq, nextirq;
	thread_t thr;

	irq = &pc_irq[irqno];
	nextirq = irq;
//	kprintf("GOT %d ", irqno);
	do
	{
		if(nextirq->handler != NULL)
		{
			thr = nextirq->owner;
			thr->context.irqflag |= IRQ_REQUEST;
//			thr->context.handler = nextirq->handler;
			if(thr->states & THREAD_BLOCKED)
				wakeup(thr);
		}

		nextirq = nextirq->list.next;
	}while(irq->list.next != &irq->list);
	
	schedule();
	//sched_set_pending(current);
}

/*
 * Request for IRQ.
 * It allocates the irq structure for the requested irq
 * The handler is called on interrupt arrival.
 * irq = irq number
 * handler = handler
 *
 */
int syscall_IRQ_request(void *dummy, int irqnum)
{
	irq_t irq, nextirq;

	if(!superuser())
		return -EPERM;

//	kprintf("my irq is %X handler %X", irqnum, handler);
	irq = &pc_irq[irqnum];
	spin_lock(&irq->lock);
	/*
	 * In a common case, there is one ISR for one IRQ,
	 * It is allocated from pc_irq. Its id is the irq number
	 */
	if(irq->handler == NULL)
	{
		irq->id = irqnum;
		irq->flag = 0;
		irq->handler = (void *)HANDLER_OK;
		irq->owner = current;
		spin_unlock(&irq->lock);

		return irq->id;
	}

	/*
	 * But for IRQ having multiple ISR's, the other entries is
	 * implemented as a linked list. It is allocated in cache and the
	 * the new id is given. The handlers are hashed and the id is used
	 * to get the structure.
	 */
	nextirq = kmem_cache_alloc(IRQ_cache, 0);

	/* add to the linked list */
	list_add(&irq->list, nextirq, list, irq_t);

	nextirq->id = next_id++;
	nextirq->flag = 0;
	nextirq->handler = (void *)HANDLER_OK;
	nextirq->owner = current;

	irqhash_insert(nextirq);
	spin_unlock(&irq->lock);
	return nextirq->id;
}

/*
 * Free the IRQ lines.
 */
int syscall_IRQ_free(void *dummy, int irqid)
{
	irq_t irq, tofree;


	if(!superuser())
		return -EPERM;

	irq = &pc_irq[irqid];

	spin_lock(&irq->lock);
	/* If it is first entry, just make the handler NULL */
	if(irqid < MAX_IRQ)
	{
		irq->handler = NULL;
		irq->flag = 0;
		irq->owner = NULL;
		spin_unlock(&irq->lock);
		return 0;
	}

	tofree = find_by_irqid(irqid);
	if(tofree == NULL)
	{
		spin_unlock(&irq->lock);
		return -EINVAL;
	}

	list_del(&irq->list, tofree, list, irq_t);

	irqhash_remove(tofree);
	kmem_cache_free(IRQ_cache, tofree);

	spin_unlock(&irq->lock);
	return 0;

}

/*
 * Wait for the IRQ.
 */
int syscall_IRQ_wait()
{
	thread_t thread = current;

	if(!superuser())
		return -EPERM;

	thread->states = IRQ_WAIT;
	remove_thread(thread);
	schedule();

	return 0;
}
