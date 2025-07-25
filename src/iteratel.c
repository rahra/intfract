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

/* \file iteratel.c
 * This file containes the iterate() function in C with integer math.
 *
 * @author Bernhard R. Fischer
 * @version 2025/07/25
 */

#include "intfract.h"


#if !defined(USE_DOUBLE) && !defined(ASM_ITERATE)
/*! This function contains the iteration loop using integer arithmetics.
 * @param real0 Real coordinate of pixel within the complex plane.
 * @param imag0 Imaginary coordinate of the pixel.
 * @return Returns the number of iterations to reach the break condition.
 */
int iterate(nint_t real0, nint_t imag0)
{
   nint_t realq, imagq, real, imag;
   int i;

   real = real0;
   imag = imag0;
   for (i = 0; i < maxiterate_; i++)
   {
#ifdef WITH_IMUL128
#ifdef GCCMUL128
      realq = ((__int128_t) real * real) >> NORM_BITS;
      imagq = ((__int128_t) imag * imag) >> NORM_BITS;
#else
      realq = sqr128shr(real);
      imagq = sqr128shr(imag);
#endif
#else
     realq = (real * real) >> NORM_BITS;
     imagq = (imag * imag) >> NORM_BITS;
#endif

     if ((realq + imagq) > (nint_t) 4 * NORM_FACT)
        break;

#ifdef WITH_IMUL128
#ifdef GCCMUL128
      imag = (((__int128_t) real * imag) >> (NORM_BITS - 1)) + imag0;
#else
      imag = imul128shr(real, imag) + imag0;
#endif
#else
     imag = ((real * imag) >> (NORM_BITS - 1)) + imag0;
#endif
     real = realq - imagq + real0;
   }
   return i;
}
#endif

