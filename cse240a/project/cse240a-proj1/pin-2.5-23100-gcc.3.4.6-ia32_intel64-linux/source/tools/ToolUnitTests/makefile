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

SUF = $(PINTOOLS_SUFFIX)

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

ifeq ($(TARGET_OS),w)
    OS_API=windows
else
    OS_API=unix
endif

ifneq ($(ENABLE_VS), 1)
    VS_FLAG = -xyzzy -virtual_segments 1
else
    VS_FLAG =
endif
## Set up tool roots

TOOL_ROOTS = returnreg args syscall partialinline memalign fini nops control_init line appcall realloc inlined-stack-arg earlymalloc inscount2_fornoinline
STATIC_TOOL_ROOTS =

TOOL_ROOTS += incebx args_err

# don't test routine replacement when virtual segments flag is enabled
#ifneq ($(ENABLE_VS),1)
#    TOOL_ROOTS += jitmalloctrace replace_free
#endif

# don't test these on windows.
ifneq ($(TARGET_OS),w)
    TOOL_ROOTS += soload mmap mt malmalloc change_syscall
    STATIC_TOOL_ROOTS += statica_locktest
endif

ifeq ($(TARGET_OS),w)
    TOOL_ROOTS += thread_count thread_count2 stracewin_ia32 exception_monitor apc_monitor debugservice_monitor segmented_ea_verifier image_entry mtflush
    TOOL_ROOTS += exception_context_monitor
    ifeq ($(TARGET),ia32)
        TOOL_ROOTS += ea_verifier_addr16
    endif
endif

ifeq ($(TARGET_OS),l)
	ifneq ($(TARGET),ipf)
		TOOL_ROOTS += segmented_ea_verifier
    
		# Definition for testing pause_tool.
		# This test runs only on Linux and has no support to test
		# ia32 on Intel64 systems (requires the cross version of GDB)
		GDB = /usr/bin/gdb
		ifeq (${KIT},0)
			PINBIN=$(PIN_BIN)
		else
			PINBIN=$(PIN_KIT)/$(TARGET_LONG)/bin/pinbin
		endif
		#ifeq ($(TARGET),$(HOST_ARCH))
		#	TOOL_ROOTS += pause_tool
		#endif
	endif
    TOOL_ROOTS += soloadrange mtflush
endif

# gcc asm in test
ifneq ($(TARGET),ipf)
   ifeq ($(TARGET_OS),l)
       TOOL_ROOTS += int3del
   endif
endif

# safecopy won't work until signals are emulated on Mac
ifneq ($(TARGET_OS),m)
    TOOL_ROOTS += safecopy
endif

ifneq ($(TARGET),ipf)
  TOOL_ROOTS += returnflags rollback leave emu_stack emu_jumps
endif

ifeq ($(TARGET_OS),w)
    ifeq ($(TARGET),ia32)
       TOOL_ROOTS += swizzle2 swizzle_seg
    else
       TOOL_ROOTS += test_ip_access 
    endif
endif

ifeq ($(TARGET_OS),m)
    TOOL_ROOTS += swizzle2
endif

ifeq ($(TARGET_OS),l)
	ifneq ($(TARGET),ipf)
		TOOL_ROOTS += swizzle2 swizzle_seg
    endif
endif


ifeq ($(TARGET),ia32)
    TOOL_ROOTS +=  rewritememop inlinecall xmmtest #swizzle3 - disabled, buggy 2007-09-12
    TOOL_ROOTS += jitmalloctrace replace_free  executeat_callback
    ifneq ($(TARGET_OS),w)
        TOOL_ROOTS += branch_target_addr
    endif
    ifeq ($(TARGET_OS),l)
		TOOL_ROOTS += swizzle5 executeat_lock
    endif
endif

ifeq ($(TARGET),ia32e)
    TOOL_ROOTS += jitmalloctrace replace_free  executeat_callback
    ifneq ($(TARGET_COMPILER),ms)
        TOOL_ROOTS += xmmtest
    endif
    ifeq ($(TARGET_OS),l)
		TOOL_ROOTS += special executeat_lock
    endif
endif

ifeq ($(TARGET),ipf)
    TOOL_ROOTS += predinlinetest retcnt
else
    TOOL_ROOTS += sse-unaligned-class sse-unaligned-class2
endif

# every tool should be tested
TEST_TOOLS = $(TOOL_ROOTS)
STATIC_TEST_TOOLS = $(STATIC_TOOL_ROOTS)

#
# tests that have no tools
#

# Generic
TEST_TOOLS += resize memory_reuse 

# ISA dependent
IA32_TEST_TOOLS = mtstatic kthread htab sse-ref smc_sse smc_ia32 smc_mt 
ifeq ($(TARGET),ia32)
   ifeq ($(TARGET_OS),l)
      TEST_TOOLS +=   int3del reservemem callsp_ia32 far_ret
   endif
    TEST_TOOLS += $(IA32_TEST_TOOLS) pusha_popa
endif
ifeq ($(TARGET),ia32e)
   ifeq ($(TARGET_OS),l)
      TEST_TOOLS +=   int3del reservemem64 callsp_intel64
   endif
    TEST_TOOLS += $(IA32_TEST_TOOLS) spalign nosahflahf
endif
ifeq ($(TARGET),ipf)
    # Add 'attachnat' when kernel is fixed
    TEST_TOOLS += kthread htab smc_ipf
endif

CALLAPP_IA32 = callapp1 callapp2 callapp10 callapp1i callapp10i 
CALLAPP_IA32 = callapp5 callapp6 callapp7 callapp8 
CALLAPP_IA32 += callapp12 replace_malloc_inst thread_callback
CALLAPP_IA32E = $(CALLAPP_IA32) callapp3 callapp4 callapp13
CALLAPP_IA32_LINUX = callapp0 callapp0i
CALLAPP_IA32_WINDOWS = callappfast10 callappstd10

# OS dependent
ifeq ($(TARGET_OS),l)
    TEST_TOOLS += pipe waitpidbug cancel earlymallocexe 
    TOOL_ROOTS += earlyout
    ifeq ($(TARGET),ia32)
        TOOL_ROOTS += strace_ia32 reptool short_name repcmpsz_tool context  
        TOOL_ROOTS += $(CALLAPP_IA32) $(CALLAPP_IA32_LINUX)
        TEST_TOOLS += badpath badfile smc_except attach
        APPS += attach
    endif
    ifeq ($(TARGET),ia32e)
        TOOL_ROOTS += strace_ia32 reptool short_name context  
	    TOOL_ROOTS += $(CALLAPP_IA32E)
        TEST_TOOLS += insertand clobber smc_except attach
        APPS += attach
    endif
    ifeq ($(TARGET),ipf)
        TOOL_ROOTS += strace_ipf short_name
    endif
MT = -mt 0
SIMPLEFOO=simplefoo
SIMPLEFOO1=simplefoo1
SIMPLEFOO2=simplefoo2
SIMPLEFOO3=simplefoo3
SIMPLEFOO4=simplefoo4
SIMPLEFOO6=simplefoo6
SIMPLEFOO7=simplefoo7
SIMPLEFOO8=simplefoo8
SIMPLEFOO10=simplefoo10
SIMPLEFOO12=simplefoo12
SIMPLEFOO13=simplefoo13
SIMPLEFOO1i=simplefoo1i
SIMPLEFOO10i=simplefoo10i
LITTLE_MALLOC=little_malloc
THREADTEST=thread_wait
endif

# We want to test using a 32 bit with 64 bit pin
#ifeq ($(TARGET),ia32e)
#    ifeq ($(TARGET_OS),l)
#	TEST_TOOLS += hello32
#    endif
#endif

ifeq ($(TARGET_OS),w)
  TEST_TOOLS += secname_tool
	ifeq ($(TARGET_COMPILER),ms)
		TOOL_ROOTS += mt malmalloc funreplace_alert syscall_std teb context
		ifeq ($(TARGET), ia32e)
			TOOL_ROOTS += $(CALLAPP_IA32E)
		else
			TOOL_ROOTS += $(CALLAPP_IA32)
		endif
		WIN_SIGNAL_TESTS = thread_count_thread_creation win_queue_apc win_divide_by_zero_exception \
		                   win_cpp_exception
		TEST_TOOLS += ${WIN_SIGNAL_TESTS}
		TEST_TOOLS += win_debug_service win_load_library win_code_on_reused_memory win_multi_dll_pintool
		TEST_TOOLS += load_dummy_dll_tool smc_except rebase_dll_tool exports_only_tool win_exception_context
		ifneq ($(TARGET_COMPILER),gnu)
			ifeq ($(TARGET),ia32)
                TEST_TOOLS += df_test_tool1 df_test_tool2 df_test_tool3 df_test_tool4 df_test_tool5 analysis_flag_overwrite_tool1 analysis_flag_overwrite_tool2 analysis_flag_overwrite_tool3
                TEST_TOOLS += analysis_flag_overwrite_tool1 analysis_flag_overwrite_tool2 analysis_flag_overwrite_tool3 reg_operands_test_tool
                ifeq ($(ICC),)
                    # ICC incorrectly compiles inline assembly code in this test 
                    TEST_TOOLS += smc_bbl
                endif
            endif
        endif
	endif
SIMPLEFOO1=winfoo1
SIMPLEFOO2=winfoo2
SIMPLEFOO3=winfoo3
SIMPLEFOO4=winfoo4
SIMPLEFOO5=winfoo5
SIMPLEFOO6=winfoo6
SIMPLEFOO7=winfoo7
SIMPLEFOO8=winfoo8
SIMPLEFOO10=winfoo10
SIMPLEFOO12=winfoo12
SIMPLEFOO13=winfoo13
SIMPLEFOO1i=winfoo1i
SIMPLEFOO10i=winfoo10i
LITTLE_MALLOC=little_malloc.exe
THREADTEST=threadtestwin.exe
endif

TOOL_ROOTS += reg_inst_gx

ifeq ($(TARGET_OS),m)
    #exclude rollback, mtstatic, sigmaskstatic
    TOOL_ROOTS = swizzle2  mt malmalloc args line \
	             returnreg returnflags partialinline syscall branch_target_addr memalign # swizzle3 -- disabled, buggy 2007-09-12
    ifeq ($(TARGET),ia32)
        TOOL_ROOTS += strace_ia32 context
    endif
    TEST_TOOLS = sigmask-mac resize kthread htab sse-ref $(TOOL_ROOTS)
endif

ifeq ($(TARGET_OS),m)
    HELLO=hello-mac
    DLTEST=dltest-mac
    MTFLUSHAPP=mtflushapp_unix
else
    ifeq ($(TARGET_OS),w)
        HELLO=hello-ms-$(TARGET)
        MTFLUSHAPP=mtflushapp_win.exe
    else
        HELLO=hello
        DLTEST=dltest
        DLTEST2=dltest2
        MTFLUSHAPP=mtflushapp_unix
    endif
endif

ONEPROG=oneprog

# if you want to build, but not test a tool, add it to TOOL_ROOTS here

TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))
STATIC_TOOLS = $(STATIC_TOOL_ROOTS:%=$(OBJDIR)%$(SATOOL_SUFFIX))
APPS  += $(DLTEST) $(DLTEST2) $(HELLO) $(MTFLUSHAPP) mmapapp pipeapp
ifeq ($(TARGET),ipf)
    APPS += smcapp_ipf attachnat
endif
ifeq ($(TARGET),ia32)
	ifeq ($(TARGET_OS),w)
		CMPXCHG8_ASM_OBJ = cmpxchg8b_ms.obj
		CMPXCHG8_WITH_EXPLICIT_EBX_ASM_OBJ = cmpxchg8b_with_explicit_ebx_ms.obj
		FLAG_SPILL_FILL_APP_ASM_OBJ = flag_spill_fill_app_ms.obj
		FLAG_SPILL_FILL_TOOL1_ASM_OBJ = flag_spill_fill_tool1_ms.obj
		ANALYSIS_FLAG_OVERWRITE_APP_ASM_OBJ = analysis_flag_overwrite_app_ms.obj
		ANALYSIS_FLAG_OVERWRITE_TOOL1_ASM_OBJ = analysis_flag_overwrite_tool1_ms.obj
		ANALYSIS_FLAG_OVERWRITE_TOOL2_ASM_OBJ = analysis_flag_overwrite_tool2_ms.obj
		ANALYSIS_FLAG_OVERWRITE_TOOL3_ASM_OBJ = analysis_flag_overwrite_tool3_ms.obj
		DF_TEST_APP_ASM_OBJ = df_test_app_ms.obj
		DF_TEST_TOOL1_ASM_OBJ = df_test_tool1_ms.obj
		DF_TEST_TOOL2_ASM_OBJ = df_test_tool2_ms.obj
		DF_TEST_TOOL3_ASM_OBJ = df_test_tool3_ms.obj
		DF_TEST_TOOL4_ASM_OBJ = df_test_tool4_ms.obj
		DF_TEST_TOOL5_ASM_OBJ = df_test_tool5_ms.obj
		REG_OPERANDS_TEST_APP_ASM_OBJ = reg_operands_test_app_ms.obj
		SEG_OVERRIDE_APP1_ASM_OBJ = seg_override_app1_ms.obj
	else
        APPS += doint_ia32
		CMPXCHG8_ASM_OBJ = cmpxchg8b_gnu.o
    endif
endif

APPS_BINARY_FILES = $(APPS:%=$(OBJDIR)%)


ifeq ($(TARGET_OS),w)
all: tools 
else
all: tools $(APPS_BINARY_FILES)
endif
tools: $(OBJDIR) $(TOOLS) $(STATIC_TOOLS)

ifeq ($(TARGET),ia32)
callapps: $(OBJDIR) $(CALLAPP_IA32:%=%.test)
endif
ifeq ($(TARGET),ia32e)
callapps: $(OBJDIR) $(CALLAPP_IA32E:%=%.test)
endif


## sanity

SANITY_TESTS = args.test line.test replace_free.test args_err.test

ifneq ($(TARGET_OS),m)
    SANITY_TESTS += safecopy.test
endif



