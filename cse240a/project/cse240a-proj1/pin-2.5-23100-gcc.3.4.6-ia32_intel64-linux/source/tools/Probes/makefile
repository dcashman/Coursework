
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

EXTRA_LIBS =
SUF = $(PINTOOL_SUFFIX)

all: tools

SANITY_TOOLS = 
TOOLS =

# rc added this test, but not ready for general testing
#TOOL_ROOTS = replaceebx


# linux tests go here.  if PROBE=0 is specified on the command line, do not run probe tests.
ifeq ($(PROBE),1)
 ifneq ($(ENABLE_VS),1)
  ifeq ($(TARGET_OS),l)

    # probe2 & probe4 are negative tests (they are supposed to fail)
    ifeq ($(TARGET),ia32)
      TOOL_ROOTS += atraceprobe probemalloctrace iprtool malloctrace2 symbolnames replace_exit 
      TOOL_ROOTS += proto1 proto2 proto3 proto4 replace_empty replacesig_empty
      TOOL_ROOTS +=  probe1 probe2 probe3 probe5 context_probe malloctrace_locktest
      TOOL_ROOTS += init forktool remove_probe debugtest probe8 probesleep
      TOOL_ROOTS += probe_err1 probe_err2
      TOOL_ROOTS += insert1 insert2 insert3 insert4 insert5 insert6 insert7 insert8 insert9
      TOOL_ROOTS += insert10 insert11
      REPLACEAPP = $(OBJDIR)replaceapp_lin32
      REPLACECALL = $(OBJDIR)replacecall_lin32
      ERRCALL = $(OBJDIR)err_call_lin32
      REPLACEFUN = $(OBJDIR)replacefun_lin32
      ERRREPLACE = $(OBJDIR)err_replace_lin32
      BADJUMP= $(OBJDIR)bad_jump_lin32
    endif

    ifeq ($(TARGET),ia32e)
      TOOL_ROOTS += probemalloctrace iprtool malloctrace2 symbolnames replace_empty replacesig_empty 
      TOOL_ROOTS += proto1 proto2 proto3 proto4 replace_exit
      TOOL_ROOTS += probe1 probe2 probe3 probe4 probe5 probe6 probe7 probe8 context_probe 
      TOOL_ROOTS += init forktool remove_probe debugtest probesleep probe9
      TOOL_ROOTS += malloctrace_locktest probe_err1 probe_err2
      TOOL_ROOTS += insert1 insert2 insert3 insert4 insert5 insert6 insert7 insert8 insert9
      TOOL_ROOTS += insert10 insert11
      REPLACEAPP = $(OBJDIR)replaceapp_lin64
      REPLACECALL = $(OBJDIR)replacecall_lin64
      REPLACEDISP = $(OBJDIR)replacedisp_lin64
      REPLACEJMP = $(OBJDIR)replacejmp_lin64
      REPLACESHORTY = $(OBJDIR)replaceshorty_lin64
      ERRREPLACE = $(OBJDIR)err_replace_lin64
      ERRCALL = $(OBJDIR)err_call_lin64
      REPLACEFUN = $(OBJDIR)replacefun_lin64
      BADJUMP= $(OBJDIR)bad_jump_lin64
    endif

    ifeq ($(TARGET),ipf)
      TOOL_ROOTS += probemalloctrace iprtool proto1 proto2 symbolnames
      TOOL_ROOTS += replace_empty replacesig_empty malloctrace4 malloctrace_tpbug 
      TOOL_ROOTS += forktool remove_probe probesleep
      TOOL_ROOTS += insert1 insert2 insert3 insert4
    endif

    TOOLS += $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))
    CP = $(TESTAPP)
    DLTEST = $(OBJDIR)dltest
    DLTEST_TP = $(OBJDIR)dltest-tp
    MALLOCWRAPPERS = $(OBJDIR)mallocwrappers.so
    IPR = $(OBJDIR)ipr
    UNLOADTEST = $(OBJDIR)unloadtest
    LITTLE_MALLOC=$(OBJDIR)little_malloc
    SLEEP = $(OBJDIR)sleep
    SIMPLEFOO9 = $(OBJDIR)simplefoo9
    SIMPLEFOO10 = $(OBJDIR)simplefoo10
  endif
 endif # Enable VS
endif # PROBE

