#ifndef __LIBUCONTEXT_H
#define __LIBUCONTEXT_H

#include <stdint.h>

/* Structure describing a signal stack.  */
typedef struct
  {
    void *ss_sp;
    size_t ss_size;
    int ss_flags;
  } libucontext_stack_t;

typedef unsigned long int libucontext_sigset_t;

#if defined(__aarch64__)
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

/* Userlevel context.  */
typedef struct libucontext_ucontext
  {
    unsigned long uc_flags;
    struct libucontext_ucontext *uc_link;
    libucontext_stack_t uc_stack;
    libucontext_sigset_t uc_sigmask;
    libucontext_mcontext_t uc_mcontext;
  } libucontext_ucontext_t;
#elif defined(__x86_64__)
/* Type for general register.  */
typedef long long int libucontext_greg_t;

/* Container for all general registers.  */
typedef libucontext_greg_t libucontext_gregset_t[23];

struct libucontext_fpxreg
{
  unsigned short int significand[4];
  unsigned short int exponent;
  unsigned short int __glibc_reserved1[3];
};

struct libucontext_xmmreg
{
  uint32_t  element[4];
};

struct libucontext_fpstate
{
  /* 64-bit FXSAVE format.  */
  uint16_t    cwd;
  uint16_t    swd;
  uint16_t    ftw;
  uint16_t    fop;
  uint64_t    rip;
  uint64_t    rdp;
  uint32_t    mxcsr;
  uint32_t    mxcr_mask;
  struct libucontext_fpxreg _st[8];
  struct libucontext_xmmreg _xmm[16];
  uint32_t    __libucontext_reserved1[24];
};

/* Structure to describe FPU registers.  */
typedef struct libucontext_fpstate *libucontext_fpregset_t;

/* Context to describe whole processor state.  */
typedef struct libucontext_mcontext
  {
    libucontext_gregset_t gregs;
    /* Note that fpregs is a pointer.  */
    libucontext_fpregset_t fpregs;
    unsigned long long __reserved1 [8];
} libucontext_mcontext_t;

/* Userlevel context.  */
typedef struct libucontext_ucontext
  {
    unsigned long int uc_flags;
    struct libucontext_ucontext *uc_link;
    libucontext_stack_t uc_stack;
    libucontext_mcontext_t uc_mcontext;
    libucontext_sigset_t uc_sigmask;
    struct libucontext_fpstate __fpregs_mem;
    unsigned long long int __ssp[4];
  } libucontext_ucontext_t;
#else
#error "mcontext not implemented!"
#endif

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
