/* Copyright 2015-2025 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
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
 * @version 2025/07/28
 */

#include "intfract.h"

   // set stack not executable
   .section .note.GNU-stack,"",@progbits

#if defined(ASM_ITERATE) && !defined(USE_DOUBLE)

   .section .text
   .align 16

/* function prototype:
 * int iterate(int real0, int imag0);
 * %eax            %rdi       %rsi
 */
   .global iterate
iterate:

   /***** function body *****/
   mov   $(4 * NORM_FACT),%rdx
   mov   %rdi,%r8             // real = real0
   mov   %rsi,%r9             // imag = imag0

   mov   maxiterate_(%rip),%ecx     // i = 64
   jmp   .Litloop
   .align 16
.Litloop:

   sar   $(NORM_BITS/2),%r8   // real >>= NORM_BITS/2
   mov   %r8,%r10
   imul  %r10,%r10            // realq = real * real

   sar   $(NORM_BITS/2),%r9   // imag >>= NORM_BITS/2
   mov   %r9,%r11
   imul  %r11,%r11            // imagq = imag * imag

   lea   (%r10,%r11),%rax     // realq + imagq
   cmp   %rdx,%rax            // > 4 * NORM_FACT ?
   jg    .Litbrk

   imul  %r8,%r9              // imag *= real
   sal   $1,%r9               // imag <<= 1
   add   %rsi,%r9             // imag += imag0

   sub   %r11,%r10            // %r10 = realq - imagq
   lea   (%rdi,%r10),%r8      // real = real0 + %r10

   dec   %ecx                 // i--
   jne   .Litloop

.Litbrk:

/***** return value goes to EAX *****/
   mov   maxiterate_(%rip),%eax
   sub   %ecx,%eax

   ret

#endif /* defined(ASM_ITERATE) && !defined(USE_DOUBLE) */

