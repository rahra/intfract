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

/* This program calculates fractal images. It shows how to use integer
 * arithmetics instead of floating point operations.
 *
 * I wrote this program originally at about 1990 for the Amiga 500 in C and
 * also wrote an assembler version which contain some additional speed
 * improvements.
 *
 * @author Bernhard R. Fischer
 * @version 2015/10/11 (this version)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>

// maximum number of iterations of the inner loop
#define MAXITERATE 64

// Define to use double (floating point operations), otherwise integer
// arithmetics is used.
//#define USE_DOUBLE
#ifdef USE_DOUBLE
#define NORM_FACT 1
typedef double nint_t;
#else
#define NORM_BITS 13
#define NORM_FACT ((nint_t)1 << NORM_BITS)
typedef long nint_t;
#endif


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
#else
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
     realq = (real * real) >> NORM_BITS;
     imagq = (imag * imag) >> NORM_BITS;

     if ((realq + imagq) > (nint_t) 4 * NORM_FACT)
        break;

     imag = ((real * imag) >> (NORM_BITS - 1)) + imag0;
     real = realq - imagq + real0;
   }
   return i;
}
#endif


/*! This function contains the outer loop, i.e. calculate the coordinates
 * within the complex plane for each pixel and then call iterate().
 * @param image Pointer to image array of size hres * vres elements.
 * @param realmin Minimun real value of image.
 * @param imagmin Minimum imaginary value of image.
 * @param realmax Maximum real value.
 * @param imagmax Maximum imaginary value.
 * @param hres Pixel width of image.
 * @param vres Pixel height of image.
 */
void mand_calc(int *image, nint_t realmin, nint_t imagmin, nint_t realmax, nint_t imagmax, int hres, int vres)
{
  nint_t deltareal, deltaimag, real0,  imag0;
  int x, y;

  deltareal = (realmax - realmin) / hres;
  deltaimag = (imagmax - imagmin) / vres;

  real0 = realmin;
  for (x = 0; x < hres; x++)
  {
    imag0 = imagmax;
    for (y = 0; y < vres; y++)
    {
      *(image + x + hres * y) = iterate(real0, imag0);
      imag0 -= deltaimag;
    }
    real0 += deltareal;
  }
}


/*! Translate iteration count into an RGB color value.
 * @param itcnt Number of iterations.
 * @return Returns an RGB color value. If itcnt is greater or equal to
 * MAXITERATE, 0 is returned (which is black).
 */
int fract_color(unsigned int itcnt)
{
   // red color set
   return itcnt >= MAXITERATE ? 0 : itcnt * (256 / MAXITERATE) << 17;
   // green and blue color set
   //return itcnt >= MAXITERATE ? 0 : (itcnt * (256 / MAXITERATE) << 1) | ((256 + itcnt * (256 / MAXITERATE)) << 10);
   // read and yellow color set
   //return itcnt >= MAXITERATE ? 0 : (itcnt * (256 / MAXITERATE) << 17) | ((256 + itcnt * (256 / MAXITERATE)) << 10);
}


/*! Save raw pixel data to PNG file using libcairo.
 * @param image Pointer to image array of size hres * vres elements.
 * @param hres Pixel width of image.
 * @param vres Pixel height of image.
 */
void cairo_save_image(const int *image, int hres, int vres)
{
   cairo_surface_t *sfc;
   unsigned char *pdata;
   int x, y, stride;


   sfc = cairo_image_surface_create(CAIRO_FORMAT_RGB24, hres, vres);
   stride = cairo_image_surface_get_stride(sfc);
   cairo_surface_flush(sfc);
   pdata = cairo_image_surface_get_data(sfc);

   for (y = 0; y < vres; y++, pdata += stride)
      for (x = 0; x < hres; x++, image++)
         // translate iteration counter into pixel color
         *((int*) pdata + x) = fract_color(*image);

   cairo_surface_mark_dirty(sfc);
   cairo_surface_write_to_png(sfc, "berge.png");
   cairo_surface_destroy(sfc);
}


int main(int argc, char **argv)
{
   double bbox[] = {-2.0, -1.2, 0.7, 1.2};   // realmin, imagmin, realmax, imagmax
   int width = 800, height = 600;            // pixel resolution
   int image[width * height];                // raw pixel data

   // parse command line arguments
   if (argc >= 2 && !strcmp(argv[1], "-h"))
      printf("usage: %s [realmin imagmin realmax imagmax]\n", argv[0]), exit(0);
   if (argc >= 5)
      for (int i = 0; i < 4; i++)
         bbox[i] = atof(argv[i + 1]);

   // call calculation of image
   mand_calc(image,
         bbox[0] * NORM_FACT, bbox[1] * NORM_FACT, bbox[2] * NORM_FACT, bbox[3] * NORM_FACT,
         width, height);

   // save image to disk
   cairo_save_image(image, width, height);

   return 0;
}

