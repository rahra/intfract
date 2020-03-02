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
 * Compile this program with
 * gcc -Wall -O2 -std=c99 `pkg-config --cflags --libs cairo` -o intfract intfract.c
 *
 * @author Bernhard R. Fischer
 * @version 2015/10/11 (this version)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>
#ifdef WITH_TIME
#include <sys/time.h>
#endif
#ifdef WITH_THREADS
#include <stdint.h>
#include <pthread.h>
#endif

#include "intfract.h"


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
void mand_calc(int *image, nint_t realmin, nint_t imagmin, nint_t realmax, nint_t imagmax, int hres, int vres, int start, int skip)
{
  nint_t deltareal, deltaimag, real0,  imag0;
  int x, y;

  deltareal = (realmax - realmin) / hres;
  deltaimag = (imagmax - imagmin) / vres;

  real0 = realmin + deltareal * start;
  for (x = start; x < hres; x += skip)
  {
    imag0 = imagmax;
    for (y = 0; y < vres; y++)
    {
      *(image + x + hres * y) = iterate(real0, imag0);
      imag0 -= deltaimag;
    }
    real0 += deltareal * skip;
  }
}


#ifdef WITH_THREADS
static int *image_;
static nint_t realmin_, imagmin_, realmax_, imagmax_;
static int hres_, vres_;
static int nthreads_ = NUM_THREADS;


void *mand_thread(void *n)
{
   mand_calc(image_, realmin_, imagmin_, realmax_, imagmax_, hres_, vres_, (intptr_t) n, nthreads_);
   return NULL;
}
#endif


/*! Translate iteration count into an RGB color value.
 * @param itcnt Number of iterations.
 * @return Returns an RGB color value. If itcnt is greater or equal to
 * MAXITERATE, 0 is returned (which is black).
 */
int fract_color(unsigned int itcnt)
{
   // red color set
   //return itcnt >= MAXITERATE ? 0 : IT8(itcnt) << 17;
   // green and blue color set
   //return itcnt >= MAXITERATE ? 0 : (IT8(itcnt) << 1) | ((256 + IT8(itcnt)) << 10);
   // read and yellow color set
   return itcnt >= MAXITERATE ? 0 : (IT8(itcnt) << 17) | ((256 + IT8(itcnt)) << 10);
   // black white color set
   //return (itcnt & 1) * 0xffffff;
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
   cairo_surface_write_to_png(sfc, "intfract.png");
   cairo_surface_destroy(sfc);
}


int main(int argc, char **argv)
{
   double bbox[] = {-2.0, -1.2, 0.7, 1.2};   // realmin, imagmin, realmax, imagmax
   int width = 600, height = 400;            // pixel resolution
   int *image;                               // raw pixel data
#ifdef WITH_THREADS
   pthread_t fdt[MAX_THREADS];
   int i;
#endif

   // parse command line arguments
   if (argc >= 2 && !strcmp(argv[1], "-h"))
      printf("usage: %s [realmin imagmin realmax imagmax]\n", argv[0]), exit(0);
   if (argc >= 5)
      for (int i = 0; i < 4; i++)
         bbox[i] = atof(argv[i + 1]);

   if ((image = malloc(width * height * sizeof(*image))) == NULL)
   {
      perror("malloc()");
      return 1;
   }

#ifdef WITH_TIME
   struct timeval tv0, tv1, tv;
   gettimeofday(&tv0, NULL);
#endif

#ifdef WITH_THREADS
   hres_ = width;
   vres_ = height;
   realmin_ = bbox[0] * NORM_FACT;
   imagmin_ = bbox[1] * NORM_FACT;
   realmax_ = bbox[2] * NORM_FACT;
   imagmax_ = bbox[3] * NORM_FACT;
   image_ = image;

   for (i = 0; i < nthreads_; i++)
      pthread_create(&fdt[i], NULL, mand_thread, (void*) (intptr_t) i);
#else
   // call calculation of image
   mand_calc(image,
         bbox[0] * NORM_FACT, bbox[1] * NORM_FACT, bbox[2] * NORM_FACT, bbox[3] * NORM_FACT,
         width, height, 0, 1);
#endif

#ifdef WITH_TIME
   gettimeofday(&tv1, NULL);
   timersub(&tv1, &tv0, &tv);
   printf("%ld.%06ld\n", tv.tv_sec, tv.tv_usec);
#endif

#ifdef WITH_THREADS
   for (i = 0; i < nthreads_; i++)
      pthread_join(fdt[i], NULL);
#endif

   // save image to disk
   cairo_save_image(image, width, height);

   free(image);
   return 0;
}

