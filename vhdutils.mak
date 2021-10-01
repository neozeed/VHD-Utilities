# MAKE_KITVER=5, MAKE_SKIPBIT=0


BASE_NAME=vhdutils


!IF "$(ARCH)" != "x86-32" && "$(ARCH)" != "x86-64"
!MESSAGE Available build configurations:
!MESSAGE NMAKE /f "$(BASE_NAME).mak" ARCH="x86-32"
!MESSAGE NMAKE /f "$(BASE_NAME).mak" ARCH="x86-64"
!MESSAGE You should also set CPPVER to match your compiler version; e.g., CPPVER=150
!ERROR A valid configuration was not specified.
!ENDIF


################################################################################
# Adjustable configuration
################################################################################


# File extension of our desired target
TARGET_EXTENSION=exe


# Output and intermediate directories
BINDIR=bin.$(ARCH)
OBJDIR=obj.$(ARCH)


# Additional include directories
CPPFLAGS_INC=
# Additional definitions
CPPFLAGS_DEF_EX=


# Linker output type: Subsystem
LINKFLAGS_SUB=CONSOLE


# Additional library paths
LINK_LIBPATHS= \
	/LIBPATH:libs\$(ARCH)


# Import libraries
LINK_LIBS= \
	kernel32.lib \
	shell32.lib \
	virtdisk.lib


# Target definitions
TARGET_1="$(BINDIR)\makevhd.$(TARGET_EXTENSION)"
LINK_OBJS_1= \
	"$(OBJDIR)\makevhd.obj" \
	"$(OBJDIR)\EntryPoint.obj" \
	"$(OBJDIR)\ParseNumber.obj"

TARGET_2="$(BINDIR)\resizevhd.$(TARGET_EXTENSION)"
LINK_OBJS_2= \
	"$(OBJDIR)\resizevhd.obj" \
	"$(OBJDIR)\EntryPoint.obj" \
	"$(OBJDIR)\ParseNumber.obj"


!IF "$(ARCH)" == "x86-32"

CPPVER_DEFAULT=120
ARCH_DEF=_M_IX86
ARCH_LNK=IX86
NTVER_DEF=0x0602
NTVER_LNK_1=6.1
NTVER_LNK_2=6.2
CPPFLAGS_EX=
LINKFLAGS_EX=

!ELSEIF "$(ARCH)" == "x86-64"

CPPVER_DEFAULT=140
ARCH_DEF=_M_AMD64
ARCH_LNK=AMD64
NTVER_DEF=0x0602
NTVER_LNK_1=6.1
NTVER_LNK_2=6.2
CPPFLAGS_EX=
LINKFLAGS_EX=

!ENDIF


################################################################################
# Standard configuration
################################################################################


# Standard version-specific compiler options
!IFNDEF CPPVER
CPPVER=$(CPPVER_DEFAULT)
!ENDIF

!IF $(CPPVER) <= 130
CPPFLAGS_STD_VER=/G6
!ELSEIF $(CPPVER) == 131
CPPFLAGS_STD_VER=/G7
!ELSE
CPPFLAGS_STD_VER=/GS-
!ENDIF


# Standard compiler options
CPPFLAGS_STD=/nologo /c /MD /W3 /GF /GR- /EHs-c- /O1
# Standard definitions
CPPFLAGS_DEF_STD=/D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_WIN32_WINNT=$(NTVER_DEF)"


# Linker output type: DLL and .def
!IF "$(TARGET_EXTENSION)" == "dll"
LINKFLAGS_DLL=/DLL /DEF:"$(BASE_NAME).def"
MAKE_DOPOST_CLEANDLL=1
!ELSE
LINKFLAGS_DLL=
MAKE_DOPOST_CLEANDLL=0
!ENDIF


# Standard linker options (/OPT:NOWIN98 is implied by SUBSYSTEM version >= 5.0)
LINKFLAGS_STD=/NOLOGO /RELEASE /OPT:REF /OPT:ICF /MERGE:.rdata=.text /IGNORE:4078
# Target OS and machine options
LINKFLAGS_TARG_1=/SUBSYSTEM:$(LINKFLAGS_SUB),$(NTVER_LNK_1) /OSVERSION:$(NTVER_LNK_1) /MACHINE:$(ARCH_LNK)
LINKFLAGS_TARG_2=/SUBSYSTEM:$(LINKFLAGS_SUB),$(NTVER_LNK_2) /OSVERSION:$(NTVER_LNK_2) /MACHINE:$(ARCH_LNK)


# Code compiler
CPP=@cl.exe
CPPFLAGS=$(CPPFLAGS_STD) $(CPPFLAGS_STD_VER) $(CPPFLAGS_EX) $(CPPFLAGS_INC) $(CPPFLAGS_DEF_STD) $(CPPFLAGS_DEF_EX)
CPPFLAGSOUT=$(CPPFLAGS) /Fo"$(OBJDIR)\\"


# Resource compiler
RC=@rc.exe
RFLAGS=/l 0x409 /d "NDEBUG" /d "$(ARCH_DEF)"


# Linker
LINK=@link.exe
LINKFLAGS_1=$(LINKFLAGS_STD) $(LINKFLAGS_DLL) $(LINKFLAGS_TARG_1) $(LINKFLAGS_EX) $(LINK_LIBPATHS) $(LINK_LIBS)
LINKFLAGS_2=$(LINKFLAGS_STD) $(LINKFLAGS_DLL) $(LINKFLAGS_TARG_2) $(LINKFLAGS_EX) $(LINK_LIBPATHS) $(LINK_LIBS)


# By default, do not post-process unless explicitly told to do so
!IFNDEF MAKE_DOPOST
MAKE_DOPOST=0
!ENDIF


################################################################################
# Recipes and rules
################################################################################


DEFAULT : "$(BINDIR)" "$(OBJDIR)" $(TARGET_1) $(TARGET_2)
!IF $(MAKE_DOPOST) > 0
	 @cd "$(BINDIR)"
	-@md5sum *.exe *.dll *.lib 1>"$(BASE_NAME).md5"
	 @cd ..
!ENDIF

$(TARGET_1) : $(LINK_OBJS_1)
	$(LINK) @<<
	$(LINKFLAGS_1) $(LINK_OBJS_1) /OUT:$@
<<

$(TARGET_2) : $(LINK_OBJS_2)
	$(LINK) @<<
	$(LINKFLAGS_2) $(LINK_OBJS_2) /OUT:$@
<<

"$(BINDIR)" :
	-@mkdir $@

"$(OBJDIR)" :
	-@mkdir $@

.rc{$(OBJDIR)}.res:
	$(RC) $(RFLAGS) /Fo$@ $<

.c{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<

.cpp{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<

{libs}.c{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<

{libs}.cpp{$(OBJDIR)}.obj::
	$(CPP) @<<
	$(CPPFLAGSOUT) $<
<<
