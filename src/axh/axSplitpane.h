/* --------------------------------------------------------------------------
 splitpane.h :

 28 Apr 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------
 - splitpane styles
 - splitpane messages
   - splitpane message parameters
 - splitpane macros
 - splitpane control data structure
-------------------------------------------------------------------------- */


#ifndef _SPLITPANE_H_
   #define _SPLITPANE_H_

#define WC_SPLITPANE           "WC_SPLITPANE"
// #define WC_SPLITPANE           ((PSZ)0xffff0100L)

// splitpane styles ---------------------------------------------------------

// Orientation style
#define SPLITS_COLUMNS          0x00000000    // divide into 2 columns
#define SPLITS_ROWS             0x00000001    // divide into 2 rows
#define SPLITS_VERTICAL         SPLITS_COLUMNS // vertical splitter
#define SPLITS_HORIZONTAL       SPLITS_ROWS    // horizontal splitter

// Collapsable style: panes can be hidden by clicking on bar arrows
#define SPLITS_HIDENONE         0x00000000
#define SPLITS_HIDEPANE1        0x00000010
#define SPLITS_HIDEPANE2        0x00000020
#define SPLITS_HIDELEFT         SPLITS_HIDEPANE1
#define SPLITS_HIDETOP          SPLITS_HIDEPANE1
#define SPLITS_HIDERIGHT        SPLITS_HIDEPANE1
#define SPLITS_HIDEBOTTOM       SPLITS_HIDEPANE2
#define SPLITS_HIDEPANES        (SPLITS_HIDEPANE1 | SPLITS_HIDEPANE2)

// Mutually exclusive styles regarding the behaviour on splitpane size change:
// the splitter new position is proportional
#define SPLITS_PROPORTIONAL     0x00000000
// the left pane width or top pane height is fixed
#define SPLITS_FIXED1           0x00000100
#define SPLITS_FIXED2           0x00000200
#define SPLITS_FIXEDLEFT        SPLITS_FIXED1
#define SPLITS_FIXEDTOP         SPLITS_FIXED1
#define SPLITS_FIXEDRIGHT       SPLITS_FIXED2
#define SPLITS_FIXEDBOTTOM      SPLITS_FIXED2
#define SPLITS_SIZECHANGE       (SPLITS_FIXED1 | SPLITS_FIXED2)

// Splitbar drag behaviour:
#define SPLITS_DRAGSYSTEM       0x00000000 // Full window drag system settings
#define SPLITS_DRAGSOLID        0x00001000 // redraw the panes as the bar is moved
#define SPLITS_DRAGFRAME        0x00002000 // redraw the panes once the bar stops

#define SPLITS_DRAGFLAGS        (SPLITS_DRAGSOLID | SPLITS_DRAGFRAME)


// --------- SPLITPANE MESSAGES AND MESSAGE PARAMETERS DEFINITIONS -----------

// message to get WC_SPLITPANE style, properties, state, etc.
#define SPLITM_QUERY           0xafc0   // query splitpane properties

// message to set WC_SPLITPANE style, properties, state, etc.
#define SPLITM_SET             0xafc1   // set splitpane properties

/* ---------------------------------------------------------------------------
 Message parameter 1 first SHORT (scope of SPLITM_QUERY/SPLITM_SET message)
*/
#define SPLITMP_CHILDREN         0     // set/get the IDs of the virtual children
#define SPLITMP_CHILDHWND        1     // set/get the handle o f a virtual child
#define SPLITMP_PANERECT         2     // get window coordinates
#define SPLITMP_PANEPOSSIZE      3     // get window position and size
#define SPLITMP_BARPOS           4     // set/get splitter position
#define SPLITMP_BARTHICKNESS     5     // set/get splitter thickness
#define SPLITMP_MINWIDTHHEIGHT   6     // set/get minimum pane width/height
#define SPLITMP_STYLE            7     // set/get WC_SPLITPANE style

