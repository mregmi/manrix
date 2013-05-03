/**
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
 **/
/**
 * File:- process.c
 * 	The Process Management Routines.
 **/

#include <errno.h>
#include <string.h>

#include <kernel/scheduling.h>
#include <kernel/vm.h>
#include <kernel/kernel_stdio.h>
#include <kernel/signal.h>
#include <kernel/info.h>
#include <kernel/timer.h>


extern struct vm_map kernel_vmmap;

kmem_cache_t process_cache = NULL;

typedef struct {
	process_t proc;
} *phashtable_t;

/* Hash table of all the processes. */
phashtable_t phash = {NULL};

/* Spinlock for hash table */
spinlock_t phashlock = SPIN_LOCK_UNLOCKED;

struct process kernel_process =	{
				{NULL,NULL},
				{NULL,NULL},
				{NULL,NULL},
				&kernel_vmmap,
				0, 0, 0, 0, 0, 0,
				NULL,	/* parent */
				NULL,
				0, 0, 0,
				{NULL, NULL},
				NULL, 0, 0, 0,
				};

process_t kernel = &kernel_process;

extern struct sysinfo sysinfo;

void process_init()
{
	process_cache = kmem_cache_create("process_cache",
		sizeof(struct process), 32, NULL, NULL);
	if(!process_cache)
		panic("process_init: Not Enough Memory to boot ManRiX\n");

	phash =	(phashtable_t)kmem_alloc(PAGE_SIZE);
	if(!phash)
		panic("process_init: Not Enough Memory to boot ManRiX\n");
	memset(phash, 0, PAGE_SIZE);

	list_init(&kernel_process.threads);
	list_init(&kernel_process.proclist);
	list_init(&kernel_process.child);
	list_init(&kernel_process.sigqueue);
	list_init(&kernel_process.timer);
	kernel_process.parent = kernel;
	list_init(&sysinfo.proclist);
	SPIN_LOCK_INIT(&kernel_process.lock);

	thread_init();
	init_signal();
	id_init();
	fd_init();
	sync_init();
}

/* Insert the process in the global hash table. */
static inline void phash_insert(process_t process)
{
	phashtable_t p;

	spin_lock(&phashlock);
	p = &phash[hash_fn(process->pid)];
	process->next = p->proc;
	p->proc = process;
	spin_unlock(&phashlock);
}

/* Remove the process from the global hash table. */
static inline void phash_remove(process_t process)
{
	phashtable_t bucket;
	process_t p, link;

	spin_lock(&phashlock);
	bucket = &phash[hash_fn(process->pid)];
	p = bucket->proc;

	if(p == process)
	{
		p = process->next;
	}
	else
	{
		for(link=p; (link->next!=process) || (link->next != NULL); link = link->next)
		;
		link->next = process->next;
	}
	spin_unlock(&phashlock);
}

/* Find a process by its pid. */
process_t find_by_pid(pid_t pid)
{
	phashtable_t bucket;
	process_t p, link;

	spin_lock(&phashlock);
	bucket = &phash[hash_fn(pid)];
	p = bucket->proc;

	/* optimize for average case. */
	if(p->pid == pid)
	{
		link = p;
		goto ok;
	}
	else
	{

		for(link=p->next; (link->pid!=pid); link=link->next)
			if(link->next == NULL)
				goto errp;
	}

ok:
	spin_unlock(&phashlock);
	return link;
errp:
	spin_unlock(&phashlock);
	return NULL;
}

void  change_pid(process_t p, pid_t new)
{
	dealloc_pid(p->pid);
//	thash_remove(thread);
	p->pid  = new;
	phash_insert(p);
}

/* Get the Zombie Child of the given process */
/*static inline */process_t get_zombie_child(process_t p)
{
	process_t child;
	spin_lock(&p->lock);
	for(child = p->child.next; child != (process_t)&p->child ; child = child->child.next)
	{
		if(!child)
			break;
		if(child->status & PROCESS_ZOMBIE)
		{
//			kprintf("Zombie child found %d ", child->pid);
			spin_unlock(&p->lock);
			return child;
		}
	}

	spin_unlock(&p->lock);
	return NULL;
}

/*
 * Get the total user time of all the Child Users.
 */
clock_t get_childs_utime(process_t p)
{
	process_t child;
	clock_t total = 0;

	spin_lock(&p->lock);
	if(list_isempty(&p->child))
		goto end;

	for(child = p->child.next; child != (process_t)&p->child ; child = child->child.next)
		total += child->user_time;
end:
	spin_unlock(&p->lock);
	return total;
}

/*
 * Get the total System time of all the Child Processes.
 */
