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
 * \date 2025/07/28
 */

//! Define to compile with thread support.
#define WITH_THREADS

//! Define to compile with assembler iterate() function.
#define ASM_ITERATE

//! Define to use double (floating point operations), otherwise integer arithmetics is used.
//#define USE_DOUBLE

#endif

