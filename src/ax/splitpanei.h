/* --------------------------------------------------------------------------
 splitpanei.h : private definitions of constants, data structures and
                macros used by splitpane.c

 11 May 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _SPLITPANEI_H_
   #define _SPLITPANEI_H_


// WC_SPLITPANE class styles
#define CS_SPLITPANE    (CS_SIZEREDRAW | CS_SYNCPAINT)
// #define CS_SPLITPANE    (CS_SIZEREDRAW | CS_SYNCPAINT | CS_PUBLIC)


#pragma pack(1)
// minimum heights and widths of the child panes
typedef struct {
   SHORT left, right, top, bottom;
} SPLITMIN, * PSPLITMIN;

typedef struct {
   SHORT x1,          // x coordinate of the tip of the first arrow
         y1,          // y coordinate of the tip of the first arrow
         x2,          // x coordinate of the tip of the second arrow
         y2;          // y coordinate of the tip of the second arrow
   UCHAR fl1,         // first arrow flags
         fl2,         // second arrow flags
         len,         // arrow length
         area;        // width of the clickable area (total is area * 2)
} ARROW, * PARROW;

typedef struct {
   HWND h1, h2;
} CHILDREN, * PCHILDREN;

// the meaning of the members depend on the current splitter orientation
typedef struct {
   SHORT position;    // splitter position relative to the splitpane
                      // left/bottom border
   SHORT restore;     // splitter restore position
   ULONG relpos;      // relative position as (0xffff-ths of its range)
   SHORT range;       // splitter motion range (width/height - thickness)
   UCHAR thkness;     // splitter spltr.thkness
   UCHAR halfthk;     // splitter spltr.thkness / 2
} SPLITTER, * PSPLITTER;

typedef struct {
   CHILDREN child;    // children window handles
   WPOSSIZE pane;     // splitpane position and size
   SPLITTER spltr;    // splitter data
   SPLITMIN min;      // minimum sizes of the child panes
   ARROW arrow;       // coordinates and styles of the hide-pane arrows
   ULONG style;       // control style (SPLITS_* flags)
   ULONG state;       // control state
} CTRLDATA, * PCTRLDATA;

#pragma pack()


/* ---------------------------------------------------------------------------
                              CONSTANTS
*/
// splitter spltr.thkness limits
#define SP_THICKMIN           3            // minimum splitter spltr.thkness
#define SP_THICKMAX         255            // maximum splitter spltr.thkness
#define SP_ARROWMIN           6            // minimum default arrow length


// control update flags used on window creation and on WM_SETWINDOWPARAMS
#define UPD_NONE              0x00000000
#define UPD_LAYOUT            0x00000001 // reset the panes sizes and positions
#define UPD_PAINT             0x00000002 // refresh the splitter
#define UPD_ARROWCOORD        0x00000004 // update the arrow coordinates
#define UPD_ARROWFLAGS        0x00000008 // update the arrow enable state
// arrow update flags
#define UPD_ARROWS            (UPD_ARROWCOORD | UPD_ARROWFLAGS)
#define UPD_RESTORE           0x00010000 // must restore hidden panes
#define UPD_ORIENTATION       0x00020000 // orientation change
#define UPD_COMPAREPOS        0x00040000 // check splitter pos change
#define UPD_THICKNESS         0x00080000 // update splitter thickness
#define UPD_MINPOS            0x00100000 // update relative to minimum pos.
#define UPD_SKIPRELPOS        0x00200000 // skip relative pos. update
#define UPD_SPLITTERFLAGS     (UPD_RESTORE | UPD_ORIENTATION | UPD_THICKNESS)
#define UPD_SPLITTERPOS       (UPD_MINPOS | UPD_SPLITTERFLAGS)
#define UPD_SKIPVALIDATION    0xffffffff

// state flags --------------------------------------------------------------

// pane hidden (collapsed) via the hide-pane arrows
#define IS_PANE1HIDDEN     0x00000001    // left/top pane collapsed
#define IS_PANE2HIDDEN     0x00000002    // right/bottom pane collapsed
#define IS_HIDDENLEFT      IS_PANE1HIDDEN
#define IS_HIDDENTOP       IS_PANE1HIDDEN
#define IS_HIDDENRIGHT     IS_PANE2HIDDEN
#define IS_HIDDENBOTTOM    IS_PANE2HIDDEN
#define IS_PANEHIDDEN      (IS_PANE1HIDDEN | IS_PANE2HIDDEN)

// pane hidden via splitter drag (or via SPLITMP_BARPOS absolute position)
#define IS_ONBORDER1       0x00000010  // splitter moved on left/top border
#define IS_ONBORDER2       0x00000020  // splitter moved on right/bottom bord.
#define IS_ONLEFTBORDER    IS_ONBORDER1
#define IS_ONTOPBORDER     IS_ONBORDER2
#define IS_ONRIGHTBORDER   IS_ONBORDER1
#define IS_ONBOTTOMBORDER  IS_ONBORDER2
#define IS_ONBORDER        (IS_ONBORDER1 | IS_ONBORDER2) // splitter on any border

// combined flags
#define IS_PANE1OUT        (IS_PANE1HIDDEN | IS_ONBORDER1)
#define IS_PANE2OUT        (IS_PANE2HIDDEN | IS_ONBORDER2)
#define IS_ONLEFT          IS_PANE1OUT
#define IS_ONRIGHT         IS_PANE2OUT
#define IS_ONTOP           IS_PANE1OUT
#define IS_ONBOTTOM        IS_PANE2OUT
#define IS_ANYPANEHIDDEN   (IS_PANEHIDDEN | IS_ONBORDER)

