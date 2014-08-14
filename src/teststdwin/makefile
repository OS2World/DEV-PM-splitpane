# To add new extensions to the inference rules uncomment and redefine this:
#.SUFFIXES:
#
#.SUFFIXES: \
#    .C .obj .rc .res

# compiler, linker, resource compiler, resource binder MACRO

# include the Api-eXtension path
%setenv INCLUDE=..\axh;$(INCLUDE)

# include the default macros
!include ..\default.mak

all: splittest.exe

splittest.exe: \
   main.obj \
   ..\ax\splitpane.obj    \
   ..\ax\axPMCtrlUtil.obj \
   mainDlg.obj \
   main.res
   $(CL) @<<
      $(LFLAGS)
      /O:splittest.exe
      ..\ax\splitpane.obj
      ..\ax\axPMCtrlUtil.obj
      main.obj
      mainDlg.obj
    <<
    $(RB) $(RBFLAGS) main.res splittest.exe

main.res: \
   main.rc \
   main.dlg \
   winIDs.h \
   captions.h \
   main.h

main.obj : \
   main.c \
   winIDs.h \
   captions.h \
   prototypes.h \
   ..\axh\*.h  \
   main.h

mainDlg.obj : \
   mainDlg.c \
   winIDs.h \
   captions.h \
   prototypes.h \
   ..\axh\*.h  \
   main.h