# windows tests go here:
ifeq ($(TARGET_OS),w)
  # proto2 uses a local symbol.  Only imports and exports can be 
  # replaced on windows.
  TOOL_ROOTS += iprtool proto1win proto3win probemalloctrace
  TOOL_ROOTS += replace_empty replacesig_empty probecdecl tpss
  TOOL_ROOTS += insert1win insert2win insert3win insert10 insert11
  SIMPLEFOO9 = $(OBJDIR)winfoo9.exe
  SIMPLEFOO10 = $(OBJDIR)winfoo10.exe
 
  # unresolved external symbol ___intel_new_proc_init when using icl
  ifneq ($(CC),icl)
    TOOL_ROOTS += probesleep
  endif

  ifeq ($(TARGET_COMPILER),ms)
    TOOL_ROOTS += malloctrace2win ssewin
    DLTEST=$(OBJDIR)dltestwin.exe
    MALLOCWRAPPERS=$(OBJDIR)mallocwrapperswin.dll
    IPR = $(OBJDIR)ipr.exe
    UNLOADTEST=$(OBJDIR)unloadtestwin.exe
    LITTLE_MALLOC=$(OBJDIR)little_malloc.exe
    SLEEP = $(OBJDIR)sleep.exe


    ifeq ($(TARGET),ia32)
      TOOL_ROOTS += probestdcall probefastcall probeheapalloc shortBB
      BBTEST=$(OBJDIR)shortBBapp.exe
    endif

    ifeq ($(TARGET),ia32e)
      TOOL_ROOTS += shortBB64
      BBTEST=$(OBJDIR)shortBB64app.exe
    endif

  else
    DLTEST=$(OBJDIR)dltest.exe
    MALLOCWRAPPERS=$(OBJDIR)mallocwrappers.dll
    IPR = ipr
  endif

  TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))
  CP = $(TESTAPP)

endif

# mac tests go here:
ifeq ($(TARGET_OS),m)
  #TOOL_ROOTS = malloctrace2 probemalloctrace
  TOOL_ROOTS = 

  TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))
  DLTEST=$(OBJDIR)dltest-mac
  MALLOCWRAPPERS=$(OBJDIR)mallocwrappers-mac.so
endif

DL_LIB = -ldl

SANITY_TESTS = $(TOOL_ROOTS:%=%.test)

tools: $(OBJDIR) $(TOOLS)
test: $(OBJDIR) $(TOOL_ROOTS:%=%.test)
tests-sanity: $(OBJDIR) $(SANITY_TESTS)

$(OBJDIR):
	mkdir -p $(OBJDIR)


###########################################
#                                         #
#    Applications                         #
#                                         #
###########################################

$(OBJDIR)traceapp: traceapp.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I../../include -I. -o $@ traceapp.c -lpinapp -g

$(OBJDIR)replaceapp_lin32: replaceapp.c $(OBJDIR)do_nothing_lin32.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)replacecall_lin32: replacecall.c $(OBJDIR)do_nothing_lin32.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)replacefun_lin32: replacefun.c $(OBJDIR)do_nothing_lin32.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)do_nothing_lin32.o: do_nothing_lin32.asm 
	$(CC) $(APP_CXXFLAGS) -x assembler-with-cpp -c -o $@ $<

$(OBJDIR)replaceapp_lin64: replaceapp.c $(OBJDIR)do_nothing_lin64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)replacecall_lin64: replacecall.c $(OBJDIR)do_nothing_lin64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)replacefun_lin64: replacefun.c $(OBJDIR)do_nothing_lin64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)replacejmp_lin64: replacejmp.c $(OBJDIR)pc64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@ -g

$(OBJDIR)replaceshorty_lin64: replaceshorty.c $(OBJDIR)do_nothing_lin64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@ -g

$(OBJDIR)replacedisp_lin64: replacedisp.c $(OBJDIR)pc64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@ -g

$(OBJDIR)pc64.o: pc64.s 
	$(CC) $(APP_CXXFLAGS)  -x assembler-with-cpp -c $< -o $@

$(OBJDIR)do_nothing_lin64.o: do_nothing_lin64.asm 
	$(CC) $(APP_CXXFLAGS) -x assembler-with-cpp -c $< -o $@

$(OBJDIR)getesp_ms.obj: getesp_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)probestdcall_app: probestdcall_app.c  $(OBJDIR)getesp_ms.obj  probe_stdcall_fastcall.h
	cp probestdcall_app.exe.zi $(OBJDIR)probestdcall_app.exe
	cp probestdcall_app.pdb.zi $(OBJDIR)probestdcall_app.pdb

$(OBJDIR)probefastcall_app: probefastcall_app.c  $(OBJDIR)getesp_ms.obj probe_stdcall_fastcall.h
	cp probefastcall_app.exe.zi $@.exe
	cp probefastcall_app.pdb.zi $@.pdb

$(OBJDIR)bad_jump_lin32: bad_jump.c $(OBJDIR)local_branch_lin32.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)err_replace_lin32: err_replace.c $(OBJDIR)local_branch_lin32.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)err_call_lin32: err_call.c $(OBJDIR)local_branch_lin32.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)local_branch_lin32.o: local_branch_lin32.asm 
	$(CC) $(APP_CXXFLAGS) -x assembler-with-cpp -c -o $@ $<

$(OBJDIR)bad_jump_lin64: bad_jump.c $(OBJDIR)local_branch_lin64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)err_replace_lin64: err_replace.c $(OBJDIR)local_branch_lin64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)err_call_lin64: err_call.c $(OBJDIR)local_branch_lin64.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $+ -o $@  -g

