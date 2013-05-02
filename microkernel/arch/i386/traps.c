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
 * File: traps.c
 *
 * We setup our traps here. This is totally architecture dependent.
 */

 #include <string.h>

#include <kernel/signal.h>
#include <kernel/scheduling.h>
#include <kernel/vm_param.h>
#include <kernel/kernel_stdio.h>

#include <asm/descriptors.h>
#include <asm/context.h>

/* Check if the DPL is kernel. */
#define IS_KERNEL(cs)		((cs) & 3 ? 0 : 1)


GATE	*idt = NULL;

extern void divide_error();
extern void debug();
extern void nmi();
extern void breakpoint();
extern void overflow();
extern void bound();
extern void invalid_opcode();
extern void device_not_available();
extern void double_fault();
extern void coprocessor_overrun();
extern void invalid_tss();
extern void segment_not_present();
extern void stack_fault();
extern void general_protection();
extern void page_fault();
extern void reserved();
extern void floating_point_error();
extern void alignment_check();
extern void machine_check();

extern void syscall_hand();


static const char *exceptions[] = {
				"divide_error",
				"debug",
				"NMI",
				"Breakpoint",
				"Overflow",
				"Bounds check",
				"Invalid opcode",
				"No coprocessor",
				"Double fault",
				"Coprocessor overrun",
				"Invalid TSS",
				"Segment not present",
				"Stack bounds",
				"General protection",
				"Page fault",
				"(reserved)",
				"Floating point error",
				"alignment_check",
				"machine_check",0
};

void setup_traps()
{
	int i;
	MM_REGS idtr;

	idt = (GATE *)alloc_physical_mem(1);

	set_trap_gate(0, &divide_error, ACS_DPL0);
	set_trap_gate(1, &debug, ACS_DPL0);
	set_trap_gate(2, &nmi, ACS_DPL0);
	set_trap_gate(3, &breakpoint, ACS_DPL3);
	set_trap_gate(4, &overflow, ACS_DPL3);
	set_trap_gate(5, &bound, ACS_DPL3);
	set_trap_gate(6, &invalid_opcode, ACS_DPL0);
	set_trap_gate(7, &device_not_available, ACS_DPL0);
	set_trap_gate(8, &double_fault, ACS_DPL0);
	set_trap_gate(9, &coprocessor_overrun, ACS_DPL0);
	set_trap_gate(10, &invalid_tss, ACS_DPL0);
	set_trap_gate(11, &segment_not_present, ACS_DPL0);
	set_trap_gate(12, &stack_fault, ACS_DPL0);
	set_trap_gate(13, &general_protection, ACS_DPL0);
	set_trap_gate(14, &page_fault, ACS_DPL0);
	set_trap_gate(15, &reserved, ACS_DPL0);	/* reserved by intel */
	set_trap_gate(16, &floating_point_error, ACS_DPL0);
	set_trap_gate(17, &alignment_check, ACS_DPL0);
	set_trap_gate(18, &machine_check, ACS_DPL0);
	for(i=19;i<32;i++)
		set_trap_gate(i, &reserved, ACS_DPL0);

	set_trap_gate(0x30, &syscall_hand, ACS_DPL3);

	idtr.limit = (sizeof(GATE) * 0x32)-1;
	idtr.base = (u32)(linear_to_virtual(idt));
	LIDT(&idtr);

}

/* For Application Processor */
#ifdef MANRIX_SMP
void ap_idt_init()
{
	MM_REGS idtr;
	idtr.limit = (sizeof(GATE) * 0x32)-1;
	idtr.base = (u32)(linear_to_virtual(idt));
	LIDT(&idtr);

}
#endif /* MANRIX_SMP */

void print_regs(struct stack_regs *r)
{
	if(IS_KERNEL(r->user_cs))
	{
		r->user_esp = r->esp;
		r->user_ss = KERNEL_DATA;
	}
	kprintf("\nCS: %X DS: %X ES: %X SS: %X\n", r->user_cs, r->ds, r->es, r->user_ss);
	kprintf("EIP: %X ESP: %X EBP: %X EDI: %X ESI: %X\n", r->user_eip, r->user_esp, r->ebp, r->edi, r->esi);
	kprintf("EAX: %X EBX: %X ECX: %X EDX: %X\n", r->eax, r->ebx, r->ecx, r->edx);
}

static inline void check_state(int num, int errorcode, struct stack_regs *regs)
{
	print_regs(regs);
//	if(!IS_KERNEL(regs->user_cs))
//		return ;

	kprintf("We received %s exception with errorcode %X in kernel mode.\n", exceptions[num], errorcode);
	panic("PANIC");
	syscall_exit(regs, 1);
}

/* handle divide by zero error */
void handle_div_error(int errorcode, struct stack_regs *r)
{
	struct siginfo info;

	check_state(0, errorcode, r);

	info.si_signo = SIGFPE;
	info.si_code = FPE_INTDIV;

	send_signal(current, SIGFPE, &info);
}