// validate splitter position flags
#define IS_SETCTLDATA      0x00000400    // suspend validation

// mouse capture flags:
#define IS_NOTCAPTURED     0x00000000   // mouse not captured
#define IS_CAPTURE1        0x00010000   // click on left/top-most arrow
#define IS_CAPTURE2        0x00020000   // click on right/bottom-most arrow
#define IS_CAPTURED        (IS_CAPTURE1 | IS_CAPTURE2)
// mouse position flags
#define IS_MOUSEOVER1      0x00040000
#define IS_MOUSEOVER2      0x00080000
#define IS_MOUSEOVERARROW  (IS_MOUSEOVER1 | IS_MOUSEOVER2)
// conversion from mouse position flag to mouse capture flag
// modify this when changing the value of the above flags
#define _mMouPosToCaptureFlag(_flag_)    ((_flag_) >> 2)
#define IS_MOUCAPTUREFLAGS (IS_CAPTURED | IS_MOUSEOVERARROW)

#define IS_CAPTUREDRAG     0x00100000   // mouse captured by the splitter drag
#define IS_CAPTUREANY      (IS_CAPTURED | IS_CAPTUREDRAG)

// hide the arrows when the splitter is too small
#define IS_HIDDENARROWS    0x01000000   // the splitter is too short to
                                        // display the arrows
#define IS_FOCUSWIN        0x02000000   // the control has the focus

// when the control size is 0 set this flag to skip unneeded processing
#define IS_NOTVISIBLE      0x80000000

/* ---------------------------------------------------------------------------
                             function-like macros
*/

/* ---------------------------------------------------------------------------
 get inner control data from the window words
*/
#define _mWndData(_hwnd_) \
   ((PCTRLDATA)WinQueryWindowPtr((_hwnd_), QWL_CONTROL))

/* ---------------------------------------------------------------------------
 check if according to the current size and flags of a SPLITCTLDATA structure
 a property can be accessed.
*/

#define _mCanAccess(_pscd_, _member_, _flag_)           \
   (                                                    \
   ((_pscd_)->cb >= sizefor(SPLITCTLDATA, _member_))    \
   &&                                                   \
   ((_pscd_)->flag & (_flag_))                          \
   )

/* --------------------------------------------------------------------------
 update the position of the splitter
*/
#define _mSetSplitterHorz(_hwnd_, _p_)                                \
   (WinSetWindowPos((_hwnd_), NULLHANDLE,                             \
                    (_p_)->pane.x, p->pane.y + (_p_)->spltr.position, \
                    (_p_)->pane.cx, (_p_)->spltr.thkness,             \
                    SWP_SIZE | SWP_MOVE | SWP_NOADJUST))

#define _mSetSplitterVert(_hwnd_, _p_)                                \
   (WinSetWindowPos((_hwnd_), NULLHANDLE,                             \
                    (_p_)->pane.x + (_p_)->spltr.position, p->pane.y, \
                    (_p_)->spltr.thkness, (_p_)->pane.cy,             \
                    SWP_SIZE | SWP_MOVE | SWP_NOADJUST))


/* --------------------------------------------------------------------------
 macro to update the position, size and visibility of the child panes
*/
#define _mSetLeftPanePos(_p_)                                         \
   if ((_p_)->child.h1)                                               \
      setPaneSizePos(p->child.h1, p->pane.x, p->pane.y, p->spltr.position, \
                     p->pane.cy)

#define _mSetTopPanePos(_p_)                                          \
   if ((_p_)->child.h1)                                               \
      setPaneSizePos((_p_)->child.h1, (_p_)->pane.x,                  \
           (_p_)->pane.y + (_p_)->spltr.position + (_p_)->spltr.thkness, \
           (_p_)->pane.cx,                                  \
           (_p_)->pane.cy - (_p_)->spltr.position - (_p_)->spltr.thkness)

#define _mSetRightPanePos(_p_)                                        \
   if ((_p_)->child.h2)                                               \
      setPaneSizePos((_p_)->child.h2,                                 \
           (_p_)->pane.x + (_p_)->spltr.position + (_p_)->spltr.thkness, \
           (_p_)->pane.y,                                   \
           (_p_)->pane.cx - (_p_)->spltr.position - (_p_)->spltr.thkness,\
           (_p_)->pane.cy)

#define _mSetBottomPanePos(_p_)                                       \
   if ((_p_)->child.h2)                                               \
      setPaneSizePos((_p_)->child.h2, (_p_)->pane.x, (_p_)->pane.y,   \
                     (_p_)->pane.cx, (_p_)->spltr.position)


#define _mSetPane1Pos(_p_)                                            \
   if ((_p_)->style & SPLITS_HORIZONTAL)                              \
      _mSetTopPanePos(_p_); else _mSetLeftPanePos(_p_)

#define _mSetPane2Pos(_p_)                                            \
   if ((_p_)->style & SPLITS_HORIZONTAL)                              \
      _mSetBottomPanePos(_p_); else _mSetRightPanePos(_p_)



// allocation
#define memAlloc malloc
#define memFree  free


/* ---------------------------------------------------------------------------
                                  prototypes
*/
MRESULT EXPENTRY pmxSplitPaneProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
VOID setCtrlData(PSPLITCTLDATA pCtlData, PCTRLDATA p);

#endif // #ifndef _SPLITPANEI_H_
