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

/*! \file imul128.S
 *
 * \author Bernhard R. Fischer
 * \version 2024/10/11
 */

#include "intfract.h"

   // set stack not executable
   .section .note.GNU-stack,"",@progbits

#ifdef WITH_IMUL128
   .section .text
   .align 16

/* function prototype:
 * long imul128(long a, long b);
 * %rax         %rdi    %rsi
 */
   .global imul128

imul128:
   mov %rdi,%rax
   imul %rsi
   shrd $NORM_BITS-1,%rdx,%rax
   //sar $NORM_BITS,%rax
   ret

   .align 16
/* function prototype:
 * long isq128(long a);
 * %rax        %rdi
 */
   .global isq128

isq128:
   mov %rdi,%rax
   imul %rdi
   shrd $NORM_BITS,%rdx,%rax
   //sar $NORM_BITS,%rax
   ret

#endif

