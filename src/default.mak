# To add new extensions to the inference rules uncomment and redefine this:
#.SUFFIXES:
#
#.SUFFIXES: \
#    .C .obj .rc .res

# compiler, linker, resource compiler, resource binder MACRO
CC = icc.exe
CL = ilink.exe
RC = rc.exe
RB = rc.exe

# compiler and linker flags

# Debug version
!ifdef DEBUG

CFLAGS = /Ss /Ti /Rn /G5 /C /Q
LFLAGS = /DE /E:2 /PM:PM /PACKD /A:4 /OPTF /NOL /M /L

!else
# RELEASE version

CFLAGS = /Ss /O /Oc /Ol /Rn /G5 /C /Q
LFLAGS = /E:2 /PM:PM /PACKD /A:4 /OPTF /NOL /M /L

!endif

RCFLAGS = -r
RBFLAGS = -x2

.rc.res:
   $(RC) $(RCFLAGS) $<

.c.obj:
   $(CC) $(CFLAGS) $<

