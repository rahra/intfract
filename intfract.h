#ifndef INTFRACT_H
#define INTFRACT_H
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

// define to compile with assembler iterate() function
#define ASM_ITERATE

// maximum number of iterations of the inner loop
#define MAXITERATE 64

// Define to use double (floating point operations), otherwise integer
// arithmetics is used.
//#define USE_DOUBLE

#ifdef USE_DOUBLE
#ifndef __ASSEMBLER__
typedef double nint_t;
#endif
#define NORM_FACT 1
#else
#ifdef __ASSEMBLER__
#define NORM_FACT (1 << NORM_BITS)
#else
typedef long nint_t;
// prototype for iterate()
int iterate(nint_t real0, nint_t imag0);
#define NORM_FACT ((nint_t)1 << NORM_BITS)
#endif
#define NORM_BITS 13
#endif

#endif

