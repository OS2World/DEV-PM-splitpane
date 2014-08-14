/* --------------------------------------------------------------------------
 setSplitter.c :

 23 May 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------

// prototypes ---------------------------------------------------------------
BOOL onDlgInit(HWND hwnd);
VOID onCommand(HWND hwnd, ULONG id);

// global variables ---------------------------------------------------------

// entry point --------------------------------------------------------------
/* --------------------------------------------------------------------------
 Splitter settings dialog procedure.
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT
-------------------------------------------------------------------------- */
MRESULT EXPENTRY setSplitter(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg)
   {
      case WM_INITDLG:
         return (MRESULT)onDlgInit(hwnd);
      case WM_ACTIVATE:
         if (mp1) onDlgInit(hwnd);
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
      case WM_CLOSE:
         mp1 = (MPARAM)DID_CANCEL;
      case WM_COMMAND: // unhandled commands fall through the def dlg proc.
         onCommand(hwnd, (ULONG)mp1);
         break;
      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   }
   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Dialog initialization
- Parameters -------------------------------------------------------------
 HWND hwnd   : window handle.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (focus window was/wasn't changed);
-------------------------------------------------------------------------- */
static
BOOL onDlgInit(HWND hwnd) {
   ULONG style;

   style = wSplitStyle(g.hwndSplitPane);
   WinCheckButton(hwnd,
                  RDBTN_DRAGSYS + ((style & SPLITS_DRAGFLAGS) >> 12), 1);
   WinCheckButton(hwnd, RDBTN_PIXELS, 1);
   dSpinBtnSetLimits(hwnd, SPIN_THICKNESS, 3, 255);
   dSpinBtnSetValue(hwnd, SPIN_THICKNESS,
                    wSplitterThickness(g.hwndSplitPane));
   dSpinBtnSetLimits(hwnd, SPIN_POS, -32768, 2000);
   dSpinBtnSetValue(hwnd, SPIN_POS,
                    wSplitterPos(g.hwndSplitPane, FALSE));

   return TRUE;
}


/* --------------------------------------------------------------------------
 Command events handler.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle
 ULONG id  : button id
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID onCommand(HWND hwnd, ULONG id) {
   ULONG style = 0;
   POINTL pt = { 4, 8 };
   LONG thickness, pos;

   if (id == BTN_APPLY)
   {
      // read the current settings
      switch(dBtnCheckIndex(hwnd, RDBTN_DRAGSYS))
      {
      case 2: style |= SPLITS_DRAGSOLID; break;
      case 3: style |= SPLITS_DRAGFRAME; break;
      }
      switch(dBtnCheckIndex(hwnd, RDBTN_PIXELS))
      {
      case 1:
         pt.x = pt.y = 1;
         break;
      case 2:
         WinMapDlgPoints(hwnd, &pt, 1, TRUE);
         pt.x >>= 2, pt.y >>= 3;
         break;
      case 3:
         pt.x = pt.y = -1;
         break;
      }
      WinSendDlgItemMsg(hwnd, SPIN_THICKNESS, SPBM_QUERYVALUE,
                        (MPARAM)&thickness, MPVOID);
      WinSendDlgItemMsg(hwnd, SPIN_POS, SPBM_QUERYVALUE,
                        (MPARAM)&pos, MPVOID);
      // update the test application
      wSplitSetStyle(g.hwndSplitPane, style, SPLITS_DRAGFLAGS);
      wSplitterSetThickness(g.hwndSplitPane, thickness);
      wSplitterSetPos(g.hwndSplitPane, FALSE, pos * pt.y);
   }
   else
      WinPostMsg(WinOwner(hwnd), WM_USER, (MPARAM)hwnd, MPVOID);
}