ifeq ($(TARGET_OS),w)
    ifeq ($(TARGET_COMPILER),ms)
        SANITY_TESTS += funreplace_alert.test syscall_std.test teb.test segmented_ea_verifier.test 
        SANITY_TESTS += smc_except.test win_exception_context.test
        SEGMENTED_EA_VERIFIER_ASM_OBJ = $(OBJDIR)segmented_ea_verifier_win1_ms.obj
        ifeq ($(TARGET),ia32)
            SANITY_TESTS += ea_verifier_addr16.test
            ifeq ($(ICC),)
                # ICC incorrectly compiles inline assembly code in this test 
                SANITY_TESTS += smc_bbl.test
            endif
        endif
    endif
endif

ifeq ($(TARGET_OS),l)
    ifneq ($(TARGET),ipf)
        SANITY_TESTS += smc_except.test segmented_ea_verifier.test
        SEGMENTED_EA_VERIFIER_ASM_OBJ = $(OBJDIR)segmented_ea_verifier_lin_$(TARGET).o
		EA_VERIFIER_ADDR16_ASM_OBJ = $(OBJDIR)ea_verifier_addr16_lin$(TARGET).o
    endif
endif


ifeq ($(TARGET),ia32e)
    SANITY_TESTS += sse-ref.test smc_sse.test smc_ia32.test smc_mt.test
    ifeq ($(TARGET_OS),l)
       SANITY_TESTS += int3.test	
    endif
    ifeq ($(TARGET_COMPILER),ms)
        SSE_ASM_OBJ     = $(OBJDIR)sse_ia32e.obj
        SSE-REF_ASM_OBJ = $(OBJDIR)sse-ref_ia32e.obj
        SP_ALIGN_ASM_OBJ = $(OBJDIR)spalign_asm_ia32e_ms.obj
        TEST_IP_ACCESS_ASM_OBJ = $(OBJDIR)test_ip_access_app_ia32e.obj
        SEGMENTED_EA_VERIFIER_ASM_OBJ = $(OBJDIR)segmented_ea_verifier_win1_ia32e_ms.obj
        EA_VERIFIER_ADDR16_ASM_OBJ = $(OBJDIR)ea_verifier_addr16_ia32e_ms.obj
        LINK_OPTION     = /link
    else
        SP_ALIGN_ASM_OBJ = $(OBJDIR)spalign_asm_ia32e_gcc.o
    endif
endif

ifeq ($(TARGET),ia32)
    SANITY_TESTS += sse-ref.test smc_sse.test smc_ia32.test smc_mt.test
    ifeq ($(TARGET_OS),l)
       SANITY_TESTS += int3.test	
    endif
    ifeq ($(TARGET_COMPILER),ms)
        SSE_ASM_OBJ     = $(OBJDIR)sse_ia32.obj
        SSE-REF_ASM_OBJ = $(OBJDIR)sse-ref_ia32.obj
        SWIZZLETEST_ASM_OBJ = $(OBJDIR)swizzlealloc_ia32.obj
        EA_VERIFIER_ADDR16_ASM_OBJ = $(OBJDIR)ea_verifier_addr16_ms.obj
    endif
endif

TESTS_TO_RUN = $(TEST_TOOLS:%=%.test) $(STATIC_TEST_TOOLS:%=%.test)
ifeq ($(TARGET),ia32e)
    ifneq ($(TARGET_OS),w)
        TESTS_TO_RUN += big_bss.test
    endif
endif
ifeq ($(TARGET),ia32)
    ifeq ($(TARGET_COMPILER),ms)
		    TESTS_TO_RUN +=  df_test1_inline.test df_test1_noinline.test df_test1_noinline_bridge.test
            TESTS_TO_RUN += df_test2_inline.test df_test2_noinline.test df_test2_noinline_bridge.test
            TESTS_TO_RUN += df_test3_inline.test df_test3_noinline.test df_test3_noinline_bridge.test 
            TESTS_TO_RUN += df_test4_inline.test df_test4_noinline.test df_test4_noinline_bridge.test 
            TESTS_TO_RUN += df_test5_inline.test df_test5_noinline.test df_test5_noinline_bridge.test 
            TESTS_TO_RUN += analysis_flag_overwrite_test1_inline.test analysis_flag_overwrite_test1_noinline.test analysis_flag_overwrite_test1_noinline_bridge.test
            TESTS_TO_RUN += analysis_flag_overwrite_test2_inline.test analysis_flag_overwrite_test2_noinline.test analysis_flag_overwrite_test2_noinline_bridge.test
            TESTS_TO_RUN += analysis_flag_overwrite_test3_inline.test analysis_flag_overwrite_test3_noinline.test analysis_flag_overwrite_test3_noinline_bridge.test
            TESTS_TO_RUN += flag_spill_fill_test1_inline.test flag_spill_fill_test1_noinline.test flag_spill_fill_test1_noinline_bridge.test
            TESTS_TO_RUN += tstcmpxchg8b_with_explicit_ebx.test reg_operands_test.test
    endif

endif

ifeq ($(TARGET_OS),w)
    ifeq ($(TARGET_COMPILER),ms)
        TESTS_TO_RUN += load_dummy_dll.test load_dummy_dll_probe.test
        TESTS_TO_RUN += rebase_dll.test rebase_dll_probe.test
        TESTS_TO_RUN += exports_only.test exports_only_probe.test
        ifeq ($(TARGET),ia32e)
            TESTS_TO_RUN += x86dll.test
        endif
    endif
endif

tests-sanity: $(OBJDIR) $(SANITY_TESTS)
# uncomment when mt working on ipf as2.1
#tests-sanity: htab.test mt.test


test: $(OBJDIR) $(TESTS_TO_RUN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

ifeq ($(TARGET),ipf)
predinlinetest.test: $(OBJDIR)predinlinetest$(PINTOOL_SUFFIX) predinlinetest.tested predinlinetest.failed simple.c
	$(CC) -o $(OBJDIR)simple_static -static simple.c
	$(PIN) -t $< -o inline.out -- ./$(OBJDIR)simple_static result1.out
	$(PIN) -xyzzy -inline 0 -t $< -o noinline.out -- ./$(OBJDIR)simple_static result2.out
	$(PIN_CMP) result1.out result2.out
	$(PIN_CMP) inline.out noinline.out
	rm inline.out noinline.out result1.out result2.out predinlinetest.failed
endif

THREAD_LIB=$(OBJDIR)threadlib.$(OBJEXT)
SYS_LIB=$(THREAD_LIB) $(OBJDIR)sys_memory.$(OBJEXT) 
RUN_LIB=$(OBJDIR)runnable.$(OBJEXT) $(OBJDIR)thread_pool.$(OBJEXT)
SMC_LIB=$(RUN_LIB) $(SYS_LIB) $(OBJDIR)smc_util.$(OBJEXT)

$(OBJDIR)threadlib.$(OBJEXT): threadlib_$(OS_API).c threadlib.h
	$(CC) $(APP_CXXFLAGS) ${COPT} ${OUTOPT}$@ $< 

$(OBJDIR)sys_memory.$(OBJEXT): sys_memory_$(OS_API).c sys_memory.h
	$(CC) $(APP_CXXFLAGS) ${COPT} ${OUTOPT}$@ $< 

$(OBJDIR)runnable.$(OBJEXT): runnable.cpp runnable.h
	$(CXX) $(APP_CXXFLAGS) ${COPT} ${OUTOPT}$@ $< 

$(OBJDIR)thread_pool.$(OBJEXT): thread_pool.cpp thread_pool.h threadlib.h
	$(CXX) $(APP_CXXFLAGS) ${COPT} ${OUTOPT}$@ $< 

$(OBJDIR)smc_util.$(OBJEXT): smc_util.cpp smc_util.h
	$(CXX) $(APP_CXXFLAGS) ${COPT} $(OPT) ${OUTOPT}$@ $< 

# file rename for hello.b is to avoid the copyright notice from changing line numbers
# gdwarf-2 is the default on newer compilers
line.test : $(OBJDIR)line$(PINTOOL_SUFFIX) line.tested line.failed $(OBJDIR)$(HELLO)
	rm -f pin.log line.pin.log
	$(PIN) -xyzzy -logfile line.pin.log -separate_memory 0 -t $< -- ./$(OBJDIR)$(HELLO)
	$(PIN_DIFF) line.output line.reference
	# Is pin writing a log file?
	[ ! -f line.pin.log ]
	rm line.failed

$(OBJDIR)hello: hello.b
	cp hello.b hello.c
	$(CC) $(APP_CXXFLAGS) -gdwarf-2 -g -O0 -o $(OBJDIR)hello hello.c -static

$(OBJDIR)hello-ms-$(TARGET) : hello-ms.exe-$(TARGET) hello-ms.pdb-$(TARGET)
	cp hello-ms.exe-$(TARGET) $(OBJDIR)hello-ms-$(TARGET).exe
	cp hello-ms.pdb-$(TARGET) $(OBJDIR)hello-ms-$(TARGET).pdb

$(OBJDIR)hello-ms.exe-$(TARGET) $(OBJDIR)hello-ms.pdb-$(TARGET): hello.b
	cp hello.b hello.c
	$(CC) /Zi /Od /MT /Fe$(OBJDIR)hello-ms.exe-$(TARGET) hello.c /D_STATIC_CPPLIB /link /DEBUG /PDB:$(OBJDIR)hello-ms.pdb-$(TARGET)

$(OBJDIR)hello-mac: hello.b
	cp hello.b hello.c
	$(CC) -g -O0 -o $(OBJDIR)hello-mac hello.c

htab.test: $(OBJDIR)htab htab.tested htab.failed
	$(PIN) -- ./$(OBJDIR)htab
	rm htab.failed

$(OBJDIR)htab: htab.c $(THREAD_LIB)
	$(CC) $(APP_CXXFLAGS)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(THREAD_LIB) $(LPTHREAD)
#	$(CC) ${OUTOPT}htab htab.c -lpthread

threadexit.test: $(OBJDIR)threadexit threadexit.tested threadexit.failed
	$(PIN) -- ./$(OBJDIR)threadexit >  $<.out 2>&1
	$(PIN_DIFF) $<.out threadexit.reference
	rm threadexit.failed $<.out

$(OBJDIR)threadexit: threadexit.c
	$(CC) ${OUTOPT}$(OBJDIR)threadexit threadexit.c -lpthread

malmalloc.test : $(OBJDIR)malmalloc$(PINTOOL_SUFFIX) $(OBJDIR)thread malmalloc.tested malmalloc.failed
	$(PIN) -t $< -- ./$(OBJDIR)thread >  $<.out 2>&1
	$(PIN_DIFF) $<.out malmalloc.reference
	rm malmalloc.failed $<.out

mt.test : $(OBJDIR)mt$(PINTOOL_SUFFIX) $(OBJDIR)thread mt.tested mt.failed
	$(PIN) -t $< -- ./$(OBJDIR)thread
	sort mt.out | grep -v 5 > mt.out.sort
	$(PIN_DIFF) mt.out.sort mt.reference
	rm mt.failed mt.out mt.out.sort

kthread.test: $(OBJDIR)kthread kthread.tested kthread.failed
	$(PIN) -- ./$(OBJDIR)kthread > $<.out 2>&1
	$(PIN_DIFF) $<.out kthread.reference
	rm kthread.failed $<.out

callsp_ia32.test: $(OBJDIR)callsp_ia32 callsp_ia32.tested callsp_ia32.failed
	$(PIN) -- ./$(OBJDIR)callsp_ia32
	rm callsp_ia32.failed

callsp_intel64.test: $(OBJDIR)callsp_intel64 callsp_intel64.tested callsp_intel64.failed
	$(PIN) -- ./$(OBJDIR)callsp_intel64
	rm callsp_intel64.failed

mtstatic.test : $(OBJDIR)thread_static mtstatic.tested mtstatic.failed
	$(PIN)  -- ./$(OBJDIR)thread_static >  $<.out 2>&1
	$(PIN_DIFF) $<.out mtstatic.reference
	rm mtstatic.failed $<.out

sigmask-mac.test: $(OBJDIR)sigmask sigmask-mac.tested sigmask-mac.failed
	$(PIN) -- ./$(OBJDIR)sigmask >  $*.out 2>&1
	$(PIN_DIFF) $*.out sigmask-mac.reference
	rm sigmask-mac.failed $*.out

sigmask.test: $(OBJDIR)sigmask sigmask.tested sigmask.failed
	$(PIN) -- ./$(OBJDIR)sigmask >  $<.out 2>&1
	$(PIN_DIFF) $<.out sigmask.reference
	rm sigmask.failed $<.out

sigmaskstatic.test: $(OBJDIR)sigmaskstatic sigmaskstatic.tested sigmaskstatic.failed
	$(PIN) -- ./$(OBJDIR)sigmaskstatic >  $<.out 2>&1
	$(PIN_DIFF) $<.out sigmask.reference
	rm sigmaskstatic.failed $<.out

$(OBJDIR)swizzlealloc: swizzlealloc.c $(SWIZZLETEST_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(SWIZZLETEST_ASM_OBJ)

$(OBJDIR)df_test_app: df_test_app.c $(OBJDIR)$(DF_TEST_APP_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(OBJDIR)$(DF_TEST_APP_ASM_OBJ) 

$(OBJDIR)reg_operands_test_app: reg_operands_test_app.c $(OBJDIR)$(REG_OPERANDS_TEST_APP_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(OBJDIR)$(REG_OPERANDS_TEST_APP_ASM_OBJ) 

$(OBJDIR)seg_override_app1: seg_override_app1.c $(OBJDIR)$(SEG_OVERRIDE_APP1_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE)  ${OUTEXE}$@ $< $(LINK_OPTION) $(OBJDIR)$(SEG_OVERRIDE_APP1_ASM_OBJ) 

$(OBJDIR)analysis_flag_overwrite_app: analysis_flag_overwrite_app.c $(OBJDIR)$(ANALYSIS_FLAG_OVERWRITE_APP_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(OBJDIR)$(ANALYSIS_FLAG_OVERWRITE_APP_ASM_OBJ) 

$(OBJDIR)flag_spill_fill_app: flag_spill_fill_app.c $(OBJDIR)$(FLAG_SPILL_FILL_APP_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(OBJDIR)$(FLAG_SPILL_FILL_APP_ASM_OBJ) 

# Assembler file tstcmpxchg8b.c
$(OBJDIR)cmpxchg8b_ms.obj: cmpxchg8b_ms.asm 
	ml /nologo /c /Fo$@ $<

$(OBJDIR)cmpxchg8b_with_explicit_ebx_ms.obj: cmpxchg8b_with_explicit_ebx_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)cmpxchg8b_gnu.o: cmpxchg8b_gnu.asm 
	$(CC) $(APP_CXXFLAGS) -x assembler-with-cpp -c $< -o $@

$(OBJDIR)tstcmpxchg8b.$(OBJEXT): tstcmpxchg8b.c
	$(CC) $(COPT) $(APP_CXXFLAGS) ${OUTOPT}$@ $<

$(OBJDIR)tstcmpxchg8b_with_explicit_ebx.$(OBJEXT): tstcmpxchg8b_with_explicit_ebx.c
	$(CC) $(COPT) ${OUTOPT}$@ $<

$(OBJDIR)tstcmpxchg8b$(EXEEXT): $(OBJDIR)tstcmpxchg8b.$(OBJEXT) $(OBJDIR)$(CMPXCHG8_ASM_OBJ)
	$(CC) $(APP_CXXFLAGS) $(OBJDIR)tstcmpxchg8b.$(OBJEXT) $(OBJDIR)$(CMPXCHG8_ASM_OBJ) ${OUTEXE}$@

$(OBJDIR)tstcmpxchg8b_with_explicit_ebx$(EXEEXT): $(OBJDIR)tstcmpxchg8b_with_explicit_ebx.$(OBJEXT) $(OBJDIR)$(CMPXCHG8_WITH_EXPLICIT_EBX_ASM_OBJ)
	$(CC)  $(OBJDIR)tstcmpxchg8b_with_explicit_ebx.$(OBJEXT) $(OBJDIR)$(CMPXCHG8_WITH_EXPLICIT_EBX_ASM_OBJ) ${OUTEXE}$@

$(OBJDIR)df_test_app_ms.obj: df_test_app_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)reg_operands_test_app_ms.obj: reg_operands_test_app_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)seg_override_app1_ms.obj: seg_override_app1_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)segmented_ea_verifier_win1_ms.obj: segmented_ea_verifier_win1_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)ea_verifier_addr16_ms.obj: ea_verifier_addr16_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)analysis_flag_overwrite_app_ms.obj: analysis_flag_overwrite_app_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)flag_spill_fill_app_ms.obj: flag_spill_fill_app_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)segmented_ea_verifier_lin_$(TARGET).o: segmented_ea_verifier_lin_$(TARGET).s
	$(CC) $(COPT) $(APP_CXXFLAGS) ${OUTOPT}$@ $<

