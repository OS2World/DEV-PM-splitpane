/* --------------------------------------------------------------------------
 splitpane.c :

 28 Apr 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// includes -----------------------------------------------------------------
#include "axBase.h"
#include "axPM.h"
#include "splitpanei.h"


// definitions --------------------------------------------------------------


// prototypes ---------------------------------------------------------------
HWND createChildPane(HWND hwndPO, PSPLITPANECHILD ppane);
MRESULT EXPENTRY SplitPaneProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT create(HWND hwnd, PSPLITCTLDATA pSplitData, PCREATESTRUCT pCreate);
VOID paint(HWND hwnd, PCTRLDATA p);
VOID adjustSplitterDrag(HWND hwnd, PSWP pswp, PCTRLDATA p);
VOID adjustPosSize(HWND hwnd, PSWP pswp, PCTRLDATA p);
ULONG setWinParms(HWND hwnd, PSPLITCTLDATA pCtlData, PCTRLDATA p);
VOID updateView(HWND hwnd, PCTRLDATA p, ULONG flag);
BOOL getWinParms(PCTRLDATA p, PWNDPARAMS pwp);
BOOL handleKeybEvents(HWND hwnd, PCTRLDATA p, PCHRMSG pch);
VOID mouseMove(HWND hwnd, PCTRLDATA p, INT x, INT y);
BOOL mouseBtnDown(HWND hwnd, PCTRLDATA p, INT x, INT y);
ULONG mousePos(PCTRLDATA p, INT x, INT y);
VOID paintArrow(HWND hwnd, PCTRLDATA p, BOOL iArrow, BOOL emph);
BOOL dragSplitter(HWND hwnd, PCTRLDATA p, INT x, INT y);
VOID mouseBtnUp(HWND hwnd, PCTRLDATA p, INT x, INT y);
MRESULT getCtlData(PCTRLDATA p, ULONG fsData, ULONG fsScope, MPARAM mp2);
MRESULT setCtlData(HWND hwnd, PCTRLDATA p,
                  ULONG fsData, ULONG fsScope, MPARAM mp2);
ULONG setStyle(PCTRLDATA p, USHORT newStyle);
ULONG setThickness(PCTRLDATA p, INT thickness, PPOINTL ppt);
ULONG setMinPaneSize(PSHORT psetval, INT newval, INT size, INT mul, INT shift);
INT curSplitterPos(PCTRLDATA p, BOOL percent);
ULONG setSplitterPos(PCTRLDATA p, BOOL relative, INT pos);
ULONG validateSplitter(PCTRLDATA p, ULONG flag);
ULONG validateRestorePos(INT pos, PCTRLDATA p, ULONG flag, ULONG flhide);
ULONG validateSplitterXPos(INT pos, PCTRLDATA p, ULONG flag);
ULONG validateSplitterYPos(INT pos, PCTRLDATA p, ULONG flag);
VOID setArrowCoords(PCTRLDATA p);
VOID setArrowFlags(PCTRLDATA p);
MRESULT setChildren(HWND hwnd, PCTRLDATA p, ULONG id1, ULONG id2);
BOOL setMinSizes(HWND hwnd, PCTRLDATA p, ULONG scope, INT size, BOOL bdu);
VOID setPaneSizePos(HWND hpane, LONG x, LONG y, LONG cx, LONG cy);

// global variables ---------------------------------------------------------

// entry point --------------------------------------------------------------

/* --------------------------------------------------------------------------
 Register the WC_SPLITPANE control.
- Parameters -------------------------------------------------------------
 HAB hab : anchor block handle.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
BOOL EXPENTRY WinSplitPaneRegister(HAB hab) {
   return WinRegisterClass(hab, WC_SPLITPANE, SplitPaneProc,
                           CS_SPLITPANE, PMC_CBWORDS);
}


/* --------------------------------------------------------------------------
 This functions creates a splitpane control and its children.
- Parameters -------------------------------------------------------------
 HWND hwndPO           : parent/owner window handle.
 ULONG style           : splitpane style.
 ULONG id              : splitpane id.
 LONG x, y             : splitpane coordinates.
 LONG cx, cy           : splitpane width and height.
 PSPLITPCREATE pCreate : splitpane control, presentation parameters and
                         children data.
- Return value -----------------------------------------------------------
 HWND : handle of the created WC_SPLITPANE or NULLHANDLE in case of error.
- Remarks ----------------------------------------------------------------
 The function assumes that the parent window is the owner too.
 The splitpane side windows are not real child windows. They are a sort of
 virtual child windows as they share the same parent with the splitpane.
-------------------------------------------------------------------------- */
HWND EXPENTRY WinCreateSplitPane(HWND hwndPO, ULONG style, ULONG id,
                                 LONG x, LONG y, LONG cx, LONG cy,
                                 PSPLITPCREATE pCreate) {
   HWND hwndSplitPane, hwndPane1, hwndPane2;

   if (NULLHANDLE == (hwndPane1 = createChildPane(hwndPO, &pCreate->pane1)))
      return NULLHANDLE;

   hwndSplitPane = WinCreateWindow(hwndPO, WC_SPLITPANE, NULL,
                                   style | WS_TABSTOP,
                                   x, y, cx, cy, hwndPO, HWND_TOP, id,
                                   pCreate->pCtlData, pCreate->pPresParams);
   if (!hwndSplitPane) goto abort_1;

   if (NULLHANDLE == (hwndPane2 = createChildPane(hwndPO, &pCreate->pane2)))
      goto abort_2;

   wSplitSetChildHandle(hwndSplitPane, SPLITMP_PANE1, hwndPane1);
   wSplitSetChildHandle(hwndSplitPane, SPLITMP_PANE2, hwndPane2);

   return hwndSplitPane;

// error handling
abort_2:
   WinDestroyWindow(hwndSplitPane);
abort_1:
   WinDestroyWindow(hwndPane1);

   return NULLHANDLE;
}


/* --------------------------------------------------------------------------
 This function creates a splitpane child.
- Parameters -------------------------------------------------------------
 HWND hwndPO : parent and owner window handle (dialog or client window).
 PSPLITPANECHILD ppane : splitpane child creation data.
- Return value -----------------------------------------------------------
 HWND : handle of the created child.
- Remarks ----------------------------------------------------------------
-------------------------------------------------------------------------- */
static
HWND createChildPane(HWND hwndPO, PSPLITPANECHILD ppane) {
   HWND hwnd;
   // check the pane type
   switch (ppane->type)
   {
   case SPLITCHILD_WND:                // the child is an ordinary window
      hwnd = WinCreateWindow(hwndPO, ppane->class, ppane->text,
                             ppane->style, 0, 0, 0, 0,
                             hwndPO, HWND_TOP, ppane->id,
                             ppane->pCtlData, ppane->pPresParams);
      break;
   case SPLITCHILD_DLG:                // the child is a dialog window
       hwnd = WinLoadDlg(hwndPO, hwndPO,
                        ppane->pDlgProc, ppane->hmod,
                        ppane->id, ppane->pCtlData);
      break;
   case SPLITCHILD_SPLIT:              // nested splitpane
       hwnd = WinCreateSplitPane(hwndPO, ppane->style, ppane->id,
                                0, 0, 0, 0, ppane->pSplitCreate);
      break;
   default:
      return NULLHANDLE;
   }
   if (hwnd) WinSetWindowBits(hwnd, QWL_STYLE, WS_VISIBLE, WS_VISIBLE);
   return hwnd;
}


