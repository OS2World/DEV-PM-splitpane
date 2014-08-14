/* --------------------------------------------------------------------------
 prototypes.h :

 28 Apr 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _PROTOTYPES_H_
   #define _PROTOTYPES_H_

MRESULT EXPENTRY setSplitpane(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY setSplitter(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY setCtldataProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY prodInfoProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

#endif // #ifndef _PROTOTYPES_H_