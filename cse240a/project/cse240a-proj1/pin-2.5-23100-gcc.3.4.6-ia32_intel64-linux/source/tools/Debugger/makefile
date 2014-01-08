##
## Tests to test the ability to debug the application while running with Pin.
##

include ../makefile.gnu.config
GDB = /usr/bin/gdb
GDB_MODERN = /usr/intel/pkgs/gdb/6.6/bin/gdb


APPS_ia32_l     = simple simple-static exec fork bptest-ia32 action-pending-app xmm-ia32 thread thread-static
APPS_ia32e_l    = simple simple-static exec fork bptest-intel64action-pending-app xmm-intel64 thread thread-static

TOOLS_ia32_l    = breaktool int3-count action-pending-tool
TOOLS_ia32e_l   = breaktool int3-count action-pending-tool

TESTS_ia32_l    = simple execfail fork breaktool breaktool-wait breaktool-nodebugger bptest-ia32 bp-icount \
                  action-pending xmm-ia32 thread thread-static
TESTS_ia32e_l   = simple execfail fork breaktool breaktool-wait breaktool-nodebugger bptest-intel64 bp-icount \
                  action-pending xmm-intel64 thread thread-static

apps  = $(APPS) $(APPS_$(TARGET)) $(APPS_$(TARGET_OS)) $(APPS_$(TARGET)_$(TARGET_OS))
tools = $(TOOLS) $(TOOLS_$(TARGET)) $(TOOLS_$(TARGET_OS)) $(TOOLS_$(TARGET)_$(TARGET_OS))
tests = $(TESTS) $(TESTS_$(TARGET)) $(TESTS_$(TARGET_OS)) $(TESTS_$(TARGET)_$(TARGET_OS)) dummy


# There is a bug in some 2.4 kernels that prevents Pin's debugger feature from
# working in cross-mode (32-bit application running on a 64-bit host).
# Disable testing in this case.  See Mantis #1414 for more details.
#
osrel = $(shell uname -r)
ifeq ($(findstring 2.4,$(osrel))-$(HOST_ARCH)-$(TARGET),2.4-ia32e-ia32)
  tests = 
endif

# There is a bug in the GDB installed on Redhat 9 (and probably previous versions too) which prevents debugging of MT
# programs with Pin.  Use a modern GDB version instead.
#
redhat = $(shell test -f /etc/redhat-release && cat /etc/redhat-release)
ifeq ($(findstring release 9,$(redhat)),release 9)
  GDB = $(GDB_MODERN)
endif


all: $(apps:%=$(OBJDIR)%) $(tools:%=$(OBJDIR)%.so)
test: $(tests:=.test)
tests-sanity: test

$(apps:%=$(OBJDIR)%) $(tools:%=$(OBJDIR)%): $(OBJDIR)make-directory

$(OBJDIR)make-directory:
	mkdir -p $(OBJDIR)
	touch  $(OBJDIR)make-directory
$(OBJDIR):
	mkdir -p $(OBJDIR)


#
# Rules to build the applications
#
$(OBJDIR)simple: simple.c
	$(CC) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ $<

$(OBJDIR)simple-static: simple.c
	$(CC) $(DBG) $(APP_CXXFLAGS) -static $(OUTOPT)$@ $<

$(OBJDIR)exec: exec.cpp
	$(CXX) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ $<

$(OBJDIR)fork: fork.cpp
	$(CXX) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ $<

$(OBJDIR)bptest-ia32: bptest.cpp bptest-asm-ia32.s
	$(CXX) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ bptest.cpp bptest-asm-ia32.s

$(OBJDIR)bptest-intel64: bptest.cpp bptest-asm-intel64.s
	$(CXX) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ bptest.cpp bptest-asm-intel64.s

$(OBJDIR)action-pending-app: action-pending-app.cpp
	$(CXX) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ $< -lpthread

$(OBJDIR)xmm-ia32: xmm.c xmm-asm-ia32.s
	$(CC) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ xmm.c xmm-asm-ia32.s

$(OBJDIR)xmm-intel64: xmm.c xmm-asm-intel64.s
	$(CC) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ xmm.c xmm-asm-intel64.s

$(OBJDIR)thread: thread.cpp
	$(CXX) $(DBG) $(APP_CXXFLAGS) $(OUTOPT)$@ $< -lpthread

