/* --------------------------------------------------------------------------
 axPMButtons.h :

 20 Dec 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _AXPMBUTTONS_H_
   #define _AXPMBUTTONS_H_

#define dBtnClick(hwnd, id, bUp) \
   ((USHORT)WinSendDlgItemMsg((hwnd), (id), BM_CLICK, (MPARAM)(bUp), MPVOID))
#define wBtnClick(hwnd, bUp) \
   ((USHORT)WinSendMsg((hwnd), BM_CLICK, (MPARAM)(bUp), MPVOID))

// sets a button check state
#define dBtnCheckSet(hwnd, id, state) \
   ((USHORT)WinSendDlgItemMsg((hwnd), (id), BM_SETCHECK, \
                              (MPARAM)(state), MPVOID))
#define wBtnCheckSet(hwnd, state) \
   ((USHORT)WinSendMsg((hwnd), BM_SETCHECK, (MPARAM)(state), MPVOID))

// gets a button check state
#define dBtnCheckState(hwnd, id) \
   ((USHORT)WinSendDlgItemMsg((hwnd), (id), BM_QUERYCHECK, MPVOID, MPVOID))
#define wBtnCheckState(hwnd) \
   ((USHORT)WinSendMsg((hwnd), BM_QUERYCHECK, MPVOID, MPVOID))

// returns the check index of a group of radiobutton (0 = first radiobutton)
#define dBtnCheckIndex(hwnd, id) \
   ((SHORT)WinSendDlgItemMsg((hwnd), (id), BM_QUERYCHECKINDEX, MPVOID, MPVOID))
#define wBtnCheckIndex(hwnd) \
   ((SHORT)WinSendMsg((hwnd), BM_QUERYCHECKINDEX, MPVOID, MPVOID))

#endif // #ifndef _AXPMBUTTONS_H_