/* ---------------------------------------------------------------------------
 Message parameter 1 second SHORT (flags, and WC_SPLITPANE component scope)
*/
// the following flags specify the scope of the requested window handle,
// coordinates, current sizes
#define SPLITMP_WHOLEPANE        0     // data relative to the whole splitpane
#define SPLITMP_PANE1            1     // left/bottom pane data
#define SPLITMP_PANE2            2     // right/top pane data
// the following flags specify the scope of the requested minimum pane
// size:
#define SPLITMP_LEFT             3     // left pane minimum width
#define SPLITMP_TOP              4     // top pane minimum height
#define SPLITMP_RIGHT            5     // right pane minimum width
#define SPLITMP_BOTTOM           6     // bottom pane minimum height

/* ---------------------------------------------------------------------------
 Splitbar position : special values.
*/
#define SPLITHIDE_1         -32768
#define SPLITHIDE_2         -32767
#define SPLITHIDE_RESTORE   -32766
#define SPLITHIDE_LEFT      SPLITHIDE_1
#define SPLITHIDE_TOP       SPLITHIDE_1
#define SPLITHIDE_RIGHT     SPLITHIDE_2
#define SPLITHIDE_BOTTOM    SPLITHIDE_2

/* ---------------------------------------------------------------------------
 This macro gets the ID of both the virtual children of the splitpane.

 mp1: SPLITMP_CHILDREN
 mp2: reserved (0).

 mr : SHORT1FROMMR : id of the left/top child
      SHORT2FROMMR : id of the right/bottom child

*/
#define wSplitChildrenIDs(_hwnd_)                                   \
  (WinSendMsg((_hwnd_), SPLITM_QUERY,                               \
                     MPFROM2SHORT(SPLITMP_CHILDREN, 0),             \
                     MPVOID))

#define dSplitChildrenIDs(_hwndDlg_, _splitId_)                     \
  (WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY,        \
                     MPFROM2SHORT(SPLITMP_CHILDREN, 0),             \
                     MPVOID))


/* ---------------------------------------------------------------------------
 This macro sets the ID of both the virtual children of the splitpane.
 This should typically be used during WM_INITDLG processing.
 Both children must have already been created when this macro is called.

 mp1: SPLITMP_CHILDREN
 mp2: SHORT : left/top child id
      SHORT : right/bottom child id

 mr : TRUE in case of success.
      FALSE in case of invalid ID(s).
*/
#define wSplitSetChildren(_hwnd_, _idLeftTop_, _idRightBottom_)   \
  ((BOOL)WinSendMsg((_hwnd_), SPLITM_SET,                          \
                    MPFROM2SHORT(SPLITMP_CHILDREN, 0),             \
                    MPFROM2SHORT((_idLeftTop_), (_idRightBottom_))))

#define dSplitSetChildren(_hwndDlg_, _splitId_, _idLeftTop_, _idRightBottom_)\
  ((BOOL)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_SET,   \
                    MPFROM2SHORT(SPLITMP_CHILDREN, 0),             \
                    MPFROM2SHORT((_idLeftTop_), (_idRightBottom_))))


/* ---------------------------------------------------------------------------
 This macro returns the handle of a virtual child of a WC_SPLITPANE control.

 mp1: SHORT : SPLITMP_CHILDHWND
      SHORT : child position identifier.
              SPLITMP_PANE1   left/top child.
              SPLITMP_PANE2   right/bottom child.
 mp2: reserved (0).

 mr : handle of the requested virtual child.
*/
#define wSplitChildHandle(_hwnd_, _childPos_)                    \
  ((HWND)WinSendMsg((_hwnd_), SPLITM_QUERY,                       \
                    MPFROM2SHORT(SPLITMP_CHILDHWND, (_childPos_)),\
                    MPVOID))

#define dSplitChildHandle(_hwndDlg_, _splitId_, _childPos_)      \
  ((HWND)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY,\
                    MPFROM2SHORT(SPLITMP_CHILDHWND, (_childPos_)),\
                    MPVOID))