/* --------------------------------------------------------------------------
 WC_SPLITPANE procedure.
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT.
-------------------------------------------------------------------------- */
MRESULT EXPENTRY
SplitPaneProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   PCTRLDATA p;
   switch (msg)
   {
   // paint the splitter according to the current state and style
   case WM_PAINT:
      if (NULL != (p = _mWndData(hwnd)))
         paint(hwnd, p);
      break;

   // splitpane visibility/size/position change or splitpane position change
   case WM_ADJUSTWINDOWPOS:
      if (NULL == (p = _mWndData(hwnd))) break;
      if (((PSWP)mp1)->fl & (SWP_SIZE | SWP_MOVE))
      {
         // position change as consequence of splitter drag: store new coords.
         if (p->state & IS_CAPTUREDRAG)
            adjustSplitterDrag(hwnd, (PSWP)mp1, p);
         else
            adjustPosSize(hwnd, (PSWP)mp1, p);
      }
      // visibility change: update visibility of controlled panes
      if (((PSWP)mp1)->fl & (SWP_SHOW | SWP_HIDE))
      {
         WinShowWindow(p->child.h1, ((PSWP)mp1)->fl & SWP_SHOW);
         WinShowWindow(p->child.h2, ((PSWP)mp1)->fl & SWP_SHOW);
      }
      break;

   // - set one or more control properties
   case WM_SETWINDOWPARAMS:
      if (   (((PWNDPARAMS)mp1)->fsStatus & WPM_CTLDATA)
          && ((PWNDPARAMS)mp1)->cbCtlData
          && ((PWNDPARAMS)mp1)->pCtlData
          && (NULL != (p = _mWndData(hwnd)))
         )
      {
         ULONG update;
         update = setWinParms(hwnd, ((PWNDPARAMS)mp1)->pCtlData, p);
         if (update) updateView(hwnd, p, update);
      }
      return MRTRUE;

   // query one or more control properties
   case WM_QUERYWINDOWPARAMS:
      if (NULL == (p = _mWndData(hwnd))) break;
      return (MRESULT)getWinParms(p, (PWNDPARAMS)mp1);

   // splitpane getting focus: hilite it
   case WM_SETFOCUS:
      if (NULL != (p = _mWndData(hwnd)))
      {
         p->state = mp2 ? p->state | IS_FOCUSWIN: p->state & ~IS_FOCUSWIN;
         WinInvalidateRect(hwnd, NULL, FALSE);
      }
      break;

   case WM_CHAR:
      if ((CHARMSG(&msg)->fs & KC_KEYUP) || (NULL == (p = _mWndData(hwnd))))
         break;
      return (MRESULT)handleKeybEvents(hwnd, p, CHARMSG(&msg));

   // if the mouse is not captured, according to the mouse position, set
   // the mouse pointer to SPTR_SIZEWE or to the hand pointer
   case WM_MOUSEMOVE:
      if (NULL == (p = _mWndData(hwnd))) break;
      mouseMove(hwnd, p, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1));
      return MRTRUE;

   // left mouse button down, if on the splitter start splitter dragging
   case WM_BUTTON1DOWN:
      if (NULL == (p = _mWndData(hwnd)) || (p->state & IS_CAPTUREANY))
         break;
      mouseBtnDown(hwnd, p, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1));
      return MRTRUE;

   // left mouse button released if on an arrow hide/restore panes
   case WM_BUTTON1UP:
      if ((NULL == (p = _mWndData(hwnd))) || !(p->state & IS_CAPTURED))
         break;
      mouseBtnUp(hwnd, p, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1));
      return MRTRUE;

   // if one of the supported  presentation parameter changed redraw the window
   case WM_PRESPARAMCHANGED:
      if (   ((ULONG)mp1 == PP_BACKGROUNDCOLOR)
          || ((ULONG)mp1 == PP_BACKGROUNDCOLORINDEX)
          || ((ULONG)mp1 == PP_BORDERLIGHTCOLOR)
          || ((ULONG)mp1 == PP_BORDERDARKCOLOR)
         )
      {
         if (WinIsWindowShowing(hwnd))
            WinInvalidateRect(hwnd, NULL, FALSE);
      }
      break;

   // redraw the window if it is visible
   case WM_SYSCOLORCHANGE:
      if (WinIsWindowShowing(hwnd))
         WinInvalidateRect(hwnd, NULL, FALSE);
      break;

   // WC_SPLITPANE message to query a control property
   case SPLITM_QUERY:
      if (NULL == (p = _mWndData(hwnd))) break;
      return getCtlData(p, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1), mp2);

   // WC_SPLITPANE message to set a control property
   case SPLITM_SET:
      if (NULL == (p = _mWndData(hwnd))) break;
      return setCtlData(hwnd, p, SHORT1FROMMP(mp1), SHORT2FROMMP(mp1), mp2);

   case WM_CREATE:
      return create(hwnd, (PSPLITCTLDATA)mp1, (PCREATESTRUCT)mp2);

   case WM_DESTROY:
      if (NULL != (p = _mWndData(hwnd))) memFree(p);
      break;

   default:
      return WinDefWindowProc(hwnd, msg, mp1, mp2);

   }
   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Control creation procedure:
 - allocate storage for the private control data,
 - initialize the private data to the default values,
 - if the user provided initialization data, update the private control data,
 - set the position of the splitter.
- Parameters -------------------------------------------------------------
 HWND hwnd                : window handle.
 PSPLITCTLDATA pSplitData : pointer to the optional creation parameters.
 PCREATESTRUCT pCreate    : ordinary window creation parameters.
- Return value -----------------------------------------------------------
 MRESULT : MRTRUE/MRFALSE (error/success).
-------------------------------------------------------------------------- */
static
MRESULT create(HWND hwnd, PSPLITCTLDATA pSplitData, PCREATESTRUCT pCreate) {
   PCTRLDATA p;
   // allocate storage for the control data
   if (NULL == (p = memAlloc(sizeof(CTRLDATA)))) return MRTRUE;
   WinSetWindowPtr(hwnd, QWL_CONTROL, p);

   // set the default data
   memset(p, 0, sizeof(CTRLDATA));
   p->style = pCreate->flStyle & 0xffff;
   p->pane.x = pCreate->x;
   p->pane.y = pCreate->y;
   p->pane.cx = pCreate->cx;
   p->pane.cy = pCreate->cy;
   p->spltr.restore = -1;
   p->spltr.thkness = PmValue(SV_CXSIZEBORDER) + 2;
   if (p->spltr.thkness < SP_THICKMIN) p->spltr.thkness = SP_THICKMIN;
   p->spltr.halfthk = p->spltr.thkness / 2;
   p->spltr.range = ((p->style & SPLITS_HORIZONTAL) ? p->pane.cy : p->pane.cx);
   if (p->spltr.range > 0)
   {
      p->spltr.position = p->spltr.range / 2 - p->spltr.halfthk;
      p->spltr.range -= p->spltr.thkness;
   }
   else
      p->state = IS_ONBORDER;
   p->spltr.relpos =  0x8000; // that is 0.5 << 16
   // if the address of a SPLITCTLDATA structure is available use the
   // data to initialize the WC_SPLITPANE control
   if (pSplitData) setWinParms(hwnd, pSplitData, p);
   setArrowCoords(p);
   setArrowFlags(p);
   // set the position of the Splitter
   if (p->style & SPLITS_HORIZONTAL)
      _mSetSplitterHorz(hwnd, p);
   else
      _mSetSplitterVert(hwnd, p);

   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Paint the control.
- Parameters -------------------------------------------------------------
 HWND hwnd   : control window handle.
 PCTRLDATA p : private control data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID paint(HWND hwnd, PCTRLDATA p) {
   HPS hps;
   POINTL pt;
   LONG clrLite, clrShade, clrFgnd, clrDis;
   INT i, shift;

   // -- initialize the presentation space setting the color mode to RGB
   if (NULLHANDLE == (hps = WinBeginPaint(hwnd, NULLHANDLE, NULL)))
      return;
   GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);

   // * draw the background overlaying the pattern outside the arrow area
   GpiSetColor(hps, PmCtrlColor(hwnd, PP_BACKGROUNDCOLOR,
                    PP_BACKGROUNDCOLORINDEX, SYSCLR_DIALOGBACKGROUND, TRUE));
   shift = 1;
   if (!(p->state & IS_HIDDENARROWS)) shift += p->arrow.area * 2;
   clrFgnd = PmCtrlColor(hwnd, PP_FOREGROUNDCOLOR, PP_FOREGROUNDCOLORINDEX,
                         SYSCLR_MENUTEXT, TRUE);
   if (p->state & IS_FOCUSWIN)
   {
      clrLite = clrShade = clrFgnd;
      clrDis = PmCtrlColor(hwnd, PP_BORDERDARKCOLOR, 0, RGB_DARKGRAY, TRUE);
   }
   else
   {
      clrLite = PmCtrlColor(hwnd, PP_BORDERLIGHTCOLOR, 0, RGB_WHITE, TRUE);
      clrShade = PmCtrlColor(hwnd, PP_BORDERDARKCOLOR, 0, RGB_DARKGRAY, TRUE);
      clrDis = clrShade;
   }
   if (p->style & SPLITS_HORIZONTAL)
   {
      pt.x = p->pane.cx - 1, pt.y = p->spltr.thkness - 1;
      GpiBox(hps, DRO_FILL, &pt, 0, 0);
      GpiSetLineType(hps, LINETYPE_ALTERNATE);
      // draw the shade points
      GpiSetColor(hps, clrShade);
      for (pt.y = p->spltr.thkness - 3; pt.y > 0; pt.y -= 2)
         PmDrawHorizontalLine(hps, shift + 1, p->pane.cx - 2, pt.y);
      // draw the lite points
      GpiSetColor(hps, clrLite);
      for (pt.y = p->spltr.thkness - 2; pt.y > 0; pt.y -= 2)
         PmDrawHorizontalLine(hps, shift, p->pane.cx - 3, pt.y);
   }
   else   // vertical bar
   {
      //shift = p->cyPane - shift - 1;
      pt.x = p->spltr.thkness - 1, pt.y = p->pane.cy;
      GpiBox(hps, DRO_FILL, &pt, 0, 0);
      GpiSetLineType(hps, LINETYPE_ALTERNATE);
      // draw the shade points
      GpiSetColor(hps, clrShade);
      for (pt.x = 2; pt.x < p->spltr.thkness - 1; pt.x += 2)
         PmDrawVerticalLine(hps, 1, p->pane.cy - shift - 1, pt.x);
      // draw the lite points
      GpiSetColor(hps, clrLite);
      for (pt.x = 1; pt.x < p->spltr.thkness - 1; pt.x += 2)
         PmDrawVerticalLine(hps, 2, p->pane.cy - shift, pt.x);
   }
   // -- draw the arrow buttons (if required by control style & state)
   if (p->arrow.area && !(p->state & IS_HIDDENARROWS))
   {
      GpiSetLineType(hps, LINETYPE_DEFAULT);
      PmDrawArrowHead(hps, p->arrow.x1, p->arrow.y1, p->arrow.len,
                      clrFgnd, clrDis, p->arrow.fl1);
      PmDrawArrowHead(hps, p->arrow.x2, p->arrow.y2, p->arrow.len,
                      clrFgnd, clrDis, p->arrow.fl2);
   }
   WinEndPaint(hps);
}


