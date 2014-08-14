# To add new extensions to the inference rules uncomment and redefine this:
#.SUFFIXES:
#
#.SUFFIXES: \
#    .C .obj .rc .res

# include the Api-eXtension path
%setenv INCLUDE=..\axh;$(INCLUDE)

!include ..\default.mak

all: splitpane.obj axPMCtrlUtil.obj

splitpane.obj :        \
   splitpane.c         \
   ..\axh\*.h          \
   splitpanei.h

axPMCtrlUtil.obj :     \
   axPMCtrlUtil.c      \
   ..\axh\*.h