clock_t get_childs_stime(process_t p)
{
	process_t child;
	clock_t total = 0;

	spin_lock(&p->lock);
	if(list_isempty(&p->child))
		goto end;

	for(child = p->child.next; child != (process_t)&p->child ; child = child->child.next)
		total += child->system_time;
end:
	spin_unlock(&p->lock);
	return total;
}

/* Duplicate a process */
process_t do_fork(process_t parent_process,  u32 flag)
{
	process_t new_process;
	int ret;

	new_process = kmem_cache_alloc(process_cache, 0);
	if(!new_process)
		return (process_t)-ENOMEM;
	memcpy(new_process, parent_process, sizeof(struct process));

	new_process->parent = parent_process;
	new_process->pid = alloc_pid();
	new_process->start_time = 0;
	new_process->user_time = 0;
	new_process->system_time = 0;
	new_process->alarm = NULL;
	SPIN_LOCK_INIT(&new_process->lock);
	list_init(&new_process->threads);
	list_init(&new_process->sigqueue);
	list_init(&new_process->timer);
	list_init(&new_process->child);
	list_init(&new_process->sync.syncs);
	list_add(&parent_process->child, new_process, child, process_t);
	sysinfo_add_process(new_process);

	ret = fd_fork(new_process, parent_process);
	if(ret < 0)
		goto errout;

	init_signals(new_process->action);
	phash_insert(new_process);
	list_init(&new_process->threads);

	if(flag & DUP_SHARE)
	{
		atomic_inc(&parent_process->map->ref_count);
		new_process->status = STAT_VFORKED;
		goto out;
	}

	new_process->map = vm_map_fork(parent_process->map, flag);
	if(!new_process->map)
	{
		ret = -ENOMEM;
		goto errout;
	}

out:
	dup_thread(new_process);
	new_process->flag = flag;
//	kprintf("Fork Success ");
	return new_process;

errout:
	kmem_cache_free(process_cache, new_process);
	return (process_t)ret;
}

process_t do_spawn(process_t parent, struct thread_attr *attr)
{
	process_t new_process;
	thread_t thread;

	new_process = kmem_cache_alloc(process_cache,0);
	if(!new_process)
		return (process_t)-ENOMEM;
	memcpy(new_process, parent, sizeof(struct process));

	new_process->parent = parent;
	new_process->pid = alloc_pid();
	new_process->start_time = 0;
	new_process->user_time = 0;
	new_process->system_time = 0;
	new_process->status = PROCESS_ACTIVE;
	list_init(&new_process->threads);
	list_init(&new_process->sigqueue);
	list_init(&new_process->timer);
	list_init(&new_process->child);
	list_init(&new_process->sync.syncs);
	SPIN_LOCK_INIT(&new_process->lock);
	list_add(&parent->child, new_process, child, process_t);
	sysinfo_add_process(new_process);

	new_process->fd.fd_max = MAX_DEF_FD;
	new_process->fd.fd_hint = 0;
	new_process->fd.bitmap = 0;
	strcpy(new_process->fd.cwdstr, "/");
	memset((void *)&new_process->fd.fileptr, 0, sizeof(vm_offset_t) * MAX_DEF_FD);


	init_signals(new_process->action);
	phash_insert(new_process);

	new_process->map = vm_map_fork(parent->map, DUP_NONE);

	thread = create_thread(new_process, attr);

	if(thread == NULL)
		kprintf(" Thread creation failed ");
	if(!new_process->map)
		return (process_t)-ENOMEM;

	enqueue_thread(thread);
//	spin_unlock(&child->lock);

	return new_process;
}

/*
 * De-allocate the process struct.
 * processes are totally destroyed.
 */
error_t process_free(process_t process)
{
	return kmem_cache_free(process_cache, process);
}


/*
 * According to POSIX,
 * Dupicate only that thread that called fork
 */
error_t dup_thread(process_t new_process)
{
	thread_t chld, parent = current;
	struct thread_attr attr;

	attr.policy = parent->schedpolicy;
	attr.prio.sched_priority = parent->priority_stat;
	chld = create_thread(new_process, &attr);
	if(!chld)
		return -1;

	return 0;
}


int waitpid(pid_t pid, int *stat_loc, int flags)
{
	thread_t thr = current;
	process_t proc = thr->proc, chld = NULL;
	int ret;

	if(!have_children(proc))
		return -ECHILD;

	if(pid > 0)
	{
		chld = find_by_pid(pid);
		if(!chld)
			return -ECHILD;
		if(chld->parent != proc)
			return -ECHILD;
	}
/*	else
		chld = get_zombie_child(proc);

	do
	{
*/		if(chld && (chld->status == PROCESS_ZOMBIE))
		{
			*stat_loc = chld->exit_status;
			ret = chld->pid;
			list_del(&proc->child, chld, child, process_t);
			process_free(chld);
			return ret;
		}

		if(flags & WNOHANG)
			return -ECHILD;
		if(!have_children(proc))
			return -ECHILD;
		if(!thr->sigpending)
		{
			sleep_interruptible(thr);
		}
		kprintf("Wow child exited ");
//		chld = get_zombie_child(proc);
//	}while(1);

	return 0;
}