$(OBJDIR)ea_verifier_lin_addr16_$(TARGET).o: ea_verifier_lin_addr16_$(TARGET).s
	$(CC) $(COPT) $(APP_CXXFLAGS) ${OUTOPT}$@ $<


inscount2_fornoinline.test: $(OBJDIR)inscount2_fornoinline$(PINTOOL_SUFFIX) $(TESTAPP)  inscount2_fornoinline.tested inscount2_fornoinline.failed
	$(PIN) -xyzzy -inline 0 -t $< -- $(TESTAPP) makefile inscount2_fornoinline.makefile.copy
	$(PIN_DIFF) makefile inscount2_fornoinline.makefile.copy
	rm inscount2_fornoinline.failed inscount2_fornoinline.tested inscount2_fornoinline.makefile.copy	

reg_operands_test.test: $(OBJDIR)reg_operands_test_tool$(PINTOOL_SUFFIX) $(OBJDIR)reg_operands_test_app reg_operands_test.tested reg_operands_test.failed
	$(PIN)  -t $< -- ./$(OBJDIR)reg_operands_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out reg_operands_test.reference
	rm reg_operands_test.failed $<.out

df_test1_inline.test: $(OBJDIR)df_test_tool1$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test1_inline.tested df_test1_inline.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test1_inline.reference
	rm df_test1_inline.failed $<.out

df_test1_noinline.test: $(OBJDIR)df_test_tool1$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test1_noinline.tested df_test1_noinline.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test1_noinline.reference
	rm df_test1_noinline.failed $<.out

df_test1_noinline_bridge.test: %.test : $(OBJDIR)df_test_tool1$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test1_noinline_bridge.reference
	rm df_test1_noinline_bridge.failed $<.out

df_test2_inline.test: $(OBJDIR)df_test_tool2$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test2_inline.tested df_test2_inline.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test2_inline.reference
	rm df_test2_inline.failed $<.out

df_test2_noinline.test: $(OBJDIR)df_test_tool2$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test2_noinline.tested df_test2_noinline.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test2_noinline.reference
	rm df_test2_noinline.failed $<.out

df_test2_noinline_bridge.test: %.test : $(OBJDIR)df_test_tool2$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test2_noinline_bridge.reference
	rm df_test2_noinline_bridge.failed $<.out

df_test3_inline.test: $(OBJDIR)df_test_tool3$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test3_inline.tested df_test3_inline.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test3_inline.reference
	rm df_test3_inline.failed $<.out

df_test3_noinline.test: %.test : $(OBJDIR)df_test_tool3$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test3_noinline.reference
	rm df_test3_noinline.failed $<.out

df_test3_noinline_bridge.test: %.test : $(OBJDIR)df_test_tool3$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test3_noinline_bridge.reference
	rm df_test3_noinline_bridge.failed $<.out

df_test4_inline.test: $(OBJDIR)df_test_tool4$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test4_inline.tested df_test4_inline.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)df_test_app  >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test4_inline.reference
	rm df_test4_inline.failed $<.out

df_test4_noinline.test: $(OBJDIR)df_test_tool4$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test4_noinline.tested df_test4_noinline.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)df_test_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test4_noinline.reference
	rm df_test4_noinline.failed $<.out

df_test4_noinline_bridge.test: %.test : $(OBJDIR)df_test_tool4$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)df_test_app  >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test4_noinline_bridge.reference
	rm df_test4_noinline_bridge.failed $<.out

df_test5_inline.test: $(OBJDIR)df_test_tool5$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app df_test5_inline.tested df_test5_inline.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)df_test_app  >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test5_inline.reference
	rm df_test5_inline.failed $<.out

df_test5_noinline.test: %.test : $(OBJDIR)df_test_tool5$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)df_test_app  >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test5_noinline.reference
	rm df_test5_noinline.failed $<.out

df_test5_noinline_bridge.test: %.test : $(OBJDIR)df_test_tool5$(PINTOOL_SUFFIX) $(OBJDIR)df_test_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)df_test_app  >  $<.out 2>&1
	$(PIN_DIFF) $<.out df_test5_noinline_bridge.reference
	rm df_test5_noinline_bridge.failed $<.out

flag_spill_fill_test1_inline.test: $(OBJDIR)flag_spill_fill_tool1$(PINTOOL_SUFFIX) $(OBJDIR)flag_spill_fill_app flag_spill_fill_test1_inline.tested flag_spill_fill_test1_inline.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)flag_spill_fill_app  >  $<.out 2>&1
	$(PIN_DIFF) $<.out flag_spill_fill_test1_inline.reference
	rm flag_spill_fill_test1_inline.failed $<.out

flag_spill_fill_test1_noinline.test: $(OBJDIR)flag_spill_fill_tool1$(PINTOOL_SUFFIX) $(OBJDIR)flag_spill_fill_app flag_spill_fill_test1_noinline.tested flag_spill_fill_test1_noinline.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)flag_spill_fill_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out flag_spill_fill_test1_noinline.reference
	rm flag_spill_fill_test1_noinline.failed $<.out

flag_spill_fill_test1_noinline_bridge.test: $(OBJDIR)flag_spill_fill_tool1$(PINTOOL_SUFFIX) $(OBJDIR)flag_spill_fill_app flag_spill_fill_test1_noinline_bridge.tested flag_spill_fill_test1_noinline_bridge.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)flag_spill_fill_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out flag_spill_fill_test1_noinline_bridge.reference
	rm flag_spill_fill_test1_noinline_bridge.failed $<.out

analysis_flag_overwrite_test1_inline.test: %.test: $(OBJDIR)analysis_flag_overwrite_tool1$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app  >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test1_inline.reference
	rm analysis_flag_overwrite_test1_inline.failed $<.out

analysis_flag_overwrite_test1_noinline.test: %.test: $(OBJDIR)analysis_flag_overwrite_tool1$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test1_noinline.reference
	rm analysis_flag_overwrite_test1_noinline.failed $<.out

analysis_flag_overwrite_test1_noinline_bridge.test: %.test: $(OBJDIR)analysis_flag_overwrite_tool1$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test1_noinline_bridge.reference
	rm analysis_flag_overwrite_test1_noinline_bridge.failed $<.out

analysis_flag_overwrite_test2_inline.test: %.test: $(OBJDIR)analysis_flag_overwrite_tool2$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test2_inline.reference
	rm analysis_flag_overwrite_test2_inline.failed $<.out

analysis_flag_overwrite_test2_noinline.test: %.test: $(OBJDIR)analysis_flag_overwrite_tool2$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test2_noinline.reference
	rm analysis_flag_overwrite_test2_noinline.failed $<.out

analysis_flag_overwrite_test2_noinline_bridge.test:  %.test:$(OBJDIR)analysis_flag_overwrite_tool2$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test2_noinline_bridge.reference
	rm analysis_flag_overwrite_test2_noinline_bridge.failed $<.out

analysis_flag_overwrite_test3_inline.test: %.test: $(OBJDIR)analysis_flag_overwrite_tool3$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 1 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test3_inline.reference
	rm analysis_flag_overwrite_test3_inline.failed $<.out

analysis_flag_overwrite_test3_noinline.test:  %.test: $(OBJDIR)analysis_flag_overwrite_tool3$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test3_noinline.reference
	rm analysis_flag_overwrite_test3_noinline.failed $<.out

analysis_flag_overwrite_test3_noinline_bridge.test:  %.test: $(OBJDIR)analysis_flag_overwrite_tool3$(PINTOOL_SUFFIX) $(OBJDIR)analysis_flag_overwrite_app %.tested %.failed
	$(PIN) -xyzzy -inline 0 -inline_bridge 0 -t $< -- ./$(OBJDIR)analysis_flag_overwrite_app >  $<.out 2>&1
	$(PIN_DIFF) $<.out analysis_flag_overwrite_test3_noinline_bridge.reference
	rm analysis_flag_overwrite_test3_noinline_bridge.failed $<.out

swizzle2.test: $(OBJDIR)swizzle2$(PINTOOL_SUFFIX) $(OBJDIR)swizzlealloc swizzle2.tested swizzle2.failed
	$(PIN) -t $< -- ./$(OBJDIR)swizzlealloc >  $<.out 2>&1
	$(PIN_DIFF) $<.out swizzle2.reference
	rm swizzle2.failed $<.out

swizzle3.test: $(OBJDIR)swizzle3$(PINTOOL_SUFFIX) $(OBJDIR)swizzlealloc swizzle3.tested swizzle3.failed
	$(PIN) -t $< -- ./$(OBJDIR)swizzlealloc >  $<.out 2>&1
	$(PIN_DIFF) $<.out swizzle3.reference
	rm swizzle3.failed $<.out

swizzle5.test: $(OBJDIR)swizzle5$(PINTOOL_SUFFIX) $(OBJDIR)swizzlealloc swizzle5.tested swizzle5.failed
	$(PIN) -t $< -- ./$(OBJDIR)swizzlealloc >  $<.out 2>&1
	$(PIN_DIFF) $<.out swizzle5.reference
	rm swizzle5.failed $<.out

rewritememop.test: $(OBJDIR)rewritememop$(PINTOOL_SUFFIX) $(OBJDIR)tstcmpxchg8b$(EXEEXT) rewritememop.tested rewritememop.failed
	$(PIN) -t $< -- ./$(OBJDIR)tstcmpxchg8b$(EXEEXT) > $<.out 2>&1
	rm rewritememop.failed rewritememtrace.out $<.out


tstcmpxchg8b_with_explicit_ebx.test:  $(OBJDIR)tstcmpxchg8b_with_explicit_ebx$(EXEEXT) tstcmpxchg8b_with_explicit_ebx.tested tstcmpxchg8b_with_explicit_ebx.failed
	$(PIN)  -- ./$(OBJDIR)tstcmpxchg8b_with_explicit_ebx$(EXEEXT) > $<.out 2>&1
	$(PIN_DIFF) $<.out tstcmpxchg8b_with_explicit_ebx.reference
	rm tstcmpxchg8b_with_explicit_ebx.failed  $<.out

safecopy.test : $(OBJDIR)safecopy$(PINTOOL_SUFFIX) safecopy.tested safecopy.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -o safecopy.out -- $(TESTAPP) makefile $<.makefile.copy
	$(PIN_DIFF) safecopy.out safecopy.reference
	$(PIN_CMP) makefile $<.makefile.copy
	rm safecopy.failed  safecopy.out $<.makefile.copy


$(OBJDIR)kthread: kthread.c $(THREAD_LIB)
	$(CC) $(APP_CXXFLAGS)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(THREAD_LIB) $(LPTHREAD)

$(OBJDIR)thread: thread.c $(THREAD_LIB)
	$(CC) $(APP_CXXFLAGS)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(THREAD_LIB) $(LPTHREAD)

$(OBJDIR)thread_static: thread.c $(THREAD_LIB)
	$(CC) $(APP_CXXFLAGS) $(STATIC) $< ${OUTEXE}$@ $(APP_CXXLINK_FLAGS) $(THREAD_LIB) $(LPTHREAD)

$(OBJDIR)thread_longshort: thread_longshort.c  $(THREAD_LIB)
	$(CC) $(APP_CXXFLAGS)  ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(THREAD_LIB) $(LPTHREAD)

$(OBJDIR)thread_segmented_ea: thread_segmented_ea.c  $(THREAD_LIB) $(SEGMENTED_EA_VERIFIER_ASM_OBJ)
	$(CC) $(APP_CXXFLAGS)  ${OUTEXE}$@ $< $(LINK_OPTION) $(THREAD_LIB) $(LPTHREAD) $(SEGMENTED_EA_VERIFIER_ASM_OBJ)

