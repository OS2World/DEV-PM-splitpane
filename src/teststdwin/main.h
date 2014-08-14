/* --------------------------------------------------------------------------
 main.h :

 30 Dec 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */



#ifndef _SPLITTEST_MAIN_H_
   #define _SPLITTEST_MAIN_H_
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
      #include "axBase.h"
   #endif

   #include "captions.h"
   #include "winIDs.h"


/* --------------------------------------------------------------------------
*/

/* --------------------------------------------------------------------------
*/

typedef struct {
   ULONG mode;
} GLOBAL, *PGLOBAL;

extern GLOBAL g;

#endif // #ifndef _SPLITTEST_MAIN_H_