/* SYSCALL:Exit the process. */
int syscall_exit(void *stackptr, int status)
{
	thread_t thr = current, parent;
	process_t p = thr->proc;
	struct sigaction *sig = &p->action[SIGCHLD-1];
	kprintf(" Exit Called by tid %d pid %d",thr->tid, p->pid);

	spin_lock(&p->lock);
	phash_remove(p);
	delete_sync(&p->sync);
	/* Destroy all timers belonging to this process */
	exit_timers(&p->timer);

	/* Destroy all signal queues */
	exit_signals(p);

	dealloc_pid(p->pid);


	fd_free(&p->fd);

	sysinfo_del_process(p);

	thr->states = PROCESS_EXITING;

	/*
	 * Destroy all threads except the current
	 * Current will be destroyed during switch.
	 */
	exit_threads(&p->threads);


	remove_thread(thr);

	/* notify parent about death of child */
	if(!((sig->sa_flags & SA_NOCLDWAIT) || ((int)sig->sa_handler == SIG_IGN)))
	{
		p->exit_status |= status & 0xFF;
		p->exit_status |= WIFEXIT;
		p->status |= PROCESS_ZOMBIE;
		send_signal_any(p->parent, SIGCHLD, 0);
		destroy_thread(thr);
	}

	/*
	 * Destroy the memory mappings.
	 * But pmap will be destroyed later.
	 */
	if(p->status & STAT_VFORKED)
	{
		/* The parent can now run */
//		kprintf("NNNNN ");
		p->status &= ~STAT_VFORKED;
		parent = (thread_t)p->parent->threads.next;
		enqueue_thread(parent);
		atomic_dec(&p->map->ref_count);
	}
	else
		exit_vm_map(p->map);

	/* Do arch dependent destroy */
//	arch_process_destroy();

	if(atomic_get(&p->map->ref_count) == 0)
	{
		pmap_destroy(p->map->pmap);
		vm_map_destroy(p->map);
	}

free:
	destroy_thread(thr);
	if(p->status & PROCESS_ZOMBIE)
		process_free(p);
scd:
	schedule();

	/* We wont get here. But just in case ??? */
	panic("\nProcess %d running after exit", p->pid);

	return 0;
}

/*
 * Exec: Execute a new file
 */
int exec_kludge(struct vn_object *vn, vm_offset_t stack)
{
	thread_t thr = current, parent;
	process_t p;
	vm_map_entry_t entry;

	p = thr->proc;

	spin_lock(&p->lock);
	/*
	 * Destroy the memory mappings.
	 * But pmap will be destroyed later.
	 */
	if(p->status & STAT_VFORKED)
	{
		/* The parent can now run */
		parent = (thread_t)p->parent->threads.next;
		enqueue_thread(parent);
		atomic_dec(&p->map->ref_count);
		p->status = 0;
		p->map = vm_map_fork(p->map, DUP_NONE);
		if(!p->map)
			return -1;
	}
	else
	{
		exit_vm_map(p->map);
		p->map = vm_map_fork(p->map, DUP_NONE);
		if(!p->map)
			return -1;
	}

	exit_threads(&p->threads);
	delete_sync(&p->sync);
	init_signals(p->action);
	exit_timers(&p->timer);
	exec_thread(thr);

	vm_map(p->map, vn, VM_MAP_ENTRY_OBJECT_FIXED);
	vm_map_enter(p->map, &stack, USER_STACK_ENTRY_SIZE,
			NULL, VM_MAP_ENTRY_OBJECT_FIXED|VM_MAP_ENTRY_ZERO_FILL);

	spin_unlock(&p->lock);
//////////////////////////////////////////////////////////////////////
	entry = vm_map_lookup_entry(p->map, vn->va);            /////
	if(!entry)						/////
	{							/////
		kprintf("Entry Null in EXEC\n");		/////
	}							/////
	data_request_all(entry, vn->va);			/////
//////////////////////////////////////////////////////////////////////
	return 0;
}

/*
 * SYSCALL:Get the pid of the current process if tid = 0
 * or of the given thread if tid > 0
 */
int syscall_getpid(void *regs, tid_t tid)
{
	thread_t thr;

	if(!tid)
		goto ok;

	thr = find_by_tid(tid);
	if(!thr)
		goto ok;
	return thr->proc->pid;

ok:
	return (current->proc->pid);
}


/* SYSCALL:Get the pid of the parent process */
int syscall_getppid()
{
	return (current->proc->parent->pid);
}
