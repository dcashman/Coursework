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
    LPTHREAD=-lpthread
    STATIC=-static
endif

ifeq ($(TARGET_COMPILER),ms)
    include ../makefile.ms.config
    DBG?=
endif

ifneq ($(ENABLE_VS), 1)
    VS_FLAG = -xyzzy -virtual_segments 1
else
    VS_FLAG =
endif

SUF = $(PINTOOL_SUFFIX)

###============================================================
##
## Set up the test tools
##

TEST_TOOLS = 
## linux only
ifeq ($(TARGET_OS),l)
	TEST_TOOLS += forkcallback follow_child unix_parent_tool 
endif

# mac only
ifeq ($(TARGET_OS),m)
	TEST_TOOLS += forkcallback 
endif

#windows only
ifeq ($(TARGET_OS),w)
	ifeq ($(TARGET_COMPILER),ms)
		TEST_TOOLS += follow_child_3gen_tool follow_child_3gen_tool_initsym grand_parent_tool parent_tool
	endif
endif


###===========================================================

TOOLS = $(TEST_TOOLS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))

## TESTS_TO_RUN includes all tools + some tests without tools

# linux only
ifeq ($(TARGET_OS),l)
	TESTS_TO_RUN += badexec.test forkcallback.test
	TESTS_TO_RUN += follow_execv_with_config1.test \
                        follow_execv_with_config2.test \
                        follow_execv_with_config3.test
    ifeq ($(PROBE),1)
        TESTS_TO_RUN += follow_execv_with_config1_probe.test \
                        follow_execv_with_config2_probe.test \
                        follow_execv_with_config3_probe.test
    endif

    TESTS_TO_RUN += unique_logfile.test
    TESTS_TO_RUN += injectchild.test injectparent.test

    ifeq ($(TARGET),ia32) ## linux ia32

        ifneq ($(HOST_ARCH),ia32e) ## IA32 on IA32 (not IA32 on Intel64)
            x = `touch g1`
            #can't run script cross-platform
            TESTS_TO_RUN += unix_launcher.test
        endif
    else
        TESTS_TO_RUN += unix_launcher.test
    endif

    APPLICATIONS = fork parent_process child_process injection_test_app
endif

# mac
ifeq ($(TARGET_OS),m)
    TESTS_TO_RUN += forkcallback.test
    APPLICATIONS = fork
endif

# windows only
ifeq ($(TARGET_OS),w)
    TESTS_TO_RUN += win_parent_process1.test  win_parent_process1_probed.test
    TESTS_TO_RUN += win_parent_process1_unicode_probed.test
    TESTS_TO_RUN += win_parent_process2_jjj.test win_parent_process2_jjp.test
    TESTS_TO_RUN += win_parent_process2_jpj.test win_parent_process2_jpp.test
    TESTS_TO_RUN += win_parent_process2_pjj.test win_parent_process2_pjp.test
    TESTS_TO_RUN += win_parent_process2_ppj.test win_parent_process2_ppp.test
    TESTS_TO_RUN += win_parent_process2_unicode_ppp.test
    TESTS_TO_RUN += win_parent_process2_mt_ppp.test win_parent_process1_unicode_probed_withsym.test

    APPLICATIONS = win_parent_process win_child_process
endif

TEST_APPLICATIONS = $(APPLICATIONS:%=$(OBJDIR)%)


all: $(OBJDIR) $(TOOLS) $(TEST_APPLICATIONS)

## sanity

SANITY_TESTS = $(TESTS_TO_RUN)


tests-sanity: $(OBJDIR) $(SANITY_TESTS)


test: $(OBJDIR) $(TESTS_TO_RUN)

$(OBJDIR):
	mkdir -p $(OBJDIR)


injectchild.test : $(OBJDIR)injection_test_app injectchild.tested injectchild.failed
	$(PIN) -injection child -- $(OBJDIR)injection_test_app ch_file
	rm injectchild.failed injectchild.tested 

injectparent.test : %.test: $(OBJDIR)injection_test_app %.tested %.failed
	$(PIN) -injection parent -- $(OBJDIR)injection_test_app pa_file
	rm $*.failed $*.tested