$(OBJDIR)thread_ea_addr16: thread_ea_addr16.c  $(THREAD_LIB) $(EA_VERIFIER_ADDR16_ASM_OBJ)
	$(CC) $(APP_CXXFLAGS)  ${OUTEXE}$@ $< $(LINK_OPTION) $(THREAD_LIB) $(LPTHREAD) $(EA_VERIFIER_ADDR16_ASM_OBJ)

$(OBJDIR)callsp_ia32: callsp_ia32.s
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$@ callsp_ia32.s

$(OBJDIR)callsp_intel64: callsp_intel64.s
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$@ callsp_intel64.s

$(OBJDIR)sigmaskstatic: sigmask.c
	$(CC) ${OUTOPT}$@ sigmask.c -static

$(OBJDIR)sigmask: sigmask.c
	$(CC) ${OUTOPT}$@ sigmask.c 

findthreadwithappstack.test : %.test :$(OBJDIR)malmalloc$(PINTOOL_SUFFIX) $(OBJDIR)thread_longshort %.tested %.failed
	$(PIN) -xyzzy -switchstack 0 -t $< -- ./$(OBJDIR)thread_longshort >  $<.out 2>&1
	$(PIN_DIFF) $<.out findthreadwithappstack.reference
	rm findthreadwithappstack.failed $<.out

$(OBJDIR)dummy.so : dummy.c
	$(CC) $(APP_CXXFLAGS) -shared $< ${OUTOPT}$@

stackunswitch.test: $(OBJDIR)stackunswitch$(PINTOOL_SUFFIX) $(OBJDIR)thread stackunswitch.tested stackunswitch.failed
	$(PIN)  -t $< -- ./$(OBJDIR)thread >  $<.out 2>&1
	$(PIN_DIFF) $<.out stackunswitch.reference
	rm stackunswitch.failed $<.out


# setting rpath did not work with cc on mmdcs082
soload.test: $(OBJDIR)soload${PINTOOL_SUFFIX} $(OBJDIR)$(DLTEST) soload.tested soload.failed
	export LD_LIBRARY_PATH=`pwd`\/$(OBJDIR); $(PIN) -t $< -- ./$(OBJDIR)$(DLTEST)
	$(PIN_DIFF) soload.out soload.reference
	rm soload.failed soload.out

soloadrange.test: $(OBJDIR)soloadrange${PINTOOL_SUFFIX} $(OBJDIR)$(DLTEST2) soloadrange.tested soloadrange.failed
	touch soloadrange.reference
	export LD_LIBRARY_PATH=`pwd`\/$(OBJDIR); $(PIN) -t $< -- ./$(OBJDIR)dltest2 > /dev/null 2>&1
	$(PIN_CMP) soloadrange.out soloadrange.reference
	rm soloadrange.failed soloadrange.out soloadrange.reference

# Test that flushing an MT app happens in a timely manner
$(OBJDIR)mtflushapp_unix: mtflushapp_unix.cpp
	$(CXX) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ mtflushapp_unix.cpp -lpthread

$(OBJDIR)mtflushapp_win.exe: mtflushapp_win.cpp
	$(CXX) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ mtflushapp_win.cpp

mtflush.test: $(OBJDIR)mtflush$(PINTOOL_SUFFIX) $(OBJDIR)$(MTFLUSHAPP) mtflush.tested mtflush.failed
	$(PIN) -t $(OBJDIR)mtflush${PINTOOL_SUFFIX} -- ./$(OBJDIR)$(MTFLUSHAPP)
	$(PIN_DIFF) $(@:.test=.out) $(@:.test=.reference)
	rm mtflush.failed

# Test the automatic resizing of stripes
resize.test: resize.tested resize.failed
	$(PIN) -xyzzy -max_rtn 1024 -recycle_rtn 0 -- $(TESTAPP) makefile resize.makefile.copy
	$(PIN_CMP) makefile resize.makefile.copy
	rm resize.failed resize.makefile.copy

# Test the small secondary stack
nosahflahf.test:  %.test : %.tested %.failed
	$(PIN) -xyzzy -use_sahf 0 -t -- $(TESTAPP) makefile nosahflahf.makefile.copy 
	$(PIN_DIFF) makefile nosahflahf.makefile.copy
	rm nosahflahf.failed nosahflahf.makefile.copy 

# Test reserve memory
$(OBJDIR)reserve_memory: reserve_memory.c
	$(CC) $(APP_CXXFLAGS) -o $@ $< -static

reservemem.test: reservemem.tested reservemem.failed
	$(PIN) -xyzzy -reserve_memory large.address -- $(TESTAPP) makefile reservemem.makefile.copy
	$(PIN_CMP) makefile reservemem.makefile.copy
	rm reservemem.failed reservemem.makefile.copy

reservemem64.test: $(OBJDIR)reserve_memory reservemem64.tested reservemem64.failed reservemem.tested reservemem.failed
	$(PIN) -xyzzy -reserve_memory large.address -- $(TESTAPP) makefile reservemem.makefile.copy
	$(PIN_CMP) makefile reservemem.makefile.copy
	rm reservemem.failed reservemem.makefile.copy
	$(OBJDIR)reserve_memory bigrange.address 0 > reservemem64.native.out
	$(PIN) -xyzzy -reserve_memory bigrange.address -- $(OBJDIR)reserve_memory bigrange.address 1 > reservemem64.pin.out
	cmp reservemem64.native.out reservemem64.pin.out
	rm reservemem64.failed reservemem64.native.out reservemem64.pin.out

partialinline2.test : $(OBJDIR)partialinline$(PINTOOL_SUFFIX) partialinline2.tested partialinline2.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -xyzzy -inline 0 -t $< -- $(TESTAPP) makefile $<.makefile.copy
	$(PIN_CMP) makefile $<.makefile.copy
	rm partialinline2.failed $<.makefile.copy

syscall.test : $(OBJDIR)syscall$(PINTOOL_SUFFIX) syscall.tested syscall.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy
	rm syscall.failed $<.makefile.copy

inlinecall.test : $(OBJDIR)inlinecall$(PINTOOL_SUFFIX) inlinecall.tested inlinecall.failed $(OBJDIR)$(ONEPROG)
	$(PIN) -t $< -- ./$(OBJDIR)$(ONEPROG) >  $<.out 2>&1
	$(PIN_DIFF) $<.out inlinecall.reference
	rm inlinecall.failed $<.out

rollback.test : $(OBJDIR)rollback$(PINTOOL_SUFFIX) rollback.tested rollback.failed $(OBJDIR)$(HELLO)
	$(PIN) -t $< -save 80 -resume 110 -- ./$(OBJDIR)$(HELLO)
	$(PIN) -t $< -save 80 -resume 110 -usectxt -- ./$(OBJDIR)$(HELLO)
	rm rollback.failed

appcall.test : $(OBJDIR)appcall$(PINTOOL_SUFFIX) appcall.tested appcall.failed $(OBJDIR)$(HELLO)
	$(PIN) -t $<  -- ./$(OBJDIR)$(HELLO) > $<.out
	$(PIN_DIFF) $<.out appcall.reference
	rm appcall.failed $<.out

strace_ipf.test : $(OBJDIR)strace_ipf$(PINTOOL_SUFFIX) strace_ipf.tested strace_ipf.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep -q "Success" strace.out
	grep -q -v "Failure" strace.out
	grep -q "eof" strace.out
	rm strace_ipf.failed strace.out $<.out

strace_ia32.test : $(OBJDIR)strace_ia32$(PINTOOL_SUFFIX) strace_ia32.tested strace_ia32.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep -q "Success" strace.out
	grep -q -v "Failure" strace.out
	grep -q "eof" strace.out
	rm strace_ia32.failed strace.out $<.out

stracewin_ia32$(PINTOOL_SUFFIX).test : $(OBJDIR)stracewin_ia32$(PINTOOL_SUFFIX) stracewin_ia32.tested stracewin_ia32.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep -q "Success" stracewin.out
	grep -q -v "Failure" stracewin.out
	grep -q "eof" stracewin.out
	rm stracewin_ia32.failed stracewin.out $<.out

context.test : $(OBJDIR)context$(PINTOOL_SUFFIX) context.tested context.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -o $<.out -- $(TESTAPP) makefile $<.makefile.copy
	grep -q -v "Failure" $<.out
	grep -q "Success" $<.out
	rm context.failed $<.out $<.makefile.copy

jitmalloctrace.test : $(OBJDIR)jitmalloctrace$(PINTOOL_SUFFIX) jitmalloctrace.tested jitmalloctrace.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	$(PIN_CMP) makefile $<.makefile.copy
	grep "malloc" jitmalloctrace.outfile
	rm jitmalloctrace.failed $<.out $<.makefile.copy jitmalloctrace.outfile

replace_free.test : $(OBJDIR)replace_free$(PINTOOL_SUFFIX) replace_free.tested replace_free.failed $(OBJDIR)$(LITTLE_MALLOC)
	$(PIN) -t $< -- ./$(OBJDIR)$(LITTLE_MALLOC) > $<.out 2>&1
	grep -q "free" $<.out
	rm replace_free.failed $<.out

rtnreplace_free.test : $(OBJDIR)rtnreplace_free$(PINTOOL_SUFFIX) rtnreplace_free.tested rtnreplace_free.failed $(OBJDIR)$(LITTLE_MALLOC)
	$(PIN) -t $< -- ./$(OBJDIR)$(LITTLE_MALLOC) > $<.out 2>&1
	grep -q "free" $<.out
	rm rtnreplace_free.failed $<.out


short_name.test : $(OBJDIR)short_name$(PINTOOL_SUFFIX) short_name.tested short_name.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep fopen short_name.outfile
	$(PIN) -t $< -use_dynsym -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep fopen: short_name.outfile
	$(PIN) -t $< -use_dynsym -short_name -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep fopen: short_name.outfile
	$(PIN) -t $< -short_name -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep fopen short_name.outfile
	rm short_name.failed $<.out $<.makefile.copy short_name.outfile

replace_malloc_inst.test : $(OBJDIR)replace_malloc_inst$(PINTOOL_SUFFIX) replace_malloc_inst.tested replace_malloc_inst.failed $(OBJDIR)$(LITTLE_MALLOC)
	$(PIN) -t $< -- ./$(OBJDIR)$(LITTLE_MALLOC) >  $<.out 2>&1
	grep -q "Test passed" $<.out
	rm replace_malloc_inst.failed $<.out

thread_callback.test : $(OBJDIR)thread_callback$(PINTOOL_SUFFIX) $(OBJDIR)$(THREADTEST) thread_callback.tested thread_callback.failed
	-$(PIN) -t $< -- ./$(OBJDIR)$(THREADTEST) >  $<.out 2>&1
	echo "thread_callback should fail.  Ignore the error."
	rm thread_callback.failed $<.out

executeat_callback.test : $(OBJDIR)executeat_callback$(PINTOOL_SUFFIX) $(OBJDIR)$(THREADTEST) executeat_callback.tested executeat_callback.failed
	-$(PIN) -t $< -- ./$(OBJDIR)$(THREADTEST) >  $<.out 2>&1
	echo "executeat_callback should fail.  Ignore the error."
	grep -e "PIN_ExecuteAt() cannot be called from a callback." $<.out
	-$(PIN) -error_file myerr.log -t $< -- ./$(OBJDIR)$(THREADTEST) >  $<.out 2>&1
	grep -e "PIN_ExecuteAt() cannot be called from a callback." myerr.log
	rm executeat_callback.failed $<.out myerr.log

executeat_lock.test : $(OBJDIR)executeat_lock$(PINTOOL_SUFFIX) $(OBJDIR)$(THREADTEST) executeat_lock.tested executeat_lock.failed
	-$(PIN) -t $< -- ./$(OBJDIR)$(THREADTEST) >  $<.out 2>&1
	echo "executeat_lock should fail.  Ignore the error."
	grep -e "Pin Client Lock" $<.out
	rm executeat_lock.failed $<.out

args_err.test : $(OBJDIR)args_err$(PINTOOL_SUFFIX) args_err.tested args_err.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	echo "args_err should fail.  Ignore the error."
	-$(PIN) -error_file myerr.log -xyzzy -error -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep "Testing errors" myerr.log
	rm myerr.log
	echo "args_err should fail.  Ignore the error."
	-$(PIN) -error_file myerr.log -xyzzy -fatal -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep "Testing errors" myerr.log
	rm myerr.log
	echo "args_err should fail.  Ignore the error."
	-$(PIN) -error_file myerr.log -xyzzy -assert -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep "Testing assertions" myerr.log
	rm myerr.log
	echo "args_err should fail.  Ignore the error."
	-$(PIN) -error_file myerr.log -falafel -t $< -- $(TESTAPP) makefile $<.makefile.copy >  $<.out 2>&1
	grep "unknown option" myerr.log
	rm myerr.log
	echo "args_err should fail.  Ignore the error."
	-$(PIN) -error_file myerr.log -t $< >  $<.out 2>&1
	grep "Missing application name" myerr.log
	rm myerr.log
	echo "args_err should fail.  Ignore the error."
	rm args_err.failed $<.out


callapp0.test : $(OBJDIR)callapp0$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO) callapp0.tested callapp0.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO) >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	rm callapp0.failed $<.out

callapp0i.test : $(OBJDIR)callapp0i$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO) callapp0i.tested callapp0i.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO) >  $<.out 2>&1
	grep -q -e  "Hello from Bar" $<.out
	rm callapp0i.failed $<.out

callapp1.test : $(OBJDIR)callapp1$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO1) callapp1.tested callapp1.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO1) >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "one = 1" $<.out
	rm callapp1.failed $<.out

callapp1i.test : $(OBJDIR)callapp1i$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO1) callapp1i.tested callapp1i.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO1) >  $<.out 2>&1
	grep -q -e  "Hello from Bar" $<.out
	grep -q -e "one = 1" $<.out
	rm callapp1i.failed $<.out

callapp2.test : $(OBJDIR)callapp2$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO2) callapp2.tested callapp2.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO2) >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "res = 3" $<.out
	rm callapp2.failed $<.out

callapp3.test : $(OBJDIR)callapp3$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO3) callapp3.tested callapp3.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO3) >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "res = 3" $<.out
	rm callapp3.failed $<.out

