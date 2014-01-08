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
    CXXFLAGS ?= -I$(PIN_HOME)/InstLib -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
endif

##############################################################
#
# build rules
#
##############################################################

EXTRA_LIBS =

TOOL_ROOTS = memtrace


all: tools
SANITY_TOOLS = 

TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))
TESTS = $(TOOL_ROOTS:%=%.test)
ifeq ($(TARGET_OS),l)
	TESTS += memtracemt.test
endif

tools: $(OBJDIR) $(TOOLS)
test: $(OBJDIR) $(TESTS)
tests-sanity: $(OBJDIR) $(SANITY_TOOLS:%=%.test)

## build rules

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(EXTRA_LIBS) $(DBG) 

memtracemt.test : $(OBJDIR)memtrace$(PINTOOL_SUFFIX) memtracemt.tested memtracemt.failed $(OBJDIR)thread
	touch $*.makefile.copy; rm $*.makefile.copy
	$(PIN) -t $< -emit 0 -- $(OBJDIR)thread
	rm memtracemt.failed

## cleaning
clean:
	-rm -rf $(OBJDIR) *.out *.out.* *.tested *.failed

$(OBJDIR)thread: thread.c threadlib_unix.c
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)thread thread.c threadlib_unix.c -lpthread

%.test : $(OBJDIR)%$(PINTOOL_SUFFIX) %.tested %.failed
	touch $*.makefile.copy; rm $*.makefile.copy
	$(PIN) -t $< -emit 0 -- $(TESTAPP) makefile $*.makefile.copy
	$(PIN_CMP) makefile $*.makefile.copy
	rm $*.makefile.copy; rm $*.failed

-include *.d
