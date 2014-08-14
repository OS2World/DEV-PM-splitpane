/* --------------------------------------------------------------------------
 axPMCtrlUtil.h : common data structures and macro used to design
                  enhanced PM dialog controls

 24 May 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _AXPMCTRLUTIL_H_
   #define _AXPMCTRLUTIL_H_

/*
 All controls should typically reserve space for 2 pointers in the window words.
 The first pointer space is reserved for user data (QWL_USER).
 The remaining space is reserved for the address of the memory
 used by the control in its procedure.
*/
#define PMC_CBWORDS            ((sizeof(PVOID)) << 1)

/*
 WinQuery/SetWindowULong value to get/set the address of the data used
 by the control procedure
*/
#define QWL_CONTROL            (sizeof(PVOID))


/*
 macro to access more comfortably the members of the CREATESTRUCT structure
 passed as the 2nd message parameter of WM_CREATE
*/
#define pCreateData(_mp2_) ((PCREATESTRUCT)(mp2))

/*
 PmDrawArrowHead() flags
*/
#define ARWHEAD_UP             0x0001
#define ARWHEAD_LEFT           0x0002
#define ARWHEAD_DOWN           0x0004
#define ARWHEAD_RIGHT          0x0008
#define ARWHEAD_WHERE  \
        (ARWHEAD_UP | ARWHEAD_LEFT | ARWHEAD_DOWN | ARWHEAD_RIGHT)

/*
 use this flag to make the arrow tip 2 pixels wide rather than 1 pixel
 in this case the x,y coordinates are relative to the left-top pixel
 of the 2 pixels in the arrow tip
*/
#define ARWHEAD_THICK         0x0010
// draw the arrow with the disabled color
#define ARWHEAD_DISABLED      0x0020   // disable arrow head


BOOL PmDraw3DBorder(HPS hps, PRECTL pr, LONG cpbrd,
                    LONG clrtl, LONG clrbr, LONG clrthk);
LONG PmCtrlColor(HWND hwnd, ULONG ulid1, ULONG ulid2, LONG ldef, BOOL bi);
VOID PmDrawArrowHead(HPS hps, LONG x, LONG y, LONG len,
                     LONG clr, LONG clrdis, ULONG fl);
BOOL PmDrawHorizontalLine(HPS hps, LONG start, LONG end, LONG y);
BOOL PmDrawVerticalLine(HPS hps, LONG start, LONG end, LONG x);


#endif // #ifndef _AXPMCTRLUTIL_H_