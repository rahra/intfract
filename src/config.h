#ifndef CONFIG_H
#define CONFIG_H
/* Copyright 2024-2026 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
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

/* \file config.h
 * This file contains basic config macros.
 *
 * \author Bernhard R. Fischer, <bf@abenteuerland.at>
 * \date 2026/09/11
 */

//! Define to compile with thread support.
#define WITH_THREADS

//! Define to compile with assembler iterate() function.
#define ASM_ITERATE

//! Define to use double (floating point operations), otherwise integer arithmetics is used.
//#define USE_DOUBLE

//! Type to be used in the floating point variant (i.e. USE_DOUBLE is defined).
// The default type (if not defined) is double. This may define as float,
//double, or long double.
//#define FLOAT_TYPE float

//! Type to be used in the integer varian (i.e. USE_DOUBLE is not defined). The
// default type is long (if not defined). This may be defined as int or long.
// Please not that you should adjust NORM_BITS below as well.
//#define INT_TYPE int

//! Number of bits to be used for normalization for the integer variant.
// Basically a higher number gives a higher resolution if it is too high, the
// calculations might overflow giving incorrect results. And obiously, the
// number of bits must be less than the size of the type.
// The default value for long (64 bit wide) is 50 and a suitable value for int
// (32 bit wide) is 18.
// NORM_BITS must be an even number.
//#define NORM_BITS 18

//! Define to use OpenCL (GPU support)
//#define USE_OPENCL

#endif

