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

/* \file gcolor.c
 * This file contains the color gradient definitions and the function to
 * convert the iteration count returned by iterate() into a color value.
 *
 * \author Bernhard R. Fischer, <bf@abenteuerland.at>
 * \date 2026/01/24
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "intfract.h"
#include "gcolor.h"


//! default background color
static int bgcolor_ = 0;
static int asc_ = 1;


typedef struct col_gradient
{
   double r, g, b;
   double l;
} col_gradient_t;


static col_gradient_t g_r_[] =
{
   {0.0, 0.0, 0.0, 0.0},
   {1.0, 0.0, 0.0, 1.0}
};

static col_gradient_t g_gb_[] =
{
   {0.0, 0.0, 0.0, 0.00},
   {0.0, 1.0, 0.5, 0.25},
   {0.0, 1.0, 1.0, 0.50},
   {0.0, 0.5, 1.0, 0.75},
   {0.0, 1.0, 1.0, 1.00}
};

static col_gradient_t g_ry_[] =
{
   {0.0, 0.0, 0.0, 0.00},
   {1.0, 0.0, 0.0, 0.25},
   {1.0, 1.0, 0.0, 0.50},
   {1.0, 0.0, 0.0, 1.00},
};

static col_gradient_t g_gy_[] =
{
   {0.0, 0.0, 0.0, 0.00},
   {0.5, 1.0, 0.0, 0.25},
   {1.0, 1.0, 0.0, 0.50},
   {1.0, 0.5, 0.0, 0.75},
   {1.0, 1.0, 0.0, 1.00}
};

static col_gradient_t g_b_[] =
{
   {0.0, 0.0, 0.0, 0.0},
   {0.0, 0.0, 1.0, 1.0}
};

static col_gradient_t g_rb_[] =
{
   {0.0, 0.0, 0.0, 0.0},
   {1.0, 0.0, 0.0, 0.5},
   {0.0, 0.0, 1.0, 1.0}
};
static col_gradient_t g_rvg_[] =
{
   {0.0, 0.0, 0.0, 0.0},
   {0.961, 0.106, 0.129, 0.25},  // #f51b3d red
   {0.439, 0.016, 0.737, 0.5},   // #7004bc violett
   {0.627, 0.957, 0.408, 0.75},  // #a0f468 green
   {0.439, 0.016, 0.737, 1.0}    // #7004bc violett
};
static col_gradient_t g_rvg2_[] =
{
   {0.0, 0.0, 0.0, 0.0},
   {0.961, 0.106, 0.129, 0.2},  // #f51b3d red
   {0.439, 0.016, 0.737, 0.4},   // #7004bc violett
   {0.627, 0.957, 0.408, 0.5},  // #a0f468 green
   {0.439, 0.016, 0.737, 0.6},    // #7004bc violett
   {0.961, 0.106, 0.129, 1.0}  // #f51b3d red
};
static col_gradient_t g_bw_[] =
{
   {0.0, 0.0, 0.0, 0.0},
   {1.0, 1.0, 1.0, 1.0}
};

static col_gradient_t *g_list_[] = 
{
   g_r_,
   g_gb_,
   g_ry_,
   g_b_,
   g_rb_,
   g_rvg_,
   g_rvg2_,
   g_gy_,
   g_bw_,
   NULL
};


/*! Invert order of color set from ascending to descending.
 */
void inv_colset(void)
{
   asc_ = 0;
}


/*! Parse string and set background color.
 * @param cstr Color string in HTML format #aarrggbb where the transparency is
 * defined between 0x00 and 0x7f.
 * @return Returns parse color value, or -1 in case of error.
 */
int set_bgcolor(const char *cstr)
{
   int i;

   if (cstr == NULL || *cstr != '#')
      return -1;

   for (bgcolor_ = 0, cstr++, i = 0; *cstr != '\0' && i < 8; i++, cstr++)
   {
      if (!isxdigit(*cstr))
         return -1;
      bgcolor_ <<= 4;
      if (*cstr >= '0' && *cstr <= '9')
         bgcolor_ |= *cstr - '0';
      else if (*cstr >= 'A' && *cstr <= 'F')
         bgcolor_ |= *cstr - 'A' + 10;
      else if (*cstr >= 'a' && *cstr <= 'f')
         bgcolor_ |= *cstr - 'a' + 10;
   }

   bgcolor_ &= 0x7fffffff;
   return bgcolor_;
}


/*! Return number of available color sets.
 */
int num_colsets(void)
{
   int i;

   for (i = 0; g_list_[i] != NULL; i++);

   return i;
}


/*! Convert decimal color value to 8 bit integer.
 * @param v Decimal input value, 0.0 <= v <= 1.0.
 * @return Returns a value between 0 and 255.
 */
static int creg(double v)
{
   if (v < 0.0)
      return 0;
   else if (v > 1.0)
      return 255;

   return round(v * 255);
}


/*! Return a 32 bit color value corresponding to the iteration count itcnt
 * according to the chosen colorset.
 * @param itcnt Iteration count as return by iterate(). This is typically
 * 0 <= itcnt < maxiterate_.
 * @return Returns a 32 bit color value in the format 0x00rrggbb.
 */
int fract_gcolor(unsigned int itcnt)
{
   static int _g_cnt = -1;
   static int *_cg = NULL;

   // return black if itcnt is out of range
   if (itcnt >= maxiterate_ || itcnt < 0)
      return bgcolor_;

   // init color table at 1st function call
   if (_g_cnt < 0)
   {
      _g_cnt = num_colsets();

      // safety check
      if (!_g_cnt)
         goto fc_get;
      // safety check
      if (colset_ < 0 || colset_ >= _g_cnt)
      {
         _g_cnt = 0;
         goto fc_get;
      }

      if ((_cg = malloc(sizeof(*_cg) * maxiterate_)) == NULL)
         perror("malloc failed"), exit(1);

      // count number of colors in gradient list
      int c_cnt;
      for (c_cnt = 0; g_list_[colset_][c_cnt].l < 1.0; c_cnt++);

      // safety check
      if (!c_cnt || g_list_[colset_][c_cnt].l != 1.0)
      {
         _g_cnt = 0;
         goto fc_get;
      }

      col_gradient_t *cg[2];
      double r, g, b, d;
      int i, j, k, n;

      for (i = 0, j = 0; j < c_cnt; j++)
      {  
         cg[0] = &g_list_[colset_][j];
         cg[1] = &g_list_[colset_][j + 1];
 
         n = round((cg[1]->l - cg[0]->l) * maxiterate_);
         for (k = 0; k < n && i < maxiterate_; k++, i++)
         {
            d = (double) k / n;
            r = (cg[1]->r - cg[0]->r) * d + cg[0]->r;
            g = (cg[1]->g - cg[0]->g) * d + cg[0]->g;
            b = (cg[1]->b - cg[0]->b) * d + cg[0]->b;
            _cg[asc_ ? i : maxiterate_ - 1 - i] = creg(r) << 16 | creg(g) << 8 | creg(b);
         }
      }
   }

fc_get:
   if (!_g_cnt)
      // return black & white color
      return (itcnt & 1) * 0xffffff;
   
   return _cg[itcnt];
}