callapp4.test : $(OBJDIR)callapp4$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO4) callapp4.tested callapp4.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO4) >  $<.out 2>&1
	grep -q "Bar" $<.out
	grep -q -e "cafe00" $<.out
	rm callapp4.failed $<.out

callapp5.test : $(OBJDIR)callapp5$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO2) callapp5.tested callapp5.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO2) >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "res = 3" $<.out
	rm callapp5.failed $<.out

callapp6.test : $(OBJDIR)callapp6$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO6) callapp6.tested callapp6.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO6) >  $<.out 2>&1
	grep -q "Blue6" $<.out
	grep -q "Bar6" $<.out
	grep -q "myBar" $<.out
	grep -q "myBlue" $<.out
	grep -q -e "res = 3" $<.out
	rm callapp6.failed $<.out

callapp7.test : $(OBJDIR)callapp7$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO7) callapp7.tested callapp7.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO7) >  $<.out 2>&1
	grep -q "Blue7: twice" $<.out
	grep -q "Bar7: twice" $<.out
	grep -q "myBar" $<.out
	grep -q "myBlue" $<.out
	grep -q -e "res = 3" $<.out
	rm callapp7.failed $<.out

callapp8.test : $(OBJDIR)callapp8$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO8) callapp8.tested callapp8.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO8) >  $<.out 2>&1
	grep -q "Bar8: thrice" $<.out
	grep -q "myBar" $<.out
	grep -q -e "res = 3" $<.out
	rm callapp8.failed $<.out

callapp10.test : $(OBJDIR)callapp10$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO10) callapp10.tested callapp10.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO10) >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "one = 1" $<.out
	grep -q -e "ret = 45" $<.out
	rm $<.out
	$(PIN) $(MT) -t $< -- ./$(OBJDIR)$(SIMPLEFOO10) >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "one = 1" $<.out
	grep -q -e "ret = 45" $<.out
	rm callapp10.failed $<.out

callapp10i.test : $(OBJDIR)callapp10i$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO10) callapp10i.tested callapp10i.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO10) >  $<.out 2>&1
	grep -q -e  "Hello from Bar" $<.out
	grep -q -e "one = 1" $<.out
	grep -q -e "ret = 45" $<.out
	rm callapp10i.failed $<.out

callappfast10.test : $(OBJDIR)callappfast10$(PINTOOL_SUFFIX) $(OBJDIR)winfast10 callappfast10.tested callappfast10.failed
	$(PIN) -t $< -- ./$(OBJDIR)winfast10 >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "one = 1" $<.out
	grep -q -e "ret = 45" $<.out
	rm callappfast10.failed $<.out

callappstd10.test : $(OBJDIR)callappstd10$(PINTOOL_SUFFIX) $(OBJDIR)winstd10 callappstd10.tested callappstd10.failed
	$(PIN) -t $< -- ./$(OBJDIR)winstd10 >  $<.out 2>&1
	grep -q "Blue" $<.out
	grep -q -v "Bar" $<.out
	grep -q -e "one = 1" $<.out
	grep -q -e "ret = 45" $<.out
	rm callappstd10.failed $<.out

callapp12.test : $(OBJDIR)callapp12$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO12) callapp12.tested callapp12.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO12) >  $<.out 2>&1
	grep -q "passed" $<.out
	grep -q "Correct" $<.out
	rm callapp12.failed $<.out

callapp13.test : $(OBJDIR)callapp13$(PINTOOL_SUFFIX) $(OBJDIR)$(SIMPLEFOO13) callapp13.tested callapp13.failed
	$(PIN) -t $< -- ./$(OBJDIR)$(SIMPLEFOO13) >  $<.out 2>&1
	grep -q "passed" $<.out
	grep -q "Correct" $<.out
	rm callapp13.failed $<.out

change_syscall.test : $(OBJDIR)change_syscall$(PINTOOL_SUFFIX) $(OBJDIR)change_syscall_app change_syscall.tested change_syscall.failed
	$(PIN) -t $< -- ./$(OBJDIR)change_syscall_app
	rm change_syscall.failed

$(OBJDIR)simple: simple.c
	$(CC) -o $(OBJDIR)simple simple.c

$(OBJDIR)simplefoo1: simplefoo1.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo1 simplefoo1.c simplebar.c  -g

$(OBJDIR)simplefoo2: simplefoo2.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo2 simplefoo2.c simplebar.c  -g

$(OBJDIR)simplefoo3: simplefoo3.c simplebar.c simplebar64.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo3 simplefoo3.c simplebar.c simplebar64.c  -g

$(OBJDIR)simplefoo4: simplefoo4.c simplebar.c simplebar64.c
	$(CC) $(APP_CXXFLAGS) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo4 simplefoo4.c simplebar.c simplebar64.c -g

$(OBJDIR)simplefoo6: simplefoo6.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo6 simplefoo6.c simplebar.c  -g

$(OBJDIR)simplefoo7: simplefoo7.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo7 simplefoo7.c simplebar.c  -g

$(OBJDIR)simplefoo8: simplefoo8.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo8 simplefoo8.c simplebar.c  -g

$(OBJDIR)simplefoo10: simplefoo10.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo10 simplefoo10.c simplebar.c  -g

$(OBJDIR)simplefoo12: simplefoo12.c simplebar.c
	$(CC) $(APP_CXXFLAGS) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo12 simplefoo12.c simplebar.c  -g

$(OBJDIR)simplefoo13: simplefoo13.c simplebar.c simplebar64.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $(OBJDIR)simplefoo13 simplefoo13.c simplebar.c simplebar64.c  -g

$(OBJDIR)simplefoo: simplefoo.c simplebar.c $(OBJDIR)simplesp.o
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -static -o $(OBJDIR)simplefoo simplefoo.c simplebar.c $(OBJDIR)simplesp.o -g

$(OBJDIR)simplesp.o: simplesp.asm 
	$(CC) $(APP_CXXFLAGS) -x assembler-with-cpp -c $(OUTOPT)$@ $<

$(OBJDIR)thread_wait: thread_wait.c addit.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $@ thread_wait.c addit.c -g -lpthread

$(OBJDIR)little_malloc: little_malloc.c
	$(CC) $(APP_CXXFLAGS) $(PIN_LPATHS) -I../Include -I. -o $@ little_malloc.c -g

$(OBJDIR)little_malloc.exe: little_malloc.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ little_malloc.c

$(OBJDIR)winfoo1: simplefoo1.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo1.c simplebar.c

$(OBJDIR)winfoo2: simplefoo2.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo2.c simplebar.c

$(OBJDIR)winfoo3: simplefoo3.c simplebar.c simplebar64.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ $< simplefoo3.c simplebar.c simplebar64.c

$(OBJDIR)winfoo4: simplefoo4.c simplebar.c simplebar64.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo4.c simplebar.c simplebar64.c

$(OBJDIR)winfoo5: simplefoo5.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo5.c simplebar.c

$(OBJDIR)winfoo6: simplefoo6.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo6.c simplebar.c

$(OBJDIR)winfoo7: simplefoo7.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo7.c simplebar.c

$(OBJDIR)winfoo8: simplefoo8.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo8.c simplebar.c

$(OBJDIR)winfoo10: simplefoo10.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo10.c simplebar.c

$(OBJDIR)winfast10: simplefast10.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) /Zi ${OUTEXE}$@ simplefast10.c simplebar.c

$(OBJDIR)winstd10: simplestd10.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) /Zi ${OUTEXE}$@ simplestd10.c simplebar.c

$(OBJDIR)winfoo12: simplefoo12.c simplebar.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo12.c simplebar.c

$(OBJDIR)winfoo13: simplefoo13.c simplebar.c simplebar64.c 
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ simplefoo13.c simplebar.c simplebar64.c


$(OBJDIR)threadtestwin.exe: threadtestwin.c addit.c
	$(CC) $(NO_OPTIMIZE) $(NO_LOGO) $(APP_CXXFLAGS) $(DBG) ${OUTEXE}$@ threadtestwin.c addit.c

$(OBJDIR)tmtest : tmtest.cc
	$(CXX) $(APP_CXXFLAGS) ${OUTOPT}$@ tmtest.cc -lpthread

transactionalmem.test : $(OBJDIR)transactionalmem$(PINTOOL_SUFFIX) transactionalmem.tested transactionalmem.failed tmtest
	$(PIN) -t $< -- ./tmtest
	rm transactionalmem.failed

sse-ref.test : $(OBJDIR)sse-ref$(PINTOOL_SUFFIX) $(OBJDIR)sse $(SSE-REF_ASM_OBJ) sse-ref.tested sse-ref.failed
	$(PIN)  -t $< -- ./$(OBJDIR)sse 5 > $<.out
	diff --ignore-space-change sse-ref.reference $<.out
	rm sse-ref.failed $<.out

# Same as sse-ref.test, but enforces SMC checks in all regions
smc_sse.test : $(OBJDIR)sse-ref$(PINTOOL_SUFFIX) $(OBJDIR)sse $(SSE-REF_ASM_OBJ) smc_sse.tested smc_sse.failed
	$(PIN) -xyzzy -smc_check_all 1 -t $< -- ./$(OBJDIR)sse 5 > $(OBJDIR)smc_sse.out
	diff --ignore-space-change sse-ref.reference $(OBJDIR)smc_sse.out
	rm smc_sse.failed $(OBJDIR)smc_sse.out

test_ip_access.test : $(OBJDIR)test_ip_access$(PINTOOL_SUFFIX) $(OBJDIR)test_ip_access_app  test_ip_access.tested test_ip_access.failed
	$(PIN)  -t $< -- ./$(OBJDIR)test_ip_access_app  > $<.out
	grep   "SUCCESS" $<.out
	rm test_ip_access.failed $<.out	

int3.test: $(OBJDIR)int3del$(PINTOOL_SUFFIX) $(OBJDIR)int3test int3.tested int3.failed
	$(PIN)  -t $< -- ./$(OBJDIR)int3test > $<.out
	rm int3.failed $<.out

# Assembler files for sse-ref
$(OBJDIR)sse-ref_ia32.obj: sse-ref_ia32.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)sse-ref_ia32e.obj: sse-ref_ia32e.asm
	ml64 /nologo /c /Fo$@ $<

$(OBJDIR)segmented_ea_verifier_win1_ia32e_ms.obj: segmented_ea_verifier_win1_ia32e_ms.asm
	ml64 /nologo /c /Fo$@ $<

$(OBJDIR)ea_verifier_addr16_ia32e_ms.obj: ea_verifier_addr16_ia32e_ms.asm
	ml64 /nologo /c /Fo$@ $<

$(OBJDIR)analysis_flag_overwrite_tool1_ms.obj: analysis_flag_overwrite_tool1_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)analysis_flag_overwrite_tool2_ms.obj: analysis_flag_overwrite_tool2_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)analysis_flag_overwrite_tool3_ms.obj: analysis_flag_overwrite_tool3_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)flag_spill_fill_tool1_ms.obj: flag_spill_fill_tool1_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)df_test_tool1_ms.obj: df_test_tool1_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)df_test_tool2_ms.obj: df_test_tool2_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)df_test_tool3_ms.obj: df_test_tool3_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)df_test_tool4_ms.obj: df_test_tool4_ms.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)df_test_tool5_ms.obj: df_test_tool5_ms.asm
	ml /nologo /c /Fo$@ $<

xmmtest.test : $(OBJDIR)xmmtest$(PINTOOL_SUFFIX) $(OBJDIR)xmmapp xmmtest.tested xmmtest.failed
	$(PIN)  -t $< -- ./$(OBJDIR)xmmapp 9 > $<.out
	./$(OBJDIR)xmmapp 9 > xmmapp.out
	diff --ignore-space-change xmmapp.out $<.out
	rm xmmtest.failed xmmapp.out $<.out

$(OBJDIR)swizzlealloc_ia32.obj: swizzlealloc_ia32.asm
	ml /nologo /c /Fo$@ $< 

# A test program that uses SSE 
$(OBJDIR)sse: sse.cpp $(SSE_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS2) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE) $(SSE2) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(SSE_ASM_OBJ)

$(OBJDIR)sse_ia32.obj: sse_ia32.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)sse_ia32e.obj: sse_ia32e.asm
	ml64 /nologo /c /Fo$@ $<

$(OBJDIR)test_ip_access_app: test_ip_access_app.cpp $(TEST_IP_ACCESS_ASM_OBJ)
	${CXX} $(APP_CXXFLAGS) $(NO_LOGO) $(DBG) $(NO_OPTIMIZE) $(SSE2) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(TEST_IP_ACCESS_ASM_OBJ)

$(OBJDIR)test_ip_access_app_ia32e.obj: test_ip_access_app_ia32e.asm
	ml64 /nologo /c /Fo$@ $<

# A test program that for xmm test 
$(OBJDIR)xmmapp.o: xmmapp.cpp
	${CXX} $(APP_CXXFLAGS) $(DBG) $(NO_OPTIMIZE) $(SSE2) ${OUTOPT}$@ -c $<

$(OBJDIR)xmmapp: $(OBJDIR)xmmapp.o $(SSE_ASM_OBJ) $(SSE-REF_ASM_OBJ)
	${LINKER} $(APP_CXXFLAGS) ${LINK_OUT}$@ $< $(SSE_ASM_OBJ) $(SSE-REF_ASM_OBJ)

branch_target_addr.test : %.test : $(OBJDIR)branch_target_addr$(PINTOOL_SUFFIX) $(OBJDIR)doint_ia32 %.tested %.failed
	$(PIN)  -t $< -- ./$(OBJDIR)doint_ia32
	rm branch_target_addr.failed

$(OBJDIR)doint_ia32: doint.c doint_ia32.s
	$(CC) $(APP_CXXFLAGS) -o $@ doint.c -x assembler-with-cpp doint_ia32.s

mmap.test: $(OBJDIR)mmap$(PINTOOL_SUFFIX) $(OBJDIR)mmapapp mmap.tested mmap.failed
	$(PIN) -t $(OBJDIR)mmap$(PINTOOL_SUFFIX) -- ./$(OBJDIR)mmapapp
	rm -f mmap.failed

$(OBJDIR)mmapapp: mmapapp.c
	$(CC) $(APP_CXXFLAGS) ${OUTEXE}$@ mmapapp.c

