/* --------------------------------------------------------------------------
 main.h :

 28 Apr 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */



#ifndef _SPLITPANE_MAIN_H_
   #define _SPLITPANE_MAIN_H_
   #define INCL_WIN
   #define INCL_GPI
   #define INCL_DOS
   #define INCL_DOSSEMAPHORES
   #define INCL_DOSERRORS
   #define INCL_DOSMISC
   #define INCL_DOSMODULEMGR
   #define INCL_DOSFILEMGR
   #define INCL_DOSPROCESS
   #define INCL_DOSNLS

   #include <os2.h>
   #include <stdlib.h>
   #include <stdio.h>
   #include <malloc.h>
   #include <string.h>

   #ifndef RC_INVOKED
      #include "axPM.h"
      #include "prototypes.h"
   #else
      #include "axSplitpane.h"
   #endif

   #include "captions.h"
   #include "winIDs.h"



/* --------------------------------------------------------------------------
*/

/* --------------------------------------------------------------------------
*/

typedef struct {
   HWND hwndSettings;
   HWND hwndSplitPane;
} GLOBAL, *PGLOBAL;

extern GLOBAL g;


#endif // #ifndef _SPLITPANE_H_
