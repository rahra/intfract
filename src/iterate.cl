/* Copyright 2026 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
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

/* \file iterate.cl
 * This file contains the function iterate() written in OpenCL.
 *
 * @author Bernhard R. Fischer
 * @version 2026/09/02 (this version)
 */

#include "intfract.h"


/*! This function contains the iteration loop for OpenCL. Depending if
 * USE_DOUBLE is defined it is either compiled for floating point or integer
 * arithmatics.
 * @param real0 Pointer to list of real coordinates of all `hres` pixels within the complex plane.
 * @param imag0 Pointer to list of imaginary coordinate of all `vres` pixels.
 * @param maxiterate Maximum iteration count.
 * @param result Pointer to 2-dimensional array of the iteration count for each pixel. The dimension of the array must be hres * vres.
 */
__kernel void iterate(__global nint_t const *real0, __global nint_t const *imag0, int maxiterate, __global int *result)
{ 
   nint_t realq, imagq, real, imag;
   int i;

   int x = get_global_id(0);
   int y = get_global_id(1);

   real = real0[x];
   imag = imag0[y];
   
   for (i = 0; i < maxiterate; i++)
   {
#ifndef USE_DOUBLE
      real >>= NORM_BITS >> 1;
      imag >>= NORM_BITS >> 1;
#endif
      realq = real * real;
      imagq = imag * imag;

      if ((realq + imagq) > (nint_t) 4 * NORM_FACT)
         break;

#ifdef USE_DOUBLE
      imag = real * imag * 2 + imag0[y];
#else
      imag = ((real * imag) << 1) + imag0[y];
#endif
      real = realq - imagq + real0[x];
   }

   result[x + y * get_global_size(0)] = i;
}


/*! Calculate i-th coordinate in the fractal space.
 * @param c0 Coordinate for index 0 (i = 0).
 * @param dc Distance between coordinates.
 * @param c Buffer to receive result.
 */
__kernel void fract_coords(nint_t c0, nint_t dc, __global nint_t *c)
{
   int i = get_global_id(0);
   c[i] = c0 + dc * i / get_global_size(0);
}


// vim: ft=opencl
