/* --------------------------------------------------------------------------
 axPMCtrlUtil.h :

 24 May 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// includes -----------------------------------------------------------------
#include "axPM.h"

// definitions --------------------------------------------------------------

// prototypes ---------------------------------------------------------------

// global variables ---------------------------------------------------------

/* --------------------------------------------------------------------------
 Draw a 3D border within the rectangle pr.
- Parameters -------------------------------------------------------------
 HPS hps     : presentation space handle
 PRECTL pr   : (input) coordinates of the rectangle within which to a 3D
               border must be painted.
               (output) coordinates of the rectange enclosed within the
               3D border.
 LONG cpbrd  : border thickness in pixels. When clrthk is >= 0 the
               thickness is increased by one pixel.
 LONG clrtl  : top-left border color (usually white).
 LONG clrbr  : bottom-right border color (usually dark grey).
 LONG clrthk : color used to increase the border 3D effect (usually black).
               When this is not less than 0, a first 1 pixel border is
               painted using clrbr on the top-left and clrthk on the
               bottom-right. Then an ordinary cpbrd pixels thick border
               is painted inside the previous one.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
- Remarks ----------------------------------------------------------------
-------------------------------------------------------------------------- */
BOOL PmDraw3DBorder(HPS hps, PRECTL pr, LONG cpbrd,
                    LONG clrtl, LONG clrbr, LONG clrthk) {
   POINTL apt[2];

   if (clrthk >= 0)
   {
      if (!PmDraw3DBorder(hps, pr, 1, clrbr, clrthk, -1))
         return FALSE;
   }
   // the rectangle must not be negative (i.e. bottom-left must be left
   // than top-right
   if (cpbrd > (pr->xRight - pr->xLeft))
      cpbrd = pr->xRight - pr->xLeft;
   if (cpbrd > (pr->yTop - pr->yBottom))
      cpbrd = pr->yTop - pr->yBottom;
   if (cpbrd <= 0) return FALSE;
   // if requested draw the outer border
   while (cpbrd--) {
      // bordo sinistro e superiore
      GpiSetColor(hps, clrtl);
      PointSet(apt, pr->xLeft, pr->yBottom);
      GpiMove(hps, apt);
      PointSet(apt, pr->xLeft, pr->yTop);
      PointSet(apt + 1, pr->xRight, pr->yTop);
      GpiPolyLine(hps, 2, apt);
      // bordo destro e inferiore
      GpiSetColor(hps, clrbr);
      PointSet(apt, apt[1].x, apt[1].y);
      GpiMove(hps, apt);
      PointSet(apt, pr->xRight, pr->yBottom);
      PointSet(apt + 1, pr->xLeft + 1, pr->yBottom);
      GpiPolyLine(hps, 2, apt);
      RectInflate(pr, -1, -1);
   }
   return TRUE;
}


/* --------------------------------------------------------------------------
 Get the RGB value of the color used to paint a control.
 First check for presentation parameters.
 Then, if a valid system color index was provided as 'ldef' parameter,
 check for the system colors, otherwise consider 'ldef' as an RGB value.
- Parameters -------------------------------------------------------------
 HWND hwnd   : control handle
 ULONG ulid1 : presentation parameter ID
 ULONG ulid2 : presentation parameter INDEX
 LONG ldef   : default color as SYSCLR_* or RGB value
 BOOL bi     : optional inheritance flag -> TRUE inherit/FALSE don't inherit
- Return value -----------------------------------------------------------
 LONG : color as a 24 bit RGB value
-------------------------------------------------------------------------- */
LONG PmCtrlColor(HWND hwnd, ULONG ulid1, ULONG ulid2, LONG ldef, BOOL bi) {
   LONG lclr = 0;
   bi = bi? 0 : QPF_NOINHERIT;
   // first checks for presentation parameters
   if (WinQueryPresParam(hwnd, ulid1, ulid2, NULL, 4, (PVOID)&lclr,
                         bi | QPF_PURERGBCOLOR | QPF_ID2COLORINDEX))
      return lclr;
   // if ldef refers to a valid SYSCLR_* index gets the RGB value
   if ((ldef >= SYSCLR_SHADOWHILITEBGND) && (ldef <= SYSCLR_HELPHILITE))
      return WinQuerySysColor(HWND_DESKTOP, ldef, 0L);
   return ldef;
}


