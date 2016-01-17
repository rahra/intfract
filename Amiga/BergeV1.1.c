/*! This program calculates the well-known Mandelbrot image. This is the
 * original code written on an Amiga 500 somewhere around 1990.
 * @author Bernhard R. Fischer
 */
#include <exec/types.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>

#define SUPER  register int
#define HYPER  register long

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

struct IntuitionBase *IntuitionBase;
struct GfxBase       *GfxBase;
struct Screen        *scr;
struct Window        *win;
struct MandCoord      myco;

struct NewScreen ns =
{
  0, 0, 320, 256, 5, 0, 1,
  NULL, CUSTOMSCREEN,
  NULL, NULL, NULL, NULL
};

struct NewWindow nw =
{
  0, 0, 320, 256, 0, 1,
  MOUSEBUTTONS, ACTIVATE |RMBTRAP,
  NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0,
  CUSTOMSCREEN
};


VOID get_3d_apfel(awin, apf_coord, HOEHEN, ITMAX, FARBEN, HRES, VRES)
struct Window    *awin;
struct MandCoord *apf_coord;
SHORT  HOEHEN, ITMAX, FARBEN, HRES, VRES;
{
  struct IntuiMessage *msg;
  struct RastPort     *arp;
  LONG   deltareal, deltaimag,
         realmaxn, realminn,
         imagmaxn, imagminn,
         real0,  imag0;
  SHORT  h_kord,
         farbe, u = 0,
         x_k, y_k;
  SUPER  itschritt, real, imag, v_kord;
  HYPER  realq, imagq;
  FLOAT  mulf;

  arp = awin -> RPort;
  mulf = (FLOAT) FARBEN / (FLOAT) HOEHEN;
  realminn  = (LONG) (apf_coord -> mc_min.Real * (FLOAT) F);
  realmaxn  = (LONG) (apf_coord -> mc_max.Real * (FLOAT) F);
  imagminn  = (LONG) (apf_coord -> mc_min.Imag * (FLOAT) F);
  imagmaxn  = (LONG) (apf_coord -> mc_max.Imag * (FLOAT) F);
  deltareal = (realmaxn-realminn) / (LONG) HRES;
  deltaimag = (imagmaxn-imagminn) / (LONG) VRES;
  real0 = realminn;
  for (h_kord = 0; h_kord < HRES; h_kord ++)
  {
    imag0 = imagmaxn;
    if (msg = (struct IntuiMessage*) GetMsg(awin -> UserPort))
    {
      ReplyMsg(msg);
      return();
    }
    u = 0;
    for (v_kord = 0; v_kord < VRES; v_kord ++)
    {
      real = real0;
      imag = imag0;
      for (itschritt = 0; itschritt < ITMAX; itschritt++)
      {
        realq = (real * real) >> 13;
        imagq = (imag * imag) >> 13;
        if ((realq+imagq) > 32768) break;
        imag = ((real * imag) >> 12) + imag0;
        real = realq - imagq + real0;
      }
      if (itschritt >= ITMAX) farbe = HOEHEN;
      else                    farbe = (itschritt % HOEHEN);
      x_k = h_kord - u + VRES;
      y_k = v_kord + HOEHEN;
      SetAPen(arp, 1);
      Move(arp, x_k, y_k);
      Draw(arp, x_k, y_k - farbe);
      SetAPen(arp, 3);
      Move(arp, x_k + 1, y_k);
      Draw(arp, x_k + 1, y_k - farbe);
      if (itschritt >= ITMAX) SetAPen(arp, 4);
      else SetAPen(arp, 31 - (SHORT) ((FLOAT) farbe * mulf));
      WritePixel(arp, x_k, y_k - farbe);
      imag0 -= deltaimag;
      u++;
    }
    real0 += deltareal;
  }
}


VOID main()
{
  IntuitionBase = (struct IntuitionBase*) OpenLibrary("intuition.library", 0L);
  GfxBase       = (struct GfxBase*)       OpenLibrary("graphics.library", 0L);
  printf("rmin,rmax,imin,imax : ");
  scanf("%f,%f,%f,%f", &myco.mc_min.Real, &myco.mc_max.Real,
                       &myco.mc_min.Imag, &myco.mc_max.Imag);
  if (!(scr     = (struct Screen*)        OpenScreen(&ns))) exit(10);
  nw.Screen     = scr;
  if (!(win     = (struct Window*)        OpenWindow(&nw))) exit(10);
  get_3d_apfel(win, &myco, 40, 40, 32, 200, 120);
  CloseWindow(win);
  CloseScreen(scr);
  CloseLibrary(GfxBase);
  CloseLibrary(IntuitionBase);
}
