#ifndef INTFRACT_H
#define INTFRACT_H
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

/* \file intfract.h
 * This file contains some definitions.
 */

#include "config.h"

#ifdef WITH_THREADS
#ifndef __ASSEMBLER__
#include <pthread.h>
#endif
#define MAX_THREADS 32
#define NUM_THREADS 4
#else
#define NUM_THREADS 1
#endif

#if defined(ASM_ITERATE) && defined(USE_DOUBLE)
#error you cannot define ASM_ITERATE and USE_DOUBLE
#endif

// maximum number of iterations of the inner loop
#define MAXITERATE 64


#define IT8(x) ((x) * 255 / maxiterate_)

#ifdef USE_DOUBLE
#ifndef __ASSEMBLER__
typedef double nint_t;
#endif
#define NORM_FACT 1L
#define NORM_BITS 0
#else
#ifdef __ASSEMBLER__
#define NORM_FACT (1 << NORM_BITS)
#else
typedef long nint_t;
#define NORM_FACT ((nint_t)1 << NORM_BITS)
#endif
/*! The max number of bits b be depends on the machines word size n. This code
 * is written to work with a 64 bit word size (x86_64) and would need adaption
 * for different architectures.
 */
#ifdef WITH_IMUL128
//! In multiprecision imul (128 bit) 50 bit seems to be the maximum.
#define NORM_BITS 50
#else
/*! If single precision imul (64 bit) is used it will overflow earlier, thus a
 * lower normalization factor must be used. 29 seems to be the maximum.
 */
#define NORM_BITS 29
#endif
#endif

#ifndef __ASSEMBLER__
// prototype for iterate()
int iterate(nint_t real0, nint_t imag0);
extern int maxiterate_;
#endif

#endif

