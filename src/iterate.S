/* Copyright 2015-2024 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
 *
 * IntFract is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * IntFract is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IntFract. If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file iterate.S
 * This is a solution for the function iterate() written in x86_64 assembler.
 * The file contains two variants. First, define the cpp macro CONSERVATIVE to
 * activate a traditional solution using stack variables. The second solution
 * (undefine CONSERVATIVE) shows a faster register-only implementation.
 * This code was written for demonstrational purpose in lecture for Assembler
 * programming and reverse engineering.
 *
 * @author Bernhard R. Fischer
 * @version 2024/10/11
 */

#include "intfract.h"

   // set stack not executable
   .section .note.GNU-stack,"",@progbits

#ifdef ASM_ITERATE
#ifndef USE_DOUBLE

   .section .text
   .align 16

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
   movzx maxiterate_(%rip),%rcx

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
   mov   maxiterate_(%rip),%eax
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
#ifdef WITH_IMUL128
   push  %rbx
   mov   $(4 * NORM_FACT),%rbx
#else
   mov   $(4 * NORM_FACT),%rdx
#endif
   mov   %rdi,%r8             // real = real0
   mov   %rsi,%r9             // imag = imag0

   mov   maxiterate_(%rip),%ecx     // i = 64
   jmp   .Litloop
   .align 16
.Litloop:

#ifdef WITH_IMUL128
   mov   %r8,%rax
   imul  %r8
   shrd  $NORM_BITS,%rdx,%rax
   mov   %rax,%r10

   mov   %r9,%rax
   imul  %r9
   shrd  $NORM_BITS,%rdx,%rax
   mov   %rax,%r11
#else
   mov   %r8,%r10
   imul  %r10,%r10            // realq = real * real
   sar   $NORM_BITS,%r10      // realq >>= 13

   mov   %r9,%r11
   imul  %r11,%r11            // imagq = imag * imag
   sar   $NORM_BITS,%r11      // imagq >>= 13
#endif

   lea   (%r10,%r11),%rax     // realq + imagq
#ifdef WITH_IMUL128
   cmp   %rbx,%rax            // > 4 * NORM_FACT ?
#else
   cmp   %rdx,%rax            // > 4 * NORM_FACT ?
#endif
   jg    .Litbrk

#ifdef WITH_IMUL128
   mov   %r9,%rax
   imul  %r8
   shrd  $(NORM_BITS - 1),%rdx,%rax
   mov   %rax,%r9
#else
   imul  %r8,%r9              // imag *= real
   sar   $(NORM_BITS - 1),%r9 // imag >>= NORM_BITS - 1
#endif
   add   %rsi,%r9             // imag += imag0

   sub   %r11,%r10            // realq - imagq
   lea   (%rdi,%r10),%r8
   
   dec   %ecx
   jne   .Litloop

.Litbrk:

/***** return value goes to EAX *****/
   mov   maxiterate_(%rip),%eax
   sub   %ecx,%eax

#ifdef WITH_IMUL128
   pop   %rbx
#endif
   ret

#endif // !CONSERVATIVE

#endif // !USE_DOUBLE

#endif // ASM_ITERATE