/* --------------------------------------------------------------------------
 Handle position changes resulting from splitter dragging by updating the
 position and sizes of the virtual children.
- Parameters -------------------------------------------------------------
 HWND hwnd   : control window handle.
 PSWP pswp   : new window position, size and flags.
 PCTRLDATA p : private control data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID adjustSplitterDrag(HWND hwnd, PSWP pswp, PCTRLDATA p) {
   ULONG update;
   if (p->style & SPLITS_HORIZONTAL)      // rows layout
   {
      update = validateSplitterYPos(pswp->y - p->pane.y,
                                    p, UPD_RESTORE | UPD_COMPAREPOS);
      if (update)
      {
         _mSetTopPanePos(p);
         _mSetBottomPanePos(p);
      }
   }
   else                                   // column layout
   {
      update = validateSplitterXPos(pswp->x - p->pane.x,
                                    p, UPD_RESTORE | UPD_COMPAREPOS);
      if (update)
      {
         _mSetLeftPanePos(p);
         _mSetRightPanePos(p);
      }
   }
   if (update & UPD_ARROWFLAGS) setArrowFlags(p);
   WinInvalidateRect(hwnd, NULL, FALSE);
}


/* --------------------------------------------------------------------------
 The real WC_SPLITPANE coordinates are those of the splitter.
 The programmer, anyway, must interface with the control as if its
 coordinates were those of the virtual rectangle (xPane, yPane, cxPane
 and cyPane CTRLDATA members).
 On size or position change, this procedure:
 - stores the new coordinates of the pane rectangle
 - updates the coordinates of the virtual children
 - updates and stores the new splitter coordinates
 - if the new WC_SPLITPANE size is less than the sum of the minimum width
   or heigth ignore the minimum sizes limit.
- Parameters -------------------------------------------------------------
 HWND hwnd   : control window handle.
 PSWP pswp   : new window position, size and flags.
 PCTRLDATA p : private control data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID adjustPosSize(HWND hwnd, PSWP pswp, PCTRLDATA p) {
   INT pos;
   ULONG update = UPD_NONE;

   if ((pswp->cx <= 0) || (pswp->cy <= 0))
      pswp->cx = pswp->cy = 0, p->state |= IS_NOTVISIBLE;
   else
      p->state &= ~IS_NOTVISIBLE;

   pos = (p->spltr.restore >= 0)? p->spltr.restore : p->spltr.position;
   p->pane.x = pswp->x;
   p->pane.y = pswp->y;

   // horizontal splitter (row division)
   if (p->style & SPLITS_HORIZONTAL)
   {
      // width change : the arrow may be hidden/unhidden according to the width
      if (pswp->cx != p->pane.cx)
      {
         p->pane.cx = pswp->cx;
         if (pswp->cx > 0) setArrowCoords(p);
      }
      // height change: the current or restore position might change
      if (pswp->cy != p->pane.cy)
      {
         p->spltr.range = pswp->cy - p->spltr.thkness;
         if (p->spltr.range > 1)
         {
            if (p->style & SPLITS_FIXED1)
               update = validateSplitterYPos(pos + pswp->cy - p->pane.cy,
                                             p, UPD_NONE);
            else if (p->style & SPLITS_FIXED2)
               update = validateSplitterYPos(pos, p, UPD_NONE);
            else
               update = validateSplitterYPos(
                      ((p->spltr.relpos * pswp->cy) >> 16) - p->spltr.halfthk,
                      p, UPD_SKIPRELPOS);
            if (update & UPD_ARROWFLAGS) setArrowFlags(p);
         }
         else
            p->state |= IS_ONBORDER, setArrowFlags(p);
         p->pane.cy = pswp->cy;
      }
      // set the real splitter coordinates
      pswp->y += p->spltr.position;
      if (pswp->cy > 0) pswp->cy = p->spltr.thkness;
      // update the position of the panes
      _mSetTopPanePos(p);
      _mSetBottomPanePos(p);
   }
   // vertical splitter (column division)
   else
   {
      // height change : the arrow may be hidden/unhidden according to the width
      if (pswp->cy != p->pane.cy)
      {
         p->pane.cy = pswp->cy;
         if (pswp->cx > 0) setArrowCoords(p);
      }
      // width change: the current or restore position might change
      if (pswp->cx != p->pane.cx)
      {
         p->spltr.range = pswp->cx - p->spltr.thkness;
         if (p->spltr.range > 1)
         {
            if (p->style & SPLITS_FIXED1)
               update = validateSplitterXPos(pos, p, UPD_NONE);
            else if (p->style & SPLITS_FIXED2)
               update = validateSplitterXPos(pos + pswp->cx - p->pane.cx,
                                             p, UPD_NONE);
            else
              update = validateSplitterXPos(
                      ((p->spltr.relpos * pswp->cx) >> 16) - p->spltr.halfthk,
                      p, UPD_SKIPRELPOS);
            if (update & UPD_ARROWFLAGS) setArrowFlags(p);
         }
         else
            p->state |= IS_ONBORDER, setArrowFlags(p);
         p->pane.cx = pswp->cx;
      }
      // set the real splitter coordinates
      pswp->x += p->spltr.position;
      if (pswp->cx > 0) pswp->cx = p->spltr.thkness;
      // update the position of the panes
      _mSetLeftPanePos(p);
      _mSetRightPanePos(p);
   }
   pswp->fl |= SWP_MOVE;
   WinInvalidateRect(hwnd, NULL, FALSE);
}


/* --------------------------------------------------------------------------
 Update control data according to data from a SPTLTCTLDATA structure.
 This is called during control creation and on WM_SETWINDOWPARAMS.
 Note: negative values for some members are considered as percent values.
- Parameters -------------------------------------------------------------
 HWND hwnd             : WC_SPLITPANE window handle.
 PSPLITCTLDATA pCtlData : data of the control which are being set.
 PCTRLDATA p      : private control data.
- Return value -----------------------------------------------------------
 ULONG : update flags.
-------------------------------------------------------------------------- */
static
ULONG setWinParms(HWND hwnd, PSPLITCTLDATA pCtlData, PCTRLDATA p) {
   POINTL pt = { 4, 8 } ;
   ULONG update = UPD_NONE;

   p->state |= IS_SETCTLDATA;

   if (pCtlData->flag & SPLITCD_DIALOGUNITS)
      WinMapDlgPoints(WinParent(hwnd), &pt, 1, TRUE);

   if (_mCanAccess(pCtlData, style, SPLITCD_STYLE))
      update |= setStyle(p, pCtlData->style);

   if (_mCanAccess(pCtlData,thickness, SPLITCD_THICKNESS))
      update |= setThickness(p, (INT)pCtlData->thickness, &pt);

   if (_mCanAccess(pCtlData, minLeft, SPLITCD_MINLEFT))
      if (!(p->style & SPLITS_HORIZONTAL))
         update |= setMinPaneSize(&p->min.left, (INT)pCtlData->minLeft,
                                  p->pane.cx, pt.x, 2);

   if (_mCanAccess(pCtlData, minRight, SPLITCD_MINRIGHT))
      if (!(p->style & SPLITS_HORIZONTAL))
         update |= setMinPaneSize(&p->min.right, (INT)pCtlData->minRight,
                                  p->pane.cx, pt.x, 2);

   if (_mCanAccess(pCtlData, minTop, SPLITCD_MINTOP))
      if (p->style & SPLITS_HORIZONTAL)
         update |= setMinPaneSize(&p->min.top, (INT)pCtlData->minTop,
                                  p->pane.cy, pt.y, 3);

   if (_mCanAccess(pCtlData, minBottom, SPLITCD_MINBOTTOM))
      if (p->style & SPLITS_HORIZONTAL)
         update |= setMinPaneSize(&p->min.bottom, (INT)pCtlData->minBottom,
                                  p->pane.cy, pt.y, 3);

   p->state &= ~IS_SETCTLDATA;

   if (update & UPD_SPLITTERPOS) update |= validateSplitter(p, update);

   if (_mCanAccess(pCtlData, splitterPos, SPLITCD_POS))
   {
      // if not a percentage value calculate dialog points
      if (pCtlData->splitterPos > 0)
         pCtlData->splitterPos = (p->style & SPLITS_HORIZONTAL) ?
                                 pt.y * pCtlData->splitterPos / 8:
                                 pt.x * pCtlData->splitterPos / 4;
      // set the position checking the value consistency
      update |= setSplitterPos(p, FALSE, (INT)pCtlData->splitterPos);
   }
   return update;
}


