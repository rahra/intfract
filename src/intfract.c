/* Copyright 2015-2026 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
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

/* \file intfract.c
 * This program calculates fractal images. It shows how to use integer
 * arithmetics instead of floating point operations.
 *
 * I wrote this program originally at about 1990 for the Amiga 500 in C and
 * also wrote an assembler version which contain some additional speed
 * improvements.
 *
 * Compile this program with
 * gcc -Wall -O2 -std=c99 `pkg-config --cflags --libs cairo` -o intfract intfract.c
 *
 * @author Bernhard R. Fischer, <bf@abenteuerland.at>
 * @date 2026/09/11
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <cairo.h>
#ifdef WITH_TIME
#include <sys/time.h>
#endif
#ifdef WITH_THREADS
#include <stdint.h>
#include <pthread.h>
#endif

#include "intfract.h"
#include "gcolor.h"

#define WIDTH 1920
#define HEIGHT 1080


int maxiterate_ = MAXITERATE;
int colset_ = 0;
int wg_size_ = WG_SIZE;


#ifndef USE_OPENCL
/*! This function contains the outer loop, i.e. calculate the coordinates
 * within the complex plane for each pixel and then call iterate().
 * @param image Pointer to image array of size hres * vres elements.
 * @param realmin Minimun real value of image.
 * @param imagmin Minimum imaginary value of image.
 * @param realmax Maximum real value.
 * @param imagmax Maximum imaginary value.
 * @param hres Pixel width of image.
 * @param vres Pixel height of image.
 * @param start Column to start calculation with.
 * @param skip Number of columns to skip before starting with the next column.
 */
void mand_calc(int *image, nint_t realmin, nint_t imagmin, nint_t realmax, nint_t imagmax, int hres, int vres, int start, int skip)
{
   nint_t real0[hres],  imag0[vres];
   int x, y;

   // calculate coordinates in fractal space along real axis for each pixel column
   for (x = start; x < hres; x += skip)
      real0[x] = realmin + (realmax - realmin) / hres * x;
   // calculate coordinates in fractal space along imaginary axis for each pixel row
   for (y = 0; y < vres; y++)
      imag0[y] = imagmin + (imagmax - imagmin) / vres * y;

   // calculate inner loop (iterate()) for each pixel
   for (x = start; x < hres; x += skip)
      for (y = 0; y < vres; y++)
         *(image + x + hres * (vres - y - 1)) = iterate(real0[x], imag0[y]);
}
#endif


#ifdef USE_OPENCL
/*! This function contains the outer loop, i.e. calculate the coordinates
 * within the complex plane for each pixel and then call iterate().
 * @param image Pointer to image array of size hres * vres elements.
 * @param realmin Minimun real value of image.
 * @param imagmin Minimum imaginary value of image.
 * @param realmax Maximum real value.
 * @param imagmax Maximum imaginary value.
 * @param hres Pixel width of image.
 * @param vres Pixel height of image.
 * @param ifcl Pointer to initialized if_cl_t structure (defined in intfract.h).
 */
void mand_calc(int *image, nint_t realmin, nint_t imagmin, nint_t realmax, nint_t imagmax, int hres, int vres, if_cl_t *ifcl)
{
   size_t local_size[2] = {wg_size_, wg_size_}, global_size[2];
   cl_mem real0, imag0, img;
   cl_kernel kernel;
   int err;

   realmax -= realmin;
   imagmax -= imagmin;

   real0 = clCreateBuffer(ifcl->context, CL_MEM_READ_WRITE, sizeof(nint_t) * hres, NULL, NULL);
   imag0 = clCreateBuffer(ifcl->context, CL_MEM_READ_WRITE, sizeof(nint_t) * vres, NULL, NULL);

   // create kernel for fractal coordinate calculation
   if ((kernel = clCreateKernel(ifcl->program, "fract_coords", NULL)) == NULL)
      fprintf(stderr, "clCreateKernel() failed"), exit(1);

   // calculate fractal real coordinates
   if (clSetKernelArg(kernel, 0, sizeof(nint_t), &realmin) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);
   if (clSetKernelArg(kernel, 1, sizeof(nint_t), &realmax) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);
   if (clSetKernelArg(kernel, 2, sizeof(cl_mem), &real0) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);

   global_size[0] = hres;

   if ((err = clEnqueueNDRangeKernel(ifcl->queue, kernel, 1, NULL, global_size, local_size, 0, NULL, NULL)) != CL_SUCCESS)
      fprintf(stderr, "clEnqueueNDRangeKernel(real0) failed: %d\n", err), exit(1);

   // calculate fractal imaginary coordinates
   if (clSetKernelArg(kernel, 0, sizeof(nint_t), &imagmin) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);
   if (clSetKernelArg(kernel, 1, sizeof(nint_t), &imagmax) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);
   if (clSetKernelArg(kernel, 2, sizeof(cl_mem), &imag0) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);

   global_size[0] = vres;

   if ((err = clEnqueueNDRangeKernel(ifcl->queue, kernel, 1, NULL, global_size, local_size, 0, NULL, NULL)) != CL_SUCCESS)
      fprintf(stderr, "clEnqueueNDRangeKernel(imag0) failed: %d\n", err), exit(1);

   clReleaseKernel(kernel);

   // create kernel for inner iteration (the real workload)
   if ((kernel = clCreateKernel(ifcl->program, "iterate", NULL)) == NULL)
      fprintf(stderr, "clCreateKernel() failed"), exit(1);

   img = clCreateBuffer(ifcl->context, CL_MEM_WRITE_ONLY, hres * vres * sizeof(int), NULL, NULL);

   if (clSetKernelArg(kernel, 0, sizeof(cl_mem), &real0) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);
   if (clSetKernelArg(kernel, 1, sizeof(cl_mem), &imag0) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);
   if (clSetKernelArg(kernel, 2, sizeof(int), &maxiterate_) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);
   if (clSetKernelArg(kernel, 3, sizeof(cl_mem), &img) != CL_SUCCESS)
      fprintf(stderr, "clSetKernalArg() failed"), exit(1);

   global_size[0] = hres;
   global_size[1] = vres;

   if ((err = clEnqueueNDRangeKernel(ifcl->queue, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL)) != CL_SUCCESS)
      fprintf(stderr, "clEnqueueNDRangeKernel(iterate) failed: %d\n", err), exit(1);

   /* Wait for the command queue to get serviced before reading
   back results */
   clFinish(ifcl->queue);

   if (clEnqueueReadBuffer(ifcl->queue, img, CL_TRUE, 0, hres * vres * sizeof(int), image, 0, NULL, NULL) != CL_SUCCESS)
      fprintf(stderr, "clEnqueueReadBuffer() failed"), exit(1);


   /* Deallocate resources */
   clReleaseKernel(kernel);
   clReleaseMemObject(real0);
   clReleaseMemObject(imag0);
   clReleaseMemObject(img);
}
#endif