/* --------------------------------------------------------------------------
 Draw an arrowhead as a solid triangle.
 Note: as arrow tip coordinates are meant the coordinates of the leftmost/
 topmost pixel.
- Parameters -------------------------------------------------------------
 HPS hps     : presentation space handle
 LONG x      : x coordinate of the arrow tip
 LONG y      : y coordinate of the arrow tip
 LONG len    : arrowhead length
 LONG clr    : arrow foreground color.
 LONG clrdis : disabled arrow color.
 ULONG fl    : direction and tip thickness flags.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID PmDrawArrowHead(HPS hps, LONG x, LONG y, LONG len,
                     LONG clr, LONG clrdis, ULONG fl) {
   POINTL aptl[2];
   LONG dx0, dy0, dx1, dy1;
   aptl[0].x = x;
   aptl[0].y = y;
   switch (fl & ARWHEAD_WHERE)
   {
   case ARWHEAD_UP:
      aptl[1].x = x + (FALSE != (fl & ARWHEAD_THICK));
      aptl[1].y = y;
      dx0 = -1, dy0 = -1, dx1 = 1, dy1 = -1;
      break;
   case ARWHEAD_LEFT:
      aptl[1].x = x;
      aptl[1].y = y - (FALSE != (fl & ARWHEAD_THICK));
      dx0 = 1, dy0 = 1, dx1 = 1, dy1 = -1;
      break;
   case ARWHEAD_DOWN:
      aptl[1].x = x + (FALSE != (fl & ARWHEAD_THICK));
      aptl[1].y = y;
      dx0 = -1, dy0 = 1, dx1 = 1, dy1 = 1;
      break;
   case ARWHEAD_RIGHT:
      aptl[1].x = x;
      aptl[1].y = y - (FALSE != (fl & ARWHEAD_THICK));
      dx0 = -1, dy0 = +1, dx1 = -1, dy1 = -1;
      break;
   }
   GpiSetColor(hps, (fl & ARWHEAD_DISABLED) ? clrdis : clr);
   while (len--)
   {
      GpiMove(hps, aptl);
      GpiLine(hps, aptl + 1);
      aptl[0].x += dx0, aptl[0].y += dy0, aptl[1].x += dx1, aptl[1].y += dy1;
   }
}


/* --------------------------------------------------------------------------
 Joins two points with a horizontal line.
- Parameters -------------------------------------------------------------
 HPS hps    : presentation space handle.
 LONG start : x coordinate of line start.
 LONG end   : x coordinate of line end;
 LONG y     : y coordinate;
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL PmDrawHorizontalLine(HPS hps, LONG start, LONG end, LONG y) {
   POINTL pt = {start, y};
   if (GpiMove(hps, &pt))
   {
      pt.x = end;
      return (GPI_ERROR != GpiLine(hps, &pt));
   }
   return FALSE;
}

/* --------------------------------------------------------------------------
 Joins two points with a vertical line.
- Parameters -------------------------------------------------------------
 HPS hps    : presentation space handle.
 LONG start : y coordinate of line start.
 LONG end   : y coordinate of line end;
 LONG x     : x coordinate;
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL PmDrawVerticalLine(HPS hps, LONG start, LONG end, LONG x) {
   POINTL pt = {x, start};
   if (GpiMove(hps, &pt))
   {
      pt.y = end;
      return (GPI_ERROR != GpiLine(hps, &pt));
   }
   return FALSE;
}


