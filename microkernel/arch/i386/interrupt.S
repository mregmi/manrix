/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	(rajesh dot rc at gmail dot com)
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
 * File: interrupt.S
 *
 * This file handle all the interupts and syscalls.
 */


#define KERNEL_DATA	0x10

 .global divide_error
 .global debug
 .global nmi
 .global breakpoint
 .global overflow
 .global bound
 .global invalid_opcode
 .global device_not_available
 .global double_fault
 .global coprocessor_overrun
 .global invalid_tss
 .global segment_not_present
 .global stack_fault
 .global general_protection
 .global page_fault
 .global reserved
 .global floating_point_error
 .global alignment_check
 .global machine_check
 .global syscall_hand
 .global desc_set
 .global syscall_return
 .global syscall_return_new
 .global special_int
 .global syscall
 
 .global irq_special_tm
 .global irq_0
 .global irq_1
 .global irq_2
 .global irq_3
 .global irq_4
 .global irq_5
 .global irq_6
 .global irq_7
 .global irq_8
 .global irq_9
 .global irq_10
 .global irq_11
 .global irq_12
 .global irq_13
 .global irq_14
 .global irq_15

EAX = 32
ECODE = 44

#define SAVEREGS			\
	cli;				\
	pushl %ds;			\
	pushl %es;			\
	pusha; 				\
	mov $(KERNEL_DATA), %dx;	\
	mov %dx, %ds;			\
	mov %dx, %es;			\
	pushl %esp	/* pointer to stack_regs layout */

#define RESTOREREGS	\
	addl $4, %esp;	\
	popa;		\
	popl %es;	\
	popl %ds	




.text

syscall_hand:
	pushl %eax	/* since we use the same stack_regs for syscall and excption
	                   we are pushing dummy errorcode */   
	SAVEREGS
	andl $(0xFFFF), %eax	/* Only ax contains the syscall number */	
	call *syscall(,%eax,4)	/* syscall */
	movl %eax, EAX(%esp)	/* Save the return value to eax */
	
syscall_return:	
	call check_signal
	RESTOREREGS
	addl $4, %esp
	iret 
	
syscall_return_new:
	call new_spawn	/* a special function for module loaded by bootloader */
	
	RESTOREREGS
	addl $4, %esp
	movb $0x20, %al	
	outb %al, $0x20	/* EOI for timer chip */
	movl $0, %eax
	iret 

divide_error:
	pushl $0	/* pseudo error code */
	SAVEREGS
	pushl ECODE(%esp)
	call handle_div_error
	addl $4, %esp

ret_from_exception:
	call check_signal
	
	RESTOREREGS
	addl $4, %esp	/* error code */

 	iret


debug:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_debug
	addl $4, %esp
	
	jmp ret_from_exception

nmi:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_nmi
	addl $4, %esp
	
	jmp ret_from_exception

breakpoint:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_breakpoint
	addl $4, %esp
	
	jmp ret_from_exception

overflow:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_overflow
	addl $4, %esp
	
	jmp ret_from_exception

bound:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_bound
	addl $4, %esp
	
	jmp ret_from_exception
	
invalid_opcode:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_inv_opcode
	addl $4, %esp
	
	jmp ret_from_exception

device_not_available:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_dev_not_available
	addl $4, %esp
	
	jmp ret_from_exception

double_fault:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_double_fault
	addl $4, %esp
	
	jmp ret_from_exception

coprocessor_overrun:
	pushl $0
	SAVEREGS
	pushl ECODE(%esp)
	call handle_cop_overrun
	addl $4, %esp
	
	jmp ret_from_exception

invalid_tss:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_inv_tss
	addl $4, %esp
	
	jmp ret_from_exception

segment_not_present:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_seg_not_present
	addl $4, %esp
	
	jmp ret_from_exception

stack_fault:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_stack_fault
	addl $4, %esp
	
	jmp ret_from_exception
	
general_protection:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_gp
	addl $4, %esp
	
	jmp ret_from_exception
	
page_fault:
	SAVEREGS

	call do_page_fault

	jmp ret_from_exception

/* Will it occour */
reserved:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_reserved
	addl $4, %esp
		
	jmp ret_from_exception
	
floating_point_error:
	pushl $0	
	SAVEREGS
	pushl ECODE(%esp)
	call handle_fpe
	addl $4, %esp
	
	jmp ret_from_exception
	
alignment_check:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_alignment
	addl $4, %esp
	
	jmp ret_from_exception
	