$(OBJDIR)local_branch_lin64.o:local_branch_lin64.asm 
	$(CC) $(APP_CXXFLAGS) -x assembler-with-cpp -c $< -o $@

$(OBJDIR)protofoo: protofoo.c protobar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $@ protofoo.c protobar.c -g

$(OBJDIR)protorfoo: protofoo.c protorbar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $@ protofoo.c protorbar.c -g

$(OBJDIR)simplefoo10: simplefoo10.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $@ simplefoo10.c simplebar.c -g

$(OBJDIR)winfoo10.exe: simplefoo10.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo10.c simplebar.c

$(OBJDIR)winfast.exe: simplefast10.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefast10.c simplebar.c

$(OBJDIR)winstd.exe: simplestd10.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplestd10.c simplebar.c

$(OBJDIR)simplefoo9: simplefoo9.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $@ simplefoo9.c simplebar.c -g

$(OBJDIR)winfoo9.exe: simplefoo9.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo9.c simplebar.c

$(OBJDIR)empty: empty.c emptysub.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. $(OUTEXE)$@ empty.c emptysub.c -O3

$(OBJDIR)little_malloc: little_malloc.c
	$(CC) $(APP_CXXFLAGS) -g little_malloc.c -o $@

$(OBJDIR)little_malloc.exe: little_malloc.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ little_malloc.c

$(OBJDIR)ipr: ipr.c
	$(CC) $(APP_CXXFLAGS) -g -o $@ ipr.c

$(OBJDIR)ipr.exe: ipr.c
	$(CC) ipr.c $(OUTEXE)$@

$(OBJDIR)sleep: sleep.c
	$(CC) $(APP_CXXFLAGS) -g -o $@ sleep.c

$(OBJDIR)sleep.exe: sleepwin.c
	$(CC) sleepwin.c $(OUTEXE)$@

$(OBJDIR)mallocwrappers.so: mallocwrappers.c
	$(CC) $(APP_CXXFLAGS) -o $@ -fpic -shared mallocwrappers.c -g

$(OBJDIR)mallocwrapperswin.dll: mallocwrapperswin.c
	$(CC) mallocwrapperswin.c /nologo $(PIN_CXXFLAGS) /link /dll /out:$@

$(OBJDIR)mallocwrappers.dll: mallocwrapperswin.c
	$(CC) -o $(OBJDIR)mallocwrapperswin.dll -shared mallocwrapperswin.c -g  $(APP_CXXFLAGS)

$(OBJDIR)mallocwrappers-mac.so: mallocwrappers.c
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)mallocwrappers-mac.so -fPIC -dynamiclib mallocwrappers.c -g

$(OBJDIR)dltest: dltest.c one.c two.c 
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)two.so -fpic -shared two.c -g
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)one.so -fpic -shared one.c -g
	$(CC) -o $(OBJDIR)dltest $(APP_CXXFLAGS) dltest.c $(DL_LIB) -Wl,-rpath,`pwd`\/$(OBJDIR) -g

$(OBJDIR)dltest-tp: dltest.c one.c two.c 
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)two.so -fpic -shared two.c -g
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)one.so -fpic -shared one.c -g
	$(CC) -o $(OBJDIR)dltest-tp $(APP_CXXFLAGS) dltest.c $(DL_LIB) -Wl,-rpath,`pwd`\/$(OBJDIR) -g -lpthread

$(OBJDIR)dltest-pf: dltestpf.c one.c two.c 
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)two.so -fpic -shared two.c -g
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)one.so -fpic -shared one.c -g
	$(CC) -o $(OBJDIR)dltest-pf $(APP_CXXFLAGS) dltestpf.c $(DL_LIB) -Wl,-rpath,`pwd`\/$(OBJDIR) -g

$(OBJDIR)dltest-mac: dltest.c one.c two.c
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)two.so -fPIC -dynamiclib two.c -g
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)one.so -fPIC -dynamiclib one.c -g
	$(CC) -o $(OBJDIR)dltest-mac $(APP_CXXFLAGS) dltest.c $(DL_LIB) -g


$(OBJDIR)dltest.exe: dltestwin.c onewin.c twowin.c 
	$(CC) -o $(OBJDIR)two.dll -shared twowin.c $(APP_CXXFLAGS)
	$(CC) -o $(OBJDIR)one.dll -shared onewin.c $(APP_CXXFLAGS)
	$(CC) -o $(OBJDIR)dltest.exe -g dltestwin.c $(APP_CXXFLAGS) 

$(OBJDIR)shortBBapp.exe: shortBBapp.c 
	$(CC) -o $@ $(APP_CXXFLAGS) shortBBapp.c

$(OBJDIR)shortBB64app.exe: shortBB64app.c $(OBJDIR)shortBB64foo.obj
	$(CC) $(OUTEXE)$@ $(APP_CXXFLAGS) $+