$(OBJDIR)change_syscall_app: change_syscall_app.c
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$@ change_syscall_app.c

pipe.test: $(OBJDIR)pipeapp pipe.tested pipe.failed
	$(PIN) -- ./$(OBJDIR)pipeapp
	rm -f pipe.failed

$(OBJDIR)pipeapp: pipeapp.c
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$@ pipeapp.c

smc_ipf.test: $(OBJDIR)smcapp_ipf smc_ipf.tested smc_ipf.failed
	$(PIN) -- ./$(OBJDIR)smcapp_ipf > $<.out 2>&1
	$(PIN_DIFF) $<.out smc_ipf.reference
	rm smc_ipf.failed $<.out

smc_ia32.test: $(OBJDIR)smcapp_ia32 smc_ia32.tested smc_ia32.failed
	$(PIN) -- ./$(OBJDIR)smcapp_ia32
	rm smc_ia32.failed

smc_except.test: $(OBJDIR)smcapp_except smc_except.tested smc_except.failed
	$(PIN) -- ./$(OBJDIR)smcapp_except
	rm smc_except.failed

smc_mt.test: $(OBJDIR)smcapp_mt smc_mt.tested smc_mt.failed
	$(PIN) -- ./$(OBJDIR)smcapp_mt
	rm smc_mt.failed

smc_bbl.test: $(OBJDIR)smcapp_bbl smc_bbl.tested smc_bbl.failed
	$(PIN) -smc_strict -- ./$(OBJDIR)smcapp_bbl
	rm smc_bbl.failed

$(OBJDIR)smcapp_ipf: smcapp_ipf.c smcapp_ipf_asm.s
	$(CC) ${OUTOPT}$@ smcapp_ipf.c -x assembler-with-cpp smcapp_ipf_asm.s

attachnat.test: $(OBJDIR)attachnat attachnat.tested attachnat.failed
	rm -f attachnat.pid; \
		./$(OBJDIR)attachnat 2> attachnat.pid 1> attachnat.out & \
		until test -s attachnat.pid; \
			do sleep 1; \
		done; \
		pid=`head -1 attachnat.pid | sed 's/Attach to me: //'`; \
		$(PIN) -pid $$pid; \
		wait $$!
	$(PIN_DIFF) attachnat.out attachnat.reference
	rm attachnat.failed attachnat.out attachnat.pid

$(OBJDIR)attachnat: attachnat.c attachnat_asm.s
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$@ attachnat.c -x assembler-with-cpp attachnat_asm.s

attach.test: $(OBJDIR)attach attach.tested attach.failed $(OBJDIR)fini$(PINTOOL_SUFFIX)
	./attach.sh $(OBJDIR) $(PIN)  > attach.err 2>&1
	$(PIN_DIFF) fini.out fini.reference || grep "THREAD_AREA" attach.err
	rm attach.failed fini.out pin_attached attach.err

$(OBJDIR)attach: attach.c
	$(CC) $(APP_CXXFLAGS) ${OUTEXE}$@ attach.c

spalign.test : $(OBJDIR)spalign$(PINTOOL_SUFFIX) spalign.tested spalign.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy
	rm spalign.failed

$(OBJDIR)spalign_asm_ia32e_ms.obj : spalign_asm_ia32e_ms.asm
	ml64 /nologo /c /Fo$@ $< 

$(OBJDIR)spalign$(PINTOOL_SUFFIX): $(OBJDIR)spalign.o $(SP_ALIGN_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG) $(SP_ALIGN_ASM_OBJ)

$(OBJDIR)insertand$(PINTOOL_SUFFIX): $(OBJDIR)insertand.o $(OBJDIR)and.o
	${LINKER} $(OBJDIR)and.o $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

$(OBJDIR)clobber$(PINTOOL_SUFFIX): $(OBJDIR)clobber.o $(OBJDIR)clobber_asm.o
	${LINKER} $(OBJDIR)clobber_asm.o $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

incebx.test : $(OBJDIR)incebx$(PINTOOL_SUFFIX) incebx.tested incebx.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -t $< -- $(TESTAPP) makefile $<.makefile.copy
	rm incebx.failed

badpath.test : badpath.tested badpath.failed
	-$(PIN) -- $(OBJDIR)BadAPPpAth >  badpath.out 2>&1
	grep "No such file" badpath.out 
	-$(PIN) -error_file myerr.log -- $(OBJDIR)BadAPPpAth >  badpath.out 2>&1
	grep "No such file" myerr.log 
	rm badpath.failed badpath.out myerr.log

badfile.test : %.test: %.tested %.failed
	-$(PIN) -error_file $*.log -- ./makefile >  $*.out 2>&1 || echo "non-zero status" >> $*.log
	grep -e "Permission denied" $*.log
	grep -q non-zero $*.log
	rm $*.failed $*.out $*.log

$(OBJDIR)cancel: cancel.c
	$(CC) $(APP_CXXFLAGS) -o $@ $< -lpthread

cancel.test: %.test: $(OBJDIR)% %.tested %.failed
	-./$(OBJDIR)cancel > cancel.native.out 2>&1
	-$(PIN) -- ./$(OBJDIR)cancel > cancel.pin.out 2>&1
	cmp cancel.native.out cancel.pin.out
	rm cancel.failed

$(OBJDIR)xmmtest.o: xmmtest.cpp
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) $(SSE2) ${OUTOPT}$@ $<

thread_count.test : %.test: $(OBJDIR)%$(PINTOOL_SUFFIX) %.tested %.failed $(OBJDIR)$(HELLO)
	touch hello.out; rm hello.out
	touch thread_count.out; rm thread_count.out
	$(PIN) -t $< -- $(OBJDIR)$(HELLO) >  hello.out 2>&1
	$(PIN_DIFF) hello.out hello.reference
	$(PIN_DIFF) thread_count.out thread_count.reference
	rm thread_count.failed thread_count.out hello.out

thread_count_thread_creation.test : %.test: $(OBJDIR)thread_count$(PINTOOL_SUFFIX) %.tested %.failed $(OBJDIR)win_thread_create_apc
	touch thread_count.out; rm thread_count.out
	touch win_thread_create_apc.out; rm win_thread_create_apc.out
	mkdir -p $(OBJDIR)"I Need My Space"
	$(PIN) -xyzzy -logfile $(OBJDIR)"I Need My Space/pin.log" -mesgon log_win -t $< -- $(OBJDIR)win_thread_create_apc >  win_thread_create_apc.out 2>&1
	$(PIN_DIFF) win_thread_create_apc.out win_thread_create_apc.reference
	$(PIN_DIFF) thread_count.out thread_count_thread_creation.reference
	rm thread_count_thread_creation.failed thread_count.out win_thread_create_apc.out $(OBJDIR)"I Need My Space/pin.log"
	rmdir $(OBJDIR)"I Need My Space"

win_multi_dll_pintool.test : %.test: $(OBJDIR)win_multi_dll_pintool %.tested %.failed $(OBJDIR)win_thread_create_apc
	touch main_dll.out; rm main_dll.out
	touch static_secondary_dll.out; rm static_secondary_dll.out
	touch dynamic_secondary_dll.out; rm dynamic_secondary_dll.out
	touch win_thread_create_apc.out; rm win_thread_create_apc.out
	$(PIN) -t main_dll -- $(OBJDIR)win_thread_create_apc >  win_thread_create_apc.out 2>&1
	$(PIN_DIFF) win_thread_create_apc.out win_thread_create_apc.reference
	$(PIN_DIFF) static_secondary_dll.out dynamic_secondary_dll.out
	rm $*.failed static_secondary_dll.out dynamic_secondary_dll.out win_thread_create_apc.out

reg_inst_gx.test : $(OBJDIR)reg_inst_gx$(PINTOOL_SUFFIX) reg_inst_gx.tested reg_inst_gx.failed $(OBJDIR)thread
	$(PIN) -t $< -o $(@:.test=.tool.out) -- ./$(OBJDIR)thread > $(@:.test=.out) 2>&1
	$(PIN_DIFF) $(@:.test=.out) $(@:.test=.reference)
	$(PIN_DIFF) $(@:.test=.tool.out) $(@:.test=.tool.reference)
	rm $(@:.test=.failed)

win_queue_apc.test : %.test: $(OBJDIR)apc_monitor$(PINTOOL_SUFFIX) %.tested %.failed $(OBJDIR)win_queue_apc
	touch apc_monitor.out; rm apc_monitor.out
	touch win_queue_apc.out; rm win_queue_apc.out
	$(PIN) -t $< -- $(OBJDIR)win_queue_apc >  win_queue_apc.out 2>&1
	$(PIN_DIFF) win_queue_apc.out win_queue_apc.reference
	$(PIN_DIFF) apc_monitor.out apc_monitor.reference 
	rm win_queue_apc.failed apc_monitor.out win_queue_apc.out

win_divide_by_zero_exception.test : %.test: $(OBJDIR)exception_monitor$(PINTOOL_SUFFIX) %.tested %.failed $(OBJDIR)win_divide_by_zero_exception
	touch win_divide_by_zero_exception.out; rm win_divide_by_zero_exception.out
	touch exception_monitor.out; rm exception_monitor.out
	$(PIN) -t $< -- $(OBJDIR)win_divide_by_zero_exception >  win_divide_by_zero_exception.out 2>&1
	$(PIN_DIFF) win_divide_by_zero_exception.out win_divide_by_zero_exception.reference
	$(PIN_DIFF) exception_monitor.out exception_monitor_for_divide_by_zero_test.reference
	rm win_divide_by_zero_exception.failed exception_monitor.out win_divide_by_zero_exception.out

win_cpp_exception.test : $(OBJDIR)exception_monitor$(PINTOOL_SUFFIX) win_cpp_exception.tested win_cpp_exception.failed $(OBJDIR)win_cpp_exception
	touch win_cpp_exception.out; rm win_cpp_exception.out
	touch exception_monitor.out; rm exception_monitor.out
	$(PIN) -t $< -- $(OBJDIR)win_cpp_exception >  win_cpp_exception.out 2>&1
	$(PIN_DIFF) win_cpp_exception.out win_cpp_exception.reference
	$(PIN_DIFF) exception_monitor.out exception_monitor_for_cpp_exception_test.reference
	rm win_cpp_exception.failed exception_monitor.out win_cpp_exception.out

win_debug_service.test : %.test: $(OBJDIR)debugservice_monitor$(PINTOOL_SUFFIX) %.tested %.failed $(OBJDIR)win_debug_service
	touch debugservice_monitor.out; rm debugservice_monitor.out
	touch win_debug_service.out; rm win_debug_service.out
	$(PIN) -t $< -- $(OBJDIR)win_debug_service >  win_debug_service.out 2>&1
	$(PIN_DIFF) win_debug_service.out win_debug_service.reference
	$(PIN_DIFF) debugservice_monitor.out debugservice_monitor.reference
	rm win_debug_service.failed debugservice_monitor.out win_debug_service.out

win_load_library.test : %.test: $(OBJDIR)thread_count$(PINTOOL_SUFFIX) %.tested %.failed $(OBJDIR)win_load_library
	touch thread_count.out; rm thread_count.out
	touch win_load_library.out; rm win_load_library.out
	$(PIN) -t $< -- $(OBJDIR)win_load_library >  win_load_library.out 2>&1
	$(PIN_DIFF) win_load_library.out win_load_library.reference
	$(PIN_DIFF) thread_count.out thread_count_load_library.reference
	rm win_load_library.failed thread_count.out win_load_library.out

win_code_on_reused_memory.test : %.test : %.tested %.failed $(OBJDIR)win_code_on_reused_memory
	touch win_code_on_reused_memory.out; rm win_code_on_reused_memory.out
	$(PIN) -- $(OBJDIR)win_code_on_reused_memory >  win_code_on_reused_memory.out 2>&1
	$(PIN_DIFF) win_code_on_reused_memory.out win_code_on_reused_memory.reference
	rm win_code_on_reused_memory.failed win_code_on_reused_memory.out	

win_exception_context.test: %.test: $(OBJDIR)winapp_exception_context $(OBJDIR)exception_context_monitor$(PINTOOL_SUFFIX) %.tested %.failed 
	$(PIN) -t $(OBJDIR)exception_context_monitor -checkfp 1 -- ./$(OBJDIR)winapp_exception_context
	rm win_exception_context.failed

image_entry.test : %.test: $(OBJDIR)image_entry$(PINTOOL_SUFFIX) %.tested %.failed $(OBJDIR)win_thread_create_apc
	touch image_entry.out; rm image_entry.out
	touch win_thread_create_apc.out; rm win_thread_create_apc.out
	$(PIN) -probe -t $< -out_file  image_entry_probe.out -- $(OBJDIR)win_thread_create_apc  >  win_thread_create_apc.out 2>&1
	$(PIN_DIFF) win_thread_create_apc.out win_thread_create_apc.reference
	$(PIN_DIFF) image_entry_probe.out image_entry.reference
	rm win_thread_create_apc.out
	$(PIN) -t $< -out_file  image_entry_jit.out -- $(OBJDIR)win_thread_create_apc  >  win_thread_create_apc.out 2>&1
	$(PIN_DIFF) win_thread_create_apc.out win_thread_create_apc.reference
	$(PIN_DIFF) image_entry_jit.out image_entry.reference
	rm win_thread_create_apc.out
	rm image_entry.failed image_entry_probe.out image_entry_jit.out

funreplace_alert.test : $(OBJDIR)funreplace_alert$(PINTOOL_SUFFIX) funreplace_alert.tested funreplace_alert.failed $(OBJDIR)socket_app
	touch funreplace_alert.out; rm funreplace_alert.out
	$(PIN) -t $< -- $(OBJDIR)socket_app >  funreplace_alert.out 2>&1
	$(PIN_DIFF) funreplace_alert.out funreplace_alert.reference
	rm funreplace_alert.failed funreplace_alert.out

syscall_std.test : $(OBJDIR)syscall_std$(PINTOOL_SUFFIX) syscall_std.tested syscall_std.failed $(OBJDIR)syscall_std_app
	touch syscall_std.out; rm syscall_std.out
	$(PIN) -t $< -- $(OBJDIR)syscall_std_app >  syscall_std.out 2>&1
	$(PIN_DIFF) syscall_std.out syscall_std.reference
	rm syscall_std.failed syscall_std.out

