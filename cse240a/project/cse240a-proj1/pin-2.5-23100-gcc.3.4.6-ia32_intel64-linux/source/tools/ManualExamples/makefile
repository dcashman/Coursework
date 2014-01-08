##
## PIN tools
##

##############################################################
#
# Here are some things you might want to configure
#
##############################################################

TARGET_COMPILER?=gnu
ifdef OS
    ifeq (${OS},Windows_NT)
        TARGET_COMPILER=ms
    endif
endif

##############################################################
#
# include *.config files
#
##############################################################

ifeq ($(TARGET_COMPILER),gnu)
    include ../makefile.gnu.config
    LINKER?=${CXX}
    CXXFLAGS ?= -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT)
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
endif

##############################################################
#
# Tools sets
#
##############################################################


TOOL_ROOTS = inscount0 inscount1 inscount2 proccount imageload staticcount detach
STATIC_TOOL_ROOTS =

# pinatrace and itrace currently hang cygwin gnu windows
ifneq ($(TARGET_OS),w)
    TOOL_ROOTS += pinatrace itrace
else
    TOOL_ROOTS += replacesigprobed
    ifneq ($(TARGET_COMPILER),gnu)
        TOOL_ROOTS += pinatrace itrace
    endif
endif

ifeq (${TARGET_OS},l)
    TOOL_ROOTS += strace
    STATIC_TOOL_ROOTS += statica
endif

ifeq (${TARGET_OS},m)
TOOL_ROOTS += strace
STATIC_TOOL_ROOTS += statica
endif

ifeq ($(TARGET),ia32)
TOOL_ROOTS += isampling
endif

ifeq ($(TARGET),ia32e)
TOOL_ROOTS += isampling
endif

TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))
STATIC_TOOLS = $(STATIC_TOOL_ROOTS:%=$(OBJDIR)%$(SATOOL_SUFFIX))

##############################################################
#
# build rules
#
##############################################################

all: tools
tools: $(OBJDIR) $(TOOLS) $(STATIC_TOOLS)
test: $(OBJDIR) $(TOOL_ROOTS:%=%.test) $(STATIC_TOOL_ROOTS:%=%.test) 


# stand alone pin tool
statica.test: $(OBJDIR)statica$(SATOOL_SUFFIX) statica.tested statica.failed $(OBJDIR)statica
	./$(OBJDIR)statica$(SATOOL_SUFFIX) -i ./$(OBJDIR)statica  > statica.dmp
	rm statica.failed statica.dmp

# probe mode requires special libraries to be installed on newer Linux OSes.
# overriding build rules
$(OBJDIR)replacesigprobed$(PINTOOL_SUFFIX): $(OBJDIR)replacesigprobed.o
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

$(OBJDIR)replacesigprobed.o : replacesigprobed.cpp
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

replacesigprobed.test : $(OBJDIR)replacesigprobed$(PINTOOL_SUFFIX) replacesigprobed.tested replacesigprobed.failed
	$(PIN) -probe -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	rm replacesigprobed.failed  $<.out $<.makefile.copy

## build rules

$(OBJDIR):
	mkdir -p $(OBJDIR)


$(OBJDIR)%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): $(PIN_LIBNAMES)

$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

$(STATIC_TOOLS): $(PIN_LIBNAMES)

$(STATIC_TOOLS): %$(SATOOL_SUFFIX) : %.o
	${LINKER} $(PIN_SALDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(SAPIN_LIBS) $(DBG)

## cleaning
clean:
	-rm -rf $(OBJDIR) *.out *.tested *.failed *.makefile.copy 