/* ---------------------------------------------------------------------------
 This macro replaces a virtual child of a WC_SPLITPANE with a new
 dialog control returning the handle of the replaced child.
 It is care of the programmer to destroy the replaced child.

 mp1: SHORT : SPLITMP_CHILDHWND
      SHORT : child position identifier.
              SPLITMP_PANE1   left/top child.
              SPLITMP_PANE2   right/bottom child.
 mp2: HWND  : new child handle.

 mr : handle of the replaced virtual child.
*/
#define wSplitSetChildHandle(_hwnd_, _childPos_, _newChildHwnd_) \
  ((HWND)WinSendMsg((_hwnd_), SPLITM_SET,                         \
                    MPFROM2SHORT(SPLITMP_CHILDHWND, (_childPos_)),\
                    (MPARAM)(_newChildHwnd_)))

#define dSplitSetChildHandle(_hwndDlg_, _splitId_, _childPos_, _newChildHwnd_) \
  ((HWND)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_SET,   \
                    MPFROM2SHORT(SPLITMP_CHILDHWND, (_childPos_)), \
                    (MPARAM)(_newChildHwnd_)))


/* ---------------------------------------------------------------------------
 This macro queries the coordinates of a WC_SPLITPANE control or of one of
 its children.

 mp1: SHORT : SPLITMP_PANERECT
      SHORT : scope of the query.
              SPLITMP_WHOLEPANE   splitpane coordinates.
              SPLITMP_PANE1       left/top child coordinates..
              SPLITMP_PANE2       right/bottom child coordinates..
 mp2: PRECTL : address of a rectangle structure for the requested
               coordinates.

 mr : BOOL : TRUE/FALSE (success/error).

 Remarks:

 WinSetWindowPos() is used to change the position and size of the virtual
 WC_SPLITPANE rectangle.

 wSplitterSetPos() and dSplitterSetPos() are used to change the sizes of
 both the virtual children.

*/
#define wSplitQueryRectl(_hwnd_, _flag_, _prect_)                 \
  ((BOOL)WinSendMsg((_hwnd_), SPLITM_QUERY,                       \
                    MPFROM2SHORT(SPLITMP_PANERECT, (_flag_)),     \
                    (MPARAM)(_prect_)))

#define dSplitQueryRectl(_hwndDlg_, _splitId_, _flag_, _prect_)   \
  ((BOOL)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY,\
                    MPFROM2SHORT(SPLITMP_PANERECT, (_flag_)),     \
                    (MPARAM)(_prect_)))


/* ---------------------------------------------------------------------------
 This macro queries the position and size of a WC_SPLITPANE control or of
 one of its children.

 mp1: SHORT : SPLITMP_PANEPOSSIZE
      SHORT : scope of the query.
              SPLITMP_WHOLEPANE   splitpane coordinates.
              SPLITMP_PANE1       left/top child coordinates..
              SPLITMP_PANE2       right/bottom child coordinates..
 mp2: PWPOSSIZE : address of a position-size structure for the requested
              data.

 mr : BOOL : TRUE/FALSE (success/error).

 Remarks:
 WinSetWindowPos() is used to change the position and size of the virtual
 WC_SPLITPANE rectangle.
 wSplitterSetPos() and dSplitterSetPos() are used to change the sizes of
 both the virtual children.
*/
#define wSplitQueryPosSize(_hwnd_, _flag_, _ppossize_)            \
  ((BOOL)WinSendMsg((_hwnd_), SPLITM_QUERY,                       \
                    MPFROM2SHORT(SPLITMP_PANEPOSSIZE, (_flag_)),  \
                    (MPARAM)(_ppossize_)))

#define dSplitQueryPosSize(_hwndDlg_, _splitId_, _flag_, _ppossize_) \
  ((BOOL)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY,   \
                    MPFROM2SHORT(SPLITMP_PANEPOSSIZE, (_flag_)),     \
                    (MPARAM)(_ppossize_)))