# This test is not run by default because it is expected to fail sometimes.  The "self"
# injection mode will fail if the kerel happens to load the Pin image in a range that
# conflicts with the application.  Since there is some randomness in the address ranges
# the kernel selects for Pin and it's shared libraries, this test will sometimes fail
# and sometimes succeed.  It appears to me (gmlueck) that it fails about 1 in 10 tries.
#
injectself.test : injectself.tested injectself.failed
	touch injectself.makefile.copy; rm injectself.makefile.copy
	$(PIN) -injection self -- $(TESTAPP) makefile injectself.makefile.copy
	$(PIN_DIFF) makefile injectself.makefile.copy
	rm injectself.failed injectself.tested injectself.makefile.copy

$(OBJDIR)injection_test_app: injection_test_app.c
	$(CC) $(APP_CXXFLAGS) $< -o $@

##======================================================
## "fork()" callbacks in JIT mode (Linux and Mac)
##======================================================
forkcallback.test : $(OBJDIR)forkcallback$(SUF) $(OBJDIR)fork forkcallback.tested forkcallback.failed
	$(PIN) -t $< -- ./$(OBJDIR)fork
	cat forkcallback.child.out >> forkcallback.out
	$(PIN_DIFF) forkcallback.out forkcallback.reference
	rm forkcallback.failed forkcallback.out forkcallback.child.out forkcallback.tested



##======================================================
## -follow_execv one level tests: (Linux and mac)
##======================================================
##
## (1) do not run child under the Pin
##
follow_execv_with_config1.test: %.test : $(OBJDIR)unix_parent_tool$(SUF) $(OBJDIR)parent_process $(OBJDIR)child_process %.failed %.tested
	$(PIN) -follow_execv 1 -t $< -xyzzy -app ./child_process_xxx -- ./$(OBJDIR)parent_process ./$(OBJDIR)child_process "param1 param2" param3 >$*.out
	## child_process_xxx is not equal to child_process, do not run child under Pin at all
	grep "Do not run Pin under the child process" $*.out
	## unix_parent_tool should appear 1 time in the output since it was used in parent only
	test `grep "In unix_parent_tool PinTool" $*.out | wc -l` -eq "1"
	rm $*.failed $*.tested $*.out

##
## (1-probe) do not run child under the Pin
##
follow_execv_with_config1_probe.test:  %.test :$(OBJDIR)unix_parent_tool$(SUF) $(OBJDIR)parent_process $(OBJDIR)child_process %.failed %.tested
	$(PIN) -follow_execv 1 -t $< -xyzzy -probe -app ./child_process_xxx -- ./$(OBJDIR)parent_process $(OBJDIR)child_process "param1 param2" param3 >$*.out
	## child_process_xxx is not equal to child_process, do not run child under Pin at all
	grep "Do not run Pin under the child process" $*.out
	## unix_parent_tool should appear 1 time in the output since it was used in parent only
	test `grep "In unix_parent_tool PinTool" $*.out | wc -l` -eq "1"
	rm $*.failed $*.tested $*.out

##
## (2) run child under the Pin, but don't change Pin command line
##
follow_execv_with_config2.test:  %.test : $(OBJDIR)unix_parent_tool$(SUF) $(OBJDIR)parent_process $(OBJDIR)child_process %.failed %.tested
	$(PIN) -follow_execv 1 -t $< -xyzzy -app ./$(OBJDIR)child_process -- ./$(OBJDIR)parent_process ./$(OBJDIR)child_process "param1 param2" param3 >$*.out
	## -app argument is equal to application name, run the child process under the Pin
	grep "Pin command line remains unchanged" $*.out
	## unix_parent_tool should appear 2 times in the output since it was used in the parent and in the child
	test `grep "In unix_parent_tool PinTool" $*.out | wc -l` -eq "2"
	rm $*.failed $*.tested $*.out