$(OBJDIR)shortBB64foo.obj: shortBB64foo.asm
	ml64 /nologo /c /Fo$@ $<

$(OBJDIR)dltestwin.exe: dltestwin.c onewin.c twowin.c 
	$(CC) twowin.c  /nologo $(APP_CXXFLAGS) /link /dll /out:$(OBJDIR)two.dll
	$(CC) onewin.c  /nologo $(APP_CXXFLAGS) /link /dll /out:$(OBJDIR)one.dll
	$(CC) /Fe$@ dltestwin.c  /nologo $(APP_CXXFLAGS) 

$(OBJDIR)unloadtest: unloadtest.c one.c two.c 
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)two.so -fpic -shared two.c -g
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)one.so -fpic -shared one.c -g
	$(CC) $(APP_CXXFLAGS) -o $(OBJDIR)unloadtest $(APP_CXXFLAGS) unloadtest.c $(DL_LIB) -Wl,-rpath,`pwd`/$(OBJDIR) -g

$(OBJDIR)unloadtestwin.exe: unloadtestwin.c onewin.c twowin.c 
	$(CC) twowin.c  /nologo $(APP_CXXFLAGS) /link /dll /out:two.dll
	$(CC) onewin.c  /nologo $(APP_CXXFLAGS) /link /dll /out:one.dll
	$(CC) /Fe$@ unloadtestwin.c  /nologo $(APP_CXXFLAGS) 

$(OBJDIR)heavy_stack_win.exe: heavy_stack_win.c
	$(CC) /Fe$@ heavy_stack_win.c  /nologo $(APP_CXXFLAGS) 

$(OBJDIR)forkapp: forkapp.cpp
	$(CXX) $(APP_CXXFLAGS) $< -o $@

$(OBJDIR)ebxhandle: ebxhandle.c ebx.s
	$(CC) $(APP_CXXFLAGS) -g -o $@ ebxhandle.c ebx.s

$(OBJDIR)simplefoo: simplefoo.c simplebar.c $(OBJDIR)simplesp.o
	$(CC) $(PIN_LPATHS) $(APP_CXXFLAGS) -I../Include -I. -static -o $@ $+ -g

$(OBJDIR)simplesp.o: simplesp.asm 
	$(CC) $(APP_CXXFLAGS) -x assembler-with-cpp -c $< -o $@

$(OBJDIR)thd_malloc: thd_malloc.c
	$(CC) $(APP_CXXFLAGS) -o $@ thd_malloc.c -lpthread

$(OBJDIR)mttraceapp: mttraceapp.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I../../include -I. -o $(OBJDIR)mttraceapp mttraceapp.c -lpinapp -lpthread -g

##############################
#                            #
#     Tools                  #
#                            #
##############################

probe1.test : $(OBJDIR)probe1$(SUF) probe1.tested probe1.failed $(REPLACEAPP)
	$(PIN) -t $< -- ./$(REPLACEAPP) >  $<.out 2>&1
	grep "Nothing" probe1.outfile
	rm -f probe1.failed  $<.out

#probe2 is a negative test.
probe2.test : $(OBJDIR)probe2$(SUF) probe2.tested probe2.failed $(ERRREPLACE)
	-$(PIN) -t $< -- ./$(ERRREPLACE) >  probe2.out 2>&1
	echo "probe2 should fail.  Ignore the error."
	rm -f probe2.failed  $<.out

probe3.test : $(OBJDIR)probe3$(SUF) probe3.tested probe3.failed $(REPLACECALL)
	$(PIN) -t $< -- ./$(REPLACECALL) >  $<.out 2>&1
	grep "Nothing" probe3.outfile
	rm -f probe3.failed  $<.out

#probe4 works correctly on ia32 with probe size = 5. negative test on intel64.
probe4.test : $(OBJDIR)probe4$(SUF) probe4.tested probe4.failed $(ERRCALL)
	-$(PIN) -t $<  -xyzzy -mesgon log_probe -- ./$(ERRCALL) >  probe4.out 2>&1
	echo "probe4 should fail.  Ignore the error."
	grep call_function pintool.log
	rm -f probe4.failed  $<.out pintool.log

probe5.test : $(OBJDIR)probe5$(SUF) probe5.tested probe5.failed $(REPLACEFUN)
	$(PIN) -t $< -- ./$(REPLACEFUN) >  $<.out 2>&1
	grep "Nothing" probe5.outfile
	rm -f probe5.failed  $<.out

probe6.test : $(OBJDIR)probe6$(SUF) probe6.tested probe6.failed $(REPLACEDISP)
	$(PIN) -t $< -- ./$(REPLACEDISP) >  $<.out 2>&1
	grep "Doing" probe6.outfile
	rm -f probe6.failed  $<.out


probe7.test : $(OBJDIR)probe7$(SUF) probe7.tested probe7.failed $(REPLACEJMP)
	-$(PIN) -t $< -xyzzy -mesgon log_probe -- ./$(REPLACEJMP) >  $<.out 2>&1
	grep "Doing" probe7.outfile
	rm -f probe7.failed  $<.out

