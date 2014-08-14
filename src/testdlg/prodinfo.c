/* --------------------------------------------------------------------------
 prodinfo.c :

 23 May 2006 - Alessandro Felice Cantatore

- Notes : ----------------------------------------------------------------
- Contents: --------------------------------------------------------------

-------------------------------------------------------------------------- */

// includes -----------------------------------------------------------------
#include "main.h"

// definitions --------------------------------------------------------------
#define APPSTART_DEFAULT              0x0000
#define APPSTART_CHILD                SAF_STARTCHILDAPP
#define APPSTART_MAX                  SAF_MAXIMIZED
#define APPSTART_MINI                 SAF_MINIMIZED
#define APPSTART_BKGND                SAF_BACKGROUND
#define APPSTART_HIDDEN               SAF_MINIMIZED
#define APPSTART_FULLSCREEN           0x2000

// prototypes ---------------------------------------------------------------
BOOL onDlgInit(HWND hwnd, PVOID pdata);
BOOL onCommand(HWND hwnd, ULONG id);
HAPP applStartApp(HWND hwnd, PSZ pszExe, PSZ pszParms,
                  PSZ pszWorkDir, PSZ pszEnv, ULONG flMode);

// global variables ---------------------------------------------------------

// entry point --------------------------------------------------------------

/* --------------------------------------------------------------------------
 Product information dialog procedure.
- Parameters -------------------------------------------------------------
 Ordinary window procedure parameters.
- Return value -----------------------------------------------------------
 MRESULT
-------------------------------------------------------------------------- */

MRESULT EXPENTRY prodInfoProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
   switch (msg) {
      case WM_INITDLG:
         return (MRESULT)onDlgInit(hwnd, mp2);
         break;
      case WM_COMMAND: // unhandled commands fall through the def dlg proc.
         if (onCommand(hwnd, (ULONG)mp1)) break;
      default:
         return WinDefDlgProc(hwnd, msg, mp1, mp2);
   } /* endswitch */
   return MRFALSE;
}


/* --------------------------------------------------------------------------
 Dialog initialization
- Parameters -------------------------------------------------------------
 HWND hwnd   : window handle.
 PVOID pdata : data associated with the current dialog.
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (focus window was/wasn't changed);
-------------------------------------------------------------------------- */
static
BOOL onDlgInit(HWND hwnd, PVOID pdata) {
   SWP swp;
   HWND hCtrl;

   hCtrl = DlgItemHandle(hwnd, BTN_URL);
   WinQueryWindowPos(hCtrl, &swp);
   WinSetWindowPos(hCtrl, 0, swp.x - 10, swp.y, 0, 0, SWP_MOVE);
   hCtrl = DlgItemHandle(hwnd, BAR_DARK);
   WinQueryWindowPos(hCtrl, &swp);
   WinSetWindowPos(hCtrl, 0, swp.x, swp.y - swp.cy + 1, swp.cx, 1,
                   SWP_SIZE | SWP_MOVE);
   hCtrl = DlgItemHandle(hwnd, BAR_LITE);
   WinQueryWindowPos(hCtrl, &swp);
   WinSetWindowPos(hCtrl, 0, swp.x + 1, swp.y + swp.cy - 2, swp.cx, 1,
                   SWP_SIZE | SWP_MOVE);
   return FALSE;
}



/* --------------------------------------------------------------------------
 Command events handler.
- Parameters -------------------------------------------------------------
 HWND hwnd : window handle
 ULONG id  : button id
- Return value -----------------------------------------------------------
 BOOL : TRUE/FALSE (handled/unhandled command).
-------------------------------------------------------------------------- */
static
BOOL onCommand(HWND hwnd, ULONG id) {
   CHAR pszExe[CCHMAXPATH];
   CHAR pszPath[CCHMAXPATHCOMP];

   if (id != BTN_URL) return FALSE;
   if (PrfQueryProfileString(HINI_USER, "WPURLDEFAULTSETTINGS",
                             "DefaultBrowserExe", "", pszExe, sizeof(pszExe))
       &&
       PrfQueryProfileString(HINI_USER, "WPURLDEFAULTSETTINGS",
                             "DefaultWorkingDir", "", pszPath, sizeof(pszPath)))
      applStartApp(hwnd, pszExe, "http://space.tin.it/scienza/acantato",
                   pszPath, NULL, APPSTART_DEFAULT);
   return TRUE;
}

/* --------------------------------------------------------------------------
 Start an application.
- Parameters -------------------------------------------------------------
 HWND hwnd      : notification window handle
 PSZ pszExe     : executable name
 PSZ pszParms   : parameters
 PSZ pszWorkDir : working directory
 PSZ pszEnv     : environment values
 ULONG flMode   : start mode      APPSTART_DEFAULT
                                  mutually exclusive:
                                  APPSTART_MAX
                                  APPSTART_MINI
                                  APPSTART_BKGND
                                  APPSTART_HIDDEN
                                  other:
                                  APPSTART_CHILD
                                  APPSTART_FULLSCREEN
- Return value -----------------------------------------------------------
 HAPP : handle of the started application.
-------------------------------------------------------------------------- */
static
HAPP applStartApp(HWND hwnd, PSZ pszExe, PSZ pszParms,
                  PSZ pszWorkDir, PSZ pszEnv, ULONG flMode) {
   PROGDETAILS pd;
   memset(&pd, 0, sizeof(PROGDETAILS));
   pd.Length = sizeof(PROGDETAILS);
   pd.progt.progc = (flMode & APPSTART_FULLSCREEN) ?
                    PROG_FULLSCREEN : PROG_DEFAULT;
   pd.progt.fbVisible = SHE_VISIBLE;
   pd.pszExecutable = pszExe;
   pd.pszParameters = pszParms;
   pd.pszStartupDir = pszWorkDir;
   pd.pszEnvironment = pszEnv;
   return WinStartApp(hwnd, &pd, NULL, NULL,
                      SAF_INSTALLEDCMDLINE | (flMode & 0xff));
}

