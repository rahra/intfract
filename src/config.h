#ifndef CONFIG_H
#define CONFIG_H
/* Copyright 2024-2025 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
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
 * \date 2025/07/25
 */

//! Define to compile with thread support.
#define WITH_THREADS

//! Define to use a double precision multiplication implemented in assembler, this is the single-operand variant of imul. This uses slightly more instructions. It will also be used in the C variante of iterate() unless the following macro GCCMUL128 is defined.
#define WITH_IMUL128

//! Define this to use a pure but sub-optimal C implementation of the 128 bit multiplication.
//#define GCCMUL128

//! Define to compile with assembler iterate() function.
#define ASM_ITERATE

//! Define to use double (floating point operations), otherwise integer arithmetics is used.
//#define USE_DOUBLE

//! Define to use conservative stack-variable-solution. This is only effective if ASM_ITERATE is defined.
//#define CONSERVATIVE

//! Define to use instruction "enter" for function prolog. This is only effective if CONSERVATIVE is defined.
//#define WITH_ENTER

//! Define to use instruction "leave" for function epilog. This is only effective if CONSERVATIVE is defined.
//#define WITH_LEAVE

#endif

