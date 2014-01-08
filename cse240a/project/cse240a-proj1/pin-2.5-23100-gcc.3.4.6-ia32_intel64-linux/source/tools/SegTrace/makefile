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
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
endif


ifeq ($(TARGET_OS),w)
    ifeq ($(TARGET),ia32)
        APPS  = hello
        TOOL_ROOTS = segtrace
        TESTS = trace_hello
    endif
    ifeq ($(TARGET),ia32e)
        APPS  = hello
        TOOL_ROOTS = segtrace
        TESTS = trace_hello
    endif
else
    ifeq ($(TARGET),ia32)
        APPS  = thread
        TOOL_ROOTS = segtrace
        TESTS = trace_thread
    endif
    ifeq ($(TARGET),ia32e)
        APPS  = thread
        TOOL_ROOTS = segtrace
        TESTS = trace_thread
    endif
endif

TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))

all: $(OBJDIR) $(APPS:%=$(OBJDIR)%) $(TOOLS)
test: $(OBJDIR) $(TESTS:=.test)
tests-sanity: test

$(OBJDIR):
	mkdir -p $(OBJDIR)



#
# Rules to build the applications
#
$(OBJDIR)thread: thread.c
	$(CC) $(APP_CXXFLAGS) $(OUTEXE)$@ $< -lpthread

$(OBJDIR)hello: hello.c
	$(CC) $(APP_CXXFLAGS) $(OUTEXE)$@ $<


#
# Rules to build the tools
#
$(OBJDIR)%.o : %.cpp
	$(CXX) $(COPT) $(CXXFLAGS) $(PIN_CXXFLAGS) $(OUTOPT)$@ $< 

$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	$(LINKER) $(PIN_LDFLAGS) $(LINK_DEBUG) $(LINK_OUT)$@ $< $(PIN_LPATHS) $(PIN_LIBS) $(DBG)


#
# Rules to run the tests.
#
trace_thread.test: trace_thread.tested trace_thread.failed $(OBJDIR)segtrace$(PINTOOL_SUFFIX) $(OBJDIR)thread
	$(PIN) -t $(OBJDIR)segtrace$(PINTOOL_SUFFIX) -- ./$(OBJDIR)thread
	rm $(@:.test=.failed)

trace_hello.test: trace_hello.tested trace_hello.failed $(OBJDIR)segtrace$(PINTOOL_SUFFIX) $(OBJDIR)hello
	$(PIN) -t $(OBJDIR)segtrace$(PINTOOL_SUFFIX) -- ./$(OBJDIR)hello
	rm $(@:.test=.failed)


clean:
	rm -rf $(OBJDIR) $(TESTS:=.tested) $(TESTS:=.failed) *.out
