/* --------------------------------------------------------------------------
 axMenu.h :

 20 Dec 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _AXMENU_H_
   #define _AXMENU_H_

// creates a submenu to be dynamically inserted in a menu
// hwnd : parent menu handle
#define WinCreateSubmenu(hwnd) \
((HWND)WinCreateWindow(HWND_OBJECT, WC_MENU, NULL, 0, 0, 0, 0, 0, (hwnd), \
                       HWND_TOP, 0, NULL, NULL))

// inserts a menu item
// hwnd     : parent menu handle
// pmi      : address of MENUITEM structure
// itemText : item text
#define wMenuItemIns(hwnd, pmi, itemText) \
((SHORT)WinSendMsg((hwnd), MM_INSERTITEM, (MPARAM)(pmi), (MPARAM)(itemText)))

// check/unchecks menuitem id
#define dMenuItemCheck(hwnd, id, bCheck) \
   ((BOOL)WinSendDlgItemMsg(hwnd, FID_MENU, MM_SETITEMATTR, \
                MPFROM2SHORT(id, TRUE), \
                MPFROM2SHORT(MIA_CHECKED, ((!(!(bCheck))) << 13))))
#define wMenuItemCheck(hwnd, id, bCheck) \
   ((BOOL)WinSendMsg(hwnd, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
                MPFROM2SHORT(MIA_CHECKED, ((!(!(bCheck))) << 13))))

// enable/disable a menuitem id (hwnd is the handle of the frame window)
#define dMenuItemEnable(hwnd, id, bEnable) \
   ((BOOL)WinSendDlgItemMsg(hwnd, FID_MENU, MM_SETITEMATTR, \
                     MPFROM2SHORT(id, TRUE), \
                     MPFROM2SHORT(MIA_DISABLED, ((!(bEnable)) << 14))))
// enable/disable a menuitem id
#define wMenuItemEnable(hwnd, id, bEnable) \
   ((BOOL)WinSendMsg(hwnd, MM_SETITEMATTR, MPFROM2SHORT(id, TRUE), \
                     MPFROM2SHORT(MIA_DISABLED, ((!(bEnable)) << 14))))

// changes the text of the menuitem id
#define wMenuItemTextSet(hwnd, id, text) \
   ((BOOL)WinSendMsg((hwnd), MM_SETITEMTEXT, (MPARAM)(id), (MPARAM)(text)))

#endif // #ifndef _AXMENU_H_