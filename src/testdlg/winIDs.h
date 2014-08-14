/* --------------------------------------------------------------------------
 winIDs.h :

 28 Apr 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

#ifndef _WINIDS_H_
   #define _WINIDS_H_


// common IDs
   #define ID_STATIC           0xffff
   #define ID_MAINRES          1

   #define BTN_APPLY           10
   #define BTN_DONE            11


// main dialog
   #define DLG_SPLITPANE       100
   #define SPLIT_1             101
   #define MLE_1               102
   #define SPLIT_2             103
   #define MLE_2_1             104
   #define MLE_2_2             105

// splitpane settings dialog
   #define DLG_SETSPLITPANE    200
   #define RDBTN_VERTICAL      201
   #define RDBTN_HORIZONTAL    202
   #define RDBTN_HIDENONE      203
   #define RDBTN_HIDEPANE1     204
   #define RDBTN_HIDEPANE2     205
   #define RDBTN_HIDEBOTHPANES 206
   #define RDBTN_RATEPANESIZE  207
   #define RDBTN_FIXSIZE1      208
   #define RDBTN_FIXSIZE2      209
   #define SPIN_LEFTWIDTH      210
   #define SPIN_RIGHTWIDTH     211
   #define SPIN_TOPHEIGHT      212
   #define SPIN_BOTTOMHEIGHT   213

// splitter settings dialog
   #define DLG_SETSPLITTER     300
   #define RDBTN_DRAGSYS       301
   #define RDBTN_DRAGSOLID     302
   #define RDBTN_DRAGFRAME     303
   #define RDBTN_PIXELS        304
   #define RDBTN_DLGUNITS      305
   #define RDBTN_PERCENT       306
   #define SPIN_THICKNESS      307
   #define SPIN_POS            308

// control data dialog
   #define DLG_CTLDATA         400
   #define CHK_SPLITTERPOS     401
   #define EF_SPLITTERPOS      402
   #define CHK_MINLEFT         403
   #define EF_MINLEFT          404
   #define CHK_MINRIGHT        405
   #define EF_MINRIGHT         406
   #define CHK_THICKNESS       407
   #define EF_THICKNESS        408
   #define CHK_MINTOP          409
   #define EF_MINTOP           410
   #define CHK_MINBOTTOM       411
   #define EF_MINBOTTOM        412
   #define CHK_STYLE           413
   #define EF_STYLE            414
   #define CHK_DLGUNITS        415

// product information dialog
   #define DLG_PRODINFO        500
   #define BAR_DARK            501
   #define BAR_LITE            502
   #define BTN_URL             503

// menuitems IDs
   #define MI_FILE             10000
   #define MI_OPTIONS          10001
   #define MI_SPLITPANE1       10002
   #define MI_SPLITPANE2       10003
   #define MI_SPLITTER1        10004
   #define MI_SPLITTER2        10005
   #define MI_CTLDATA1         10006
   #define MI_CTLDATA2         10007
   #define MI_HELP             10008
   #define MI_HELPGENERAL      10009
   #define MI_HELPPRODINFO     10010

#endif // #ifndef _WINIDS_H_
