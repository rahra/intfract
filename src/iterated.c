/* Copyright 2015 Bernhard R. Fischer, 2048R/5C5FFD47 <bf@abenteuerland.at>
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

/* \file iterated.c
 * This file contains the function iterate() in C for the type double.
 *
 * @author Bernhard R. Fischer
 * @version 2015/10/11 (this version)
 */

#include "intfract.h"


#ifdef USE_DOUBLE
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
   for (i = 0; i < MAXITERATE; i++)
   {
     realq = real * real;
     imagq = imag * imag;

     if ((realq + imagq) > (nint_t) 4)
        break;

     imag = real * imag * 2 + imag0;
     real = realq - imagq + real0;
   }
   return i;
}
#endif

