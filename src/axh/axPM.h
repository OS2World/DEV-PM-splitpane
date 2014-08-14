/* --------------------------------------------------------------------------
 axPM.h : general PM API extensions, macro and data definitions.

 30 May 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------
 - useful message result and message parameters macro
 - window coordinates structures
 - point and rectangle macros (Point and Rect prefixes)
 - mouse and mouse pointer macros (Mou prefix)
 - generic window related macros (Win prefix)
 - generic PM macros (Pm prefix)
 - Dialog macros (Dlg prefix)
-------------------------------------------------------------------------- */

#ifndef _AXPM_H_
   #define _AXPM_H_

#ifndef OS2_INCLUDED
   #define INCL_WIN
   #define INCL_GPI
   #include <os2.h>
#endif
   #include <stdlib.h>
   #include <stddef.h>
   #include <string.h>


/* ---------------------------------------------------------------------------
 useful message result and message parameters macro
*/

// common message result values:
#define MRFALSE ((MRESULT)0)
#define MRTRUE  ((MRESULT)1)

// common message parameter values:
#define MPFALSE ((MPARAM)0)
#define MPTRUE  ((MPARAM)1)


/* ---------------------------------------------------------------------------
 Structures describing window position and size
*/

typedef struct {
   LONG x, y, cx, cy;
} WPOSSIZE, * PWPOSSIZE;

#pragma pack(2)

typedef struct {
   SHORT x, y, cx, cy;
} WPOSSIZES, * PWPOSSIZES;

typedef struct {
   SHORT cx, cy;
} SIZES, * PSIZES;

#pragma pack()



/* ---------------------------------------------------------------------------
 Point and rectangle macros
*/

/*
 initializes a POINTL structure.
 Parameters:
 PPOINTL _ppt_ : address of the POINTL to be initialized.
 LONG    _x_   : value to be assigned to the x member.
 LONG    _y_   : value to be assigned to the y member.
 Example:
 PointSet(pptl, 10, 10);
*/
#define PointSet(_ppt_, _x_, _y_) \
   (((PPOINTL)(_ppt_))->x = (_x_), ((PPOINTL)(_ppt_))->y = (_y_))


/*
 initializes a rectangle structure.
 Parameters:
 PRECTL _pr_  : address of the RECTL structure to be shifted.
 LONG   _x0_  : xLeft value.
 LONG   _y0_  : yBottom value.
 LONG   _x1_  : xRight value.
 LONG   _y1_  : xTop value.
*/
#define RectSet(_pr_, _x0_, _y0_, _x1_, _y1_)  \
   (((PRECTL)(_pr_))->xLeft = (_x0_),          \
    ((PRECTL)(_pr_))->yBottom = (_y0_),        \
    ((PRECTL)(_pr_))->xRight = (_x1_),         \
    ((PRECTL)(_pr_))->yTop = (_y1_))

/*
 makes a rectangle simmetrically wider and heigher by (_dx_ * 2) and
 (_dy_ * 2) pixels.
 Parameters:
 PRECTL _pr_   : address of the RECTL structure to be shifted.
 LONG   _dx_   : x-axis shift.
 LONG   _dy_   : y-axis shift.
 Example:
 RectInflate(pRect, 1, 1);  // create a rectangle 2 pixels wider and higher
*/
#define RectInflate(_pr_, _dx_, _dy_) \
   (((PRECTL)(_pr_))->xLeft -= (_dx_),     \
    ((PRECTL)(_pr_))->yBottom -= (_dy_),   \
    ((PRECTL)(_pr_))->xRight += (_dx_),    \
    ((PRECTL)(_pr_))->yTop += (_dy_))

/*
 moves a rectangle by _dx_ and _dy_ pixels.
 Parameters:
 PRECTL _pr_   : address of the RECTL structure to be shifted.
 LONG   _dx_   : x-axis shift.
 LONG   _dy_   : y-axis shift.
*/
#define RectMove(_pr_, _dx_, _dy_)        \
   (((PRECTL)(_pr_))->xLeft += (_dx_),    \
    ((PRECTL)(_pr_))->yBottom += (_dy_),  \
    ((PRECTL)(_pr_))->xRight += (_dx_),   \
    ((PRECTL)(_pr_))->yTop += (_dy_))


/*
 initializes a WPOSSIZE structure.
*/
#define PosSizeSet(_pwps_, _x_, _y_, _cx_, _cy_) \
 (((PWPOSSIZE)(_pwps_))->x = _x_,                \
  ((PWPOSSIZE)(_pwps_))->y = _y_,                \
  ((PWPOSSIZE)(_pwps_))->cx = _cx_,              \
  ((PWPOSSIZE)(_pwps_))->cy = _cy_)


/* ---------------------------------------------------------------------------
 mouse and mouse pointer macros
*/