##
## (2-probe) run child under the Pin, but don't change Pin command line
##
follow_execv_with_config2_probe.test: %.test :$(OBJDIR)unix_parent_tool$(SUF) $(OBJDIR)parent_process $(OBJDIR)child_process follow_execv_with_config2_probe.failed follow_execv_with_config2_probe.tested
	$(PIN) -follow_execv 1 -t $< -xyzzy -probe -app ./$(OBJDIR)child_process -- ./$(OBJDIR)parent_process ./$(OBJDIR)child_process "param1 param2" param3 >$*.out
	## -app argument is equal to application name, run the child process under the Pin
	grep "Pin command line remains unchanged" $*.out
	## unix_parent_tool should appear 2 times in the output since it was used in the parent and in the child
	test `grep "In unix_parent_tool PinTool" $*.out | wc -l` -eq "2"
	rm $*.failed $*.tested $*.out

##
## (3) run child under the Pin, and change Pin command line
##
follow_execv_with_config3.test: %.test : $(OBJDIR)unix_parent_tool$(SUF) $(OBJDIR)parent_process $(OBJDIR)child_process follow_execv_with_config3.failed follow_execv_with_config3.tested $(OBJDIR)follow_child$(SUF)
	$(PIN) -follow_execv 1 -t $< -xyzzy -app ./$(OBJDIR)child_process -pin "$(PIN) -t ./$(OBJDIR)follow_child$(SUF)" -- ./$(OBJDIR)parent_process ./$(OBJDIR)child_process "param1 param2" param3 >$*.out
	## change Pin command line
	grep "Process to execute" $*.out
	## unix_parent_tool should appear 1 time in the output since it was used in the parent only
	test `grep "In unix_parent_tool PinTool" $*.out | wc -l` -eq "1"
	## follow_child tool should appear 1 time in the output since it was used in the child only
	test `grep "In follow_child PinTool" $*.out | wc -l` -eq "1"
	rm $*.failed $*.tested $*.out

##
## (3-probe) run child under the Pin, and change Pin command line
##
follow_execv_with_config3_probe.test: %.test : $(OBJDIR)unix_parent_tool$(SUF) $(OBJDIR)parent_process $(OBJDIR)child_process %.failed %.tested $(OBJDIR)follow_child$(SUF)
	$(PIN) -follow_execv 1 -t $< -xyzzy -probe -app ./$(OBJDIR)child_process -pin "$(PIN) -t ./$(OBJDIR)follow_child$(SUF)" -- ./$(OBJDIR)parent_process ./$(OBJDIR)child_process "param1 param2" param3 >$*.out
	## change Pin command line
	grep "Process to execute" $*.out
	## unix_parent_tool should appear 1 time in the output since it was used in the parent only
	test `grep "In unix_parent_tool PinTool" $*.out | wc -l` -eq "1"
	## follow_child tool should appear 1 time in the output since it was used in the child only
	test `grep "In follow_child PinTool" $*.out | wc -l` -eq "1"
	rm $*.failed $*.tested $*.out

##======================================================
## -follow_execv two level tests: (Linux and mac)
##======================================================

##
## launcher runs parent_process twice; parent_process runs child_process
##
unix_launcher.test: %.test : $(OBJDIR)follow_child$(SUF) $(OBJDIR)parent_process $(OBJDIR)child_process parent_process_launcher.sh %.failed %.tested
	./parent_process_launcher.sh $(OBJDIR)parent_process $(OBJDIR)child_process> unix_launcher.reference
	$(PIN) -follow_execv 1 -t $< -- /bin/sh ./parent_process_launcher.sh $(OBJDIR)parent_process $(OBJDIR)child_process > $*.out 
	grep -v PinTool $*.out > $*.out1
	grep PinTool $*.out > $*.out2
	$(PIN_DIFF) $*.out1 unix_launcher.reference
	## PinTool should appear 5 times: 
	## parent_process_launcher.sh - 1, parent_process - 2 child_process - 2
	test `grep PinTool $*.out|wc -l` -eq "5"
	rm unix_launcher.out* unix_launcher.reference $*.failed $*.tested

##======================================================
## Execve fails and returns with error code
##======================================================

