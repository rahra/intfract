/* Copyright 2025 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
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
 * This implements the 64 multiplication with a 128 bit result in x86_64 assembler. The result is shifted and again returned as a 64 bit variable. There is no way to do this directly in the C language without some workarounds.
 *
 * @author Bernhard R. Fischer
 * @version 2025/07/25
 */

#include "intfract.h"

   // set stack not executable
   .section .note.GNU-stack,"",@progbits


   .section .text
   .align 16

/* function prototype:
 * int sqr128shr(int a);
 * %rax           %rdi
 * This function returns (int64_t) (((int128_t) a * a) >> NORMBITS) where a is of type int64_t.
 */
   .global sqr128shr
sqr128shr:
   mov   %rdi,%rax
   imul  %rdi
   shrd  $NORM_BITS,%rdx,%rax
   ret


/* function prototype:
 * int imul128shr(int a, int b);
 * %rax           %rdi   %rsi
 * This function returns (int64_t) (((int128_t) a * b) >> NORMBITS - 1)  where a and b are of type int64_t.
 */
   .align 16
   .global imul128shr
imul128shr:
   mov   %rsi,%rax
   imul  %rdi
   shrd  $NORM_BITS - 1,%rdx,%rax
   ret