#probe8 is a negative test.
probe8.test : $(OBJDIR)probe8$(SUF) probe8.tested probe8.failed $(BADJUMP)
	-$(PIN) -t $<  -xyzzy -mesgon log_probe -- ./$(BADJUMP) >  probe8.out 2>&1
	echo "probe8 should fail.  Ignore the error."
	grep unconditional pintool.log
	rm -f probe8.failed  probe8.out probe8.outfile pintool.log


probe9.test : $(OBJDIR)probe9$(SUF) probe9.tested probe9.failed $(REPLACESHORTY)
	-$(PIN) -t $< -xyzzy -mesgon log_probe -- ./$(REPLACESHORTY) >  $<.out 2>&1
	grep "Short" probe9.outfile
	rm -f probe9.failed  $<.out


#probe_err1 is a negative test.
probe_err1.test : $(OBJDIR)probe_err1$(SUF) probe_err1.tested probe_err1.failed $(BADJUMP)
	-$(PIN) -error_file myerr.log -t $<  -- ./$(BADJUMP) >  probe_err1.out 2>&1
	echo "probe_err1 should fail.  Ignore the error."
	grep bad_jump myerr.log
	rm -f probe_err1.failed probe_err1.out myerr.log

#probe_err2 is a negative test.
probe_err2.test : $(OBJDIR)probe_err2$(SUF) probe_err2.tested probe_err2.failed $(LITTLE_MALLOC)
	-$(PIN) -error_file myerr.log -t $<  -- ./$(LITTLE_MALLOC) >  probe_err2.out 2>&1
	echo "probe_err2 should fail.  Ignore the error."
	grep -e "not supported" myerr.log
	rm -f probe_err2.failed probe_err2.out myerr.log

probemalloctrace.test : $(OBJDIR)probemalloctrace$(SUF) probemalloctrace.tested probemalloctrace.failed
	$(PIN) -t $< -- $(CP) makefile $<.makefile.copy >  $<.out 2>&1
	diff makefile $<.makefile.copy
	grep "Probe" probemalloctrace.outfile
	rm -f probemalloctrace.failed  $<.out $<.makefile.copy

probecdecl.test : $(OBJDIR)probecdecl$(SUF) probecdecl.tested probecdecl.failed
	$(PIN) -t $< -- $(CP) makefile $<.makefile.copy >  $<.out 2>&1
	diff makefile $<.makefile.copy
	grep "Probe" probecdecl.outfile
	rm -f probecdecl.failed  $<.out $<.makefile.copy

probeheapalloc.test : $(OBJDIR)probeheapalloc$(SUF) probeheapalloc.tested probeheapalloc.failed
	$(PIN) -t $< -- $(CP) makefile $<.makefile.copy >  $<.out 2>&1
	diff makefile $<.makefile.copy
	grep "Probe" probeheapalloc.outfile
	rm -f probeheapalloc.failed  $<.out $<.makefile.copy

probestdcall.test : $(OBJDIR)probestdcall$(SUF) probestdcall.tested probestdcall.failed $(OBJDIR)probestdcall_app
	$(PIN) -t $< -- $(OBJDIR)probestdcall_app >  $<.out 2>&1
	grep  "SUCCESS" $<.out
	rm -f probestdcall.failed  $<.out

probefastcall.test : $(OBJDIR)probefastcall$(SUF) probefastcall.tested probefastcall.failed $(OBJDIR)probefastcall_app
	$(PIN) -t $< -- $(OBJDIR)probefastcall_app >  $<.out 2>&1
	grep  "SUCCESS" $<.out
	rm -f probefastcall.failed  $<.out

proto1.test : $(OBJDIR)proto1$(SUF) proto1.tested proto1.failed
	$(PIN) -t $< -- $(CP) makefile $<.makefile.copy >  $<.out 2>&1
	diff makefile $<.makefile.copy
	rm -f proto1.failed  $<.out $<.makefile.copy

proto1win.test : $(OBJDIR)proto1win$(SUF) proto1.tested proto1.failed $(OBJDIR)dltestwin.exe
	$(PIN) -t $< -- ./$(OBJDIR)dltestwin >  $<.out 2>&1
	grep "Probe_Malloc" $<.out
	rm -f proto1.failed  $<.out $<.makefile.copy

proto2.test : $(OBJDIR)proto2$(SUF) $(OBJDIR)protofoo proto2.tested proto2.failed
	$(PIN) -t $< -- ./$(OBJDIR)protofoo >  $<.out 2>&1
	grep -e "33" $<.out
	grep -e "100" $<.out
	rm -f proto2.failed  $<.out

replacesig_empty.test : $(OBJDIR)replacesig_empty$(SUF) $(OBJDIR)empty replacesig_empty.tested replacesig_empty.failed
	$(PIN) -t $< -- ./$(OBJDIR)empty >  $<.out 2>&1
	grep -e "33" $<.out
	rm -f replacesig_empty.failed  $<.out