teb.test : $(OBJDIR)teb$(PINTOOL_SUFFIX) teb.tested teb.failed $(OBJDIR)teb_app
	touch teb.out; rm teb.out
	$(PIN) -t $< -- $(OBJDIR)teb_app >  teb.out 2>&1
	$(PIN_DIFF) teb.out teb.reference
	rm teb.failed teb.out

earlyout.test: $(OBJDIR)earlyout$(PINTOOL_SUFFIX) earlyout.tested earlyout.failed $(OBJDIR)$(HELLO)
	$(PIN) -t $< -- ./$(OBJDIR)$(HELLO)
	rm earlyout.failed

$(OBJDIR)socket_app: socket_app.cpp
	$(CXX)  ${APP_CXXFLAGS} $(NO_OPTIMIZE) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) ws2_32.lib

$(OBJDIR)smcapp_ia32: smcapp_ia32.cpp $(SMC_LIB) smc_util.h sys_memory.h threadlib.h
	$(CXX)  ${APP_CXXFLAGS} $(NO_OPTIMIZE) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(SMC_LIB) $(LPTHREAD)

$(OBJDIR)smcapp_except: smcapp_except.cpp $(SMC_LIB) smc_util.h sys_memory.h threadlib.h
	$(CXX)  ${APP_CXXFLAGS} $(NO_OPTIMIZE) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(SMC_LIB) $(LPTHREAD)

$(OBJDIR)smcapp_mt: smcapp_mt.cpp $(SMC_LIB) smc_util.h sys_memory.h threadlib.h
	$(CXX)  ${APP_CXXFLAGS} $(NO_OPTIMIZE) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(SMC_LIB) $(LPTHREAD)

$(OBJDIR)smcapp_bbl: smcapp_bbl.cpp $(SMC_LIB) smc_util.h sys_memory.h threadlib.h
	$(CXX)  ${APP_CXXFLAGS} $(OPT) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(SMC_LIB) $(LPTHREAD)

$(OBJDIR)syscall_std_app: syscall_std_app.cpp
	$(CXX)  ${APP_CXXFLAGS} $(NO_OPTIMIZE) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)teb_app: teb_app.cpp
        #use big stack in application
	$(CXX)  ${APP_CXXFLAGS} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) /STACK:0x800000,0x1000

$(OBJDIR)win_load_library: win_load_library.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)win_code_on_reused_memory: win_code_on_reused_memory.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${NO_COMDAT_FLAG} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)win_thread_create_apc: win_thread_create_apc.cpp $(OBJDIR)win_tls_dll.dll
	$(CXX)  ${APP_CXXFLAGS2} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS) $(OBJDIR)win_tls_dll.lib

$(OBJDIR)win_tls_dll.dll: win_tls_dll.cpp
	$(CXX) $(APP_CXXFLAGS2)  $(OUTEXE)$@ $< $(APP_CXXLINK_FLAGS) /dll

$(OBJDIR)win_queue_apc: win_queue_apc.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)win_divide_by_zero_exception: win_divide_by_zero_exception.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)win_cpp_exception: win_cpp_exception.cpp
	$(CXX)  ${APP_CXXFLAGS2} $(NO_OPTIMIZE) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)winapp_exception_context: winapp_exception_context.cpp
	$(CXX)  ${APP_CXXFLAGS} $(NO_OPTIMIZE) ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)win_debug_service: win_debug_service.cpp
	$(CXX)  ${APP_CXXFLAGS2} ${OUTEXE}$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)rep: rep_$(TARGET).s
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$@ $< -static -g

$(OBJDIR)big_bss: big_bss.c
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$@ big_bss.c

reptool.test: $(OBJDIR)reptool$(PINTOOL_SUFFIX) $(OBJDIR)rep reptool.tested reptool.failed
	$(PIN) -t $< -- ./$(OBJDIR)rep > reptool.out
	$(PIN_DIFF) reptool.out reptool_$(TARGET).reference
	rm reptool.failed

$(OBJDIR)repcmpsz: repcmpsz.s
	$(AS) $(AS_FLAGS) -o $@.o repcmpsz.s
	$(LD) $(ASLD_FLAGS) -o $@ $@.o

repcmpsz_tool.test: $(OBJDIR)repcmpsz_tool$(PINTOOL_SUFFIX) $(OBJDIR)repcmpsz repcmpsz.tested repcmpsz_tool.failed
	$(PIN) -t $< -- ./$(OBJDIR)repcmpsz
	$(PIN_DIFF) repcmpsz_tool.out repcmpsz_tool.reference
	rm repcmpsz_tool.failed

waitpidbug.test: %.test: $(OBJDIR)waitpidbug %.tested %.failed
	./$< $(PIN_BIN) $(TESTAPP)
	rm $*.failed

$(OBJDIR)waitpidbug: waitpidbug.c
	$(CC) -o $(OBJDIR)waitpidbug waitpidbug.c

big_bss.test: big_bss.failed big_bss.tested $(OBJDIR)big_bss
	$(PIN) -- $(OBJDIR)big_bss
	rm big_bss.failed

hello32.test: hello32.failed hello32.tested
	# test 32 bit app with 64 bit pin, this should fail
	-$(PIN) -- ./hello32 >& hello32.out
	# and we expect to see an error message
	grep 'expected binary for the ' hello32.out
	-$(PIN) -error_file myerr.log -- ./hello32 >& hello32.out
	grep 'expected binary for the ' myerr.log
	rm hello32.failed myerr.log

statica_locktest.test: $(OBJDIR)statica_locktest$(SATOOL_SUFFIX) statica_locktest.tested statica_locktest.failed
	./$(OBJDIR)statica_locktest$(SATOOL_SUFFIX) -i $(OBJDIR)statica_locktest
	rm statica_locktest.failed

#on Linux, virual segments mode is default. In order to check the "segments effective address" mode, vurtual segments 
# should be switched off
segmented_ea_verifier.test : $(OBJDIR)segmented_ea_verifier$(PINTOOL_SUFFIX) $(OBJDIR)thread_segmented_ea segmented_ea_verifier.tested segmented_ea_verifier.failed
	$(PIN) -xyzzy -virtual_segments 0  -t $< -- ./$(OBJDIR)thread_segmented_ea >  $<.out 2>&1
	grep  "SUCCESS" $<.out 
	rm segmented_ea_verifier.failed $<.out

ea_verifier_addr16.test : $(OBJDIR)ea_verifier_addr16$(PINTOOL_SUFFIX) $(OBJDIR)thread_ea_addr16 ea_verifier_addr16.tested ea_verifier_addr16.failed
	$(PIN)  -t $< -- ./$(OBJDIR)thread_ea_addr16 >  $<.out 2>&1
	grep  "SUCCESS" $<.out 
	rm ea_verifier_addr16.failed $<.out

seg_override_win1.test : $(OBJDIR)segmented_ea_verifier$(PINTOOL_SUFFIX) $(OBJDIR)seg_override_app1 seg_override_win1.tested seg_override_win1.failed
	$(PIN)  -t $< -- ./$(OBJDIR)seg_override_app1 >  $<.out 2>&1
	grep  "SUCCESS" $<.out 
	rm seg_override_win1.failed $<.out

# Test reuse of memory pages
memory_reuse.test : memory_reuse.tested memory_reuse.failed
	touch memory_reuse.makefile.copy; rm memory_reuse.makefile.copy
	$(PIN) -xyzzy -malloc_reuse_pages -- $(TESTAPP) makefile memory_reuse.makefile.copy
	$(PIN_DIFF) makefile memory_reuse.makefile.copy
	rm memory_reuse.failed

# Tests delivery of dll load / unload events for dynamically loaded images in JIT mode
load_dummy_dll.test: %.test : $(OBJDIR)load_dummy_dll_tool$(PINTOOL_SUFFIX) $(OBJDIR)load_dummy_dll_app %.tested %.failed
	$(PIN) -t $< -- $(OBJDIR)load_dummy_dll$(EXEEXT)
	grep -c -i "dummy_dll.dll loaded" imageload.out | grep "2"
	grep -c -i "dummy_dll.dll unloaded" imageload.out | grep "2"
	rm load_dummy_dll.failed imageload.out

# Tests delivery of dll load / unload events for dynamically loaded images in probe mode
load_dummy_dll_probe.test: $(OBJDIR)load_dummy_dll_tool$(PINTOOL_SUFFIX) $(OBJDIR)load_dummy_dll_app load_dummy_dll_probe.tested load_dummy_dll_probe.failed
	$(PIN) -probe -t $< -- $(OBJDIR)load_dummy_dll$(EXEEXT)
	grep -c -i "dummy_dll.dll loaded" imageload.out | grep "2"
	grep -c -i "dummy_dll.dll unloaded" imageload.out | grep "2"
	rm load_dummy_dll_probe.failed imageload.out

# Tests proper processing of ia32 dll loaded in Intel 64 process on Windows
x86dll.test: %.test : $(OBJDIR)load_dummy_dll_tool$(PINTOOL_SUFFIX) $(OBJDIR)x86dll_app$(EXEEXT) %.tested %.failed
	$(PIN) -t $< -- $(OBJDIR)x86dll_app$(EXEEXT)
	rm x86dll.failed imageload.out

# Tests proper delivery of symbols for rebased dll in JIT mode
rebase_dll.test: %.test : $(OBJDIR)rebase_dll_tool$(PINTOOL_SUFFIX) $(OBJDIR)rebase_dll_app %.tested %.failed
	$(PIN) -t $< -- $(OBJDIR)rebase_dll$(EXEEXT)
	grep -c -i "nothing" imageload.out | grep "2"
	rm rebase_dll.failed imageload.out

# Tests proper delivery of symbols for rebased dll in probe mode
rebase_dll_probe.test: %.test : $(OBJDIR)rebase_dll_tool$(PINTOOL_SUFFIX) $(OBJDIR)rebase_dll_app %.tested %.failed
	$(PIN) -probe -t $< -- $(OBJDIR)rebase_dll$(EXEEXT)
	grep -c -i "nothing" imageload.out | grep "2"
	rm rebase_dll_probe.failed imageload.out

# Tests PIN_InitSymbolsAlt(EXPORT_SYMBOLS) API and exports only support without external symbol server
exports_only.test: %.test : $(OBJDIR)exports_only_tool$(PINTOOL_SUFFIX) $(OBJDIR)rebase_dll_app %.tested %.failed
	$(PIN_EXE) $(PIN_TEST_FLAGS) -xyzzy -log_server 1 -logfile pinsm.log -t $< -xyzzy -log_server 1 -logfile pinsm.log -- $(OBJDIR)rebase_dll$(EXEEXT)
	grep -c -i "nothing" imageload.out | grep "2"
	test `rm -vf pinsm.log.* | wc -l` -eq "0"
	rm exports_only.failed imageload.out

# Tests PIN_InitSymbolsAlt(EXPORT_SYMBOLS) API and exports only support without external symbol server in probe mode
exports_only_probe.test: %.test : $(OBJDIR)exports_only_tool$(PINTOOL_SUFFIX) $(OBJDIR)rebase_dll_app %.tested %.failed
	$(PIN_EXE) $(PIN_TEST_FLAGS) -probe -xyzzy -log_server 1 -logfile pinsmp.log -t $< -xyzzy -log_server 1 -logfile pinsmp.log -- $(OBJDIR)rebase_dll$(EXEEXT)
	grep -c -i "nothing" imageload.out | grep "2"
	test `rm -vf pinsmp.log.* | wc -l` -eq "0"
	rm exports_only_probe.failed imageload.out

# Tests correct retrieval of section name from PE section header
# Ensure section name does not contain trailing null symbols
secname.test: %.test : $(OBJDIR)secname_tool$(PINTOOL_SUFFIX) $(OBJDIR)secname_app$(EXEEXT) %.tested %.failed
	$(PIN) -t $< -- $(OBJDIR)secname_app$(EXEEXT)
	grep -q " dsec found" secname.out
	grep -q " asection found" secname.out
	rm secname.failed secname.out


# The test pause_tool is used to test "pause_tool".
# Therefore we need to be built it with debug info
$(OBJDIR)pause_tool.o: pause_tool.cpp
	$(CXX) -g $(COPT) $(CXXFLAGS) $(PIN_CXXFLAGS) $(OUTOPT)$@ $<
 
$(OBJDIR)pause_tool$(PINTOOL_SUFFIX): $(OBJDIR)pause_tool.o
	$(CXX) $(PIN_LDFLAGS) -g ${LINK_OUT}$@ $< $(PIN_LIBS)

pause_tool.test: $(OBJDIR)pause_tool$(PINTOOL_SUFFIX) $(OBJDIR)simple pause_tool.tested pause_tool.failed
	rm -f $(OBJDIR)$(@:.test=.out)
	$(PIN) -pause_tool 20 -t $(OBJDIR)pause_tool$(PINTOOL_SUFFIX) -- $(OBJDIR)simple > $(OBJDIR)$(@:.test=.out) &
	until grep 'add-symbol-file' $(OBJDIR)$(@:.test=.out) > /dev/null; \
	    do sleep 1; done
	sleep 1
	awk '''NR==1 { print "attach ", $$NF }''' < $(OBJDIR)$(@:.test=.out) > $(OBJDIR)$(@:.test=.gdbin)
	grep 'add-symbol-file' $(OBJDIR)$(@:.test=.out) >> $(OBJDIR)$(@:.test=.gdbin)
	cat $(@:.test=.gdb) >> $(OBJDIR)$(@:.test=.gdbin)
	$(GDB) -batch -x $(OBJDIR)$(@:.test=.gdbin) -n $(PINBIN) > $(OBJDIR)$(@:.test=.gdbout) 2>&1
	tail -5 $(OBJDIR)$(@:.test=.gdbout) > $(OBJDIR)$(@:.test=.res)
	diff pause_tool.ref $(OBJDIR)$(@:.test=.res)
	rm -f pause_tool.failed


## build rules

$(OBJDIR)%.o : %.cpp
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(OBJDIR)%.o : %.cc
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): $(PIN_LIBNAMES)

$(STATIC_TOOLS): $(PIN_LIBNAMES)

