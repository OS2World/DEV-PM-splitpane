/* --------------------------------------------------------------------------
 main.c :

 28 Apr 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// #pragma strings(readonly)

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------


// prototypes ---------------------------------------------------------------

BOOL initApplication(INT argc, PSZ* argv);
MRESULT EXPENTRY SplitPaneProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL onDlgInit(HWND hwnd, PVOID pdata);
VOID onCommand(HWND hwnd, ULONG id);
VOID onWinPosChange(HWND hwnd, PSWP pswp, MPARAM mp2);
VOID openSettings(HWND hwnd, BOOL firstPane, PFNWP dlgProc, ULONG dlgId);

// global variables ---------------------------------------------------------
GLOBAL g;

// --------------------------------------------------------------------------

INT main(INT argc, PSZ* argv) {
   HWND hwnd, hwndSettings;
   HAB hab;
   HMQ hmq;
   QMSG qmsg;

   if (!initApplication(argc, argv))
      return (INT)ERROR_INVALID_PARAMETER;

   hmq = WinCreateMsgQueue(hab = WinInitialize(0), 0);
   if (WinSplitPaneRegister(hab))
   {
      hwnd = WinLoadDlg(HWND_DESKTOP, NULLHANDLE,
                        SplitPaneProc, NULLHANDLE,
                        DLG_SPLITPANE, NULL);
      if (hwnd)
      {
         while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
            WinDispatchMsg(hab, &qmsg);
         if (g.hwndSettings) WinDestroyWindow(g.hwndSettings);
         WinDestroyWindow(hwnd);
      }
   }
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return 0;
}


/* --------------------------------------------------------------------------
 Application initialization
- Parameters -------------------------------------------------------------
 INT argc  : startup parameters count.
 PSZ* argv : startup parameters array.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL initApplication(INT argc, PSZ* argv) {

   return TRUE;
}


/* --------------------------------------------------------------------------
 Main dialog procedure.
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT
-------------------------------------------------------------------------- */
static
MRESULT EXPENTRY SplitPaneProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg)
   {
      case WM_INITDLG:
         return (MRESULT)onDlgInit(hwnd, mp2);
      case WM_COMMAND:
         onCommand(hwnd, (ULONG)mp1);
         break;
      case WM_USER:
         if (g.hwndSettings == (HWND)mp1)
         {
            WinDestroyWindow(g.hwndSettings);
            g.hwndSettings = g.hwndSplitPane = NULLHANDLE;
            dMenuItemEnable(hwnd, MI_OPTIONS, TRUE);
         }
         break;
      case WM_WINDOWPOSCHANGED:
         onWinPosChange(hwnd, (PSWP)mp1, mp2);
         break;
      case WM_CLOSE:
         WinPostMsg(hwnd, WM_QUIT, 0, 0);
         break;
      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   }
   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Main dialog initialization
- Parameters -------------------------------------------------------------
 HWND hwnd   : window handle.
 PVOID pdata : data associated with the current dialog.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (focus window was/wasn't changed);
-------------------------------------------------------------------------- */
static
BOOL onDlgInit(HWND hwnd, PVOID pdata) {

   DlgLoadIcon(hwnd, NULLHANDLE, ID_MAINRES);
   DlgLoadMenuBar(hwnd, NULLHANDLE, ID_MAINRES);
   WinSetFont(hwnd, "9.WarpSans");
   dSplitSetChildren(hwnd, SPLIT_1, MLE_1, SPLIT_2);
   dSplitSetChildren(hwnd, SPLIT_2, MLE_2_1, MLE_2_2);
   WinSetFocus(HWND_DESKTOP, DlgItemHandle(hwnd, MLE_1));
   return TRUE;
}


/* --------------------------------------------------------------------------
 Main dialog command events.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle
 ULONG id  : button id
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID onCommand(HWND hwnd, ULONG id) {
   switch (id)
   {
      case MI_SPLITPANE1:
         openSettings(hwnd, TRUE, setSplitpane, DLG_SETSPLITPANE);
         break;
      case MI_SPLITPANE2:
         openSettings(hwnd, FALSE, setSplitpane, DLG_SETSPLITPANE);
         break;
      case MI_SPLITTER1:
         openSettings(hwnd, TRUE, setSplitter, DLG_SETSPLITTER);
         break;
      case MI_SPLITTER2:
         openSettings(hwnd, FALSE, setSplitter, DLG_SETSPLITTER);
         break;
      case MI_CTLDATA1:
         openSettings(hwnd, TRUE, setCtldataProc, DLG_CTLDATA);
         break;
      case MI_CTLDATA2:
         openSettings(hwnd, FALSE, setCtldataProc, DLG_CTLDATA);
         break;
     case MI_HELPGENERAL:
         break;
      case MI_HELPPRODINFO:
         WinDlgBox(HWND_DESKTOP, hwnd, prodInfoProc, 0, DLG_PRODINFO, NULL);
         break;
   }
}


/* --------------------------------------------------------------------------
 Handle dialog window size change.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle.
 PSWP pswp : array of 2 SWP structures describing the new and old window
             position and size.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID onWinPosChange(HWND hwnd, PSWP pswp, MPARAM mp2) {
   RECTL r = {0, 0, pswp->cx, pswp->cy};

   if ((pswp->fl & SWP_SIZE) && WinCalcFrameRect(hwnd, &r, TRUE))
   {
      DlgItemSetPos(hwnd, SPLIT_1, 0, r.xLeft, r.yBottom,
                    r.xRight - r.xLeft, r.yTop - r.yBottom,
                    SWP_SIZE | SWP_MOVE);

   }
   WinDefDlgProc(hwnd, WM_WINDOWPOSCHANGED, (MPARAM)pswp, mp2);
   WinInvalidateRect(hwnd, NULL, TRUE);
}


/* --------------------------------------------------------------------------
 Open a settings dialog.
- Parameters -------------------------------------------------------------
 HWND hwnd      : dialog window handle.
 BOOL firstPane : TRUE/FALSE (update 1st/2nd splitpane).
 PFNWP dlgProc  : dialog procedure.
 ULONG dlgId    : dialog id.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID openSettings(HWND hwnd, BOOL firstPane, PFNWP dlgProc, ULONG dlgId) {
   dMenuItemEnable(hwnd, MI_OPTIONS, FALSE);
   if (g.hwndSettings)
   {
      WinAlarm(HWND_DESKTOP, WA_ERROR);
   }
   else
   {
      g.hwndSplitPane = DlgItemHandle(hwnd, (firstPane ? SPLIT_1: SPLIT_2));
      g.hwndSettings = WinLoadDlg(HWND_DESKTOP, hwnd, dlgProc,
                                  NULLHANDLE, dlgId, NULL);
   }
}