$(OBJDIR)thread-static: thread.cpp
	$(CXX) $(DBG) $(APP_CXXFLAGS) -static $(OUTOPT)$@ $< -lpthread


#
# Rules to build the object files
#
$(OBJDIR)%.o: %.cpp
	$(CXX) $(COPT) $(CXXFLAGS) $(PIN_CXXFLAGS) $(OUTOPT)$@ $<

#
# Rules to build the tools
#
$(OBJDIR)breaktool$(PINTOOL_SUFFIX): $(OBJDIR)breaktool.o
	$(CXX) $(PIN_LDFLAGS) $(DBG) ${LINK_OUT}$@ $< $(PIN_LIBS)

$(OBJDIR)int3-count$(PINTOOL_SUFFIX): $(OBJDIR)int3-count.o
	$(CXX) $(PIN_LDFLAGS) $(DBG) ${LINK_OUT}$@ $< $(PIN_LIBS)

$(OBJDIR)action-pending-tool$(PINTOOL_SUFFIX): $(OBJDIR)action-pending-tool.o
	$(CXX) $(PIN_LDFLAGS) $(DBG) ${LINK_OUT}$@ $< $(PIN_LIBS)


#
# Rules to run the tests.
#

# Basic test of debugger features.
#
simple.test: $(OBJDIR)simple $(OBJDIR)simple.tested $(OBJDIR)simple.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -- $(OBJDIR)simple > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)simple > $(OBJDIR)$(@:.test=.gdbout)
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Verify we can debug across a failed exec() call.
#
execfail.test: $(OBJDIR)exec $(OBJDIR)execfail.tested $(OBJDIR)execfail.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -follow_execv -- $(OBJDIR)exec ./does-not-exist > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)exec > $(OBJDIR)$(@:.test=.gdbout)
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Verify we can debug across a parent call to fork().
#
fork.test: $(OBJDIR)fork $(OBJDIR)fork.tested $(OBJDIR)fork.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -- $(OBJDIR)fork > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)fork > $(OBJDIR)$(@:.test=.gdbout)
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Basic test of PIN_ApplicationBreakpoint()
#
breaktool.test: $(OBJDIR)simple $(OBJDIR)breaktool$(PINTOOL_SUFFIX) $(OBJDIR)breaktool.tested $(OBJDIR)breaktool.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -t $(OBJDIR)breaktool$(PINTOOL_SUFFIX) -wait_for_debugger 0 -- $(OBJDIR)simple > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)simple > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Verify that PIN_ApplicationBreakpoint(.., TRUE, ..) will wait if there's no debugger.
#
breaktool-wait.test: $(OBJDIR)simple $(OBJDIR)breaktool$(PINTOOL_SUFFIX) $(OBJDIR)breaktool-wait.tested $(OBJDIR)breaktool-wait.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_disable 0 -t $(OBJDIR)breaktool$(PINTOOL_SUFFIX) -wait_for_debugger 1 -- $(OBJDIR)simple > $(OBJDIR)$(@:.test=.out) &
	until grep 'on port' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	sleep 5
	port=`grep 'on port' $(OBJDIR)$(@:.test=.out) | sed 's/.*on port //'`; \
	    echo "target remote :$$port" >  $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)simple > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Verify that PIN_ApplicationBreakpoint(.., FALSE, ..) does not wait if there's no debugger.
#
breaktool-nodebugger.test: $(OBJDIR)simple $(OBJDIR)breaktool$(PINTOOL_SUFFIX) $(OBJDIR)breaktool-nodebugger.tested $(OBJDIR)breaktool-nodebugger.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_disable 0 -t $(OBJDIR)breaktool$(PINTOOL_SUFFIX) -wait_for_debugger 0 -- $(OBJDIR)simple
	rm -f $(OBJDIR)$(@:.test=.failed)

# Test breakpoints in various circumstances.
#
bptest-ia32.test: $(OBJDIR)bptest-ia32 $(OBJDIR)bptest-ia32.tested $(OBJDIR)bptest-ia32.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -- $(OBJDIR)bptest-ia32 > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)bptest-ia32 > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