badexec.test : badexec.tested badexec.failed $(OBJDIR)badexec
	./$(OBJDIR)badexec > badexec.reference
	$(PIN) -xyzzy -follow_execv 1 -- ./$(OBJDIR)badexec > badexec.out 2>&1
	$(PIN_DIFF) badexec.out badexec.reference
	rm badexec.failed badexec.out badexec.reference badexec.tested


##======================================================
## Test for getpid() inside Pin; -unique_logfile calls to getpid()
##======================================================

unique_logfile.test: %.test : $(OBJDIR)parent_process $(OBJDIR)child_process %.failed %.tested
	rm -f pin.log.*
	$(PIN) -follow_execv 1 -xyzzy -mesgon log_syscall -unique_logfile -injection child -- $(OBJDIR)parent_process $(OBJDIR)child_process "param1 param2" param3 >$*.out
	test `ls pin.log.*|wc -l` -eq 2
	rm -f pin.log.*
	$(PIN) -follow_execv 1 -xyzzy -mesgon log_syscall -unique_logfile -injection parent -- $(OBJDIR)parent_process $(OBJDIR)child_process "param1 param2" param3 >$*.out
	test `ls pin.log.*|wc -l` -eq 2
	rm pin.log.* $*.out $*.failed $*.tested

##======================================================
## -follow_execv Windows tests
##======================================================
##
## (1) run child under the Pin, but don't change Pin command line
##
win_parent_process1.test : $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process1.tested win_parent_process1.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process1.out; rm win_parent_process1.out
	$(PIN_EXE) $(PIN_TEST_FLAGS) -p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -t $< -- cmd /c win_parent_process_launcher.bat $(OBJDIR)>  win_parent_process1.out 2>&1
	$(PIN_DIFF) win_parent_process1.out win_parent_process1.reference
	rm win_parent_process1.failed win_parent_process1.out

win_parent_process1_probed.test : $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process1_probed.tested win_parent_process1_probed.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process1_probed.out; rm win_parent_process1_probed.out
	$(PIN_EXE) $(PIN_TEST_FLAGS) -p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t $< -- cmd /c win_parent_process_launcher.bat $(OBJDIR)>  win_parent_process1_probed.out 2>&1
	$(PIN_DIFF) win_parent_process1_probed.out win_parent_process1_probed.reference
	rm win_parent_process1_probed.failed win_parent_process1_probed.out	

win_parent_process1_unicode_probed.test : $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process1_unicode_probed.tested win_parent_process1_unicode_probed.failed $(OBJDIR)win_parent_process_unicode $(OBJDIR)win_child_process
	touch win_parent_process1_unicode_probed.out; rm win_parent_process1_unicode_probed.out
	$(PIN_EXE) $(PIN_TEST_FLAGS) -p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t $< -- cmd /c win_parent_process_unicode_launcher.bat $(OBJDIR)>  win_parent_process1_unicode_probed.out 2>&1
	$(PIN_DIFF) win_parent_process1_unicode_probed.out win_parent_process1_unicode_probed.reference
	rm win_parent_process1_unicode_probed.failed win_parent_process1_unicode_probed.out	

win_parent_process1_unicode_probed_withsym.test : $(OBJDIR)follow_child_3gen_tool_initsym$(SUF) win_parent_process1_unicode_probed_withsym.tested win_parent_process1_unicode_probed_withsym.failed $(OBJDIR)win_parent_process_unicode $(OBJDIR)win_child_process
	touch win_parent_process1_unicode_probed_withsym.out; rm win_parent_process1_unicode_probed_withsym.out
	$(PIN_EXE) $(PIN_TEST_FLAGS) -p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t $< -- cmd /c win_parent_process_unicode_launcher.bat $(OBJDIR)>  win_parent_process1_unicode_probed_withsym.out 2>&1
	$(PIN_DIFF) win_parent_process1_unicode_probed_withsym.out win_parent_process1_unicode_probed_withsym.reference
	rm win_parent_process1_unicode_probed_withsym.failed win_parent_process1_unicode_probed_withsym.out	 

