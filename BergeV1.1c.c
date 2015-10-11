#include <stdio.h>
#include <cairo.h>

#define MAXITERATE 40

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
         *((int*) pdata + x) = *image >= MAXITERATE ? 0 : *image * 6 << 9;

   cairo_surface_mark_dirty(sfc);
   cairo_surface_write_to_png(sfc, "berge.png");
   cairo_surface_destroy(sfc);
}


int main(int argc, char **argv)
{
   double bbox[] = {-2.0, -1.2, 0.7, 1.2};   // realmin, imagmin, realmax, imagmax
   int width = 800, height = 600;            // pixel resolution
   int image[width * height];                // raw pixel data

   mand_calc(image,
         bbox[0] * NORM_FACT, bbox[1] * NORM_FACT, bbox[2] * NORM_FACT, bbox[3] * NORM_FACT,
         width, height);

   cairo_save_image(image, width, height);

   return 0;
}