replace_empty.test : $(OBJDIR)replace_empty$(SUF) $(OBJDIR)empty replace_empty.tested replace_empty.failed
	$(PIN) -t $< -- ./$(OBJDIR)empty >  $<.out 2>&1
	grep -e "33" $<.out
	rm -f replace_empty.failed  $<.out

proto3.test : $(OBJDIR)proto3$(SUF) proto3.tested proto3.failed
	$(PIN) -probe -t $< -- $(CP) makefile $<1.makefile.copy >  $<1.out 2>&1
	diff makefile $<1.makefile.copy
	grep -q "Probe_Malloc" $<1.out
	$(PIN) -t $< -- $(CP) makefile $<2.makefile.copy >  $<2.out 2>&1
	diff makefile $<2.makefile.copy
	grep -q "Jit_Malloc" $<2.out
	rm -f proto3.failed  $<1.out $<2.out $<1.makefile.copy $<2.makefile.copy

proto3win.test : $(OBJDIR)proto3win$(SUF) proto3win.tested proto3win.failed $(OBJDIR)dltestwin.exe
	$(PIN) -probe -t $< -- ./$(OBJDIR)dltestwin.exe >  $<1.out 2>&1
	grep -q "Probe_Malloc" $<1.out
	$(PIN) -t $< -- ./$(OBJDIR)dltestwin.exe >  $<2.out 2>&1
	grep -q "Jit_Malloc" $<2.out
	rm -f proto3win.failed  $<1.out $<2.out $<1.makefile.copy $<2.makefile.copy

proto4.test : $(OBJDIR)proto4$(SUF) proto4.tested proto4.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<1.out 2>&1
	grep "passed" $<1.out
	$(PIN) -t $< -- ./$(LITTLE_MALLOC) >  $<2.out 2>&1
	grep "passed" $<2.out
	rm -f proto4.failed  $<1.out $<2.out

insert1.test : $(OBJDIR)insert1$(SUF) insert1.tested insert1.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "passed" $<.out
	rm -f insert1.failed  $<.out

insert2.test : $(OBJDIR)insert2$(SUF) insert2.tested insert2.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "passed" $<.out
	grep -q -e "Before 3" $<.out
	rm -f insert2.failed  $<.out

insert3.test : $(OBJDIR)insert3$(SUF) insert3.tested insert3.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<1.out 2>&1
	grep "passed" $<1.out
	grep -q -e "Before_MyMalloc" $<1.out
	grep -q -e "Before_MyFree" $<1.out
	rm -f insert3.failed  $<1.out

insert4.test : $(OBJDIR)insert4$(SUF) insert4.tested insert4.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "passed" $<.out
	grep -q -e "Before_Malloc" $<.out
	grep -q -e "Before_Free" $<.out
	rm -f insert4.failed  $<.out

insert5.test : $(OBJDIR)insert5$(SUF) insert5.tested insert5.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "passed" $<.out
	grep -q -e "Before_Malloc" $<.out
	grep -q -e "After_Malloc" $<.out
	rm -f insert5.failed  $<.out

insert6.test : $(OBJDIR)insert6$(SUF) insert6.tested insert6.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "passed" $<.out
	grep -q -e "Before_Malloc" $<.out
	grep -q -e "After_Malloc" $<.out
	rm -f insert6.failed  $<.out

insert7.test : $(OBJDIR)insert7$(SUF) insert7.tested insert7.failed $(OBJDIR)protofoo
	$(PIN) -probe -t $< -- ./$(OBJDIR)protofoo >  $<.out 2>&1
	grep -q -e "345" $<.out
	grep -q -e "Before" $<.out
	grep -q -e "After" $<.out
	rm -f insert7.failed  $<.out

insert8.test : $(OBJDIR)insert8$(SUF) insert8.tested insert8.failed $(SIMPLEFOO10)
	$(PIN) -probe -t $< -- ./$(SIMPLEFOO10) >  $<.out 2>&1
	grep -q -e "one = 1" $<.out
	grep -q -e "Before: original arguments = ( 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 )" $<.out
	grep -q -e "After: return value = 45" $<.out
	grep -q -e "main: total = 45" $<.out
	rm -f insert8.failed  $<.out

insert9.test : $(OBJDIR)insert9$(SUF) insert9.tested insert9.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "passed" $<.out
	grep -q -e "After_Malloc" $<.out
	rm -f insert9.failed  $<.out

insert10.test : $(OBJDIR)insert10$(SUF) insert10.tested insert10.failed $(SIMPLEFOO10)
	$(PIN) -probe -t $< -- ./$(SIMPLEFOO10) >  $<.out 2>&1
	grep -q -e "After: return value = 45" $<.out
	grep -q -e "main: total = 45" $<.out
	rm -f insert10.failed  $<.out

