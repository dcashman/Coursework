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
    CXXFLAGS ?= -Wall -Werror -Wno-unknown-pragmas $(DBG) $(OPT) -MMD
    ifeq ($(TARGET_OS),w)
        BIGBINARYFLAGS = -mno-cygwin
    else
	BIGBINARYFLAGS ?= 
    endif
    LPTHREAD?=-lpthread
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    BIGBINARYFLAGS ?= /nologo
    DBG?=
    LPTHREAD?=
endif


#bb_test and orig_address are not run on cygwin due to inapility to print from Fini function because on
#cygwin the runtime is shared between the app and the tool
ifneq ($(TARGET_OS),w)
    SPECIALRUN = bb_test cache_flusher 
    NORMALRUN = cache_simulator watch_fragmentation trace_insertions enter_exit link_unlink event_trace cache_stats insertDelete deleteTrace  orig_address mem_usage
else
    ifeq ($(TARGET_COMPILER),gnu)
        SPECIALRUN = 
        NORMALRUN = cache_simulator watch_fragmentation trace_insertions enter_exit link_unlink event_trace cache_stats insertDelete deleteTrace 
    else
        SPECIALRUN = bb_test
        NORMALRUN = cache_simulator watch_fragmentation trace_insertions enter_exit link_unlink event_trace cache_stats insertDelete deleteTrace  orig_address
    endif
endif
TOOL_ROOTS = $(SPECIALRUN) $(NORMALRUN)

TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))

SANITY_TOOLS = 

ifeq ($(TARGET_OS),w)
#Thread wait depends on pthreads, so it doesn't work on Windows
THREADTESTAPP =
BIGTESTAPP = ./$(OBJDIR)bigBinary
else 
THREADTESTAPP = ./$(OBJDIR)threadWait
BIGTESTAPP = ./$(OBJDIR)bigBinary
endif

#OS Dependent
ifeq ($(TARGET_OS),l)
SPECIALRUN += mtFlush
endif

CACHELIMIT = 262144

# Pin for the Intel(R) 64 architecture does not support variable size caches
#also  cache_doubler is not run on cygwin due to inapility to print from Fini function because on
#cygwin the runtime is shared between the app and the tool
ifneq ($(TARGET),ia32e)
    ifneq ($(TARGET_OS),w)
        TOOL_ROOTS += cache_doubler
    else
        ifneq ($(TARGET_COMPILER),gnu)
            TOOL_ROOTS += cache_doubler
        endif
    endif
endif

all: tools 
tools: $(OBJDIR) $(TOOLS) 
test: $(OBJDIR) $(TOOL_ROOTS:%=%.test)
tests-sanity: $(OBJDIR) $(SANITY_TOOLS:%=%.test)

$(OBJDIR):
	mkdir -p $(OBJDIR)


## special testing rules

cache_flusher.test: $(OBJDIR)cache_flusher$(PINTOOL_SUFFIX) cache_flusher.tested cache_flusher.failed $(BIGTESTAPP)
	$(PIN) -limit_code_cache $(CACHELIMIT) -t $< -o $<.out -- $(BIGTESTAPP)
	grep -q SWOOSH $<.out
	grep -q eof $<.out
	rm cache_flusher.failed; rm $<.out

cache_doubler.test: $(OBJDIR)cache_doubler$(PINTOOL_SUFFIX) cache_doubler.tested cache_doubler.failed $(BIGTESTAPP)
	$(PIN) -limit_code_cache $(CACHELIMIT) -t $< -o $<.out -- $(BIGTESTAPP) 
	grep -q STRETCH $<.out
	grep -q eof $<.out
	rm cache_doubler.failed; rm $<.out

flushAndInsert.test: $(OBJDIR)flushAndInsert$(PINTOOL_SUFFIX) flushAndInsert.tested flushAndInsert.failed $(BIGTESTAPP)
	$(PIN) -limit_code_cache $(CACHELIMIT) -t $< -o $<.out -- $(BIGTESTAPP) 
	grep -q SWOOSH $<.out
	grep -q insertion $<.out
	rm flushAndInsert.failed; rm $<.out

bb_test.test: $(OBJDIR)bb_test$(PINTOOL_SUFFIX) bb_test.tested bb_test.failed $(BIGTESTAPP)
	$(PIN) -t $< -b 10 -o $<.out -- $(BIGTESTAPP)
	grep -q "Final trace count" $<.out
	grep -q -v "Final trace count: 0" $<.out
	grep -q -v "Final cache size: 0" $<.out
	rm bb_test.failed; rm $<.out

mtFlush.test: $(OBJDIR)mtFlush$(PINTOOL_SUFFIX) mtFlush.tested mtFlush.failed $(THREADTESTAPP)
	$(PIN)  -t $< -- $(THREADTESTAPP) 
	grep -q SWOOSH mtFlush.out
	rm mtFlush.failed; rm mtFlush.out

orig_address.test:$(OBJDIR)orig_address$(PINTOOL_SUFFIX) orig_address.tested orig_address.failed $(BIGTESTAPP)
	$(PIN) -t $< -o $<.out -- $(BIGTESTAPP) 
	grep -q "Success" $<.out
	grep -q -v "Failure" $<.out
	grep -q "eof" $<.out
	rm orig_address.failed; rm $<.out

$(OBJDIR)threadWait: threadWait.c
	$(CC) $(APP_CXXFLAGS) $(DBG) -o $@ $< -lpthread

$(OBJDIR)bigBinary: bigBinary.c
	$(CC) $(APP_CXXFLAGS) $(BIGBINARYFLAGS) ${OUTEXE}$@ $<

## build rules

$(OBJDIR)%.o : %.cpp $(BIGTESTAPP) $(THREADTESTAPP)
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
	
$(TOOLS): $(PIN_LIBNAMES)

$(TOOLS): $(OBJDIR)%$(PINTOOL_SUFFIX) : $(OBJDIR)%.o $(OBJDIR)utils.o
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(OBJDIR)utils.o $(DBG)

## cleaning
clean:
	-rm -rf $(OBJDIR) *.out *.tested *.failed *.exp *.lib 

tidy:
	-rm -rf $(OBJDIR) *.tested *.failed

-include *.d
