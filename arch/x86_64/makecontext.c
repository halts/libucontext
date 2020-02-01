/*
 * Copyright (c) 2018 William Pitcock <nenolod@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stddef.h>
#include <stdarg.h>
#ifdef CUSTOM_IMPL
#include <libucontext.h>
#include "defs.h"
#define UCONTEXT_T libucontext_ucontext_t
#define GREG_T libucontext_greg_t
#else
#define UCONTEXT_T ucontext_t
#define GREG_T greg_t
#include <signal.h>
#include <string.h>
#include <stdint.h>
#endif
#include "defs.h"


extern void _start_context(void);


void
libucontext_makecontext(UCONTEXT_T *ucp, void (*func)(void), int argc, ...)
{
	GREG_T *sp;
	va_list va;
	int i;
	unsigned int uc_link;

	uc_link = (argc > 6 ? argc - 6 : 0) + 1;

	sp = (GREG_T *) ((uintptr_t) ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size);
	sp -= uc_link;
	sp = (GREG_T *) (((uintptr_t) sp & -16L) - 8);

	ucp->uc_mcontext.gregs[REG_RIP] = (uintptr_t) func;
	ucp->uc_mcontext.gregs[REG_RBX] = (uintptr_t) &sp[uc_link];
	ucp->uc_mcontext.gregs[REG_RSP] = (uintptr_t) sp;

	sp[0] = (uintptr_t) &_start_context;
	sp[uc_link] = (uintptr_t) ucp->uc_link;

	va_start(va, argc);

	for (i = 0; i < argc; i++)
		switch (i)
		{
		case 0:
			ucp->uc_mcontext.gregs[REG_RDI] = va_arg (va, GREG_T);
			break;
		case 1:
			ucp->uc_mcontext.gregs[REG_RSI] = va_arg (va, GREG_T);
			break;
		case 2:
			ucp->uc_mcontext.gregs[REG_RDX] = va_arg (va, GREG_T);
			break;
		case 3:
			ucp->uc_mcontext.gregs[REG_RCX] = va_arg (va, GREG_T);
			break;
		case 4:
			ucp->uc_mcontext.gregs[REG_R8] = va_arg (va, GREG_T);
			break;
		case 5:
			ucp->uc_mcontext.gregs[REG_R9] = va_arg (va, GREG_T);
			break;
		default:
			sp[i - 5] = va_arg (va, GREG_T);
			break;
		}

	va_end(va);
}


#ifndef CUSTOM_IMPL
extern __typeof(libucontext_makecontext) makecontext __attribute__((weak, __alias__("libucontext_makecontext")));
#endif