insert11.test : $(OBJDIR)insert11$(SUF) insert11.tested insert11.failed $(SIMPLEFOO9)
	$(PIN) -probe -t $< -- ./$(SIMPLEFOO9) >  $<.out 2>&1
	grep -q -e "one = 1" $<.out
	grep -q -e "Before: original arguments = ( 1, 2, 3, 4, 5, 6, 7, 8, 9 )" $<.out
	grep -q -e "After: return value = 45" $<.out
	grep -q -e "main: total = 45" $<.out
	rm -f insert11.failed  $<.out

insertfast.test : $(OBJDIR)insertfast$(SUF) insertfast.tested insertfast.failed $(OBJDIR)winfast.exe
	$(PIN) -probe -t $< -- ./$(OBJDIR)winfast.exe >  $<.out 2>&1
	grep -q -e "one = 1" $<.out
	grep -q -e "Before: arguments = ( 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 )" $<.out
	grep -q -e "After: return value = 45" $<.out
	grep -q -e "three = 3" $<.out
	rm -f insertfast.failed  $<.out

insertstd.test : $(OBJDIR)insertstd$(SUF) insertstd.tested insertstd.failed $(OBJDIR)winstd.exe
	$(PIN) -probe -t $< -- ./$(OBJDIR)winstd.exe >  $<.out 2>&1
	grep -q -e "one = 1" $<.out
	grep -q -e "Before: arguments = ( 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 )" $<.out
	grep -q -e "After: return value = 45" $<.out
	rm -f insertstd.failed  $<.out

insert1win.test : $(OBJDIR)insert1win$(SUF) insert1win.tested insert1win.failed
	$(PIN) -t $< -- $(CP) makefile $<.makefile.copy >  $<.out
	diff makefile $<.makefile.copy
	grep "Before" $<.out
	rm -f insert1win.failed  $<.out $<.makefile.copy

insert2win.test : $(OBJDIR)insert2win$(SUF) insert2win.tested insert2win.failed
	$(PIN) -t $< -- $(CP) makefile $<.makefile.copy >  $<.out
	diff makefile $<.makefile.copy
	grep -q -e "Before 3" $<.out
	rm -f insert2win.failed  $<.out $<.makefile.copy

insert3win.test : $(OBJDIR)insert3win$(SUF) insert3win.tested insert3win.failed
	$(PIN) -t $< -- $(CP) makefile $<.makefile.copy >  $<.out
	diff makefile $<.makefile.copy
	grep -q -e "Before_RtlAllocateHeap" $<.out
	rm -f insert3win.failed  $<.out $<.makefile.copy

debugtest.test : $(OBJDIR)debugtest$(SUF) debugtest.tested debugtest.failed $(LITTLE_MALLOC)
	$(PIN) -probe -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "passed" $<.out
	rm -f debugtest.failed  $<.out


iprtool.test : $(OBJDIR)iprtool$(SUF) iprtool.tested iprtool.failed $(IPR)
	$(PIN) -t $< -- ./$(IPR) >  $<.out 2>&1
	grep -e "a = 1" $<.out
	rm -f iprtool.failed  $<.out

probesleep.test : $(OBJDIR)probesleep$(SUF) probesleep.tested probesleep.failed $(SLEEP)
	$(PIN) -t $< -- ./$(SLEEP) >  $<.out 2>&1
	grep -e "SleepProbe" $<.out
	rm -f probesleep.failed  $<.out

atraceprobe.test : $(OBJDIR)atraceprobe$(SUF) atraceprobe.tested atraceprobe.failed $(OBJDIR)traceapp
	$(PIN) -t $< -- ./$(OBJDIR)traceapp
	rm -f  atraceprobe.failed

ssewin.test : %.test : $(OBJDIR)ssewin.dll %.tested %.failed $(OBJDIR)heavy_stack_win.exe
	$(PIN) -probe -t $< -- ./$(OBJDIR)heavy_stack_win.exe > $<1.out 2>&1
	rm -f $<1.out
	$(PIN) -t $< -- ./$(OBJDIR)heavy_stack_win.exe > $<2.out 2>&1
	rm -f ssewin.failed $<2.out

malloctrace2win.test : %.test : $(OBJDIR)malloctrace2win.dll $(MALLOCWRAPPERS) %.tested %.failed $(DLTEST)
	$(PIN) -probe -t $< -- ./$(DLTEST) > $<1.out 2>&1
	grep -q free $<1.out
	rm -f $<1.out
	$(PIN) -t $< -- ./$(DLTEST) > $<2.out 2>&1
	grep -q malloc $<2.out
	rm -f malloctrace2win.failed $<2.out

# segment registers are causing failures in the jit version, so disable it for now.
# it does pass on older systems that do not use segment registers.
malloctrace2.test : $(OBJDIR)malloctrace2$(SUF) malloctrace2.tested malloctrace2.failed $(DLTEST) $(MALLOCWRAPPERS)
	$(PIN) -probe -t $< -- ./$(DLTEST) > $<1.out 2>&1
	grep Allocated $<1.out
	rm -f $<1.out
	rm -f malloctrace2.failed

