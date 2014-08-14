/* --------------------------------------------------------------------------
 setctldata.c :

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
VOID setEntry(HWND hwnd, ULONG id, INT value, BOOL hex);
LONG getEntry(HWND hwnd, ULONG id, PUSHORT pfs, ULONG flag);

// global variables ---------------------------------------------------------

// entry point --------------------------------------------------------------
/* --------------------------------------------------------------------------
 Splitter settings dialog procedure.
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT
-------------------------------------------------------------------------- */
MRESULT EXPENTRY setCtldataProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
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
   WNDPARAMS wp;
   SPLITCTLDATA scd;
   INT i;

   memset(&scd, 0, sizeof(scd));
   scd.cb = sizeof(scd);
   memset(&wp, 0, sizeof(wp));
   wp.fsStatus = WPM_CBCTLDATA | WPM_CTLDATA;
   wp.cbCtlData = scd.cb;
   wp.pCtlData = &scd;
   // get the control data and initialize the interface
   if (WinSendMsg(g.hwndSplitPane, WM_QUERYWINDOWPARAMS, (MPARAM)&wp, MPVOID))
   {
      setEntry(hwnd, EF_SPLITTERPOS, (INT)scd.splitterPos, FALSE);
      setEntry(hwnd, EF_MINLEFT, (INT)scd.minLeft, FALSE);
      setEntry(hwnd, EF_MINRIGHT, (INT)scd.minRight, FALSE);
      setEntry(hwnd, EF_MINTOP, (INT)scd.minTop, FALSE);
      setEntry(hwnd, EF_MINBOTTOM, (INT)scd.minBottom, FALSE);
      setEntry(hwnd, EF_THICKNESS, (INT)scd.thickness, FALSE);
      setEntry(hwnd, EF_STYLE, (INT)scd.style, TRUE);
   }
   // disable all controls
   else
   {
      DlgItemEnableMulti(hwnd, i, CHK_SPLITTERPOS, CHK_DLGUNITS, FALSE);
      DlgItemEnable(hwnd, BTN_APPLY, FALSE);
   }
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
   WNDPARAMS wp;
   SPLITCTLDATA scd;

   if (id == BTN_APPLY)
   {
      memset(&scd, 0, sizeof(scd));
      scd.cb = sizeof(scd);
      memset(&wp, 0, sizeof(wp));
      wp.fsStatus = WPM_CTLDATA;
      wp.cbCtlData = scd.cb;
      wp.pCtlData = &scd;
      // read the current settings
      scd.splitterPos = (SHORT)getEntry(hwnd, CHK_SPLITTERPOS,
                                        &scd.flag, SPLITCD_POS);
      scd.minLeft = (SHORT)getEntry(hwnd, CHK_MINLEFT,
                                    &scd.flag, SPLITCD_MINLEFT);
      scd.minRight = (SHORT)getEntry(hwnd, CHK_MINRIGHT,
                                     &scd.flag, SPLITCD_MINRIGHT);
      scd.minTop = (SHORT)getEntry(hwnd, CHK_MINTOP,
                                   &scd.flag, SPLITCD_MINTOP);
      scd.minBottom = (SHORT)getEntry(hwnd, CHK_MINBOTTOM,
                                      &scd.flag, SPLITCD_MINBOTTOM);
      scd.thickness = (USHORT)getEntry(hwnd, CHK_THICKNESS,
                                       &scd.flag, SPLITCD_THICKNESS);
      scd.style = (USHORT)getEntry(hwnd, CHK_STYLE,
                                   &scd.flag, SPLITCD_STYLE);
      if (WinQueryButtonCheckstate(hwnd, CHK_DLGUNITS))
         scd.flag |= SPLITCD_DIALOGUNITS;
      WinSendMsg(g.hwndSplitPane, WM_SETWINDOWPARAMS, (MPARAM)&wp, MPVOID);
   }
   else
      WinPostMsg(WinOwner(hwnd), WM_USER, (MPARAM)hwnd, MPVOID);
}


/* --------------------------------------------------------------------------
 Fill an entry field with the value provided.
- Parameters -------------------------------------------------------------
 HWND hwnd : dialog window handle.
 ULONG id  : entry field id.
 INT value : value to be converted to text.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID setEntry(HWND hwnd, ULONG id, INT value, BOOL hex) {
   CHAR buf[32];
   sprintf(buf, (hex ? "0x%04x": "%d"), value);
//   _itoa(value, buf, (hex ? 16 : 10));
   DlgItemSetText(hwnd, id, buf);
}


/* --------------------------------------------------------------------------
 If a checkbox is checked get the content of the entry field, convert it to
 a short value, and set the data flag.
- Parameters -------------------------------------------------------------
 HWND hwnd   : window handle.
 ULONG id    : checkbox id (the entry field id is id + 1)
 PUSHORT pfs : (output) flag to be update if the checkbox is checked.
 ULONG flag  : value to be ORed to pfs.
- Return value -----------------------------------------------------------
 LONG converted value.
-------------------------------------------------------------------------- */
static
LONG getEntry(HWND hwnd, ULONG id, PUSHORT pfs, ULONG flag) {
   CHAR buf[32];
   LONG value = 0;
   if (WinQueryButtonCheckstate(hwnd, id))
   {
      if (DlgItemText(hwnd, id + 1, sizeof(buf), buf))
      {
         *pfs |= (USHORT)flag;
         value= strtol(buf, NULL, 0);
      }
   }
   return value;
}