/* --------------------------------------------------------------------------
 Update the layout of the child panes.
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane window handle.
 PCTRLDATA p : private control data.
 ULONG flag  : update flag.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID updateView(HWND hwnd, PCTRLDATA p, ULONG flag) {
   if (p->state & IS_NOTVISIBLE) return;
   if (flag & UPD_LAYOUT)
   {
      if (p->style & SPLITS_ROWS)
      {
         _mSetTopPanePos(p);
         _mSetSplitterHorz(hwnd, p);
         _mSetBottomPanePos(p);
      }
      else
      {
         _mSetLeftPanePos(p);
         _mSetSplitterVert(hwnd, p);
         _mSetRightPanePos(p);
      }
   }

   if (flag & UPD_ARROWCOORD) setArrowCoords(p);
   if ((p->style & SPLITS_HIDEPANES) && (flag & UPD_ARROWFLAGS))
      setArrowFlags(p);

   if ((flag & UPD_PAINT) && WinIsWindowShowing(hwnd))
      WinInvalidateRect(hwnd, NULL, FALSE);
}


/* --------------------------------------------------------------------------
 Return the control data associated with the splitpane
- Parameters -------------------------------------------------------------
 PCTRLDATA p    : private control data.
 PWNDPARAMS pwp : buffer where to write the requested window parameters.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
- Remarks ----------------------------------------------------------------
 The sizefor() macro is defined in axBase.h.
-------------------------------------------------------------------------- */
static
BOOL getWinParms(PCTRLDATA p, PWNDPARAMS pwp) {
   // control text is not supported: just reset the flag
   if ((pwp->fsStatus & WPM_TEXT) || (pwp->fsStatus & WPM_CCHTEXT))
   {
      if (pwp->cchText && pwp->pszText) pwp->pszText = '\0';
      pwp->cchText = 0;
      pwp->fsStatus &= ~(WPM_TEXT | WPM_CCHTEXT);
   }
   // copy the control properties to the SPLITCTLDATA structure
   if (pwp->fsStatus & WPM_CTLDATA)
   {
      switch (pwp->cbCtlData)
      {
      case sizefor(SPLITCTLDATA, style):
         ((PSPLITCTLDATA)pwp->pCtlData)->style = p->style;
      case sizefor(SPLITCTLDATA, minBottom):
         ((PSPLITCTLDATA)pwp->pCtlData)->minBottom = p->min.bottom;
      case sizefor(SPLITCTLDATA, minTop):
         ((PSPLITCTLDATA)pwp->pCtlData)->minTop = p->min.top;
      case sizefor(SPLITCTLDATA, minRight):
         ((PSPLITCTLDATA)pwp->pCtlData)->minRight = p->min.right;
      case sizefor(SPLITCTLDATA, minLeft):
         ((PSPLITCTLDATA)pwp->pCtlData)->minLeft = p->min.left;
      case sizefor(SPLITCTLDATA, thickness):
         ((PSPLITCTLDATA)pwp->pCtlData)->thickness = p->spltr.thkness;
      case sizefor(SPLITCTLDATA, splitterPos):
         ((PSPLITCTLDATA)pwp->pCtlData)->splitterPos = curSplitterPos(p, FALSE);
      case sizefor(SPLITCTLDATA, flag):
         ((PSPLITCTLDATA)pwp->pCtlData)->flag = 0;
      case sizefor(SPLITCTLDATA, cb):
         ((PSPLITCTLDATA)pwp->pCtlData)->cb = sizeof(SPLITCTLDATA);
      }
      pwp->fsStatus &= ~WPM_CTLDATA;
   }
   // set the size of control properties data structure
   if (pwp->fsStatus & WPM_CBCTLDATA)
   {
      pwp->cbCtlData = sizeof(SPLITCTLDATA);
      pwp->fsStatus &= ~WPM_CBCTLDATA;
   }
   return !pwp->fsStatus;
}


/* --------------------------------------------------------------------------
 Handle the keyboard messages:
 VK_HOME, VK_PAGEUP
         hides the left/top pane (if the style allows that) or reduces it to
         to its minimum width/height;
 VK_END, VK_PAGEDOWN
         hides the right/bottom pane (if the style allows that) or reduces it
         to its minimum width/height;
 VK_LEFT, VK_UP
         moves the splitter left/up-ward (step length == p->thickness);
 VK_RIGHT, VK_DOWN
         moves the splitter right/down-ward (step length == p->thickness).
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane control handle.
 PCTRLDATA p : private control data.
 PCHRMSG pch : keyboard event details.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (event processed/dismissed).
-------------------------------------------------------------------------- */
static
BOOL handleKeybEvents(HWND hwnd, PCTRLDATA p, PCHRMSG pch) {
   ULONG update = UPD_NONE;
   switch (pch->vkey)
   {
   case VK_TAB:
      WinSetFocus(HWND_DESKTOP,
                  WinEnumDlgItem(WinOwner(hwnd), hwnd, EDI_NEXTTABITEM));
      return TRUE;
   case VK_BACKTAB:
      WinSetFocus(HWND_DESKTOP,
                  WinEnumDlgItem(WinOwner(hwnd), hwnd, EDI_PREVTABITEM));
      return TRUE;
   case VK_PAGEUP:
      if (p->state & IS_PANE1OUT) return FALSE;
         update = setSplitterPos(p, FALSE,
                           ((p->style & SPLITS_HORIZONTAL) ? p->pane.cy : 0));
      break;
   case VK_PAGEDOWN:
      if (p->state & IS_PANE2OUT) return FALSE;
      update = setSplitterPos(p, FALSE,
                           ((p->style & SPLITS_HORIZONTAL) ? 0 : p->pane.cx));
      break;
   case VK_END:
      update = setSplitterPos(p, FALSE,
                                ((p->state & IS_PANE1HIDDEN) ?
                                   SPLITHIDE_RESTORE : SPLITHIDE_2));
      break;
   case VK_HOME:
      update = setSplitterPos(p, FALSE,
                                ((p->state & IS_PANE2HIDDEN) ?
                                   SPLITHIDE_RESTORE : SPLITHIDE_1));
      break;
   case VK_LEFT:
      if (p->style & SPLITS_HORIZONTAL) return FALSE;
      update = setSplitterPos(p, TRUE, -p->spltr.thkness);
      break;
   case VK_UP:
      if (!(p->style & SPLITS_HORIZONTAL)) return FALSE;
      update = setSplitterPos(p, TRUE, p->spltr.thkness);
      break;
   case VK_RIGHT:
      if (p->style & SPLITS_HORIZONTAL) return FALSE;
      update = setSplitterPos(p, TRUE, p->spltr.thkness);
      break;
   case VK_DOWN:
      if (!(p->style & SPLITS_HORIZONTAL)) return FALSE;
      update = setSplitterPos(p, TRUE, -p->spltr.thkness);
      break;
   default :
      return FALSE;
   }
   if (update) updateView(hwnd, p, update);

   return TRUE;
}


/* --------------------------------------------------------------------------
 On mouse move event change the mouse pointer according to the control
 style, state and mouse position.
 When the splitbar size is less than the clickable area consider the splitter
 borders as valid draggable areas.
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane control handle.
 PCTRLDATA p : private control data.
 INT x, y    : mouse coordinates.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID mouseMove(HWND hwnd, PCTRLDATA p, INT x, INT y) {
   ULONG curstate, prevstate, sysptr, captureArrow;

   curstate = mousePos(p, x, y);
   sysptr = curstate & ~IS_MOUSEOVERARROW;
   curstate &= IS_MOUSEOVERARROW;
   prevstate = p->state & IS_MOUSEOVERARROW;

   // check if any arrow must be emphasized or deemphasized
   if (0 != (captureArrow = p->state & IS_CAPTURED))
   {
      if (curstate ^ prevstate)
      {
         if (prevstate)
         {
            paintArrow(hwnd, p, (prevstate == IS_MOUSEOVER2), FALSE);
            p->state &= ~prevstate;
         }
         else if (_mMouPosToCaptureFlag(curstate) == captureArrow)
         {
            p->state |= curstate;
            paintArrow(hwnd, p, (curstate == IS_MOUSEOVER2), TRUE);
         }
      }  // show default pointer regardless of mouse position when captured
      sysptr = SPTR_ARROW;
   }
   // display the proper mouse pointer
   WinSetSysPointer(curstate ? SPTR_ARROW : sysptr);
}


/* --------------------------------------------------------------------------
 Mouse button down event:
 - if the button is pressed over a hide-pane arrow capture it to get control
   of the next button up event
 - if the button is pressed over the splitter start splitter-drag.
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane control handle.
 PCTRLDATA p : private control data.
 INT x, y    : mouse coordinates.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL mouseBtnDown(HWND hwnd, PCTRLDATA p, INT x, INT y) {
   ULONG curstate;

   curstate = mousePos(p, x, y) & IS_MOUSEOVERARROW;

   // if the mouse was not pressed over an arrow drag the splitter
   if (!curstate) return dragSplitter(hwnd, p, x, y);

   // mouse pressed on a disabled arrow: emit warning sound
   if (
      ((curstate == IS_MOUSEOVER1) && (p->arrow.fl1 & ARWHEAD_DISABLED))
      ||
      ((curstate == IS_MOUSEOVER2) && (p->arrow.fl2 & ARWHEAD_DISABLED))
      )
      return WinAlarm(HWND_DESKTOP, WA_WARNING);

   // update the control state flag and capture the mouse
   if (WinSetCapture(HWND_DESKTOP, hwnd))
   {
      p->state |= curstate | _mMouPosToCaptureFlag(curstate);
      paintArrow(hwnd, p, (curstate == IS_MOUSEOVER2), TRUE);
      return TRUE;
   }
   return FALSE;
}


/* --------------------------------------------------------------------------
 Return a flag reporting the current mouse position (over arrow 1, arrow 2,
 horizontal splitter, vertical splitter).
- Parameters -------------------------------------------------------------
 PCTRLDATA p : private control data.
 INT x, y    : mouse coordinates.
- Return value -----------------------------------------------------------
 ULONG : mouse position flags.
-------------------------------------------------------------------------- */
static
ULONG mousePos(PCTRLDATA p, INT x, INT y) {
   ULONG curstate;

   // set curstate according to the current mouse position
   if (p->style & SPLITS_HORIZONTAL)            // horizontal splitter
   {
      curstate = SPTR_SIZENS;
      if (   !(p->state & IS_HIDDENARROWS)
          && (x > 0) && (y > 0) && (y < (p->spltr.thkness - 1)))
      {
         if (x < p->arrow.area)
            curstate |= IS_MOUSEOVER1;
         else if (x < (p->arrow.area * 2))
            curstate |= IS_MOUSEOVER2;
      }
   }
   else                                         // vertical splitter
   {
      curstate = SPTR_SIZEWE;
      if (   !(p->state & IS_HIDDENARROWS)
          && (y < p->pane.cy) && (x > 0) && (x < (p->spltr.thkness - 1)))
      {
         if (y > (p->pane.cy - p->arrow.area - 1))
            curstate |= IS_MOUSEOVER1;
         else if (y > (p->pane.cy - 1 - p->arrow.area * 2))
            curstate |= IS_MOUSEOVER2;
      }
   }
   return curstate;
}