static int nthreads_ = NUM_THREADS;
#ifdef WITH_THREADS
static int *image_;
static nint_t realmin_, imagmin_, realmax_, imagmax_;
static int hres_, vres_;


void *mand_thread(void *n)
{
   mand_calc(image_, realmin_, imagmin_, realmax_, imagmax_, hres_, vres_, (intptr_t) n, nthreads_);
   return NULL;
}
#endif


/*! This function reads the number of CPUs from /proc/cpuinfo and returns it.
 * @return The function returns the number of processors found in
 * /proc/cpuinfo. If the file does not exist, -1 is returned. If the file
 * exists but no processors are found, 0 is returned.
 */
int get_ncpu(void)
{
   char buf[1024];
   FILE *cpuinfo;
   int n;

   if ((cpuinfo = fopen("/proc/cpuinfo", "r")) == NULL)
      return -1;

   for (n = 0; fgets(buf, sizeof(buf), cpuinfo) != NULL;)
      if (!strncmp(buf, "processor", 9))
         n++;

   fclose(cpuinfo);
   return n;
}


static cairo_status_t cairo_write(void *closure, const unsigned char *data, unsigned int length)
{
   return fwrite(data, length, 1, closure) ? CAIRO_STATUS_SUCCESS : CAIRO_STATUS_WRITE_ERROR;
}


/*! Save raw pixel data to PNG file using libcairo.
 * @param image Pointer to image array of size hres * vres elements.
 * @param hres Pixel width of image.
 * @param vres Pixel height of image.
 */
void cairo_save_image(const int *image, int hres, int vres, const char *s)
{
   cairo_surface_t *sfc;
   unsigned char *pdata;
   int x, y, stride;
   FILE *f;

   // safety check
   if (image == NULL || s == NULL)
   {
      fprintf(stderr, "this should never happen...\n");
      return;
   }

   // check for stdout
   if (!strcmp(s, "-"))
   {
      f = stdout;
   }
   else if ((f = fopen(s, "w")) == NULL)
   {
      fprintf(stderr, "failed to open file %s\n", s);
      return;
   }

   sfc = cairo_image_surface_create(CAIRO_FORMAT_RGB24, hres, vres);
   stride = cairo_image_surface_get_stride(sfc);
   cairo_surface_flush(sfc);
   pdata = cairo_image_surface_get_data(sfc);

   for (y = 0; y < vres; y++, pdata += stride)
      for (x = 0; x < hres; x++, image++)
         // translate iteration counter into pixel color
         *((int*) pdata + x) = fract_gcolor(*image);

   cairo_surface_mark_dirty(sfc);
   cairo_surface_write_to_png_stream(sfc, cairo_write, f);
   cairo_surface_destroy(sfc);

   fclose(f);
}


