/* --------------------------------------------------------------------------
 main.c :

 30 Dec 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
 Example program of a standard window containing nested splitpanes
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// #pragma strings(readonly)

// includes -----------------------------------------------------------------

#include "main.h"

// definitions --------------------------------------------------------------

#define SZ_CLASS   "SplittestWinClass"
#define FRAMEFLAGS   FCF_TITLEBAR       \
                   | FCF_SYSMENU        \
                   | FCF_MENU           \
                   | FCF_MINMAX         \
                   | FCF_SIZEBORDER     \
                   | FCF_SHELLPOSITION  \
                   | FCF_TASKLIST       \
                   | FCF_ACCELTABLE     \
                   | FCF_ICON


// prototypes ---------------------------------------------------------------

BOOL initApplication(INT argc, PSZ* argv);
MRESULT EXPENTRY splittestProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL handleKeybEvents(HWND hwnd, PCHRMSG pch);
MRESULT onClientInit(HWND hwnd, PCREATESTRUCT pcreate);
VOID onSizeChange(HWND hwnd, INT cx, INT cy);
BOOL onCtrlNotify(HWND hwnd, ULONG id, ULONG event, HWND hCtrl);
VOID onCommand(HWND hwnd, ULONG id);

// global variables ---------------------------------------------------------
GLOBAL g;

// --------------------------------------------------------------------------

INT main(INT argc, PSZ* argv) {
   HWND hwnd, hwndc;
   HAB hab;
   HMQ hmq;
   QMSG qmsg;
   ULONG fl = FRAMEFLAGS;

   if (!initApplication(argc, argv))
      return (INT)ERROR_INVALID_PARAMETER;

   hmq = WinCreateMsgQueue(hab = WinInitialize(0), 0);
   WinRegisterClass(hab, SZ_CLASS, splittestProc, CS_SIZEREDRAW, 0);
   WinSplitPaneRegister(hab);
   hwnd = WinCreateStdWindow(HWND_DESKTOP,    // parent
                             WS_VISIBLE,      // frame window style
                             &fl,             // frame flags
                             SZ_CLASS,        // client class name
                             SZ_TITLE,        // window title
                             0,               // client window style
                             NULLHANDLE,      // resource module handle
                             ID_SPLITTEST,  // resources ID
                             &hwndc);         // client window handle
   if (hwnd) {
      while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
         WinDispatchMsg(hab, &qmsg);
      WinDestroyWindow(hwnd);
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
 Main window procedure
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT
-------------------------------------------------------------------------- */
static
MRESULT EXPENTRY splittestProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg)
   {
      case WM_CHAR:
         if (CHARMSG(&msg)->fs & KC_KEYUP)
            break;
         return (MRESULT)handleKeybEvents(hwnd, CHARMSG(&msg));
      case WM_CREATE:
         return onClientInit(hwnd, (PCREATESTRUCT)mp2);
      case WM_ERASEBACKGROUND:
         return (MRESULT)TRUE;
      case WM_CONTROL:
         onCtrlNotify(hwnd,  SHORT1FROMMP(mp1), SHORT2FROMMP(mp1), (HWND)mp2);
         break;
      case WM_COMMAND:
         onCommand(hwnd, (ULONG)mp1);
         break;
      case WM_SIZE:
         onSizeChange(hwnd, SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
         break;
      default:
         return WinDefWindowProc(hwnd, msg, mp1, mp2);
   }
   return (MRESULT)FALSE;
}


/* --------------------------------------------------------------------------
 Handle the keyboard messages VK_TAB and VK_BACKTAB to switch the focus
 among its children.
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane control handle.
 PCHRMSG pch : keyboard event details.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (event processed/dismissed).
-------------------------------------------------------------------------- */
static
BOOL handleKeybEvents(HWND hwnd, PCHRMSG pch) {
   switch (pch->vkey)
   {
   case VK_TAB:
      WinSetFocus(HWND_DESKTOP,
                  WinEnumDlgItem(hwnd,
                                 WinQueryFocus(HWND_DESKTOP),
                                 EDI_NEXTTABITEM));
      return TRUE;
   case VK_BACKTAB:
      WinSetFocus(HWND_DESKTOP,
                  WinEnumDlgItem(hwnd,
                                 WinQueryFocus(HWND_DESKTOP),
                                 EDI_PREVTABITEM));
      return TRUE;
   }
   return FALSE;
}

