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
    CXXFLAGS ?= -I$(PIN_HOME)/InstLib -fomit-frame-pointer -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
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

TOOL_ROOTS = 
SANITY_TOOLS = 
TEST_TOOLS_ROOTS = 

ifneq ($(TARGET),ipf)
  ifneq (${TARGET_OS},m)
    TOOL_ROOTS += icount1sp icount2sp 
    SANITY_ROOTS += icount1sp icount2sp 
    TEST_TOOLS_ROOTS += icount1sp icount2sp 
  endif
endif

TEST_TOOLS = $(TEST_TOOLS_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))

TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))

all: tools
tools: $(OBJDIR) $(TOOLS)
test: $(OBJDIR) $(TEST_TOOLS_ROOTS:%=%.test)
tests-sanity: $(OBJDIR) $(SANITY_TOOLS:%=%.test)

## build rules

$(OBJDIR):
	mkdir -p $(OBJDIR)


icount1sp.test: $(OBJDIR)icount1sp${PINTOOL_SUFFIX} $(OBJDIR)hello_static icount1sp.tested icount1sp.failed 
	$(PIN) -xyzzy -sp 1 -virtual_segments 0 -t $< -- ./$(OBJDIR)hello_static >& $<.out
	sleep 5s ; cat $<.out ;  grep -q PIPELINE $<.out
	rm icount1sp.failed $<.out

icount2sp.test: $(OBJDIR)icount2sp${PINTOOL_SUFFIX} $(OBJDIR)hello_static icount2sp.tested icount2sp.failed 
	$(PIN) -xyzzy -sp 1 -virtual_segments 0 -t $< -- ./$(OBJDIR)hello_static >& $<.out
	sleep 5s ; cat $<.out ;  grep -q PIPELINE $<.out
	rm icount2sp.failed $<.out

$(OBJDIR)hello_static: hello_static.c
	$(CC) -static -o $@ hello_static.c

$(OBJDIR)%.o : %.cpp 
	${CXX} ${COPT} $(CXXFLAGS) ${PIN_CXXFLAGS} ${OUTOPT}$@ $< 

$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINKER} ${PIN_LDFLAGS} $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} ${PIN_LIBS} $(DBG)

## cleaning
clean:
	-rm -rf $(OBJDIR) *.out *.tested *.failed  *makefile.copy 

-include *.d
