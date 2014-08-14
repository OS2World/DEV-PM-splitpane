/* --------------------------------------------------------------------------
 settings.c :

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
 Splitpane settings dialog procedure.
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT
-------------------------------------------------------------------------- */
MRESULT EXPENTRY setSplitpane(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
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
            ((style & SPLITS_HORIZONTAL) ? RDBTN_HORIZONTAL: RDBTN_VERTICAL),
            1);
   WinCheckButton(hwnd,
                  RDBTN_HIDENONE + ((style & SPLITS_HIDEPANES) >> 4), 1);
   WinCheckButton(hwnd,
                  RDBTN_RATEPANESIZE + ((style & SPLITS_SIZECHANGE) >> 8), 1);
   dSpinBtnSetLimits(hwnd, SPIN_LEFTWIDTH, 0, 2000);
   dSpinBtnSetValue(hwnd, SPIN_LEFTWIDTH,
                    wSplitMinSize(g.hwndSplitPane, SPLITMP_LEFT));
   dSpinBtnSetLimits(hwnd, SPIN_RIGHTWIDTH, 0, 2000);
   dSpinBtnSetValue(hwnd, SPIN_RIGHTWIDTH,
                    wSplitMinSize(g.hwndSplitPane, SPLITMP_RIGHT));
   dSpinBtnSetLimits(hwnd, SPIN_TOPHEIGHT, 0, 2000);
   dSpinBtnSetValue(hwnd, SPIN_TOPHEIGHT,
                    wSplitMinSize(g.hwndSplitPane, SPLITMP_TOP));
   dSpinBtnSetLimits(hwnd, SPIN_BOTTOMHEIGHT, 0, 2000);
   dSpinBtnSetValue(hwnd, SPIN_BOTTOMHEIGHT,
                    wSplitMinSize(g.hwndSplitPane, SPLITMP_BOTTOM));

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
   LONG left, right, bottom, top;

   if (id == BTN_APPLY)
   {
      // read the current settings
      if (2 == dBtnCheckIndex(hwnd, RDBTN_VERTICAL))
         style |= SPLITS_HORIZONTAL;
      switch (dBtnCheckIndex(hwnd, RDBTN_HIDENONE))
      {
      case 2: style |= SPLITS_HIDEPANE1; break;
      case 3: style |= SPLITS_HIDEPANE2; break;
      case 4: style |= SPLITS_HIDEPANE1 | SPLITS_HIDEPANE2; break;
      }
      switch (dBtnCheckIndex(hwnd, RDBTN_RATEPANESIZE))
      {
      case 2: style |= SPLITS_FIXED1; break;
      case 3: style |= SPLITS_FIXED2; break;
      }
      WinSendDlgItemMsg(hwnd, SPIN_LEFTWIDTH, SPBM_QUERYVALUE,
                        &left, MPVOID);
      WinSendDlgItemMsg(hwnd, SPIN_RIGHTWIDTH, SPBM_QUERYVALUE,
                        &right, MPVOID);
      WinSendDlgItemMsg(hwnd, SPIN_TOPHEIGHT, SPBM_QUERYVALUE,
                        &top, MPVOID);
      WinSendDlgItemMsg(hwnd, SPIN_BOTTOMHEIGHT, SPBM_QUERYVALUE,
                        &bottom, MPVOID);
      // update the test application
      // update the style
      wSplitSetStyle(g.hwndSplitPane, style,
                     SPLITS_HORIZONTAL | SPLITS_HIDEPANES | SPLITS_SIZECHANGE);
      // set minimum pane size
      wSplitSetMinSize(g.hwndSplitPane, SPLITMP_LEFT, left, FALSE);
      wSplitSetMinSize(g.hwndSplitPane, SPLITMP_RIGHT, right, FALSE);
      wSplitSetMinSize(g.hwndSplitPane, SPLITMP_TOP, top, FALSE);
      wSplitSetMinSize(g.hwndSplitPane, SPLITMP_BOTTOM, bottom, FALSE);
   }
   else
      WinPostMsg(WinOwner(hwnd), WM_USER, (MPARAM)hwnd, MPVOID);
}