/* --------------------------------------------------------------------------
 Re-paint a splitter arrow.
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane control handle.
 PCTRLDATA p : private control data.
 BOOL iArrow : arrow index.
 BOOL emph   : TRUE/FALSE (emphasize/deemphasize).
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID paintArrow(HWND hwnd, PCTRLDATA p, BOOL iArrow, BOOL emph) {
   HPS hps;
   LONG clr;
   if (NULLHANDLE != (hps = WinGetPS(hwnd)))
   {
      GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);
      clr = emph ?
            PmCtrlColor(hwnd, PP_BORDERLIGHTCOLOR, 0, RGB_WHITE, TRUE) :
            PmCtrlColor(hwnd, PP_FOREGROUNDCOLOR, PP_FOREGROUNDCOLORINDEX,
                        SYSCLR_MENUTEXT, TRUE);

      if (iArrow)
         PmDrawArrowHead(hps, p->arrow.x2, p->arrow.y2, p->arrow.len,
                         clr, 0, p->arrow.fl2);
      else
         PmDrawArrowHead(hps, p->arrow.x1, p->arrow.y1, p->arrow.len,
                         clr, 0, p->arrow.fl1);
      WinReleasePS(hps);
   }
}


/* --------------------------------------------------------------------------
 Drag the splitter and update the state of the splitpane.
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane control handle.
 PCTRLDATA p : private control data.
 INT x, y    : mouse coordinates.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL dragSplitter(HWND hwnd, PCTRLDATA p, INT x, INT y) {
   TRACKINFO ti;
   BOOL rc = FALSE;
   ULONG style;

   memset(&ti, 0, sizeof(ti));
   ti.fs = TF_MOVE | TF_ALLINBOUNDARY;
   ti.cxBorder = ti.cyBorder = ti.cxKeyboard = ti.cyKeyboard = p->spltr.halfthk;
   ti.rclBoundary.xLeft = p->pane.x;
   ti.rclBoundary.xRight = p->pane.x + p->pane.cx;
   ti.rclBoundary.yBottom = p->pane.y;
   ti.rclBoundary.yTop = p->pane.y + p->pane.cy;
   ti.ptlMaxTrackSize.x = p->pane.cx;
   ti.ptlMaxTrackSize.y = p->pane.cy;
   if (p->style & SPLITS_HORIZONTAL)
   {
      ti.rclTrack.xLeft = p->pane.x;
      ti.rclTrack.xRight = p->pane.x + p->pane.cx;
      ti.rclTrack.yBottom = p->pane.y + p->spltr.position;
      ti.rclTrack.yTop = ti.rclTrack.yBottom + p->spltr.thkness;
      if (p->spltr.range > (p->min.bottom + p->min.top))
      {
         ti.rclBoundary.yTop -= p->min.top;
         ti.rclBoundary.yBottom += p->min.bottom;
      }
   }
   else
   {
      ti.rclTrack.xLeft = p->pane.x + p->spltr.position;
      ti.rclTrack.xRight = ti.rclTrack.xLeft + p->spltr.thkness;
      ti.rclTrack.yBottom = p->pane.y;
      ti.rclTrack.yTop = p->pane.y + p->pane.cy;
      if (p->spltr.range > (p->min.left + p->min.right))
      {
         ti.rclBoundary.xLeft += p->min.left;
         ti.rclBoundary.xRight -= p->min.right;
      }
   }
   // set the splitter drag style (full or framed)
   p->state |= IS_CAPTUREDRAG;
   if (0 == (style = p->style & SPLITS_DRAGFLAGS))
      style = PmValue(SV_FULLWINDOWDRAG) ? SPLITS_DRAGSOLID : SPLITS_DRAGFRAME;
   if (style & SPLITS_DRAGFRAME)
   {
      if (WinTrackRect(WinParent(hwnd), NULLHANDLE, &ti))
      {
         rc = TRUE;
         WinSetWindowPos(hwnd, 0, ti.rclTrack.xLeft, ti.rclTrack.yBottom,
                         0, 0, SWP_MOVE);
         WinInvalidateRect(hwnd, NULL, TRUE);
      }
   }
   else
   {
      if (WinTrackWindow(hwnd, &ti)) rc = TRUE;
   }
   p->state &= ~IS_CAPTUREDRAG;
   return rc;
}


/* --------------------------------------------------------------------------
 Mouse button released event:
 - if the button is released over the same area where it was previously
   pressed executes the appropriate hide/restore pane operation.
- Parameters -------------------------------------------------------------
 HWND hwnd   : splitpane control handle.
 PCTRLDATA p : private control data.
 INT x, y    : mouse coordinates.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
VOID mouseBtnUp(HWND hwnd, PCTRLDATA p, INT x, INT y) {
   ULONG curstate, prevstate, captureArrow;

   curstate = mousePos(p, x, y) & IS_MOUSEOVERARROW;
   prevstate = p->state & IS_MOUSEOVERARROW;
   captureArrow = p->state & IS_CAPTURED;

   if (captureArrow)
   {
      if (WinSetCapture(HWND_DESKTOP, NULLHANDLE))
         p->state &= ~IS_MOUCAPTUREFLAGS;
      // mouse released over the arrow which originated the capture
      if (_mMouPosToCaptureFlag(curstate) == captureArrow)
      {
         if (curstate & IS_MOUSEOVER1)
            curstate = setSplitterPos(p, FALSE,
                                        ((p->state & IS_PANE2HIDDEN) ?
                                           SPLITHIDE_RESTORE : SPLITHIDE_1));
         else
            curstate = setSplitterPos(p, FALSE,
                                        ((p->state & IS_PANE1HIDDEN) ?
                                           SPLITHIDE_RESTORE : SPLITHIDE_2));
         updateView(hwnd, p, curstate);
      }
   }
   // if needed remove the arrow emphasis
   else if (prevstate)
      paintArrow(hwnd, p, (prevstate == IS_MOUSEOVER2), FALSE);
}


/* --------------------------------------------------------------------------
 Return the requested data of the splitpane window.
- Parameters -------------------------------------------------------------
 PCTRLDATA p   : private control data.
 ULONG fsData  : requested data flag.
 ULONG fsScope : scope of the requested data.
 MPARAM mp2    : second message parameter.
- Return value -----------------------------------------------------------
 MRESULT : requested data.
-------------------------------------------------------------------------- */
static
MRESULT getCtlData(PCTRLDATA p, ULONG fsData, ULONG fsScope, MPARAM mp2) {
   switch (fsData)
   {
   // return the IDs of the WC_SPLITPANE virtual children
   case SPLITMP_CHILDREN:
      return MRFROM2SHORT(WinID(p->child.h1), WinID(p->child.h2));
   // return the window handle of a virtual child
   case SPLITMP_CHILDHWND:
      switch (fsScope)
      {
      case SPLITMP_LEFT:
      case SPLITMP_TOP:
         return (MRESULT)p->child.h1;
      case SPLITMP_RIGHT:
      case SPLITMP_BOTTOM:
         return (MRESULT)p->child.h2;
      }
      break;
   // return the rectangle of a child or of the whole splitpane
   case SPLITMP_PANERECT:
      if (!mp2) break;
      if (fsScope == SPLITMP_WHOLEPANE)
      {
         RectSet(((PRECTL)mp2), p->pane.x, p->pane.y,
                 p->pane.x + p->pane.cx, p->pane.y + p->pane.cy);
      }
      else if (p->style & SPLITS_HORIZONTAL)
      {
         ((PRECTL)mp2)->xLeft = p->pane.x;
         ((PRECTL)mp2)->xRight = p->pane.x + p->pane.cx;
         if (fsScope == SPLITMP_PANE1)
            ((PRECTL)mp2)->yBottom
                          = p->pane.y + p->spltr.position + p->spltr.thkness,
            ((PRECTL)mp2)->yTop = p->pane.y + p->pane.cy;
         else
            ((PRECTL)mp2)->yBottom = p->pane.y,
            ((PRECTL)mp2)->yTop = p->pane.y + p->spltr.position;
      }
      else
      {
          ((PRECTL)mp2)->yBottom = p->pane.y;
          ((PRECTL)mp2)->yTop = p->pane.y + p->pane.cy;
         if (fsScope == SPLITMP_PANE1)
            ((PRECTL)mp2)->xLeft = p->pane.x,
            ((PRECTL)mp2)->xRight = p->pane.x + p->spltr.position;
         else
            ((PRECTL)mp2)->xLeft
                          = p->pane.x + p->spltr.position + p->spltr.thkness,
            ((PRECTL)mp2)->xRight = p->pane.x + p->pane.cx;
      }
      return MRTRUE;
   // return the position and size (WPOSSIZE) of a child or of the splitpane
   case SPLITMP_PANEPOSSIZE:
      if (!mp2) break;
      if (fsScope == SPLITMP_WHOLEPANE)
      {
         ((PWPOSSIZE)mp2)->x = p->pane.x;
         ((PWPOSSIZE)mp2)->y = p->pane.y;
         ((PWPOSSIZE)mp2)->cx = p->pane.cx;
         ((PWPOSSIZE)mp2)->cy = p->pane.cy;
      }
      else if (p->style & SPLITS_HORIZONTAL)
      {
         ((PWPOSSIZE)mp2)->x = p->pane.x;
         ((PWPOSSIZE)mp2)->cx = p->pane.cx;
         if (fsScope == SPLITMP_PANE1)
             ((PWPOSSIZE)mp2)->y
                         = p->pane.y + p->spltr.position + p->spltr.thkness,
             ((PWPOSSIZE)mp2)->cy = p->spltr.range - p->spltr.position;
         else
             ((PWPOSSIZE)mp2)->y = p->pane.y,
             ((PWPOSSIZE)mp2)->cy = p->spltr.position;
      }
      else
      {
         ((PWPOSSIZE)mp2)->y = p->pane.y;
         ((PWPOSSIZE)mp2)->cy = p->pane.cy;
         if (fsScope == SPLITMP_PANE1)
            ((PWPOSSIZE)mp2)->x = p->pane.x,
            ((PWPOSSIZE)mp2)->cx = p->spltr.position;
         else
            ((PWPOSSIZE)mp2)->x
                         = p->pane.x + p->spltr.position + p->spltr.thkness,
            ((PWPOSSIZE)mp2)->cx = p->spltr.range - p->spltr.position;
      }
      return MRTRUE;
   // return the position of the splitter axis (unless any pane is collapsed)
   case SPLITMP_BARPOS:
      return (MPARAM)curSplitterPos(p, fsScope);
   case SPLITMP_BARTHICKNESS:
      return (MRESULT)p->spltr.thkness;
   case SPLITMP_MINWIDTHHEIGHT:
      switch (fsScope)
      {
      case SPLITMP_LEFT:       return (MRESULT)p->min.left;
      case SPLITMP_TOP:        return (MRESULT)p->min.top;
      case SPLITMP_RIGHT:      return (MRESULT)p->min.right;
      case SPLITMP_BOTTOM:     return (MRESULT)p->min.bottom;
      }
      break;
   case SPLITMP_STYLE:
      return (MRESULT)p->style;
   }
   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Update a given piece of control data.
- Parameters -------------------------------------------------------------
 HWND hwnd     : splitpane control handle.
 PCTRLDATA p   : private control data.
 ULONG fsData  : requested data flag.
 ULONG fsScope : scope of the requested data.
 MPARAM mp2    : second message parameter.
- Return value -----------------------------------------------------------
 MRESULT : value depending on the value of fsData.
-------------------------------------------------------------------------- */
static
MRESULT setCtlData(HWND hwnd, PCTRLDATA p,
                  ULONG fsData, ULONG fsScope, MPARAM mp2) {

   switch (fsData)
   {
   // complete control initialization passing the IDs of the children
   case SPLITMP_CHILDREN:
      return setChildren(hwnd, p, SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
   // replaces a child with a new one
   case SPLITMP_CHILDHWND:
      // use hwnd to store the handle of the replaced child
      switch (fsScope)
      {
      case SPLITMP_PANE1:
         hwnd = p->child.h1, p->child.h1 = (HWND)mp2;
         _mSetPane1Pos(p);
         break;
      case SPLITMP_PANE2:
         hwnd = p->child.h2, p->child.h2 = (HWND)mp2;
         _mSetPane2Pos(p);
         break;
      default:
         hwnd = NULLHANDLE;
         break;
      }
      return (MRESULT)hwnd;
   // change the splitter position
   case SPLITMP_BARPOS:
      if (UPD_NONE != (fsScope = setSplitterPos(p, fsScope, (SHORT)mp2)))
         updateView(hwnd, p, fsScope);
      return MRTRUE;
   // change the splitterthickness
   case SPLITMP_BARTHICKNESS:
      if (UPD_NONE != (fsScope = setThickness(p, (INT)mp2, NULL)))
         updateView(hwnd, p, fsScope);
      return MRTRUE;
   case SPLITMP_MINWIDTHHEIGHT:
      return (MRESULT)setMinSizes(hwnd, p, fsScope,
                                  (INT)SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
   case SPLITMP_STYLE:
      fsData = p->style & ~((ULONG)mp2 >> 16) | ((ULONG)mp2 & 0xffff);
      fsScope = setStyle(p, fsData);
      updateView(hwnd, p, fsScope);
      return MRTRUE;
   }
   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Update the WC_SPLITPANE style.
- Parameters -------------------------------------------------------------
 PCTRLDATA p : private control data.
 USHORT newStyle  : new style.
- Return value -----------------------------------------------------------
 ULONG : UPDATE_NONE          (no update required)
         UPDATE_PAINT         (repaint the control)
         UPDATE_ORIENTATION   (update orientation)
-------------------------------------------------------------------------- */
static
ULONG setStyle(PCTRLDATA p, USHORT newStyle) {
   USHORT styleChange;
   ULONG update = UPD_NONE;

   styleChange = p->style ^ newStyle;
   p->style = newStyle;

   // hide-pane style change
   if (styleChange & SPLITS_HIDEPANES)
   {
      update |= UPD_PAINT | UPD_ARROWS;
      // restore an hidden pane if the corresponding hide-pane-style is unset
      if (
          (!(p->style & SPLITS_HIDEPANE1) && (p->state & IS_PANE1HIDDEN))
          ||
          (!(p->style & SPLITS_HIDEPANE2) && (p->state & IS_PANE2HIDDEN))
         )
         update |= UPD_RESTORE;
   }

   // orientation change
   if (styleChange & SPLITS_ROWS) update |= UPD_ORIENTATION;

   if ((update & UPD_SPLITTERFLAGS) && !(p->state & IS_SETCTLDATA))
      update |= validateSplitter(p, update);

   return update;
}


/* --------------------------------------------------------------------------
 Update the thickness of the splitter and the splitter range.
- Parameters -------------------------------------------------------------
 PCTRLDATA p   : private control data.
 INT thickness : new splitter thickness.
 PPOINTL ppt   : POINTL structure containing the size of 4 * 8 dialog
                   units in pixel.
- Return value -----------------------------------------------------------
 ULONG : update flag (UPD_*)
-------------------------------------------------------------------------- */
static
ULONG setThickness(PCTRLDATA p, INT thickness, PPOINTL ppt) {
   ULONG update = UPD_NONE;

   if (ppt)
      thickness = (p->style & SPLITS_HORIZONTAL) ?
                  (thickness * ppt->y) / 8 : (thickness * ppt->x) / 4;

   if (thickness < SP_THICKMIN) thickness = SP_THICKMIN;
   else if (thickness > SP_THICKMAX) thickness = SP_THICKMAX;

   if (p->spltr.thkness != thickness)
   {
      update = UPD_PAINT | UPD_LAYOUT | UPD_ARROWCOORD | UPD_THICKNESS;
      p->spltr.range += p->spltr.thkness - thickness;
      p->spltr.thkness = thickness;
      thickness = p->spltr.halfthk;
      p->spltr.halfthk = p->spltr.thkness / 2;
      thickness -= p->spltr.halfthk;  // set to old - new halfthickness delta
      // if halfthickness changed update the splitter position
      if (thickness)
      {
         if (p->spltr.restore >= 0) p->spltr.restore += thickness;
         p->spltr.position += thickness;
         if (!(p->state & IS_SETCTLDATA))
            update |= validateSplitter(p, UPD_THICKNESS);
      }
   }
   return update;
}


/* --------------------------------------------------------------------------
 Set the minimum size of a child pane. Negative values are interpreted as
 percentage of the total splitpane size.
- Parameters -------------------------------------------------------------
 PSHORT psetval : value to be set.
 INT newval     : new minimum size value.
 INT size       : total splitpane size.
 INT mul        : multiplier (used for automatic dialog unit conversion).
 INT shift      : shift factor (used for automatic dialog unit conversion).
- Return value -----------------------------------------------------------
 ULONG : update flag.
-------------------------------------------------------------------------- */
static
ULONG setMinPaneSize(PSHORT psetval, INT newval, INT size, INT mul, INT shift) {
   ULONG update = UPD_NONE;

   // valid values are > -100 && < 0 (percentage) >= 0 absolute value
   if (newval <= -100) return UPD_NONE;

   if (newval < 0)                 // minimum size as percentage
      newval = -newval * size / 100;
   else                            // min height of bottom pane abs
      newval = (mul * newval) >> shift;

   if (newval > (INT)*psetval) update = UPD_MINPOS;
   *psetval = (SHORT)newval;

   return update;
}


/* --------------------------------------------------------------------------
 Return the current splitter position as pixels, percentage or
 SPLITHIDE_* flag.
- Parameters -------------------------------------------------------------
 PCTRLDATA p : private control data.
 BOOL percent     : TRUE/FALSE (value relative to the pane size/absolute).
- Return value -----------------------------------------------------------
 INT value : returned value.
-------------------------------------------------------------------------- */
static
INT curSplitterPos(PCTRLDATA p, BOOL percent) {
   INT pos;
   if (p->style & SPLITS_HORIZONTAL)
   {
      switch (p->state & IS_ANYPANEHIDDEN)
      {
      case IS_PANE1HIDDEN: return percent ? 100  : SPLITHIDE_1;
      case IS_PANE2HIDDEN: return percent ?   0  : SPLITHIDE_2;
      case IS_ONBORDER1:   return percent ? 100  : p->pane.cy;
      case IS_ONBORDER2:   return 0;
      }
   }
   else
   {
      switch (p->state & IS_ANYPANEHIDDEN)
      {
      case IS_PANE1HIDDEN: return percent ?   0  : SPLITHIDE_1;
      case IS_PANE2HIDDEN: return percent ? 100  : SPLITHIDE_2;
      case IS_ONBORDER1:   return 0;
      case IS_ONBORDER2:   return percent ? 100  : p->pane.cx;
      }
   }
   pos = p->spltr.position + p->spltr.halfthk;
   return percent ? (pos * 100 / (p->spltr.range + p->spltr.thkness)) : pos;
}


/* --------------------------------------------------------------------------
 Set the splitter position.
- Parameters -------------------------------------------------------------
 PCTRLDATA p   : private control data.
 BOOL relative : TRUE/FALSE (relative / absolute position).
 INT pos       : new splitter axis position as pixel, percentage or
                 SPLITHIDE_* flag.
- Return value -----------------------------------------------------------
 ULONG : update flag (UPD_*).
-------------------------------------------------------------------------- */
static
ULONG setSplitterPos(PCTRLDATA p, BOOL relative, INT pos) {
   ULONG update = UPD_PAINT;

   switch (pos)
   {
   // hide the left/top pane
   case SPLITHIDE_1:
      // nothing to do if already hidden or not hide-able
      if ((p->state & IS_PANE1HIDDEN) || !(p->style & SPLITS_HIDEPANE1))
         return UPD_NONE;
      p->state = p->state & ~IS_PANE2OUT | IS_PANE1HIDDEN;
      if (p->spltr.restore == -1) p->spltr.restore = p->spltr.position;
      p->spltr.position = (p->style & SPLITS_HORIZONTAL) ? p->spltr.range : 0;
      update |= UPD_LAYOUT | UPD_ARROWFLAGS;
      break;
   // hide the right/bottom pane
   case SPLITHIDE_2:
      // nothing to do if already hidden or not hide-able
      if ((p->state & IS_PANE2HIDDEN) || !(p->style & SPLITS_HIDEPANE2))
         return UPD_NONE;
      p->state = p->state & ~IS_PANE1OUT | IS_PANE2HIDDEN;
      if (p->spltr.restore == -1) p->spltr.restore = p->spltr.position;
      p->spltr.position = (p->style & SPLITS_HORIZONTAL) ? 0 : p->spltr.range;
      update |= UPD_LAYOUT | UPD_ARROWFLAGS;
      break;
   // restore an hidden pane
   case SPLITHIDE_RESTORE:
      // restoring an already restored splitter is an error
      if (p->spltr.restore == -1) return UPD_NONE;
      update |= validateSplitter(p, UPD_RESTORE);
      break;
   // the position is not a collapse/restore flag
   default:
      if (relative)                    // relative position
      {
         if (!pos) return UPD_NONE;
         pos += p->spltr.position + p->spltr.halfthk;
         if (pos < 0) pos = 0;
      }
      if (pos < -100)             // invalid percentage position
         return UPD_NONE;
      else if (pos < 0)                // calculate percentage
      {
         pos = -pos;
         p->spltr.relpos = (pos << 16) / 100;
         pos = (p->spltr.range + p->spltr.thkness) * pos / 100;
      }
      else                             // convert from axis coordinate to
         pos -= p->spltr.halfthk;      // low-left corner coordinate
      update |= (p->style & SPLITS_HORIZONTAL) ?
               validateSplitterYPos(pos, p, UPD_RESTORE):
               validateSplitterXPos(pos, p, UPD_RESTORE);
      break;
   }
   return update;
}


/* --------------------------------------------------------------------------
 This function validates the splitter position according to the current
 control orientation and update flags.
- Parameters -------------------------------------------------------------
 PCTRLDATA p : private control data.
 ULONG flag  : validation flags.
- Return value -----------------------------------------------------------
 ULONG : update flags.
- Remarks ----------------------------------------------------------------
-------------------------------------------------------------------------- */
static
ULONG validateSplitter(PCTRLDATA p, ULONG flag) {
   INT newpos;

   if (p->spltr.restore >= 0)
   {
      if (!(flag & UPD_SPLITTERFLAGS)) return UPD_NONE;
      newpos = p->spltr.restore;
   }
   else
      newpos = p->spltr.position;

   // horizontal splitbar (row division)
   if (p->style & SPLITS_HORIZONTAL)
   {
      if (flag & UPD_ORIENTATION)         // override previous flags
      {
         p->spltr.range = p->pane.cy - p->spltr.thkness;
         newpos = (((0x10000 - p->spltr.relpos) * p->pane.cy) >> 16)
                - p->spltr.halfthk;
      }
      flag = validateSplitterYPos(newpos, p, flag);
   }
   // vertical splitbar (column division)
   else
   {
      if (flag & UPD_ORIENTATION)
      {
         p->spltr.range = p->pane.cx - p->spltr.thkness;
         newpos = (((0x10000 - p->spltr.relpos) * p->pane.cx) >> 16)
                - p->spltr.halfthk;
      }
      flag = validateSplitterXPos(newpos, p, flag);
   }
   return flag;
}


/* --------------------------------------------------------------------------
 This function:
 - checks if the splitter position changed,
 - checks if the splitter is within the valid range
 - optionally restores hidden panes
 - if a pane is hidden updates the restore position and ensures the splitter
   is not moved from the splitpane border.
- Parameters -------------------------------------------------------------
 INT pos      : new splitter position (left/bottom border position).
 PCTRLDATA p  : private control data.
 ULONG flag   : validation flags.
 ULONG flhide : hide flag.
- Return value -----------------------------------------------------------
 ULONG previous state. Return 0xffffffff to skip further validation.
-------------------------------------------------------------------------- */
static
ULONG validateRestorePos(INT pos, PCTRLDATA p, ULONG flag, ULONG flhide) {
   ULONG prevstate = p->state;

   // ignore if position did not change
   if ((flag & UPD_COMPAREPOS) && (pos == p->spltr.position))
      return UPD_SKIPVALIDATION;

   // if there was a previously hidden pane
   if (p->spltr.restore >= 0)
   {
      // ensure new position is within overall splitpane limits
      if (pos < 0)                  pos = 0;
      else if (pos > p->spltr.range) pos = p->spltr.range;
      // restore
      if (flag & UPD_RESTORE)
      {
         p->state &= ~IS_PANEHIDDEN;
         p->spltr.restore = -1;
      }
      // update both the restore and the current position
      else
      {
         p->spltr.restore = pos;
         p->spltr.position = (p->state & flhide) ? 0 : p->spltr.range;
      }
   }
   return prevstate;
}


/* --------------------------------------------------------------------------
 Check if the new x position of the splitter is compatible with the other
 settings and the current state of the control.
 Note: the new splitter position is not due to pane collapsing.
- Parameters -------------------------------------------------------------
 INT pos     : new splitter position (left border position).
 PCTRLDATA p : private control data.
 ULONG flag  : validation flags.
- Return value -----------------------------------------------------------
 ULONG : update flag.
-------------------------------------------------------------------------- */
static
ULONG validateSplitterXPos(INT pos, PCTRLDATA p, ULONG flag) {
   ULONG prevstate;
   ULONG update = UPD_NONE;

   if (p->spltr.range < 1) return UPD_ARROWFLAGS | UPD_PAINT;
   prevstate = validateRestorePos(pos, p, flag, IS_PANE1HIDDEN);
   if (prevstate == UPD_SKIPVALIDATION) return UPD_NONE;

   if (p->spltr.restore == -1)
   {
      // ensure the minimum pane widths are respected
      if (p->spltr.range > (p->min.left + p->min.right))
      {
         if (pos < p->min.left)
            pos = p->min.left;
         else if (pos > (p->spltr.range - p->min.right))
            pos = p->spltr.range - p->min.right;
      }
      // if the new position is on the left/right border set the flags
      p->state &= ~IS_ONBORDER;
      if (pos == 0)
         p->spltr.relpos = 0, p->state|= IS_ONBORDER1;
      else if (pos == p->spltr.range)
         p->spltr.relpos = 0x00010000, p->state |= IS_ONBORDER2;
      else if (!(flag & UPD_SKIPRELPOS))
         p->spltr.relpos = ((pos + p->spltr.halfthk) << 16)
                           / (p->spltr.range + p->spltr.thkness);
      p->spltr.position = pos;
   }
   if ((p->state ^ prevstate) & IS_ANYPANEHIDDEN)
      update |= UPD_ARROWFLAGS | UPD_PAINT;
   if (flag & UPD_ORIENTATION)
      update |= UPD_PAINT | UPD_ARROWCOORD;

   return UPD_LAYOUT | update;
}


/* --------------------------------------------------------------------------
 Check if the new y position of the splitter is compatible with the other
 settings and the current state of the control.
 Note: the new splitter position is not due to pane collapsing.
- Parameters -------------------------------------------------------------
 INT pos     : new splitter position (left border position).
 PCTRLDATA p : private control data.
 ULONG flag  : validation flags.
- Return value -----------------------------------------------------------
 ULONG : update flag.
-------------------------------------------------------------------------- */
static
ULONG validateSplitterYPos(INT pos, PCTRLDATA p, ULONG flag) {
   ULONG prevstate;
   ULONG update = UPD_NONE;

   if (p->spltr.range < 1) return UPD_ARROWFLAGS | UPD_PAINT;
   prevstate = validateRestorePos(pos, p, flag, IS_PANE2HIDDEN);
   if (prevstate == UPD_SKIPVALIDATION) return UPD_NONE;

   if (p->spltr.restore == -1)
   {
      // ensure the minimum pane widths are respected
      if (p->spltr.range > (p->min.bottom + p->min.top))
      {
         if (pos < p->min.bottom)
            pos = p->min.bottom;
         else if (pos > (p->spltr.range - p->min.top))
            pos = p->spltr.range - p->min.top;
      }
      // if the new position is on the top/bottom border set the flags
      p->state &= ~IS_ONBORDER;
      if (pos == 0)
         p->spltr.relpos = 0, p->state |= IS_ONBORDER2;
      else if (pos == p->spltr.range)
         p->spltr.relpos = 0x00010000, p->state |= IS_ONBORDER1;
      else if (!(flag & UPD_SKIPRELPOS))
         p->spltr.relpos = ((pos + p->spltr.halfthk) << 16)
                           / (p->spltr.range + p->spltr.thkness);
      p->spltr.position = pos;
   }
   if ((p->state ^ prevstate) & IS_ANYPANEHIDDEN)
      update |= UPD_ARROWFLAGS | UPD_PAINT;
   if (flag & UPD_ORIENTATION)
      update |= UPD_PAINT | UPD_ARROWCOORD;

   return UPD_LAYOUT | update;
}


/* --------------------------------------------------------------------------
 Update the coordinate and flags needed to draw the arrow used to hide the
 side panes.
- Parameters -------------------------------------------------------------
 PCTRLDATA p : private control data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID setArrowCoords(PCTRLDATA p) {
   INT deflen, curlen, pad, shift;

   p->arrow.fl1 &= ~ARWHEAD_WHERE;
   p->arrow.fl2 &= ~ARWHEAD_WHERE;
   // if any pane is hide-able calculate the arrow coordinates
   if (p->style & SPLITS_HIDEPANES)
   {
      // calculate the arrow length
      deflen = PmValue(SV_CXSIZEBORDER) * 2;
      if (deflen < SP_ARROWMIN) deflen = SP_ARROWMIN;
      p->arrow.area = deflen * 2;
      // the real arrow length depend on the splitter thickness
      if (p->spltr.thkness >= deflen)
         p->arrow.len = deflen - 2,
         pad = (p->spltr.thkness - p->arrow.len) / 2;
      else if (p->spltr.thkness >= (2 + (deflen / 2)))
         pad = 1, p->arrow.len = p->spltr.thkness - 2;
      else
         pad = 0,
         p->arrow.len = p->spltr.thkness
                      - (p->spltr.thkness == 1 + (deflen / 2));

      // horizontal splitter
      if (p->style & SPLITS_HORIZONTAL)
      {
         p->arrow.x1 = deflen - 1;       // arrow tip coordinates
         p->arrow.y1 = pad + p->arrow.len - 1;
         p->arrow.x2 = p->arrow.x1 + p->arrow.area;
         p->arrow.y2 = pad;
         p->arrow.fl1 |= ARWHEAD_UP | ARWHEAD_THICK;
         p->arrow.fl2 |= ARWHEAD_DOWN | ARWHEAD_THICK;
         // hide the arrows when the splitter is shorter than clickable area
         if (p->pane.cx < (p->arrow.area * 2)) p->state |= IS_HIDDENARROWS;
         else p->state &= ~IS_HIDDENARROWS;
      }
      // vertical splitter
      else
      {
         p->arrow.x1 = pad;                    // arrow tip coordinates
         p->arrow.y1 = p->pane.cy - deflen;
         p->arrow.x2 = pad + p->arrow.len - 1;
         p->arrow.y2 = p->arrow.y1 - p->arrow.area;
         p->arrow.fl1 |= ARWHEAD_LEFT | ARWHEAD_THICK;
         p->arrow.fl2 |= ARWHEAD_RIGHT | ARWHEAD_THICK;
         // hide the arrows when the splitter is too short
         if (p->pane.cy < (p->arrow.area * 2)) p->state |= IS_HIDDENARROWS;
         else p->state &= ~IS_HIDDENARROWS;
      }
   }
   else
      p->arrow.area = 0;
}


/* --------------------------------------------------------------------------
 Update the disabled state of the splitter arrows.
- Parameters -------------------------------------------------------------
 PCTRLDATA p : private control data.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID setArrowFlags(PCTRLDATA p) {

   if (p->style & SPLITS_HIDEPANES)
   {
      if (p->spltr.range < 1)
      {
         p->arrow.fl1 |= ARWHEAD_DISABLED;
         p->arrow.fl2 |= ARWHEAD_DISABLED;
      }
      else
      {
         p->arrow.fl1 &= ~ARWHEAD_DISABLED;
         p->arrow.fl2 &= ~ARWHEAD_DISABLED;

         // check if according to the current style and state arrow must be disabled
         if (p->state & IS_PANE1HIDDEN)
            p->arrow.fl1 |= ARWHEAD_DISABLED;
         else if (p->state & IS_ONBORDER1)
         {
            p->arrow.fl1 |= ARWHEAD_DISABLED;
            if (!(p->style & SPLITS_HIDEPANE2))
               p->arrow.fl2 |= ARWHEAD_DISABLED;
         }
         else if (p->state & IS_PANE2HIDDEN)
            p->arrow.fl2 |= ARWHEAD_DISABLED;
         else if (p->state & IS_ONBORDER2)
         {
            p->arrow.fl2 |= ARWHEAD_DISABLED;
            if (!(p->style & SPLITS_HIDEPANE1))
               p->arrow.fl1 |= ARWHEAD_DISABLED;
         }
         else if (!(p->style & SPLITS_HIDEPANE1))
            p->arrow.fl1 |= ARWHEAD_DISABLED;
         else if (!(p->style & SPLITS_HIDEPANE2))
            p->arrow.fl2 |= ARWHEAD_DISABLED;
      }
   }
}


/* --------------------------------------------------------------------------
 Sets the IDs of the WC_SPLITPANE virtual children.
- Parameters -------------------------------------------------------------
 HWND hwnd       : splitpane window handle.
 PCTRLDATA p     : private control data.
 ULONG id1, id2  : children IDs.
- Return value -----------------------------------------------------------
 MRESULT (BOOL) : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
MRESULT setChildren(HWND hwnd, PCTRLDATA p, ULONG id1, ULONG id2) {
   WPOSSIZE awps[2];
   // get the handles of the children
   if (id1)
      p->child.h1 = DlgItemHandle(WinParent(hwnd), id1);
   if (id2)
      p->child.h2 = DlgItemHandle(WinParent(hwnd), id2);
   // update the position and size of the virtual children
   // splitpane divided into rows (horizontal splitter)
   if (p->style & SPLITS_HORIZONTAL)
   {
      awps[0].x = p->pane.x;
      awps[0].y = p->pane.y + p->spltr.position + p->spltr.thkness;
      awps[0].cx = p->pane.cx;
      awps[0].cy = p->spltr.range - p->spltr.position;
      awps[1].x = p->pane.x;
      awps[1].y = p->pane.y;
      awps[1].cx = p->pane.cx;
      awps[1].cy = p->spltr.position;
   }
   // splitpane divided into columns (vertical splitter)
   else
   {
      awps[0].x = p->pane.x;
      awps[0].y = p->pane.y;
      awps[0].cx = p->spltr.position;
      awps[0].cy = p->pane.cy;
      awps[1].x = p->pane.x + p->spltr.position + p->spltr.thkness;
      awps[1].y = p->pane.y;
      awps[1].cx = p->spltr.range - p->spltr.position;
      awps[1].cy = p->pane.cy;
   }
   // update the position of the virtual children
   if (p->child.h1)
      WinSetPosSize(p->child.h1, NULLHANDLE, awps, SWP_SIZE | SWP_MOVE);
   if (p->child.h2)
      WinSetPosSize(p->child.h2, NULLHANDLE, awps + 1, SWP_SIZE | SWP_MOVE);

   return (MRESULT)(p->child.h1 && p->child.h2);
}


/* --------------------------------------------------------------------------
 Update the minimum size of a pane.
- Parameters -------------------------------------------------------------
 HWND hwnd   : window handle.
 PCTRLDATA p : private control data.
 ULONG scope : type of size to be set.
 INT size    : minimum size to set.
 BOOL bdu    : TRUE     size is in dialog units
               FALSE    size is pixels or percent.
- Return value -----------------------------------------------------------
  BOOL : TRUE/FALSE (success/error).
-------------------------------------------------------------------------- */
static
BOOL setMinSizes(HWND hwnd, PCTRLDATA p, ULONG scope, INT size, BOOL bdu) {
   POINTL pt = { 4, 8 };
   ULONG update = UPD_NONE;

   if (bdu) WinMapDlgPoints(WinParent(hwnd), &pt, 1, TRUE);

   if (size <= -100) return FALSE;
   switch (scope)
   {
   case SPLITMP_LEFT:
      update = setMinPaneSize(&p->min.left, size, p->pane.cx, pt.x, 2);
      break;
   case SPLITMP_TOP:
      update = setMinPaneSize(&p->min.top, size, p->pane.cy, pt.y, 3);
      break;
   case SPLITMP_RIGHT:
      update = setMinPaneSize(&p->min.right, size, p->pane.cx, pt.x, 2);
      break;
   case SPLITMP_BOTTOM:
      update = setMinPaneSize(&p->min.bottom, size, p->pane.cy, pt.y, 3);
      break;
   }
   if (update) updateView(hwnd, p, validateSplitter(p, update));

   return TRUE;
}


/* --------------------------------------------------------------------------
 Update the position and size of a child pane.
- Parameters -------------------------------------------------------------
 HWND hpane  : pane window handle.
 LONG x,y    : pane coordinate.
 LONG cx, cy : pane width and height.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
static
VOID setPaneSizePos(HWND hpane, LONG x, LONG y, LONG cx, LONG cy) {
   WinSetWindowPos(hpane, NULLHANDLE, x, y, cx, cy,
                   SWP_SIZE | SWP_MOVE);
}
