/* --------------------------------------------------------------------------
 splittestDlg.c :

 30 Dec 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// #pragma strings(readonly)

// includes -----------------------------------------------------------------

#include "main.h"

// definitions --------------------------------------------------------------


// prototypes ---------------------------------------------------------------

MRESULT EXPENTRY splittestProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL onDlgInit(HWND hwnd, PVOID pdata);
BOOL onCtrlNotify(HWND hwnd, ULONG id, ULONG event, HWND hCtrl);
VOID onCommand(HWND hwnd, ULONG id);


// global variables ---------------------------------------------------------


// --------------------------------------------------------------------------


/* --------------------------------------------------------------------------
 Main dialog procedure.
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT
-------------------------------------------------------------------------- */
MRESULT EXPENTRY testDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg)
   {
      /*
      case WM_INITDLG:
         return (MRESULT)onDlgInit(hwnd, mp2);
      case WM_CONTROL:
         onCtrlNotify(hwnd,  SHORT1FROMMP(mp1), SHORT2FROMMP(mp1), (HWND)mp2);
         break;
      case WM_COMMAND:
         onCommand(hwnd, (ULONG)mp1);
         break;
      case WM_CLOSE:
         WinPostMsg(hwnd, WM_QUIT, 0, 0);
         break;
      */
      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   }
   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Dialog initialization
- Parameters -------------------------------------------------------------
 HWND hwnd   : window handle.
 PVOID pdata : data associated with the current dialog.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (focus window was/wasn't changed);
-------------------------------------------------------------------------- */
static
BOOL onDlgInit(HWND hwnd, PVOID pdata) {

   return FALSE;
}


/* --------------------------------------------------------------------------
 Dialog notification messages.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle
 ULONG id       : control ID
 ULONG event : notify code
 HWND hCtrl     : control handle
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL onCtrlNotify(HWND hwnd, ULONG id, ULONG event, HWND hCtrl) {
/*
   switch (id)
   {
      case :
         break;
      default:
         break;
   }
*/
   return FALSE;
}


/* --------------------------------------------------------------------------
 Dialog command events.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle
 ULONG id  : button id
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID onCommand(HWND hwnd, ULONG id) {
/*
   switch (id)
   {
      case :
         WinDismissDlg(hwnd, id);
         break;
      case :
         WinDismissDlg(hwnd, id);
         break;
   }
   WinDismissDlg(hwnd, id);
*/
}