void usage(const char *s)
{
   printf("intfract v4.0 © 2015-2026 Bernhard R. Fischer, <bf@abenteuerland.at>\n"
         "usage: %s [options] [realmin(x0)] [imagmin(y0)] [realmax(x1)] [imagmax(y1)]\n"
         "    -b <color> ....... Set background color in HTML format (#aarrggbb). Default = #000000\n"
         "    -C ............... Coordinates are given as x/y and w/h instead of x0/y0 and x1/y1.\n"
         "    -c <colset> ...... Choose color set: 0 - %d\n"
         "    -h ............... Display this help screen.\n"
         "    -I ............... Invert color set.\n"
         "    -i <n> ........... Set maximum number of iterations (default = %d).\n"
         "    -n <threads> ..... Choose number of threads (default = %d).\n"
         "    -o <filename> .... Name of output PNG file, \"-\" for stdout.\n"
         "    -w <wg_size> ..... Define workgroup size of OpenCL device (default = %d).\n"
         "    -x <width> ....... Choose image width (default = %d).\n"
         "    -y <height> ...... Choose image height (default = %d).\n"
         , s, num_colsets(), MAXITERATE, nthreads_, WG_SIZE, WIDTH, HEIGHT);
   printf("\n    defs: sizeof(nint_t) = %ld, NORM_BITS = %d, NORM_FACT = %ld\n", sizeof(nint_t), NORM_BITS, (long) NORM_FACT);
#ifdef USE_OPENCL
   printf("    USE_OPENCL is defined\n");
#endif
#ifdef USE_DOUBLE
   printf("    USE_DOUBLE is defined\n");
#endif
#ifdef ASM_ITERATE
   printf("    ASM_ITERATE is defined\n");
#endif
#ifdef WITH_THREADS
   printf("    WITH_THREADS is defined\n");
#endif
#ifdef WITH_TIME
   printf("    WITH_TIME is defined\n");
#endif
}


int main(int argc, char **argv)
{
   double bbox[] = {-2.0, -1.2, 0.7, 1.2};   // realmin, imagmin, realmax, imagmax
   int width = WIDTH, height = HEIGHT;       // pixel resolution
   int *image;                               // raw pixel data
   int n;
   char *out = "intfract.png";
   int cc = 0;
#ifdef USE_OPENCL
   if_cl_t *ifcl = prepare_cl();
#endif
#ifdef WITH_THREADS
   pthread_t fdt[MAX_THREADS];
   int i;

   nthreads_ = get_ncpu();
   if (nthreads_ <= 0)
      nthreads_ = NUM_THREADS;
#endif
   while ((n = getopt(argc, argv, "b:Cc:hIi:n:o:w:x:y:")) != -1)
      switch (n)
      {
         case 'h':
            usage(argv[0]);
            exit(EXIT_SUCCESS);

         case 'b':
            if (set_bgcolor(optarg) < 0)
               fprintf(stderr, "*** illegal color code '%s'!\n", optarg), exit(1);
            break;

         case 'C':
            cc = 1;
            break;

         case 'c':
            colset_ = atoi(optarg);
            if (colset_ < 0 || colset_ > num_colsets())
               colset_ = 0;
            break;

         case 'I':
            inv_colset();
            break;

         case 'i':
            maxiterate_ = atoi(optarg);
            if (maxiterate_ <= 0)
               maxiterate_ = MAXITERATE;
            break;

         case 'n':
#ifdef WITH_THREADS
            int nthreads = atoi(optarg);
            if (nthreads >= 1 || nthreads_ <= MAX_THREADS)
               nthreads_ = nthreads;
#else
            fprintf(stderr, "thread support not compiled\n");
#endif
            break;

         case 'o':
            out = optarg;
            break;

         case 'w':
            wg_size_ = atoi(optarg);
            break;

         case 'x':
            width = atoi(optarg);
            if (width <= 0)
               width = WIDTH;
            break;

         case 'y':
            height = atoi(optarg);
            if (height <= 0)
               height = HEIGHT;
            break;
      }

   // parse remaining command line arguments
   for (int i = 0; optind < argc; i++, optind++)
         bbox[i] = atof(argv[optind]);

   // transform coordinates given as center and width/height
   if (cc)
   {
      double a = bbox[2] / 2;
      bbox[2] = bbox[0] + a;
      bbox[0] -= a;
      a = bbox[3] / 2;
      bbox[3] = bbox[1] + a;
      bbox[1] -= a;
   }

#ifdef USE_OPENCL
   // adjust image size to workgroup size
   width = ceil((double) width / wg_size_) * wg_size_;
   height = ceil((double) height / wg_size_) * wg_size_;
#endif

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
         width, height,
#ifdef USE_OPENCL
         ifcl
#else
         0, 1
#endif
         );

#endif

#ifdef WITH_THREADS
   for (i = 0; i < nthreads_; i++)
      pthread_join(fdt[i], NULL);
#endif

#ifdef WITH_TIME
   gettimeofday(&tv1, NULL);
   timersub(&tv1, &tv0, &tv);
   fprintf(stderr, "%ld.%06ld\n", tv.tv_sec, tv.tv_usec);
#endif

#ifdef USE_OPENCL
   release_cl(ifcl);
#endif

   // save image to disk
   cairo_save_image(image, width, height, out);

   free(image);
   return 0;
}