/* ---------------------------------------------------------------------------
 This macro returns the position of the splitter axis.
 Depending on the current division style (columns or rows), the position
 is the distance in pixels from the left or bottom boundary of the
 WC_SPLITPANE virtual rectangle.

 mp1: SHORT : SPLITMP_BARPOS
      SHORT : TRUE   position as percentage of the splitpane width/height.
              FALSE  absolute position.

 mp2: reserved (0).

 mr : distance of the splitter axis from the left/bottom border or:
      SPLITHIDE_1       when the left/top pane is hidden.
      SPLITHIDE_2       when the right/bottom pane is hidden.
      0                 when the splitter is onto the left/bottom border.
      splitpane width   when the splitter is onto the right border.
      splitpane height  when the splitter is onto the top border.
      100               when the splitter is onto the right/top border and
                        the position has been queried as percentage.
*/
#define wSplitterPos(_hwnd_, _flag_)                               \
  ((INT)WinSendMsg((_hwnd_), SPLITM_QUERY,                         \
                    MPFROM2SHORT(SPLITMP_BARPOS, (_flag_)),        \
                    MPVOID))

#define dSplitterPos(_hwndDlg_, _splitId_, _flag_)                 \
  ((INT)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY,  \
                    MPFROM2SHORT(SPLITMP_BARPOS, (_flag_)),        \
                    MPVOID))


/* ---------------------------------------------------------------------------
 This macro sets the position of the splitter axis.
 Depending on the current division style (columns or rows), the position
 is the distance in pixels from the left or bottom boundary of the
 WC_SPLITPANE virtual rectangle.

 mp1: SHORT : SPLITMP_BARPOS
      SHORT : TRUE : new position is relative to the current one.
              FALSE : absolute position.

 mp2: SHORT : new splitter position:
              Positive values are interpreted as pixels

              Negative values, greater than -100, are interpreted as
              percentage (e.g. -25 sets the splitter position to one fourth
              of the splitpane width/height).

              When a SPLITHIDE_* flag is specified the value of the
              second short of message parameter 1 is ignored.

 mr : BOOL : TRUE/FALSE (position changed/unchanged).
*/
#define wSplitterSetPos(_hwnd_, _flag_, _newPos_)                 \
  ((BOOL)WinSendMsg((_hwnd_), SPLITM_SET,                         \
                    MPFROM2SHORT(SPLITMP_BARPOS, (_flag_)),       \
                    (MPARAM)(_newPos_)))

#define dSplitterSetPos(_hwndDlg_, _splitId_, _flag_, _newPos_)   \
  ((BOOL)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_SET,  \
                    MPFROM2SHORT(SPLITMP_BARPOS, (_flag_)),       \
                    (MPARAM)(_newPos_)))


/* ---------------------------------------------------------------------------
 This macro returns the thickness of the splitter.

 mp1: SHORT : SPLITMP_BARTHICKNESS
      SHORT : reserved (0).

 mp2: reserved (0).

 UINT : splitter thickness or 0 in case of error.
*/
#define wSplitterThickness(_hwnd_)                                 \
  ((UINT)WinSendMsg((_hwnd_), SPLITM_QUERY,                        \
                    MPFROM2SHORT(SPLITMP_BARTHICKNESS, (0)),       \
                    MPVOID))

#define dSplitterThickness(_hwndDlg_, _splitId_)                   \
  ((UINT)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY, \
                    MPFROM2SHORT(SPLITMP_BARTHICKNESS, (0)),       \
                    MPVOID))