$(OBJDIR)%.o : %.S
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(OBJDIR)%.o : %.s
	$(CXX) ${COPT} $(CXXFLAGS) $(PIN_CXXFLAGS) ${OUTOPT}$@ $<

$(TOOLS): %$(PINTOOL_SUFFIX) : %.o
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)

$(STATIC_TOOLS): %$(SATOOL_SUFFIX): %.o
	${LINKER} $(PIN_SALDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(SAPIN_LIBS) $(DBG)

$(OBJDIR)win_multi_dll_pintool : $(OBJDIR)main_dll$(PINTOOL_SUFFIX) $(OBJDIR)static_secondary_dll$(PINTOOL_SUFFIX) $(OBJDIR)dynamic_secondary_dll$(PINTOOL_SUFFIX) 

$(OBJDIR)main_dll$(PINTOOL_SUFFIX) : $(OBJDIR)main_dll.o $(OBJDIR)static_secondary_dll$(PINTOOL_SUFFIX)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG) $(OBJDIR)static_secondary_dll.lib

$(OBJDIR)static_secondary_dll$(PINTOOL_SUFFIX) : $(OBJDIR)static_secondary_dll.o 
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 

$(OBJDIR)dynamic_secondary_dll$(PINTOOL_SUFFIX) : $(OBJDIR)dynamic_secondary_dll.o 
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG)      

$(OBJDIR)sse-ref$(PINTOOL_SUFFIX) : $(OBJDIR)sse-ref.o $(SSE-REF_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG) $(SSE-REF_ASM_OBJ)

$(OBJDIR)flag_spill_fill_tool1$(PINTOOL_SUFFIX): $(OBJDIR)flag_spill_fill_tool1.o $(OBJDIR)$(FLAG_SPILL_FILL_TOOL1_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $< ${PIN_LPATHS} $(PIN_LIBS) $(DBG) $(OBJDIR)$(FLAG_SPILL_FILL_TOOL1_ASM_OBJ)	

$(OBJDIR)analysis_flag_overwrite_tool1$(PINTOOL_SUFFIX): $(OBJDIR)analysis_flag_overwrite_tool1.o $(OBJDIR)$(ANALYSIS_FLAG_OVERWRITE_TOOL1_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 

$(OBJDIR)analysis_flag_overwrite_tool2$(PINTOOL_SUFFIX): $(OBJDIR)analysis_flag_overwrite_tool2.o $(OBJDIR)$(ANALYSIS_FLAG_OVERWRITE_TOOL2_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 	

$(OBJDIR)analysis_flag_overwrite_tool3$(PINTOOL_SUFFIX): $(OBJDIR)analysis_flag_overwrite_tool3.o $(OBJDIR)$(ANALYSIS_FLAG_OVERWRITE_TOOL3_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 

$(OBJDIR)df_test_tool1$(PINTOOL_SUFFIX): $(OBJDIR)df_test_tool1.o $(OBJDIR)$(DF_TEST_TOOL1_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG)	

$(OBJDIR)reg_operands_test_tool$(PINTOOL_SUFFIX): $(OBJDIR)reg_operands_test_tool.o 
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG)	

$(OBJDIR)df_test_tool2$(PINTOOL_SUFFIX): $(OBJDIR)df_test_tool2.o $(OBJDIR)$(DF_TEST_TOOL2_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 	

$(OBJDIR)df_test_tool3$(PINTOOL_SUFFIX): $(OBJDIR)df_test_tool3.o $(OBJDIR)$(DF_TEST_TOOL3_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 	

$(OBJDIR)df_test_tool4$(PINTOOL_SUFFIX): $(OBJDIR)df_test_tool4.o $(OBJDIR)$(DF_TEST_TOOL4_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 	

$(OBJDIR)df_test_tool5$(PINTOOL_SUFFIX): $(OBJDIR)df_test_tool5.o $(OBJDIR)$(DF_TEST_TOOL5_ASM_OBJ)
	${LINKER} $(PIN_LDFLAGS) $(LINK_DEBUG) ${LINK_OUT}$@ $+ ${PIN_LPATHS} $(PIN_LIBS) $(DBG) 	

$(OBJDIR)load_dummy_dll_tool$(PINTOOL_SUFFIX) : load_dummy_dll_tool.cpp
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $< /link /manifest:no $(PIN_LDFLAGS) $(PIN_LIBS) ${LINK_OUT}$@

$(OBJDIR)rebase_dll_tool$(PINTOOL_SUFFIX) : rebase_dll_tool.cpp
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $< /link /manifest:no $(PIN_LDFLAGS) $(PIN_LIBS) ${LINK_OUT}$@

$(OBJDIR)exports_only_tool$(PINTOOL_SUFFIX) : exports_only_tool.cpp
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $< /link /manifest:no $(PIN_LDFLAGS) $(PIN_LIBS) ${LINK_OUT}$@

$(OBJDIR)secname_tool$(PINTOOL_SUFFIX) : secname_tool.cpp
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS) $< /link /manifest:no $(PIN_LDFLAGS) $(PIN_LIBS) ${LINK_OUT}$@

$(OBJDIR)stackunswitch: $(OBJDIR)stackunswitch.o $(OBJDIR)unswitcher.o
	$(CXX) $(PIN_LDFLAGS) ${OUTOPT}stackunswitch $(OBJDIR)stackunswitch.o $(OBJDIR)unswitcher.o $(PIN_LIBS)

DL_LIB = -ldl

$(OBJDIR)dlopen: $(OBJDIR)dlopen.o $(OBJDIR)dummy.so
	$(CXX) $(PIN_LDFLAGS) ${OUTOPT}$@ $< $(PIN_LIBS) $(DL_LIB} $(DBG)

$(OBJDIR)dltest: dltest.c one.c two.c
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$(OBJDIR)two.so -fpic -shared two.c -g
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$(OBJDIR)one.so -fpic -shared one.c -g
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$(OBJDIR)dltest $(APP_CXXFLAGS) dltest.c $(DL_LIB) -Wl,-rpath,`pwd`\/$(OBJDIR) -g

$(OBJDIR)dltest2: dltest2.c $(OBJDIR)dltest
	$(CC) ${OUTOPT}$@ $(APP_CXXFLAGS) dltest2.c $(DL_LIB) -Wl,-rpath,`pwd`\/$(OBJDIR) -g

$(OBJDIR)dltest-mac: dltest.c one.c two.c
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$(OBJDIR)two.so -fPIC -dynamiclib two.c -g
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$(OBJDIR)one.so -fPIC -dynamiclib one.c -g
	$(CC) $(APP_CXXFLAGS) ${OUTOPT}$(OBJDIR)dltest-mac $(APP_CXXFLAGS) dltest.c $(DL_LIB) -g

$(OBJDIR)oneprog: oneprog.c
	$(CC) ${OUTEXE}$(OBJDIR)oneprog $(NO_OPTIMIZE) $(APP_CXXFLAGS) oneprog.c

$(OBJDIR)redblue: redblue.s
	$(CC) $(APP_CXXFLAGS) -o $@ -g redblue.s

special.test: $(OBJDIR)special$(PINTOOL_SUFFIX) special.tested special.failed $(OBJDIR)redblue
	$(PIN) -t $< -- ./$(OBJDIR)redblue
	$(PIN_CMP) special.out special.ref
	rm special.failed

pusha_popa.test: $(OBJDIR)pusha_popa$(EXEEXT) pusha_popa.tested pusha_popa.failed
	$(PIN) -- ./$(OBJDIR)pusha_popa$(EXEEXT)
	rm pusha_popa.failed

$(OBJDIR)pusha_popa$(EXEEXT): $(OBJDIR)pusha_popa.$(OBJEXT) $(OBJDIR)pusha_popa_asm.$(OBJEXT)
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS)  $(OUTEXE)$@ $+ $(APP_CXXLINK_FLAGS)

$(OBJDIR)pusha_popa.$(OBJEXT): pusha_popa.cpp
	$(CXX) $(CXXFLAGS) $(PIN_CXXFLAGS)  $(COPT) $(OUTOPT)$@ $<

$(OBJDIR)pusha_popa_asm.obj: pusha_popa_asm.asm
	ml /nologo /c /Fo$@ $<

$(OBJDIR)pusha_popa_asm.o: pusha_popa_asm.s
	$(CC) $(APP_CXXFLAGS) -c $< -o $@

$(OBJDIR)load_dummy_dll_app : $(OBJDIR)load_dummy_dll$(EXEEXT) $(OBJDIR)dummy_dll$(PINTOOL_SUFFIX)

$(OBJDIR)load_dummy_dll$(EXEEXT): load_dummy_dll.c
	$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS)  $(OUTEXE)$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)dummy_dll$(PINTOOL_SUFFIX): dummy_dll.c
	$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS) $(OUTEXE)$@ $< $(APP_CXXLINK_FLAGS) /dll /noentry

$(OBJDIR)x86dll_app$(EXEEXT): x86dll_app.cpp
	$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS)  $(OUTEXE)$@ $< $(APP_CXXLINK_FLAGS)
	touch x86dll.def
	${LINKER} /DLL /MACHINE:X86 /DEF:x86dll.def $(PIN_EXTRA_LDFLAGS) /NOENTRY /MANIFEST:NO ${LINK_OUT}$(OBJDIR)x86dll.dll

$(OBJDIR)rebase_dll_app : $(OBJDIR)rebase_dll$(EXEEXT) $(OBJDIR)dummy_dll$(PINTOOL_SUFFIX) $(OBJDIR)dummy_dll2$(PINTOOL_SUFFIX)

$(OBJDIR)rebase_dll$(EXEEXT): rebase_dll.c
	$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS)  $(OUTEXE)$@ $< $(APP_CXXLINK_FLAGS)

$(OBJDIR)dummy_dll2$(PINTOOL_SUFFIX) : $(OBJDIR)dummy_dll$(PINTOOL_SUFFIX)
	cp -f $< $@

$(OBJDIR)secname_app$(EXEEXT): secname_app.cpp
	$(CXX) $(CXXFLAGS) $(APP_CXXFLAGS)  $(OUTEXE)$@ $< $(APP_CXXLINK_FLAGS)

inlined-stack-arg.test: $(OBJDIR)inlined-stack-arg$(PINTOOL_SUFFIX) inlined-stack-arg.tested inlined-stack-arg.failed
	touch $<.makefile.copy; rm $<.makefile.copy
	$(PIN) -xyzzy -inline 1 -t $< -- $(TESTAPP) makefile $<.makefile.copy
	$(PIN_CMP) makefile $<.makefile.copy
	$(PIN_DIFF) inlined-stack-arg.out inlined-stack-arg.reference
	rm inlined-stack-arg.failed $<.makefile.copy inlined-stack-arg.out

earlymalloc.test: $(OBJDIR)earlymalloc$(PINTOOL_SUFFIX) earlymalloc.tested earlymalloc.failed
	$(PIN) -t $< -- $(TESTAPP) makefile earlymalloc.makefile.copy
	rm earlymalloc.failed earlymalloc.makefile.copy

ifeq ($(SOTOOL),1)
earlymallocexe.test: earlymallocexe.tested earlymallocexe.failed
	$(MAKE) SOTOOL=0 $(OBJDIR)earlymalloc >/dev/null 2>&1
	$(PIN) -t $(OBJDIR)earlymalloc -- $(TESTAPP) makefile earlymallocexe.makefile.copy
	rm earlymallocexe.failed earlymallocexe.makefile.copy
else
earlymallocexe.test: $(OBJDIR)earlymalloc$(PINTOOL_SUFFIX) earlymallocexe.tested earlymallocexe.failed
	$(PIN) -t $< -- $(TESTAPP) makefile earlymallocexe.makefile.copy
	rm earlymallocexe.failed earlymallocexe.makefile.copy
endif

emu_stack.test: $(OBJDIR)emu_stack$(PINTOOL_SUFFIX) emu_stack.tested emu_stack.failed
	$(PIN) -t $< -- $(TESTAPP) makefile emu_stack.makefile.copy
	rm emu_stack.failed emu_stack.makefile.copy

emu_jumps.test: $(OBJDIR)emu_jumps$(PINTOOL_SUFFIX) emu_jumps.tested emu_jumps.failed
	$(PIN) -t $< -- $(TESTAPP) makefile emu_jumps.makefile.copy
	rm emu_jumps.failed emu_jumps.makefile.copy

$(OBJDIR)swizzle_seg_app_l: swizzle_seg_app.cpp swizzle_seg_app_lin_$(TARGET).s
	$(CXX) $(APP_CXXFLAGS) $^ -o $@

$(OBJDIR)swizzle_seg_app_w$(EXEEXT): swizzle_seg_app.cpp swizzle_seg_app.def swizzle_seg_app_win_$(TARGET).asm
	$(MASM) /c /Fo$(OBJDIR)swizzle_seg_app_win_$(TARGET).obj swizzle_seg_app_win_$(TARGET).asm
	$(CXX) /c $(APP_CXXFLAGS) /Fo$(OBJDIR)swizzle_seg_app.obj swizzle_seg_app.cpp 
	${LINKER} ${LINK_OUT}$@ /DEF:swizzle_seg_app.def $(OBJDIR)swizzle_seg_app.obj $(OBJDIR)/swizzle_seg_app_win_$(TARGET).obj

swizzle_seg.test: %.test: $(OBJDIR)swizzle_seg$(PINTOOL_SUFFIX) $(OBJDIR)swizzle_seg_app_$(TARGET_OS)$(EXEEXT) %.tested %.failed
	$(PIN) -t $< -- $(OBJDIR)swizzle_seg_app_$(TARGET_OS)$(EXEEXT) > $*.out
	$(PIN_DIFF)  $*.out $*.reference
	rm $*.failed $*.out
	
far_ret.test: %.test: $(OBJDIR)far %.tested %.failed
	$< >$*.ref
	$(PIN) -- $< > $*.out
	$(PIN_DIFF)  $*.out $*.ref
	rm $*.failed $*.out $*.ref

$(OBJDIR)far: farmain.cpp far.s
	$(CXX) $(APP_CXXFLAGS) $^  -o $@
	 
## cleaning
clean:
	-rm -rf $(OBJDIR) *.output  *.out *.tested *.failed 

