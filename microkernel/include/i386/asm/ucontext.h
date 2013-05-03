/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajee5283@hotmail.com)
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
 * File:- ucontext.h
 *  The Architecture Specfic Context.
 */

#ifndef __ASM_UCONTEXT_H
#define __ASM_UCONTEXT_H

#include <kernel/signal.h>
#include <asm/context.h>


typedef struct stack_regs mcontext_t;

/*
 * The CPU context for Signal Handling.
 */
struct ucontext{
	struct ucontext *uc_link;    /* Pointer to the context that is resumed
	                        when this context returns. */
	sigset_t    uc_sigmask; /* The set of signals that are blocked when this
	                        context is active. */
	stack_t     uc_stack;   /* The stack used by this context. */
	struct stack_regs uc_mcontext; /* A machine-specific representation of the saved
                        context.*/
};

typedef struct ucontext ucontext_t;

#endif /* __ASM_UCONTEXT_H */