/* ---------------------------------------------------------------------------
 This macro sets the thickness of the splitter.
 Values lower than 3 pixels are converted to 3 pixels, while values
 greater than 255 pixels are converted to 255 pixels.

 mp1: SHORT : SPLITMP_BARTHICKNESS
      SHORT : reserved (0).

 mp2: INT   : new splitter thickness.

 mr : BOOL  : TRUE/FALSE (success/error).

 Remarks:
 as a consequence of this message the virtual children size is changed
 and the controls are repainted.
*/
#define wSplitterSetThickness(_hwnd_, _thickness_)                \
  ((BOOL)WinSendMsg((_hwnd_), SPLITM_SET,                         \
                    MPFROM2SHORT(SPLITMP_BARTHICKNESS, (0)),      \
                    (MPARAM)(_thickness_)))

#define dSplitterSetThickness(_hwndDlg_, _splitId_, _thickness_)  \
  ((BOOL)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_SET,  \
                    MPFROM2SHORT(SPLITMP_BARTHICKNESS, (0)),      \
                    (MPARAM)(_thickness_)))


/* ---------------------------------------------------------------------------
 This macro returns the minimum width or height of a WC_SPLITPANE virtual
 child.

 mp1: SHORT : SPLITMP_MINWIDTHHEIGHT
      SHORT : query scope.
                 SPLITMP_LEFT      minimum width of the left child
                 SPLITMP_TOP       minimum height of the top child
                 SPLITMP_RIGHT     minimum width of the right child
                 SPLITMP_BOTTOM    minimum height of the bottom child
 mp2: reserved (0).

 mr : UINT : minimum child width/height.
*/
#define wSplitMinSize(_hwnd_, _childPos_)                              \
  ((UINT)WinSendMsg((_hwnd_), SPLITM_QUERY,                           \
                    MPFROM2SHORT(SPLITMP_MINWIDTHHEIGHT, (_childPos_)),\
                    MPVOID))

#define dSplitMinSize(_hwndDlg_, _splitId_, _childPos_)                \
  ((UINT)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY,    \
                    MPFROM2SHORT(SPLITMP_MINWIDTHHEIGHT, (_childPos_)),\
                    MPVOID))


/* ---------------------------------------------------------------------------
 This macro sets the minimum width or height of a WC_SPLITPANE virtual child.

 mp1: SHORT : SPLITMP_MINWIDTHHEIGHT
      SHORT : set scope.
                 SPLITMP_LEFT      minimum width of the left child
                 SPLITMP_TOP       minimum height of the top child
                 SPLITMP_RIGHT     minimum width of the right child
                 SPLITMP_BOTTOM    minimum height of the bottom child
 mp2: SHORT : new minimum width or height. Use 0 to unset the minimum width.
      SHORT : 1 dialog units , 0 pixels (percentage if previous parm is < 0)

 mr : BOOL  : TRUE/FALSE (success/error).
*/
#define wSplitSetMinSize(_hwnd_, _childPos_, _size_, _du_)             \
  ((BOOL)WinSendMsg((_hwnd_), SPLITM_SET,                              \
                    MPFROM2SHORT(SPLITMP_MINWIDTHHEIGHT, (_childPos_)),\
                    MPFROM2SHORT((_size_), (_du_))))

#define dSplitSetMinSize(_hwndDlg_, _splitId_, _size_, _du_)           \
  ((BOOL)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_SET,       \
                    MPFROM2SHORT(SPLITMP_MINWIDTHHEIGHT, (_childPos_)),\
                    MPFROM2SHORT((_size_), (_du_))))


/* ---------------------------------------------------------------------------
 This macro returns a WC_SPLITPANE style.

 mp1: SHORT : SPLITMP_STYLE
      SHORT : reserved (0).
 mp2: reserved (0).

 mr : ULONG : current splitpane style.
*/
#define wSplitStyle(_hwnd_)                                        \
  ((ULONG)WinSendMsg((_hwnd_), SPLITM_QUERY,                      \
                    MPFROM2SHORT(SPLITMP_STYLE, (0)),              \
                    MPVOID))

