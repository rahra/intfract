#include <stdio.h>
#include <cairo.h>

#define MAXITERATE 40

#define Move(x, y, z) cairo_move_to(x, y, z)
#define Draw(x, y, z) cairo_line_to(x, y, z); cairo_stroke(x)
#define WritePixel(x, y, z)

#define SUPER int
#define HYPER long
#define SHORT short
#define LONG long
#define FLOAT float
#define VOID void

#define F      8192
#define MFARB  0

struct Complex
{
  FLOAT Real, Imag;
};

struct MandCoord
{
  struct Complex mc_min;
  struct Complex mc_max;
};


int iterate(int real0, int imag0)
{
   long realq, imagq;
   int i, real, imag;

   real = real0;
   imag = imag0;
   for (i = 0; i < MAXITERATE; i++)
   {
     realq = (real * real) >> 13;
     imagq = (imag * imag) >> 13;
     if ((realq+imagq) > 32768) break;
     imag = ((real * imag) >> 12) + imag0;
     real = realq - imagq + real0;
   }
   return i;
}
 

VOID get_3d_apfel(cairo_surface_t *awin, struct MandCoord *apf_coord, short HRES, short VRES)
{
  cairo_t *arp;
  LONG   deltareal, deltaimag,
         realmaxn, realminn,
         imagmaxn, imagminn,
         real0,  imag0;
  int h_kord, itschritt, v_kord;

  arp = cairo_create(awin);

  realminn  = (LONG) (apf_coord -> mc_min.Real * (FLOAT) F);
  realmaxn  = (LONG) (apf_coord -> mc_max.Real * (FLOAT) F);
  imagminn  = (LONG) (apf_coord -> mc_min.Imag * (FLOAT) F);
  imagmaxn  = (LONG) (apf_coord -> mc_max.Imag * (FLOAT) F);

  deltareal = (realmaxn-realminn) / (LONG) HRES;
  deltaimag = (imagmaxn-imagminn) / (LONG) VRES;

  printf("(%ld/%ld), (%ld/%ld)\n", realminn, imagminn, realmaxn, imagmaxn);

  real0 = realminn;
  for (h_kord = 0; h_kord < HRES; h_kord ++)
  {
    imag0 = imagmaxn;
    for (v_kord = 0; v_kord < VRES; v_kord ++)
    {
      itschritt = iterate(real0, imag0);

      cairo_move_to(arp, h_kord, v_kord);
      cairo_line_to(arp, h_kord+1, v_kord);
      if (itschritt >= MAXITERATE)
         cairo_set_source_rgb(arp, 0, 0, 0);
      else
         cairo_set_source_rgb(arp, (double) itschritt / MAXITERATE, 0, 0);
      cairo_stroke(arp);
      
      imag0 -= deltaimag;
    }
    real0 += deltareal;
  }

  cairo_destroy(arp);
}


int main(int argc, char **argv)
{
   struct MandCoord myco = {{-2.5, -1.5}, {1, 1.5}};
   //struct MandCoord myco = {{0.435396403, 0.367981352}, {0.451687191, 0.380210061}};
   int width = 400, height = 300;
   cairo_surface_t *sfc;

   sfc = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
   get_3d_apfel(sfc, &myco, width, height);

   cairo_surface_write_to_png(sfc, "berge.png");
   cairo_surface_destroy(sfc);

   return 0;
}