/* --------------------------------------------------------------------------
 This function handles client window initialization, creating a splitpane
 control which completely overlaying the client area and containing nested
 splitpanes.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle.
 PCREATESTRUCT pcreate: window creation data.
- Return value -----------------------------------------------------------
 MRESULT : FALSE/TRUE (success/error).
-------------------------------------------------------------------------- */
MRESULT onClientInit(HWND hwnd, PCREATESTRUCT pcreate) {
   // create 3 splitpanes : one main and 2 recursively nested ones
   SPLITPCREATE scP, sc1, sc2;
   SPLITCTLDATA scd;
   CHAR buf[64];
   INT i;
   BOOL rc;

   zerostruct(scP);  // memset struct to 0 - see axh\axbase.h
   zerostruct(sc1);
   zerostruct(sc2);
   zerostruct(scd);

   // initialize the parent splitpane data
   // left/top pane is a listbox
   scP.pane1.class = WC_LISTBOX;
   scP.pane1.style = LS_NOADJUSTPOS | WS_TABSTOP;
   scP.pane1.id = ID_LBOX;
   // right/bottom pane is a nested splitpane
   scP.pane2.pSplitCreate = &sc1;
   scP.pane2.style = SPLITS_HORIZONTAL | SPLITS_HIDEPANES;
   scP.pane2.id = ID_NESTED1;
   scP.pane2.type = SPLITCHILD_SPLIT;    // nested splitpane
   // set the control data of the main pane
   scP.pCtlData = &scd;
   scd.cb = sizeof(scd);
   scd.minLeft = 100;      // minimum widht of the left pane
   scd.minRight = 100;     // minimum widht of the right pane
   scd.splitterPos = -30;  // 30 % of total width
   scd.flag = SPLITCD_POS | SPLITCD_MINLEFT | SPLITCD_MINRIGHT;

   // initialize the first nested splitpane
   // left/top pane is a nested splitpane
   sc1.pane1.pSplitCreate = &sc2;
   sc1.pane1.id = ID_NESTED2;
   sc1.pane1.style = SPLITS_HIDEPANES;
   sc1.pane1.type = SPLITCHILD_SPLIT;    // nested splitpane
   // right/bottom pane is a multi-line edit
   sc1.pane2.class = WC_MLE;
   sc1.pane2.text = SZ_MLETEXT;
   sc1.pane2.style = MLS_BORDER | MLS_WORDWRAP | WS_TABSTOP;
   sc1.pane2.id = ID_MLE;

   // initialize the inner nested splitpane
   // left/top pane is a dialog window
   sc2.pane1.pDlgProc = testDlgProc;
   sc2.pane1.id = ID_DLG;
   sc2.pane1.type = SPLITCHILD_DLG;
   // right/bottom pane is a static text window
   sc2.pane2.class = WC_STATIC;
   sc2.pane2.text = SZ_STATICSAMPLE;
   sc2.pane2.style = SS_TEXT | DT_WORDBREAK;
   sc2.pane2.id = ID_STATIC;

   rc = !WinCreateSplitPane(hwnd, WS_VISIBLE | SPLITS_HIDEPANES, ID_SPLITMAIN,
                            0, 0, pcreate->cx, pcreate->cy, &scP);
   if (!rc)
   {
      hwnd = WinWindowFromID(hwnd, ID_LBOX);
      for (i = 0; i < 100; ++i)
      {
         sprintf(buf, "%s%d", SZ_STDITEMTXT, i + 1);
         WinInsertLboxItem(hwnd, LIT_END, buf);
      }
   }
   return (MRESULT)rc;
}


/* --------------------------------------------------------------------------
 This function handle client window size change by updating the size of
 the splitpane so that it completely overlays the client window.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle.
 INT cx, cy : new client size.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID onSizeChange(HWND hwnd, INT cx, INT cy) {
   DlgItemSetPos(hwnd, ID_SPLITMAIN, 0, 0, 0, cx, cy, SWP_SIZE);
}


/* --------------------------------------------------------------------------
 Process notification messages.
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
 Process command events.
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
         break;
      case :
         break;
   }
*/
}