#define dSplitStyle(_hwndDlg_, _splitId_)                           \
  ((ULONG)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_QUERY,\
                    MPFROM2SHORT(SPLITMP_STYLE, (0)),               \
                    MPVOID))


/* ---------------------------------------------------------------------------
 This macro changes a WC_SPLITPANE style.

 mp1: SHORT : SPLITMP_STYLE
      SHORT : reserved (0).
 mp2: USHORT: new style data.
      USHORT. new style mask (style bits to be changed)

 mr : BOOL  : TRUE/FALSE (success/error).
*/
#define wSplitSetStyle(_hwnd_, _style_, _mask_)                    \
  ((BOOL)WinSendMsg((_hwnd_), SPLITM_SET,                          \
                    MPFROM2SHORT(SPLITMP_STYLE, (0)),              \
                    MPFROM2SHORT((_style_), (_mask_))))

#define dSplitSetStyle(_hwndDlg_, _splitId_, _style_, _mask_)      \
  ((BOOL)WinSendDlgItemMsg((_hwndDlg_), (_splitId_), SPLITM_SET,   \
                    MPFROM2SHORT(SPLITMP_STYLE, (0)),              \
                    MPFROM2SHORT((_style_), (_mask_))))


/* ---------------------------------------------------------------------------
 This macro changes the orientation of a WC_SPLITPANE.

 Parameters:

 _flag_ : SPLITS_COLUMNS or SPLITS_ROWS

 Return value:
 BOOL  : TRUE/FALSE (success/error).
*/
#define wSplitSetOrientation(_hwnd_, _flag_)                      \
   (wSplitSetStyle((_hwnd_), (_flag_), SPLITS_HORIZONTAL))
#define dSplitSetOrientation(_hwndDlg_, _splitId_, _flag_)        \
   (dSplitSetStyle((_hwndDlg_), (_splitId_), (_flag_), SPLITS_HORIZONTAL))


/* ---------------------------------------------------------------------------
 This macro changes the collapsability of a WC_SPLITPANE.

 Parameters:

 _flag_ : any of the SPLITS_HIDE* flags

 Return value:
 BOOL  : TRUE/FALSE (success/error).
*/
#define wSplitSetCollapsability(_hwnd_, _flag_)               \
   (wSplitSetStyle((_hwnd_), (_flag_), SPLITS_HIDEPANES))
#define dSplitSetCollapsability(_hwndDlg_, _splitId_, _flag_) \
   (dSplitSetStyle((_hwndDlg_), (_splitId_), (_flag_), SPLITS_HIDEPANES))


/* ---------------------------------------------------------------------------
 This macro changes the behaviour of a WC_SPLITPANE on size change.

 Parameters:

 _flag_ : SPLITS_PROPORTIONAL or any of the SPLITS_FIXED* flags

 Return value:
 BOOL  : TRUE/FALSE (success/error).
*/
#define wSplitSetSizeChangeStyle(_hwnd_, _flag_)               \
   (wSplitSetStyle((_hwnd_), (_flag_), SPLITS_SIZECHANGE))
#define dSplitSetSizeChangeStyle(_hwndDlg_, _splitId_, _flag_) \
   (dSplitSetStyle((_hwndDlg_), (_splitId_), (_flag_), SPLITS_SIZECHANGE))


/* ---------------------------------------------------------------------------
 This macro changes the behaviour of a WC_SPLITPANE on splitter drag.

 Parameters:

 _flag_ : any of the SPLITS_DRAG* flags

 Return value:
 BOOL  : TRUE/FALSE (success/error).
*/
#define wSplitterSetDragStyle(_hwnd_, _flag_)               \
   (wSplitSetStyle((_hwnd_), (_flag_), SPLITS_DRAGFLAGS))
#define dSplitterSetDragStyle(_hwndDlg_, _splitId_, _flag_) \
   (dSplitSetStyle((_hwndDlg_), (_splitId_), (_flag_), SPLITS_DRAGFLAGS))


