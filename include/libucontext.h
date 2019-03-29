#ifndef __LIBUCONTEXT_H
#define __LIBUCONTEXT_H

/* Structure describing a signal stack.  */
typedef struct
  {
    void *ss_sp;
    size_t ss_size;
    int ss_flags;
  } libucontext_stack_t;

typedef unsigned long int libucontext_sigset_t;

#ifdef __aarch64__
/* Context to describe whole processor state.  This only describes
   the core registers; coprocessor registers get saved elsewhere
   (e.g. in uc_regspace, or somewhere unspecified on the stack
   during non-RT signal handlers).  */
typedef struct
  {
    unsigned long long int fault_address;
    unsigned long long int regs[31];
    unsigned long long int sp;
    unsigned long long int pc;
    unsigned long long int pstate;
    /* This field contains extension records for additional processor
       state such as the FP/SIMD state.  It has to match the definition
       of the corresponding field in the sigcontext struct, see the
       arch/arm64/include/uapi/asm/sigcontext.h linux header for details.  */
    unsigned char __reserved[4096] __attribute__ ((__aligned__ (16)));
  } libucontext_mcontext_t;
#else
#error "mcontext not implemented!"
#endif

/* Userlevel context.  */
typedef struct libucontext_ucontext
  {
    unsigned long uc_flags;
    struct libucontext_ucontext *uc_link;
    libucontext_stack_t uc_stack;
    libucontext_sigset_t uc_sigmask;
    libucontext_mcontext_t uc_mcontext;
  } libucontext_ucontext_t;

/* Get user context and store it in variable pointed to by UCP.  */
int libucontext_getcontext (libucontext_ucontext_t *__ucp);

/* Set user context from information of variable pointed to by UCP.  */
int libucontext_setcontext (const libucontext_ucontext_t *__ucp);

/* Save current context in context variable pointed to by OUCP and set
   context from variable pointed to by UCP.  */
int libucontext_swapcontext (libucontext_ucontext_t *__restrict __oucp,
      const libucontext_ucontext_t *__restrict __ucp);

/* Manipulate user context UCP to continue with calling functions FUNC
   and the ARGC-1 parameters following ARGC when the context is used
   the next time in `setcontext' or `swapcontext'.

   We cannot say anything about the parameters FUNC takes; `void'
   is as good as any other choice.  */
void libucontext_makecontext (libucontext_ucontext_t *__ucp, void (*__func) (void),
       int __argc, ...);

#endif
