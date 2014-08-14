# To add new extensions to the inference rules uncomment and redefine this:
#.SUFFIXES:
#
#.SUFFIXES: \
#    .C .obj .rc .res

# include the Api-eXtension path
%setenv INCLUDE=..\axh;$(INCLUDE)

# include the default macros
!include ..\default.mak

all: SplitPane.exe

SplitPane.exe:      \
   main.obj         \
   ..\ax\splitpane.obj    \
   ..\ax\axPMCtrlUtil.obj \
   setctldata.obj   \
   setpanes.obj     \
   setsplitter.obj  \
   prodinfo.obj     \
   main.res
   $(CL) @<<
      $(LFLAGS)
      /O:SplitPane.exe
      main.obj
      ..\ax\splitpane.obj
      ..\ax\axPMCtrlUtil.obj
      setctldata.obj
      setpanes.obj
      setsplitter.obj
      prodinfo.obj
    <<
    $(RB) $(RBFLAGS) main.res SplitPane.exe

main.res:              \
   main.rc             \
   main.dlg            \
   ctldata.dlg         \
   prodinfo.dlg        \
   splitpane.dlg       \
   splitter.dlg        \
   winIDs.h            \
   captions.h          \
   ..\axh\axSplitpane.h  \
   main.h

main.obj :             \
   main.c              \
   winIDs.h            \
   captions.h          \
   prototypes.h        \
   main.h              \
   ..\axh\*.h

setctldata.obj:        \
   setctldata.c        \
   winIDs.h            \
   captions.h          \
   prototypes.h        \
   main.h              \
   ..\axh\*.h

setpanes.obj :         \
   setpanes.c          \
   winIDs.h            \
   captions.h          \
   prototypes.h        \
   main.h              \
   ..\axh\*.h

setsplitter.obj :      \
   setsplitter.c       \
   winIDs.h            \
   prototypes.h        \
   main.h              \
   ..\axh\*.h

prodinfo.obj :         \
   prodinfo.c          \
   winIDs.h            \
   prototypes.h        \
   main.h              \
   ..\axh\*.h