// gets the x,y coordinates from a mouse message parameter
#define MouX(_mp_)      ((SHORT)(ULONG)(_mp_))
#define MouY(_mp_)      ((SHORT)(((ULONG)(_mp_)) >> 16))

/*
 checks if the mouse is within a window subrectangle.
 The mouse message parameter and the rectangle coordinates must be relative
 to the same window.
 Parameters:
 MPARAM _mp_  : WM_MOUSE* message parameter 1
 PRECTL _pr_  : rectangle to be checked
*/
#define MouInWinRect(_mp_, _pr_)                  \
   ((BOOL)( (MouX(_mp_) >= (_pr_)->xLeft) &&      \
            (MouY(_mp_) >= (_pr_)->yBottom) &&    \
            (MouX(_mp_) < (_pr_)->xRight) &&      \
            (MouX(_mp_) < (_pr_)->yTop)))

/*
 sets a system mouse pointer (used on WM_MOUSEMOVE).
 Parameters:
 _iptr_ : System pointer identifier (SPTR_* )
*/
#define WinSetSysPointer(_iptr_) \
   (WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, (_iptr_), 0)))


/* ---------------------------------------------------------------------------
 generic window related macros (WinQueryWindow/UShort/ULong macros)
*/

// get common window values:
// get the window anchor block
#define WinHAB(_hwnd_)         (WinQueryAnchorBlock(_hwnd_))
// get the window ID
#define WinID(_hwnd_)          (WinQueryWindowUShort((_hwnd_), QWS_ID))
// get the next window handle
#define WinNext(_hwnd_)        (WinQueryWindow((_hwnd_), QW_NEXT))
// get the handle of the owner
#define WinOwner(_hwnd_)       (WinQueryWindow((_hwnd_), QW_OWNER))
// get the handle of the parent
#define WinParent(_hwnd_)      (WinQueryWindow((_hwnd_), QW_PARENT))
// get the window style
#define WinStyle(_hwnd_)       (WinQueryWindowULong((_hwnd_), QWL_STYLE))

// get the window restore coordinates (for minimized/maximized frame windows)
#define WinXRestore(_hwnd_)    (WinQueryWindowUShort((_hwnd_), QWS_XRESTORE))
#define WinYRestore(_hwnd_)    (WinQueryWindowUShort((_hwnd_), QWS_YRESTORE))
#define WinCxRestore(_hwnd_)   (WinQueryWindowUShort((_hwnd_), QWS_CXRESTORE))
#define WinCyRestore(_hwnd_)   (WinQueryWindowUShort((_hwnd_), QWS_CYRESTORE))

// set the window restore coordinates (for minimized/maximized frame windows)
#define WinSetXRestore(_hwnd_, _us_) \
   (WinSetWindowUShort((_hwnd_), QWS_XRESTORE, (_us_)))
#define WinSetYRestore(_hwnd_, _us_) \
   (WinSetWindowUShort((_hwnd_), QWS_YRESTORE, (_us_)))
#define WinSetCxRestore(_hwnd_, _us_) \
   (WinSetWindowUShort((_hwnd_), QWS_CXRESTORE, (_us_)))
#define WinSetCyRestore(_hwnd_, _us_) \
   (WinSetWindowUShort((_hwnd_), QWS_CYRESTORE, (_us_)))

// set the window style
#define WinSetStyle(hwnd, style) \
   (WinSetWindowULong((hwnd), QWL_STYLE, (style)))

// set the font presentation parameter for a window
#define WinSetFont(_hwnd_, _font_) \
   (WinSetPresParam((_hwnd_), PP_FONTNAMESIZE, strlen(_font_) + 1, (_font_)))

// send a notification message to the window owner
#define WinNotifyOwner(_hwnd_, _notification_, _mp2_)         \
   (WinSendMsg(WinOwner(_hwnd_), WM_CONTROL,                  \
               MPFROM2SHORT(WinID(_hwnd_), (_notification_)), \
               (MPARAM)(_mp2_)))

// set a window position using the data of a WPOSSIZE structure
#define WinSetPosSize(_hwnd_, _hwndBehind_, _pwps_, _flag_) \
   (WinSetWindowPos((_hwnd_), (_hwndBehind_),               \
                    ((PWPOSSIZE)(_pwps_))->x,               \
                    ((PWPOSSIZE)(_pwps_))->y,               \
                    ((PWPOSSIZE)(_pwps_))->cx,              \
                    ((PWPOSSIZE)(_pwps_))->cy,              \
                    (_flag_)))


/* ---------------------------------------------------------------------------
 generic PM macros (Pm prefix)
*/
// WinQuerySysValue () macro
#define PmValue(ival)      (WinQuerySysValue(HWND_DESKTOP, (ival)))


/* ---------------------------------------------------------------------------
 Dialog macros (Dlg prefix).
 Note:
 all these macros take as first two parameters the dialog handle and the
 dialog item ID.
*/