bptest-intel64.test: $(OBJDIR)bptest-intel64 $(OBJDIR)bptest-intel64.tested $(OBJDIR)bptest-intel64.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -- $(OBJDIR)bptest-intel64 > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)bptest-intel64 > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Test that breakpoints do not cause the tool to see any extra instructions (e.g. INT3).
#
bp-icount.test: $(OBJDIR)simple-static $(OBJDIR)int3-count$(PINTOOL_SUFFIX) $(OBJDIR)bp-icount.tested $(OBJDIR)bp-icount.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -t $(OBJDIR)int3-count$(PINTOOL_SUFFIX) -func main -o $(OBJDIR)bp-icount.reference -- $(OBJDIR)simple-static
	$(PIN) -xyzzy -debug_app -t $(OBJDIR)int3-count$(PINTOOL_SUFFIX) -func main -o $(OBJDIR)bp-icount.count -- $(OBJDIR)simple-static > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)simple-static > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	$(PIN_CMP) $(OBJDIR)bp-icount.reference $(OBJDIR)bp-icount.count
	rm -f $(OBJDIR)$(@:.test=.failed)

# Test the PIN_IsActionPending() API.
#
action-pending.test: $(OBJDIR)action-pending-app $(OBJDIR)action-pending-tool$(PINTOOL_SUFFIX) $(OBJDIR)action-pending.tested $(OBJDIR)action-pending.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -t $(OBJDIR)action-pending-tool$(PINTOOL_SUFFIX) -- $(OBJDIR)action-pending-app > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)action-pending-app > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Test that we can print out XMM registers.  Older GDB's don't know how to print XMM registers well,
# so use a modern GDB for this test.
#
# We first test that GDB itself will run.  If not, we just skip the body of this test.  The modern
# GDB won't run on some old test systems.
#
xmm-ia32.test: $(OBJDIR)xmm-ia32 $(OBJDIR)xmm-ia32.tested $(OBJDIR)xmm-ia32.failed
	if $(GDB_MODERN) -batch -x quit.gdb -n; then \
	    rm -f $(OBJDIR)$(@:.test=.out); \
	    $(PIN) -xyzzy -debug_app -- $(OBJDIR)xmm-ia32 > $(OBJDIR)$(@:.test=.out) & \
	    until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	        do sleep 1; done; \
	    grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin); \
	    cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin); \
	    $(GDB_MODERN) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)xmm-ia32 > $(OBJDIR)$(@:.test=.gdbout) 2>&1; \
	    $(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout); \
	fi
	rm -f $(OBJDIR)$(@:.test=.failed)

xmm-intel64.test: $(OBJDIR)xmm-intel64 $(OBJDIR)xmm-intel64.tested $(OBJDIR)xmm-intel64.failed
	if $(GDB_MODERN) -batch -x quit.gdb -n; then \
	    rm -f $(OBJDIR)$(@:.test=.out); \
	    $(PIN) -xyzzy -debug_app -- $(OBJDIR)xmm-intel64 > $(OBJDIR)$(@:.test=.out) & \
	    until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	        do sleep 1; done; \
	    grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin); \
	    cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin); \
	    $(GDB_MODERN) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)xmm-intel64 > $(OBJDIR)$(@:.test=.gdbout) 2>&1; \
	    $(PYTHON) ../compare.py -p $(@:.test=.compare) -c $(OBJDIR)$(@:.test=.gdbout); \
	fi
	rm -f $(OBJDIR)$(@:.test=.failed)

# Simple test of a threaded program.
#
thread.test: $(OBJDIR)thread $(OBJDIR)thread.tested $(OBJDIR)thread.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -- $(OBJDIR)thread > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat thread.gdb >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)thread > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p thread.compare -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

# Simple test of a threaded program built statically (uses non-nptl thread package on Linux).
#
thread-static.test: $(OBJDIR)thread-static $(OBJDIR)thread-static.tested $(OBJDIR)thread-static.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -xyzzy -debug_app -- $(OBJDIR)thread-static > $(OBJDIR)$(@:.test=.out) &
	until grep 'target remote' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	grep 'target remote' $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	cat thread.gdb >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(OBJDIR)thread-static > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	$(PYTHON) ../compare.py -p thread.compare -c $(OBJDIR)$(@:.test=.gdbout)
	rm -f $(OBJDIR)$(@:.test=.failed)

dummy.test:


clean:
	rm -rf $(OBJDIR)
	rm -f pin.log
