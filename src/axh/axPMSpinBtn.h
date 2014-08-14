/* --------------------------------------------------------------------------
 axPMSpinBtn.h :

 20 Dec 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _AXPMSPINBTN_H_
   #define _AXPMSPINBTN_H_

// SPIN BUTTON macros -------------------------------------------------------


// gets the value of a spinbutton (as LONG)
#define dSpinBtnLong(hwnd, id, pulValue) \
   ((BOOL)WinSendDlgItemMsg((hwnd), (id), SPBM_QUERYVALUE, \
                            (MPARAM)(pulValue), \
                            MPFROM2SHORT(0, SPBQ_ALWAYSUPDATE)))

#define wSpinBtnLong(hwnd, pulValue) \
   ((BOOL)WinSendMsg((hwnd), SPBM_QUERYVALUE, \
                     (MPARAM)(pulValue), MPFROM2SHORT(0, SPBQ_ALWAYSUPDATE)))


#define dSpinBtnSetArray(hwnd, id, aPsz, cPsz) \
   ((BOOL)WinSendDlgItemMsg((hwnd), (id), SPBM_SETARRAY, \
                            (MPARAM)(aPsz), (MPARAM)(cPsz)))
#define wSpinBtnSetArray(hwnd, aPsz, cPsz) \
   ((BOOL)WinSendMsg((hwnd), SPBM_SETARRAY, (MPARAM)(aPsz), (MPARAM)(cPsz)))

#define dSpinBtnSetValue(hwnd, id, value) \
   ((BOOL)WinSendDlgItemMsg((hwnd), (id), SPBM_SETCURRENTVALUE, \
                            (MPARAM)(value), MPVOID))
#define wSpinBtnSetValue(hwnd, value) \
   ((BOOL)WinSendMsg((hwnd), SPBM_SETCURRENTVALUE, (MPARAM)(value), MPVOID))

#define dSpinBtnSetLimits(hwnd, id, bottom, top) \
   ((BOOL)WinSendDlgItemMsg((hwnd), (id), SPBM_SETLIMITS, \
                            (MPARAM)(top), (MPARAM)(bottom)))
#define wSpinBtnSetLimits(hwnd, bottom, top) \
   ((BOOL)WinSendMsg((hwnd), SPBM_SETLIMITS, (MPARAM)(top), (MPARAM)(bottom)))



#endif // #ifndef _AXPMSPINBTN_H_