/*
 * handle debug
 * For now debug registers are not examined. we simply send
 * sigtrap with info/
 */
void handle_debug(int errorcode, struct stack_regs *r)
{
	struct siginfo info;

	info.si_signo = SIGTRAP;
	info.si_code = TRAP_BRKPT;
	info.si_addr = (void *)r->user_eip;

	send_signal(current, SIGTRAP, &info);
}

/*
 * Handle Non Maskable interrupts(NMI).
 * For now, nothing is done. Just give a message
 * and resume.
 */
void handle_nmi(int errorcode, struct stack_regs *regs)
{
	kprintf("We Received a NMI with code %X, Ignoring it.\n", errorcode);
	// FIX: Take safety measures
	print_regs(regs);
}

/* handle breakpoint trap*/
void handle_breakpoint(int errorcode, struct stack_regs *regs)
{
	check_state(3, errorcode, regs);
	kprintf(" Breakpoint Trap.\n");
	send_signal(current, SIGTRAP, NULL);
}

/* handle_overflow */
void handle_overflow(int errorcode, struct stack_regs *regs)
{
	check_state(4, errorcode, regs);
	kprintf(" OverFlow.\n");
	send_signal(current, SIGTRAP, NULL);
}

/* handle_bound */
void handle_bound(int errorcode, struct stack_regs *regs)
{
	check_state(5, errorcode, regs);
	kprintf(" Bound .\n");
	send_signal(current, SIGTRAP, NULL);
}

/* Exception with invalid opcode */
void handle_inv_opcode(int errorcode, struct stack_regs *regs)
{
	struct siginfo info;

	check_state(6, errorcode, regs);

	kprintf(" Invalid Opcode.\n");
	info.si_signo = SIGILL;
	info.si_code = ILL_ILLOPC;
	info.si_addr = (void *)regs->user_eip;

	send_signal(current, SIGILL, &info);
}

/* Device not available */
void handle_dev_not_available(int errorcode, struct stack_regs *regs)
{
	check_state(7, errorcode, regs);
	kprintf("Device not available \n");
	send_signal(current, SIGSEGV, NULL);
}

/* Double fault */
void handle_double_fault(int errorcode, struct stack_regs *regs)
{
	check_state(8, errorcode, regs);
	kprintf("Double fault \n");
	send_signal(current, SIGSEGV, NULL);
}

/*
 * handle coprocessor overrun
 * Recent processors does not generate this Exception
 * except 80386 with seperate 80387 co-processor.
 */
void handle_cop_overrun(int errorcode, struct stack_regs *regs)
{
	check_state(9, errorcode, regs);
	kprintf(" coprocessor overrun \n");
	send_signal(current, SIGFPE, NULL);
}

/*
 * handle invalid tss.
 */
void handle_inv_tss(int errorcode, struct stack_regs *regs)
{
	check_state(10, errorcode, regs);
	kprintf(" invalid tss\n");
	send_signal(current, SIGSEGV, NULL);
}

/* segment not present */
void handle_seg_not_present(int errorcode, struct stack_regs *regs)
{
	check_state(11, errorcode, regs);
	kprintf(" segment not present.\n");
	send_signal(current, SIGSEGV, NULL);
}

void handle_stack_fault(int errorcode, struct stack_regs *regs)
{
	check_state(12, errorcode, regs);
	kprintf(" Stack Fault \n");
	send_signal(current, SIGSEGV, NULL);
}

void handle_gp(int errorcode, struct stack_regs *regs)
{
	check_state(13, errorcode, regs);
	kprintf(" General Exception code %x \n", errorcode);
	print_regs(regs);
	send_signal(current, SIGSEGV, NULL);
}

void handle_reserved(int errorcode, struct stack_regs *regs)
{
	kprintf("We received a reserved exception. \n");
	print_regs(regs);
	syscall_exit(regs, 1);
}

/* Handle exceptions related to math. */
void handle_fpe(int errorcode, struct stack_regs *regs)
{
	check_state(16, errorcode, regs);
	kprintf(" FP Exception .\n");
	send_signal(current, SIGFPE, NULL);
}

void handle_alignment(int errorcode, struct stack_regs *regs)
{
	struct siginfo info;

	check_state(17, errorcode, regs);

	info.si_signo = SIGBUS;
	info.si_code = BUS_ADRALN;
	info.si_addr = (void *)regs->user_eip;
	kprintf(" Alignment Exception .\n");
	send_signal(current, SIGBUS, &info);
}

/* Handle machine check faults of the Pentium Processors. */
void handle_mcheck(int errorcode, struct stack_regs *regs)
{
	kprintf("We Got Machine check fault  with errorcode %X.\n", errorcode);
	kprintf("Ignoring but it can be fatal.");
	print_regs(regs);
}

