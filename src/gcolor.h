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

/* \file gcolor.h
 * This file contains the declarations for gcolor.c
 *
 * \author Bernhard R. Fischer, <bf@abenteuerland.at>
 * \date 2026/01/24
 */

#ifndef GCOLOR_H
#define GCOLOR_H

#ifndef __ASSEMBLER__
extern int colset_;
#endif

int num_colsets(void);
int fract_gcolor(unsigned int);
int set_bgcolor(const char*);
void inv_colset(void);

#endif
