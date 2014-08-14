/* --------------------------------------------------------------------------
 axGPI.h :

 30 May 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------
 - undefined RGB color values
 - RGB color structure
 - macros
-------------------------------------------------------------------------- */

#ifndef _AXGPI_H_
   #define _AXGPI_H_



/* ---------------------------------------------------------------------------
 undefined RGB color values
*/
#define RGB_DARKGRAY       0x808080
#define RGB_PALEGRAY       0xcccccc
#define RGB_DARKGREY       RGB_DARKGRAY
#define RGB_PALEGREY       RGB_PALEGRAY
#define RGB_DARKBLUE       0x000080
#define RGB_DARKGREEN      0x008000
#define RGB_DARKCYAN       0x008080
#define RGB_DARKRED        0x800000
#define RGB_DARKPINK       0x800080
#define RGB_DARKYELLOW     0x808000

/* ---------------------------------------------------------------------------
 RGB color structure
*/
#pragma pack(1)
typedef union {
   LONG l;
   struct {
      UCHAR blu, grn, red, x;
   } rgb;
} CLR, * PCLR;
#pragma pack()


/* ---------------------------------------------------------------------------
 macros
*/

/*
 This macro calculates the most visible (white or black) foreground color
 depending on the current background color (_clrBkgnd_).
 It takes into account the fact that the human eye is most sensible
 to the green color.
*/
#define PmClrVisibleFgnd(_clrBkgnd_)         \
   ((((((_clrBkgnd_) & 0xff) +               \
     ((((_clrBkgnd_) >> 8) & 0xff) << 2) +   \
     ((_clrBkgnd_) >> 16)) > 0x3b0)   ||     \
     (_clrBkgnd_ > 0xf00000)          ||     \
     ((_clrBkgnd_ & 0xff00) > 0xc000)) ?     \
      0: 0xffffff)


/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
#endif // #ifndef _AXGPI_H_