// get the handle of a dialog item
#define DlgItemHandle(_hwnd_, _id_)   (WinWindowFromID((_hwnd_), (_id_)))


// change the id of a dialog item
#define DlgItemSetId(_hwnd_, _oldId_, newId_) \
   (WinSetWindowUShort(WinWindowFromID((_hwnd_), (_oldId_)), QWS_ID, (_newId_)))


// enable/disable a dialog item
#define DlgItemEnable(_hwnd_, _id_, _enable_) \
   (WinEnableWindow(WinWindowFromID((_hwnd_), (_id_)), (_enable_)))


// enable/disable multiple dialog controls (with sequential IDs)
#define DlgItemEnableMulti(_hwnd_, _idx_, _firstId_, _lastId_, _enable_) \
   for (_idx_ = (_firstId_); (_idx_) <= (_lastId_); ++(_idx_))           \
      DlgItemEnable(_hwnd_, _idx_, _enable_)


// check if a dialog item is enabled
#define DlgItemIsEnabled(_hwnd_, _id_) \
   (WinIsWindowEnabled(WinWindowFromID((_hwnd_), (_id_))))


// show/hide a dialog item
#define DlgItemShow(_hwnd_, _id_, _show_) \
   (WinShowWindow(WinWindowFromID((_hwnd_), (_id_)), (_show_)))


// check if a dialog item is visible
#define DlgItemIsVisible(_hwnd_, _id_) \
   (WinIsWindowVisible(WinWindowFromID((_hwnd_), (_id_))))


// query the position of a dialog item
#define DlgItemGetPos(_hwnd_, _pswp_) \
   (WinQueryWindowPos(WinWindowFromID((_hwnd_), (_id_)), (_pswp_)))
#define DlgItemQueryPos                DlgItemGetPos


// set the position of a dialog item
#define DlgItemSetPos(_hwnd_, _id_, _hwndBehind_, _x_, _y_, _cx_, _cy_, _fl_) \
   (WinSetWindowPos(WinWindowFromID((_hwnd_), (_id_)), \
                    (_hwndBehind_), (_x_), (_y_), (_cx_), (_cy_), (_fl_)))


// query the text of a dialog control
#define DlgItemText(_hwnd_, _id_, _cb_, _ptext_) \
   (WinQueryDlgItemText((_hwnd_), (_id_), (_cb_), (_ptext_)))
#define DlgItemGetText                 DlgItemText
#define DlgItemQueryText               DlgItemText


// get the length of the text of a dialog control
#define DlgItemTextLength(_hwnd_, _id_) \
   (WinQueryDlgItemTextLength((_hwnd_), (_id_)))
#define DlgItemGetTextLength           DlgItemTextLength
#define DlgItemQueryTextLength         DlgItemTextLength


// set the text of a dialog item
#define DlgItemSetText(_hwnd_, _id_, _ptext_) \
   (WinSetDlgItemText((_hwnd_), (_id_), (_ptext_)))


// query the rectangle of a dialog item
#define DlgItemRect(_hwnd_, _id_, _pr_) \
   (WinQueryWindowRect(WinWindowFromID((_hwnd_), (_id_)), (_pr_)))
#define DlgItemGetRect                 DlgItemRect
#define DlgItemQueryRect               DlgItemRect

/*
 This macro loads an icon resource to be used as system menu icon.
 Parameters:
 _hwndDlg_     : dialog window handle
 _hResLib_     : handle of the resource library
 _idIcon_      : id of the icon resource

*/
#define DlgLoadIcon(_hwndDlg_, _hResLib_, _idIcon_)                    \
   ((BOOL)WinSendMsg((_hwndDlg_), WM_SETICON,                          \
          (MPARAM)WinLoadPointer(HWND_DESKTOP, (_hResLib_), (_idIcon_)), \
          MPVOID))
/*
 This macro loads a menubar resource and adds it to a dialog.
 Parameters:
 _hwndDlg_     : dialog window handle
 _hResLib_     : handle of the resource library
 _idMenu_      : id of the menu resource

*/
#define DlgLoadMenuBar(_hwndDlg_, _hResLib_, _idMenu_)                 \
   ((BOOL)(                                                            \
   WinLoadMenu((_hwndDlg_), (_hResLib_), (_idMenu_))  ?                 \
   WinSendMsg((_hwndDlg_), WM_UPDATEFRAME, (MPARAM)FCF_MENU, MPVOID) : \
   FALSE))



/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
/* ---------------------------------------------------------------------------

*/
#include "axGPI.h"
#include "axPMButtons.h"
#include "axPMCtrlUtil.h"
#include "axPMMenu.h"
#include "axPMSpinBtn.h"
#include "axPMUndoc.h"
#include "axSplitpane.h"

#endif // #ifndef _AXPM_H_