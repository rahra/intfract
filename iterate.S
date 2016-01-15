/*! This is a solution for the function iterate() written in x86_64 assembler.
 * The file contains two variants. First, define the cpp macro CONSERVATIVE to
 * activate a traditional solution using stack variables. The second solution
 * (undefine CONSERVATIVE) shows a faster register-only implementation.
 * This code was written for demonstrational purpose in lecture for Assembler
 * programming and reverse engineering.
 *
 * @author Bernhard R. Fischer
 * @version 2016/01/14
 */

#include "intfract.h"

#ifdef ASM_ITERATE

// Define to use conservative stack-variable-solution.
//#define CONSERVATIVE
#ifdef CONSERVATIVE
// Define to use instruction "enter" for function prolog
//#define WITH_ENTER
// Define to use instruction "enter" for function epilog
//#define WITH_LEAVE
#endif


   .section .text
   .align 8

/* function prototype:
 * int iterate(int real0, int imag0);
 * %eax            %rdi       %rsi
 */
   .global iterate
iterate:

#ifdef CONSERVATIVE

// addresses of variables on stack (rel. to BP)
#define REAL -8
#define IMAG -16
#define REALQ -24
#define IMAGQ -32

/***** function prolog *****/
#ifdef WITH_ENTER
   enter $0,$32
#else
   push  %rbp
   mov   %rsp,%rbp
   sub   $32,%rsp
#endif

/***** save arguments on stack *****/
   mov   %rdi,REAL(%rbp)
   mov   %rsi,IMAG(%rbp)

/***** function body *****/
   mov   $MAXITERATE,%rcx

.Litloop:

   mov   REAL(%rbp),%rax
   imul  %rax,%rax            // realq = real * real
   sar   $NORM_BITS,%rax      // realq >> 13
   mov   %rax,REALQ(%rbp)

   mov   IMAG(%rbp),%rdx
   imul  %rdx,%rdx            // imagq = imag * imag
   sar   $NORM_BITS,%rdx      // imagq >> 13
   
   add   %rdx,%rax            // realq + imagq
   cmp   $32768,%rax
   jg    .Litbrk
   
   mov   REAL(%rbp),%rax
   imul  IMAG(%rbp),%rax      // real * imag
   sar   $(NORM_BITS - 1),%rax// >> 12
   add   %rsi,%rax            // + imag0
   mov   %rax,IMAG(%rbp)

   mov   REALQ(%rbp),%rax
   sub   %rdx,%rax            // realq * imagq
   add   %rdi,%rax            // + real0
   mov   %rax,REAL(%rbp)

   loop  .Litloop

.Litbrk:
/***** return value goes to EAX *****/
   mov   $MAXITERATE,%eax
   sub   %ecx,%eax

/***** function epilog *****/
#ifdef WITH_LEAVE
   leave
#else
#ifdef MANUAL_STACK_RESTORE
   add   $32,%rsp
   pop   %rbp
#else
   mov   %rbp,%rsp
   pop   %rbp
#endif
#endif

   ret

// ! defined(CONSERVATIVE)
#else

   /***** function body *****/
   mov   $(4 * NORM_FACT),%rdx
   mov   %rdi,%r8             // real = real0
   mov   %rsi,%r9             // imag = imag0

   mov   $MAXITERATE,%ecx     // i = 64

.Litloop:

   mov   %r8,%r10
   imul  %r10,%r10            // realq = real * real
   sar   $NORM_BITS,%r10      // realq >>= 13

   mov   %r9,%r11
   imul  %r11,%r11            // imagq = imag * imag
   sar   $NORM_BITS,%r11      // imagq >>= 13

   mov   %r11,%rax
   add   %r10,%rax            // realq + imagq
   cmp   %rdx,%rax            // > 4 * NORM_FACT ?
   jg    .Litbrk

   imul  %r8,%r9              // imag += real
   sar   $(NORM_BITS - 1),%r9 // imag >>= NORM_BITS - 1
   add   %rsi,%r9             // imag += imag0

   sub   %r11,%r10            // realq - imagq
   mov   %r10,%r8
   add   %rdi,%r8             // real += real0
   
   loop  .Litloop             // i--

.Litbrk:

/***** return value goes to EAX *****/
   mov   $MAXITERATE,%eax
   sub   %ecx,%eax

   ret

#endif

#endif // ASM_ITERATE