# Only on the IA-64 architecture
malloctrace4.test : $(OBJDIR)malloctrace4$(SUF) malloctrace4.tested malloctrace4.failed $(DLTEST_TP) $(MALLOCWRAPPERS)
	$(PIN) -probe -t $< -- ./$(DLTEST_TP) > $<1.out 2>&1
	grep malloc $<1.out
	rm -f $<1.out
	$(PIN) -t $< -- ./$(DLTEST_TP) > $<2.out 2>&1
	grep malloc $<2.out
	rm -f malloctrace4.failed $<2.out

symbolnames.test : $(OBJDIR)symbolnames$(SUF) symbolnames.tested symbolnames.failed $(DLTEST) $(DLTEST_TP)
	$(PIN) -t $< -- ./$(DLTEST) > $<1.out
	grep one $<1.out
	grep two $<1.out
	$(PIN) -t $< -- ./$(DLTEST_TP) > $<2.out
	grep one $<2.out
	grep two $<2.out
	rm -f symbolnames.failed $<1.out $<2.out

replaceebx.test : $(OBJDIR)replaceebx$(SUF) replaceebx.tested replaceebx.failed $(OBJDIR)ebxhandle
	$(PIN) -t $< -- ./$(OBJDIR)ebxhandle > $<.out 2>&1
	cmp $<.out $<.reference
	rm -f replaceebx.failed

context_probe.test : $(OBJDIR)context_probe$(SUF) $(OBJDIR)simplefoo context_probe.tested context_probe.failed
	$(PIN) -t $< -- ./$(OBJDIR)simplefoo >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	rm context_probe.failed $<.out

malloctrace_locktest.test: %.test: $(OBJDIR)malloctrace_locktest$(SUF) $(OBJDIR)thd_malloc %.tested %.failed
	$(PIN) -t $< -- ./$(OBJDIR)thd_malloc
	rm malloctrace_locktest.failed malloctrace_locktest.out

malloctrace_tpbug.test: %.test :$(OBJDIR)malloctrace_tpbug$(SUF) $(OBJDIR)thd_malloc %.tested %.failed
	$(PIN) -t $< -- ./$(OBJDIR)thd_malloc > $<.out 2>&1
	grep -q "test complete" $<.out
	rm malloctrace_tpbug.failed $<.out

init.test: $(OBJDIR)init$(SUF) init.tested init.failed $(OBJDIR)thd_malloc
	$(PIN) -t $< -- ./$(OBJDIR)thd_malloc >  $<.outfile 2>&1
	@grep "libc " $<.outfile
	@grep "libpthread" $<.outfile
	rm -f init.failed  $<.outfile

replace_exit.test : $(OBJDIR)replace_exit$(SUF) replace_exit.tested replace_exit.failed $(LITTLE_MALLOC)
	$(PIN) -t $< -- ./$(LITTLE_MALLOC) >  $<.out 2>&1
	grep "my_exit" $<.out
	rm -f replace_exit.failed $<.out

shortBB.test : $(OBJDIR)shortBB$(SUF) shortBB.tested shortBB.failed $(BBTEST)
	$(PIN) -t $< -- $(BBTEST) >  $<.out 2>&1
	grep  "7" $<.out
	rm -f shortBB.failed  $<.out

shortBB64.test : $(OBJDIR)shortBB64$(SUF) shortBB64.tested shortBB64.failed $(BBTEST)
	$(PIN) -t $< -- $(BBTEST) >  $<.out 2>&1
	grep  "7" $<.out
	rm -f shortBB64.failed  $<.out

forktool.test: $(OBJDIR)forktool$(SUF) $(OBJDIR)forkapp forktool.failed
	$(PIN) -t $< -- ./$(OBJDIR)forkapp >& $<.out
	@grep Before $<.out
	@grep child $<.out
	@grep parent $<.out
	rm -f forktool.failed $<.out 

remove_probe.test : $(OBJDIR)remove_probe$(SUF) remove_probe.tested remove_probe.failed $(UNLOADTEST)
	$(PIN)  -xyzzy -mesgon log_probe -t $< -- ./$(UNLOADTEST) > $<.out 2>&1
	grep Removing ./pin.log
	rm -f $<.out remove_probe.failed pin.log

tpss.test : $(OBJDIR)tpss$(SUF) tpss.tested tpss.failed $(OBJDIR)dltestwin.exe
	$(PIN) -t $< -- ./$(OBJDIR)dltestwin.exe >  $<.out 2>&1
	rm -f tpss.failed  $<.out

## build rules

$(OBJDIR)%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
$(TOOLS): $(PIN_LIBNAMES)
$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINKER} $(PIN_LDFLAGS)  $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(EXTRA_LIBS) $(DBG)

## cleaning
clean:
	-rm -rf $(OBJDIR) *.out *.tested *.failed pin.log  *.outfile

-include *.d