machine_check:
	SAVEREGS
	pushl ECODE(%esp)
	call handle_mcheck
	addl $4, %esp
	jmp ret_from_exception

/*
 * This is a special function to delay through Timer Chip.
 */
irq_special_tm:
	SAVEREGS
	call special_int
	movb $0x20, %al	
	outb %al, $0x20	/* EOI */
	RESTOREREGS
	iret

irq_0:
	SAVEREGS
	call timer_int
	movb $0x20, %al	
	outb %al, $0x20	/* EOI */
	RESTOREREGS
	iret

irq_1:
	SAVEREGS
	pushl $1
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret

irq_2:
	SAVEREGS

	pushl $2
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret
	
irq_3:
	SAVEREGS

	pushl $3
	call handle_IRQ
	addl $4, %esp
	
	RESTOREREGS
	iret
	
irq_4:
	SAVEREGS

	pushl $4
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret
	
irq_5:
	SAVEREGS

	pushl $5
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret
	
irq_6:
	SAVEREGS
	
	pushl $6
	call handle_IRQ
	addl $4, %esp

	
	RESTOREREGS
	iret
	
irq_7:
	SAVEREGS

	pushl $7
	call handle_IRQ
	addl $4, %esp
	
	RESTOREREGS
	iret
	
irq_8:
	SAVEREGS

	pushl $8
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret
	
irq_9:
	SAVEREGS

	pushl $9
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret
	
irq_10:
	SAVEREGS

	pushl $10
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret
	
irq_11:
	SAVEREGS

	pushl $11
	call handle_IRQ
	addl $4, %esp
	
	RESTOREREGS
	iret
	
irq_12:
	SAVEREGS

	pushl $12
	call handle_IRQ
	addl $4, %esp
	
	RESTOREREGS
	iret
	
irq_13:
	SAVEREGS

	pushl $13
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret

irq_14:
	SAVEREGS

	pushl $14
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret

irq_15:
	SAVEREGS

	pushl $15
	call handle_IRQ
	addl $4, %esp

	RESTOREREGS
	iret
		
ret_from_interrupt:
	RESTOREREGS
	iret
.data
syscall:
	.long syscall_signal_return
	
	.long syscall_fork	/* 1 */
	.long syscall_spawn
	.long syscall_vfork
	.long syscall_exec
	.long syscall_exit
	.long syscall_getpid
	.long syscall_getppid
	.long syscall_waitpid
	.long syscall_get_user
	.long syscall_set_user
	
	
	.long syscall_create_thread	 /* 11 */
	.long syscall_destroy_thread
	.long syscall_gettid
	.long syscall_get_ustack
	.long syscall_thread_join
	
	.long syscall_sched_get		/* 16 */
	.long syscall_sched_set		
	.long syscall_sched_yield
	
	.long syscall_sig_mask		/* 19 */
	.long syscall_signal_action
	.long syscall_kill
	.long syscall_sigsuspend
	.long syscall_signal_stack
	.long syscall_signal_wait
	
	.long syscall_vm_allocate	/* 25 */
	.long syscall_vm_deallocate
	.long syscall_vm_map
	.long syscall_vm_lock
	.long syscall_vm_unlock
	.long syscall_vm_protect
	
	.long syscall_send_message	/* 31 */
	.long syscall_receive_message
	.long syscall_reply_message
	
	.long syscall_IRQ_request	/* 34 */
	.long syscall_IRQ_free
	.long syscall_IRQ_wait
	.long syscall_log		/* 37 */
	.long syscall_change_iopl	/* 38 */
		
	.long syscall_sync_create	/* 39 */
	.long syscall_sync_destroy
	.long syscall_sync_lock
	.long syscall_sync_unlock
	
	.long syscall_fd_alloc		/* 43 */
	.long syscall_fd_free
	
	.long syscall_sysinfo		/* 45 */
	.long syscall_getpidlist
	.long syscall_getpstat
	.long syscall_gettstat

	.long syscall_sleep_unint	/* 49 */
	.long syscall_wake_unint	
	
	
	.long syscall_nsleep		/* 51 */
	.long syscall_gettime
	.long syscall_settime
	.long syscall_clock
	.long syscall_getresolution
	.long syscall_timer_create
	.long syscall_timer_delete
	.long syscall_get_timer
	.long syscall_set_timer
	.long syscall_alarm		/* 60 */
	
	.long syscall_dup		/* 61 */
	.long syscall_get_cwd
	.long syscall_put_cwd 