##
## (2) run child processes under Pin for some processes (filter by name), and change Pin command line for them.
##
# j means jitted, p means probed - so win_parent_process2_jjp.test means:
# grandparent jitted, parent jitted, child probed
#jjj
win_parent_process2_jjj.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_jjj.tested win_parent_process2_jjj.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_jjj.out; rm win_parent_process2_jjj.out
	cmd /c win_parent_process_test_launcher.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -t " "$< " " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_jjj.out 2>&1
	$(PIN_DIFF) win_parent_process2_jjj.out win_parent_process2_jjj.reference
	rm win_parent_process2_jjj.failed win_parent_process2_jjj.out

#jjp	
win_parent_process2_jjp.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_jjp.tested win_parent_process2_jjp.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_jjp.out; rm win_parent_process2_jjp.out
	cmd /c win_parent_process_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -t " "$<" "-probe_grand_child 1" " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_jjp.out 2>&1
	$(PIN_DIFF) win_parent_process2_jjp.out win_parent_process2_jjp.reference
	rm win_parent_process2_jjp.failed win_parent_process2_jjp.out	

#jpj
win_parent_process2_jpj.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_jpj.tested win_parent_process2_jpj.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_jpj.out; rm win_parent_process2_jpj.out
	cmd /c win_parent_process_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -t " "$<" "-probe_child 1" " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_jpj.out 2>&1
	$(PIN_DIFF) win_parent_process2_jpj.out win_parent_process2_jpj.reference
	rm win_parent_process2_jpj.failed win_parent_process2_jpj.out

#jpp
win_parent_process2_jpp.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_jpp.tested win_parent_process2_jpp.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_jpp.out; rm win_parent_process2_jpp.out
	cmd /c win_parent_process_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -t " "$<"  "-probe_child 1 -probe_grand_child 1" " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_jpp.out 2>&1
	$(PIN_DIFF) win_parent_process2_jpp.out win_parent_process2_jpp.reference
	rm win_parent_process2_jpp.failed win_parent_process2_jpp.out	

#pjj
win_parent_process2_pjj.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_pjj.tested win_parent_process2_pjj.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_pjj.out; rm win_parent_process2_pjj.out
	cmd /c win_parent_process_test_launcher.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t " "$< " " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_pjj.out 2>&1
	$(PIN_DIFF) win_parent_process2_pjj.out win_parent_process2_pjj.reference
	rm win_parent_process2_pjj.failed win_parent_process2_pjj.out	

#pjp
win_parent_process2_pjp.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_pjp.tested win_parent_process2_pjp.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_pjp.out; rm win_parent_process2_pjp.out
	cmd /c win_parent_process_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t " "$<" "-probe_grand_child 1" " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_pjp.out 2>&1
	$(PIN_DIFF) win_parent_process2_pjp.out win_parent_process2_pjp.reference
	rm win_parent_process2_pjp.failed win_parent_process2_pjp.out	

#ppj
win_parent_process2_ppj.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_ppj.tested win_parent_process2_ppj.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_ppj.out; rm win_parent_process2_ppj.out
	cmd /c win_parent_process_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t " "$<" "-probe_child 1" " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_ppj.out 2>&1
	$(PIN_DIFF) win_parent_process2_ppj.out win_parent_process2_ppj.reference
	rm win_parent_process2_ppj.failed win_parent_process2_ppj.out	

#ppp
win_parent_process2_ppp.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_ppp.tested win_parent_process2_ppp.failed $(OBJDIR)win_parent_process $(OBJDIR)win_child_process
	touch win_parent_process2_ppp.out; rm win_parent_process2_ppp.out
	cmd /c win_parent_process_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t " "$<" "-probe_child 1 -probe_grand_child 1" " -- cmd /c win_parent_process_launcher.bat $(OBJDIR)" >  win_parent_process2_ppp.out 2>&1
	$(PIN_DIFF) win_parent_process2_ppp.out win_parent_process2_ppp.reference
	rm win_parent_process2_ppp.failed win_parent_process2_ppp.out