// splitpane control data structure -----------------------------------------
// !!! do not change the member order !!!
#pragma pack(2)
typedef struct {
   USHORT cb;         // size of struct (20 or less if some members are unused)
   USHORT flag;       // defines used structure members
   SHORT splitterPos; // initial splitter position (distance from the left
                      // or bottom border according to the current orientation)
   USHORT thickness;  // bar thickness
   SHORT minLeft;     // minimum width of the left pane
   SHORT minRight;    // minimum width of the right pane
   SHORT minTop;      // minimum height of the top pane
   SHORT minBottom;   // minimum height of the bottom pane
   USHORT style;      // control style (SPLITS_* flags)
} SPLITCTLDATA, * PSPLITCTLDATA;
#pragma pack()

// control data structure flags
#define SPLITCD_POS             0x0001     // set splitterPos
#define SPLITCD_THICKNESS       0x0002     // set thickness
#define SPLITCD_STYLE           0x0004     // set the style member
#define SPLITCD_MINLEFT         0x0010     // set minLeft
#define SPLITCD_MINRIGHT        0x0020     // set minRight
#define SPLITCD_MINTOP          0x0040     // set minTop
#define SPLITCD_MINBOTTOM       0x0080     // set minBottom
#define SPLITCD_DIALOGUNITS     0x8000     // positive values are dialog units
#define SPLITCD_XDATA           (SPLITCD_MINLEFT | SPLITCD_MINRIGHT)
#define SPLITCD_YDATA           (SPLITCD_MINTOP | SPLITCD_MINBOTTOM)
#define SPLITCD_INIT       (SPLITCD_THICKNESS | SPLITCD_XDATA | SPLITCD_YDATA)


/* ---------------------------------------------------------------------------
 Data structures used by the WinCreateSplitPane() API
*/
#pragma pack(2)
typedef struct _SPLITPCREATE SPLITPCREATE;
typedef SPLITPCREATE * PSPLITPCREATE;

// child pane data
typedef struct {
   union {
      PSZ class;               // class name
      PFNWP pDlgProc;          // dialog procedure
   } ;
   union {
      PSZ text;                // pane text (if any)
      HMODULE hmod;            // module handle
   } ;
   union {
      PVOID pCtlData;          // optional control data
      PSPLITPCREATE pSplitCreate;   // nested splitpane creation data
   } ;
   PVOID pPresParams;          // optional presentation parameters
   ULONG style;                // pane style (WS_VISIBLE is set by default)
   USHORT id;                  // pane window ID
   USHORT type;                // type of child (window, dialog, splitpane)
} SPLITPANECHILD, * PSPLITPANECHILD;

struct _SPLITPCREATE {
   SPLITPANECHILD pane1;       // data of the left/top pane
   SPLITPANECHILD pane2;       // data of the right/bottom pane
   PSPLITCTLDATA pCtlData;     // optional control data
   PVOID pPresParams;          // optional presentation parameters
} ;
#pragma pack()

// split creation flags
#define SPLITCHILD_WND      0  // the child is an ordinary window
#define SPLITCHILD_DLG      1  // the child is a dialog window
#define SPLITCHILD_SPLIT    2  // the child is a nested splitpane
/*
 Remarks:
 according to the specified type of child pane the following member are used
 (the one enclosed between [] are optional).
 SPLITCHILD_WND : class, text, [pCtlData], [pPresParams], style, id, type
 SPLITCHILD_DLG : pDlgProc, hmod, id, [pCtlData], type
 SPLITCHILD_SPLIT: style, id, pSplitCreate, type
*/
// splitpane class registration
BOOL EXPENTRY WinSplitPaneRegister(HAB hab);
// splitpane window creation
HWND EXPENTRY WinCreateSplitPane(HWND hwndPO, ULONG style, ULONG id,
                                 LONG x, LONG y, LONG cx, LONG cy,
                                 PSPLITPCREATE pCreate);

#endif // #ifndef _SPLITPANE_H_