win_parent_process2_unicode_ppp.test : $(OBJDIR)grand_parent_tool$(SUF) $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_unicode_ppp.tested win_parent_process2_unicode_ppp.failed $(OBJDIR)win_parent_process_unicode $(OBJDIR)win_child_process
	touch win_parent_process2_unicode_ppp.out; rm win_parent_process2_unicode_ppp.out
	cmd /c win_parent_process_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t " "$<" "-probe_child 1 -probe_grand_child 1" " -- cmd /c win_parent_process_unicode_launcher.bat $(OBJDIR)" >  win_parent_process2_unicode_ppp.out 2>&1
	$(PIN_DIFF) win_parent_process2_unicode_ppp.out win_parent_process2_unicode_ppp.reference
	rm win_parent_process2_unicode_ppp.failed win_parent_process2_unicode_ppp.out

#multi-threaded processes	

#ppp
win_parent_process2_mt_ppp.test :  $(OBJDIR)grand_parent_tool$(SUF)  $(OBJDIR)parent_tool$(SUF) $(OBJDIR)follow_child_3gen_tool$(SUF) win_parent_process2_mt_ppp.tested win_parent_process2_mt_ppp.failed $(OBJDIR)win_parent_process_mt $(OBJDIR)win_child_process
	touch win_parent_process2_mt_ppp.out; rm win_parent_process2_mt_ppp.out
	cmd /c win_parent_process_mt_test_launcher_w_pintool_args.bat $(PIN_EXE) "$(PIN_TEST_FLAGS)" "-p32 $(PIN_EXE_32) -p64 $(PIN_EXE_64) -follow_execv -probe -t " "$<"  "-probe_child 1 -probe_grand_child 1" " -- cmd /c win_parent_process_mt_launcher.bat $(OBJDIR)" >  win_parent_process2_mt_ppp.out 2>&1
	$(PIN_DIFF) win_parent_process2_mt_ppp.out win_parent_process2_mt_ppp.reference
	rm win_parent_process2_mt_ppp.failed win_parent_process2_mt_ppp.out


##===================================================================================
## common and private build rules for tools and applications
##==================================================================================
$(OBJDIR)badexec: badexec.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<
	
$(OBJDIR)fork: fork.c
	$(CC) $(APP_CXXFLAGS) -o $@ $<

$(OBJDIR)libarglist.a: arglist.cpp arglist.h
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT} $(OBJDIR)arglist.o $<
	$(AR) cr $@ $(OBJDIR)arglist.o


$(OBJDIR)parent_process: parent_process.cpp
	$(CXX) $(APP_CXXFLAGS) $(DBG) ${NO_COMDAT_FLAG} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) 

$(OBJDIR)child_process: child_process.cpp
	$(CXX)  ${APP_CXXFLAGS} $(DBG) ${NO_COMDAT_FLAG} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)win_parent_process: win_parent_process.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${NO_COMDAT_FLAG} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) Advapi32.lib

$(OBJDIR)win_parent_process_mt: win_parent_process_mt.cpp 
	$(CXX)  ${APP_CXXFLAGS2} ${NO_COMDAT_FLAG} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)  Advapi32.lib

$(OBJDIR)win_parent_process_unicode: win_parent_process_unicode.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${NO_COMDAT_FLAG} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) Advapi32.lib

$(OBJDIR)win_child_process: win_child_process.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${NO_COMDAT_FLAG} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)



$(OBJDIR)%.o : %.cpp
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
	
$(OBJDIR)%.o : %.cc
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<
	
$(OBJDIR)%.o : %.cc
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): $(PIN_LIBNAMES)

$(STATIC_TOOLS): $(PIN_LIBNAMES)

$(OBJDIR)%.o : %.S
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): $(OBJDIR)%$(PINTOOL_SUFFIX) : $(OBJDIR)%.o
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

## overriding some build rules
$(OBJDIR)unix_parent_tool$(PINTOOL_SUFFIX): $(OBJDIR)unix_parent_tool.o $(OBJDIR)libarglist.a
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG) $(OBJDIR)libarglist.a

$(OBJDIR)unix_parent_tool.o : unix_parent_tool.cpp arglist.h
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<


## cleaning
clean:
	-rm -rf $(OBJDIR) *.tested *.failed


